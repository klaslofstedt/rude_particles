#include "ofApp.h"

#define FONT_STYLE 						"MoonFlower.ttf"
#define FONT_SIZE 						120

#define PARTICLES_NUM 					10000
#define PARTICLE_MIN_SIZE_PX 			1
#define PARTICLE_MAX_SIZE_PX 			2

#define COLOR_BACKGROUND 				39
#define COLOR_PARTICLES 				190

#define PID_SAMPLE_TIME_MS				20		
#define PID_SETPOINT					20 		// (cm)
#define PID_KP 							60		// magic number
#define PID_KD 							2		// magic number
#define PID_OUT_MAX						50000 	// pixels
#define PID_OUT_MIN						0		// pixels

#define DISTANCE_SENSOR_MAX_CM			20
#define DISTANCE_SENSOR_MIN_CM			5
#define DISTANCE_SENSOR_TRIGGER_TIME_MS	2000

//--------------------------------------------------------------
void ofApp::setup()
{
	// Load all insulting words into RAM
	ofBuffer buffer1 = ofBufferFromFile("insult1.txt");
	for (auto line : buffer1.getLines())
	{
    	word1.push_back(line);
	}
	ofBuffer buffer2 = ofBufferFromFile("insult2.txt");
	for (auto line : buffer2.getLines())
	{
    	word2.push_back(line);
	}
	ofBuffer buffer3 = ofBufferFromFile("insult3.txt");
	for (auto line : buffer3.getLines())
	{
    	word3.push_back(line);
	}

	// Load a font
	font.loadFont(FONT_STYLE, FONT_SIZE, true, true, true);
	// Prevent screen tearing
	ofSetVerticalSync(true);
	// Fill a vector with particles
	particles.assign(PARTICLES_NUM, demoParticle());
	// Set background grayscale color
	ofBackground(COLOR_BACKGROUND);
	// Generate pixel attraction points for every particle
	generateAttractionPointsFromInsult();
	
	// Initialise particles
	for (unsigned int i = 0; i < particles.size(); i++)
	{		
		particles[i].setAttractPoints(&attractPoints);
		particles[i].setParticleSizes(PARTICLE_MIN_SIZE_PX, PARTICLE_MAX_SIZE_PX);
		particles[i].reset();
	}	

	// Initialise serial port
	bool success = serial.setup("COM17", 9600);
	if (success)
	{
		ofLogNotice("Serial init success");
	}
	else
	{
		ofLogNotice("Failed to init serial");
	}
	serial.flush(true, true);

	// Set startup timestamp
	timeProgramStart_ms = ofGetSystemTimeMillis();

	// Sensor has not yet been triggered
	isSensorTriggered_cm = false;

	// Initialise PID variables
	pidDt_ms = ofGetSystemTimeMillis();
	pidOutput = 0;
	pidInputLast = 0;
	pidErrorLast = 0;
}

uint16_t ofApp::readSerial()
{
	uint16_t distance_cm = 0;
	// loop until we've read everything
	while (serial.available() > 0)
	{
		uint8_t d1 = serial.readByte();
		uint8_t d2 = serial.readByte();
		distance_cm = ((uint16_t) d2 << 8) | d1;
	}
	return distance_cm;
}


// Simple PID regulator without integrating part
void ofApp::PID_regulator(float input, float dt)
{
    // These are used for ease to read
    float p_term, d_term, error, output, rate;

	rate = (input - pidErrorLast) / dt;
    // Calculate error between current and desired position
    error = PID_SETPOINT - input;
    // Calculate the P contribution
    p_term = PID_KP * error;
	// Calculate D contribution
    d_term = PID_KD * rate;
    //Calculate output
    output = p_term - d_term;

    // Check boundaries
    if(output > PID_OUT_MAX){
        output = PID_OUT_MAX;
    }
    else if(output < PID_OUT_MIN) {
        output = PID_OUT_MIN;
    }
    // Set data for output and next loop
    pidOutput 		= output; 
    pidInputLast 	= input;
    pidErrorLast 	= error;
}

void ofApp::generateAttractionPointsFromInsult()
{
	// Fbo string into pixels 
	fbo.allocate(ofGetWidth(), ofGetHeight());
	fbo.begin();
	ofClear(0);

	string word = constructStringInsult();
	font.drawString(word, ofGetWidth() 	* 0.5 - font.stringWidth(word) 
										* 0.5, ofGetHeight() 
										* 0.5 - font.stringHeight(word) * 0.1);

	fbo.end();
	fbo.readToPixels(pixels);

	// Hardcode a new attraction point to every particle
	attractPoints.clear();

	for (int i = 0; i < PARTICLES_NUM; i++)
	{
		int x = ofRandom(ofGetWidth());
		int y = ofRandom(ofGetHeight());
		if (pixels.getColor(x, y) != ofColor(0, 0)) 
		{
			attractPoints.push_back( glm::vec3(x, y, 0));
		}
	}
}

string ofApp::constructStringInsult()
{
	return "YOU " 	+ word1[ofRandom(0, word1.size())] + " "
					+ word2[ofRandom(0, word2.size())] + " "
					+ word3[ofRandom(0, word3.size())] + ".";
}

//--------------------------------------------------------------
void ofApp::update()
{
	timeSinceProgramStart_ms = ofGetSystemTimeMillis() - timeProgramStart_ms;

	// Read sensor and constrain the value
	uint16_t distance_cm = readSerial();
	if (distance_cm >= DISTANCE_SENSOR_MAX_CM){
		distance_cm = DISTANCE_SENSOR_MAX_CM;
	}
	if (distance_cm <= DISTANCE_SENSOR_MIN_CM){
		distance_cm = DISTANCE_SENSOR_MIN_CM;
	}
	
	// Calculate a new pidOutput every PID_SAMPLE_TIME_MS
	if (ofGetSystemTimeMillis() - pidDt_ms >= PID_SAMPLE_TIME_MS)
	{
		PID_regulator(distance_cm, PID_SAMPLE_TIME_MS);
		pidDt_ms = ofGetSystemTimeMillis();
	}
	
	// Update particles every update, and update with pidOutput when neccessary
	for(int i = 0; i < particles.size(); i++)
	{
		particles[i].update(i, pidOutput);
	}
	
	// Start generating a new insult only if:
	if (ofGetKeyPressed('p') || (distance_cm == DISTANCE_SENSOR_MAX_CM))
	{
		// If sensor is triggered, start counter
		if (isSensorTriggered_cm == false)
		{
			isSensorTriggered_cm = true;
			timeSinceSensorTriggered_ms = ofGetSystemTimeMillis() - timeProgramStart_ms;
		}
		// If sensor is still triggered, generate a new, fresh insult
		if (timeSinceProgramStart_ms - timeSinceSensorTriggered_ms > DISTANCE_SENSOR_TRIGGER_TIME_MS)
		{
			for(unsigned int i = 0; i < particles.size(); i++)
			{		
				particles[i].setAttractPoints(&attractPoints);
				generateAttractionPointsFromInsult();
			}	
			// Reset the bool when a new insult has been generated
			isSensorTriggered_cm = false; 
		}
	}
	else // If sensor is not triggered, reset the bool
	{
		isSensorTriggered_cm = false;
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofSetColor(COLOR_PARTICLES);
	for(unsigned int i = 0; i < particles.size(); i++)
	{
		particles[i].draw();
	}	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
