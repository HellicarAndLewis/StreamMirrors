#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "KinectOrCamera.h"
#include "RamVideo.h"
#include "Carousel.h"

class testApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);		

	KinectOrCamera cam;

	ofxCvColorImage			colorImg;
	ofxCvGrayscaleImage		depthImg;
	ofxCvGrayscaleImage		threshImg;
	void rotateRgb90(unsigned char *a, unsigned char *b, bool clockwise, bool flipX);
	void rotate90(unsigned char *a, unsigned char *b, bool clockwise, bool flipX);
	unsigned char *rgbRot;
	
	void finishedRecording();
	RamVideo *video;
	
	vector<RamVideo*> videos;

	float lastTimeFinishedRecording;
	void loadEverything();
	void saveEverything();
	int VISION_WIDTH;
	int VISION_HEIGHT;
	bool rotateClockwise;
	bool flipX;
	
	float triggersY;
	float triggersX;
	float triggerRadius;
	void drawTriggers();

	bool drawDebug;
	int triggerDepth;
	void detectPresence(bool &left, bool &right);
	bool leftDetected;
	bool rightDetected;
	bool lastLeftDetected;
	bool lastRightDetected;
	
	
	bool recording;
	bool playing;

	int holdCount;
	
	void drawOverlays();
	Carousel carousel;
};

