#pragma once

#include "ofMain.h"
#include "demoParticle.h"

class ofApp : public ofBaseApp
{
	public:
		void setup();
		void update();
		void draw();
		void resetParticles();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		string constructStringInsult();
		void generateAttractionPointsFromInsult();
		uint16_t readSerial();
		void PID_regulator(float input, float dt);
		
		vector <demoParticle> particles;
		vector <glm::vec3> attractPoints;

		// fbo object and pixels
		ofFbo fbo;
		ofPixels pixels;

		// Insult vectors
		vector <string> word1;
		vector <string> word2;
		vector <string> word3;

		// Timestamp variables
		float timeProgramStart_ms;
		float timeSinceSensorTriggered_ms;
		float timeSinceProgramStart_ms;
		bool isSensorTriggered_cm;

		// Font object
		ofTrueTypeFont font;

		// Serial object
		ofSerial serial;

		// PID variables
		float pidOutput;
    	float pidInputLast;
    	float pidErrorLast;
		float pidDt_ms;
};
