#include "demoParticle.h"

//------------------------------------------------------------------
demoParticle::demoParticle(){
	attractPoints = NULL;
}

//------------------------------------------------------------------
void demoParticle::setMode(particleMode newMode){
	mode = newMode;
}

//------------------------------------------------------------------
void demoParticle::setAttractPoints( vector <glm::vec3> * attract ){
	attractPoints = attract;
}

void demoParticle::setParticleSizes(float minSize, float maxSize)
{
	particleMinSize = minSize;
	particleMaxSize = maxSize;
}

//------------------------------------------------------------------
void demoParticle::reset(){
	//the unique val allows us to set properties slightly differently for each particle
	uniqueVal = ofRandom(-10000, 10000);
	
	pos.x = ofRandomWidth();
	pos.y = ofRandomHeight();
    pos.z = 0; 
	
	vel.x = ofRandom(-3.9, 3.9);
	vel.y = ofRandom(-3.9, 3.9);
    vel.z = 0;
	
	frc   = glm::vec3(0,0,0);
	
	size = ofRandom(particleMinSize, particleMaxSize);
	
	drag  = ofRandom(0.92, 0.98);	
}

//------------------------------------------------------------------
void demoParticle::update(int index, float pidValue)
{	
	if (attractPoints)
	{
		//1 - find closest attractPoint 
		glm::vec3 closestPt;
		int closest = -1; 
		float closestDist = 9999999;

		float lenSq = glm::length2( attractPoints->at(index)-pos );
		if( lenSq < closestDist ){
			closestDist = lenSq;
			closest = index;
		}
		
		//2 - if we have a closest point - lets calcuate the force towards it
		if( closest != -1 ){
			closestPt = attractPoints->at(closest);
			// Add some random distance depending on pidValue	
			float dist = sqrt(closestDist) + ofRandom(0, pidValue);
			
			vel *= drag;
				
			//lets also limit our attraction to a certain distance and don't apply if 'f' key is pressed
			if ((dist < ofGetWidth()) && (dist > 1) && (!ofGetKeyPressed('p'))){
				//in this case we don't normalize as we want to have the force proportional to distance
				frc = closestPt - pos;
				vel += frc * 0.003;
				//vel += frc * 0.0001;
			}else{
				//if the particles are not close to us, lets add a little bit of random movement using noise. this is where uniqueVal comes in handy. 			
				frc.x = ofSignedNoise(uniqueVal, pos.y * 0.01, ofGetElapsedTimef()*0.2);
				frc.y = ofSignedNoise(uniqueVal, pos.x * 0.01, ofGetElapsedTimef()*0.2);
				vel += frc * 0.4;
			}
		}
	}

	//2 - UPDATE OUR POSITION
	pos += vel; 
	
	//3 - (optional) LIMIT THE PARTICLES TO STAY ON SCREEN 
	//we could also pass in bounds to check - or alternatively do this at the ofApp level
	if( pos.x > ofGetWidth() ){
		pos.x = ofGetWidth();
		vel.x *= -1.0;
	}else if( pos.x < 0 ){
		pos.x = 0;
		vel.x *= -1.0;
	}
	if( pos.y > ofGetHeight() ){
		pos.y = ofGetHeight();
		vel.y *= -1.0;
	}
	else if( pos.y < 0 ){
		pos.y = 0;
		vel.y *= -1.0;
	}	
}

//------------------------------------------------------------------
void demoParticle::draw()
{
	//ofSetColor(103, 160, 237);
	ofSetColor(239);
	
	ofDrawCircle(pos.x, pos.y, size);
}

