#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "KinectOrCamera.h"


class testApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);




	
	
	KinectOrCamera cam;

	ofxCvColorImage			colorImg;
	ofxCvGrayscaleImage		depthImg;
	ofxCvGrayscaleImage		threshImg;
	float threshold;
};

