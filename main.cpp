#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main() 
{
	ofGLWindowSettings settings;
	settings.setGLVersion(3, 2);
	//settings.setSize(720, 720);
	settings.setSize(1920, 1080);
	settings.windowMode = OF_FULLSCREEN;

	ofCreateWindow(settings);
	ofRunApp(new ofApp());
}
