#include "ofApp.h"

#define PROJECT_FPS 50
#define NUM_OF_BALLS 200

//--------------------------------------------------------------
void ofApp::setup() 
{
	ofSetFrameRate(PROJECT_FPS);

	ofBackground(0);
	box2d.init();

	box2d.setGravity(0, 10.0);

	box2d.setFPS(PROJECT_FPS);

	// There has got to be a better way to build edges than this
	lines.push_back(ofPolyline());
	int x = 0, y = 0;
    lines.back().addVertex(x, y);
	for(x = 0; x < 400; x++)
	{
		y = 350;
		lines.back().addVertex(x, y);
	}

	for(y = 350; y < 950; y++)
	{
		x = 400;
		lines.back().addVertex(x, y);
	}

	for(x = 400; x < 1000; x++)
	{
		y = 950;
		lines.back().addVertex(x, y);
	}

	for(y = 950; y > 400; y--)
	{
		x = 1000;
		lines.back().addVertex(x, y);
	}

	lines.back().simplify();
	shared_ptr<ofxBox2dEdge> edge = shared_ptr<ofxBox2dEdge>(new ofxBox2dEdge);
    
    for(int i = 0; i < lines.back().size(); i++)
	{
        edge.get()->addVertex(lines.back()[i]);
    }
    edge.get()->create(box2d.getWorld());
    edges.push_back(edge);

	number_of_targets = NUM_OF_BALLS;

	shader.load("shader/shader.vert", "shader/shader.frag");

	timeStart = ofGetElapsedTimeMillis();

	// Add some delay before first ball spawn to be able to start a screen recorder
	timeSpawn = 2000;
}
//--------------------------------------------------------------
void ofApp::update() 
{
	box2d.update();
}

//--------------------------------------------------------------
void ofApp::draw() 
{
	if (circles.size() > NUM_OF_BALLS) 
	{
		circles.erase(circles.begin());  
	}  


	float timeCurrent = ofGetElapsedTimeMillis();
	
	// Spawn a new ball
	if (timeCurrent - timeStart >= timeSpawn)
	{
		timeStart = ofGetElapsedTimeMillis();
		// Set a new ball spawns time beteen 0 and 2 seconds
		timeSpawn = ofRandom(0, 2000);

		circles.push_back(shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle));
		// Set density, bounce, friction of the last added ball
		circles.back().get()->setPhysics(1.5, 0.22, 0.1);
		// Randomise a spawn position between 0 and 220 and drop it -100px from outside the screen
		circles.back().get()->setup(box2d.getWorld(), ofRandom(0, 220), -100, 15);

		// Make ball color white
		glm::vec4 color = glm::vec4(1.0, 1.0, 1.0, 0);
		colors.push_back(color);
	}

	// Make a vector with the centre point of every circle
	vector<glm::vec4> points;
	for (int i = 0; i < circles.size(); i++) 
	{
		glm::vec4 p = glm::vec4(circles[i]->getPosition().x, circles[i]->getPosition().y, 0, 0);
		points.push_back(p);
	}

	// Add shader to point vector
	shader.begin();
	shader.setUniform1f("time", ofGetElapsedTimef());
	shader.setUniform2f("resolution", ofGetWidth(), ofGetHeight());
	shader.setUniform4fv("targets", &points[0].x, circles.size());
	shader.setUniform4fv("colors", &colors[0].x, circles.size());
	ofRect(0, 0, ofGetWidth(), ofGetHeight());

	shader.end();
}

void ofApp::mousePressed(int x, int y, int button) 
{

}
