#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "KinectOrCamera.h"
#include "RamVideo.h"
#include "Carousel.h"
#include "ActivityMonitor.h"
#include "JumpDetector.h"



#define HOLD_COUNT 80
#define MAX_VIDEO_LENGTH 50
#define MIN_VIDEO_LENGTH 4

#include "ofxSyphon.h"
enum STATE {
	WAITING_FOR_PERSON,
	READY_WITH_PERSON,
	RECORDING,
	WAITING_FOR_PERSON_TO_GO
};

class testApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	
	STATE state;
	
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

	ofFbo fbo;
	ofxSyphonServer syphon;
	
	float lastTimeFinishedRecording;
	void loadEverything();
	void saveEverything();
	int VISION_WIDTH;
	int VISION_HEIGHT;
	bool rotateClockwise;
	bool flipX;
	

	bool drawDebug;
	int nearThreshold;
		
	bool recording;
	
	

	int holdCount;
	void doCompositing();
	void drawOverlays();
	Carousel carousel;
	void doVision();
	void setupVision();

	float minRecordingInterval;
	ActivityMonitor activityMonitor;
	JumpDetector jumpDetector;
	ofTexture videoFeed;
	unsigned char *videoFeedData;
	float carouselDelay;
	ofTrueTypeFont debugFont;
};

