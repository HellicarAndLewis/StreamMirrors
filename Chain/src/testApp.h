#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "KinectOrCamera.h"
#include "RamVideo.h"


// 40 videos of 640x480 for 60 frames are 70MB each, so 40 videos is 3GB
#define MAX_NUM_VIDEOS 40


#include "Carousel.h"
#include "ActivityMonitor.h"
#include "PresenceDetector.h"

#include "DotProcessor.h"



#define HOLD_COUNT 80
#define MAX_VIDEO_LENGTH 60
#define MIN_VIDEO_LENGTH 15



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

	void setupGraphics();
	
	
	KinectOrCamera cam;
	
	ofxCvColorImage			colorImg;
	ofxCvGrayscaleImage		bgImg;
	ofxCvGrayscaleImage		depthImg;
	ofxCvGrayscaleImage		threshImg;
	
	bool learnBackground;
	int bgHysteresis;
	int farThreshold;
	
	int erosions;
	int dilations;
	
	int blurs;
	int blurSize;
	
	
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
	

	bool drawDebug;
	int triggerDepth;
	
	bool recording;

	

	int holdCount;
	void doCompositing();
	void drawOverlays();
	Carousel carousel;
	void doVision();
	void setupVision();
	
	float minRecordingInterval;
	ActivityMonitor activityMonitor;
	PresenceDetector presenceDetector;
	ofTexture videoFeed;
	unsigned char *videoFeedData;
	float carouselDelay;
    DotProcessor dots;
	
	ofImage frame;
	
	ofImage one;
	ofImage two;
	ofImage three;
	
	ofImage ready;
	
	void drawProgressBar(float v);
	ofImage progressBar;
};

