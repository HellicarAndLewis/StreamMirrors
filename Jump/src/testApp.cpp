

#include "testApp.h"
#include "ofxSimpleGuiToo.h"
#include "RamVideo.h"





void testApp::setupGraphics() {
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	frame.loadImage("frame.png");
}



//--------------------------------------------------------------
void testApp::setup(){

	setupGraphics();
	setupVision();
	
	state = WAITING_FOR_PERSON;
	debugFont.loadFont("Impact.ttf", 60);

	lastTimeFinishedRecording = -1000;

	video = new RamVideo();
	video->setup(480, 640, MAX_VIDEO_LENGTH);

	for(int i =0; i < 5; i++) {
		int size = 480*640*4;
		preroll.push_back(new unsigned char[size]);
		memset(preroll.back(), 0, size);
	}
	
	drawDebug = false;
	
	
	nearThreshold = 100;
	minRecordingInterval = 4;
	
	carousel.setVideoFeed(&videoFeed);
	
	
	gui.addToggle("Draw Debug", drawDebug);
	
	
	gui.addSlider("Min Time between recordings", minRecordingInterval, 2, 20);
	gui.addSlider("Carousel video duration", carousel.frameDuration, 10, 200);
	gui.addSlider("Carousel inactivity delay", carouselDelay, 0, 5);
	gui.addSlider("Slide time pct", carousel.slideTime, 0.05, 1);
	gui.addTitle("Carousel");
	gui.addSlider("Overlap", carousel.overlap, 0, 100);
	gui.addTitle("Triggers");
	gui.addSlider("Trigger Height", jumpDetector.triggerHeight, 0, VISION_WIDTH);
	gui.addSlider("Near Threshold", jumpDetector.nearThreshold, 0, 255);

	gui.addSlider("Far Threshold", farThreshold, 0, 255); 
	gui.addToggle("save all videos", saveAllVideos);
	gui.addToggle("clear all videos", clearAllVideos);
	
	clearAllVideos = false;
	saveAllVideos = false;
	
	
	dots.setup(480, 640);
	gui.setAlignRight(true);
	gui.setAutoSave(true);
	gui.loadFromXML();
	
	
	
	//ofSetOrientation(OF_ORIENTATION_90_LEFT);
	
	
}


void testApp::finishedRecording() {
	holdCount = 0;
	if(video->getLength()>0) {
		
		videos.push_back(video);
		printf("Finished recording video of %d frames\n", video->getLength());
		video = new RamVideo();
		video->setup(480, 640, MAX_VIDEO_LENGTH);
		carousel.replaceVideoFeedWithVideo(videos.back());
		carousel.autoScroll();
		lastTimeFinishedRecording = ofGetElapsedTimef();
	} else {
		printf("Didn't record anything!!\n");
	}
}



// just alpha out the person and upload to texture
void testApp::doCompositing() {
	int numPix = VISION_WIDTH * VISION_HEIGHT;
	unsigned char *img	= colorImg.getPixels();
	unsigned char *t = threshImg.getPixels();
	int i4 = 0;
	int i3 = 0;
	for(int i = 0; i < numPix; i++) {

		videoFeedData[i4] = (img[i3]*t[i])/255;
		videoFeedData[i4+1] = (img[i3+1]*t[i])/255;
		videoFeedData[i4+2] = (img[i3+2]*t[i])/255;
		videoFeedData[i4+3] = t[i];
		i4 += 4;
		i3 += 3;
	}
	videoFeed.loadData(videoFeedData, VISION_HEIGHT, VISION_WIDTH, GL_RGBA);
	unsigned char *f = preroll.front();
	preroll.pop_front();
	memcpy(f, videoFeedData, VISION_WIDTH*VISION_HEIGHT*4);
	preroll.push_back(f);
}


//--------------------------------------------------------------
void testApp::update(){
	ofBackground(0);

	if(clearAllVideos) {
		carousel.clearVideos();
		clearAllVideos = false;
	}
	if(saveAllVideos) {
		carousel.saveVideos();
		saveAllVideos = false;
	}
	
	carousel.checkForSize();
	
	doVision();
	doCompositing();
	activityMonitor.update(threshImg);
	jumpDetector.update(threshImg, depthImg);// mouseIsDown);
	int lastState = state;
	if(state==WAITING_FOR_PERSON) {
		if(jumpDetector.personReady()) {
			state = READY_WITH_PERSON;
		}
	} else if(state==READY_WITH_PERSON) {
		if(jumpDetector.personJumping()) {
			state = RECORDING;
			// start recording here
		} else if(!jumpDetector.personPresent()) {
			state = WAITING_FOR_PERSON;
		}
	} else if(state==RECORDING) {
		if(jumpDetector.personReady()) {
			state = WAITING_FOR_PERSON_TO_GO;
			// stop recording here
		}
	} else if(state==WAITING_FOR_PERSON_TO_GO) {
		if(!jumpDetector.personPresent()) {
			state = WAITING_FOR_PERSON;
		}
	}
	
	
	// if there's been no movement for a while, (and we're not recording)
	// start spinning the carousel
	if(!carousel.isScrolling() && activityMonitor.getTimeSinceLastMove()>carouselDelay && state!=RECORDING) {
		printf("Activating carousel\n");
		carousel.autoScroll();
	} else {
		
		// decide whether to go back to the video feed
		if(activityMonitor.getTimeSinceLastMove()<carouselDelay && carousel.isScrolling()
		   && // allow the video to play for a while after recording.
		   ofGetElapsedTimef() - lastTimeFinishedRecording>minRecordingInterval
		   ) {
			carousel.scrollToVideoFeed();
			//printf("Creating space and going to end\n");
		}
		
		
		
		if(state==RECORDING && lastState!=RECORDING) {
			video->clear();
			for(int i = 0; i < preroll.size(); i++) {
				video->record(preroll[i]);
			}
		}

		// stop recording
		if(lastState==RECORDING && state!=RECORDING && video->getLength()>MIN_VIDEO_LENGTH) {
			printf("Recording finished because user stepped away\n");
			finishedRecording();
		}
		
		if(state==RECORDING) {
			// assemble composite
			bool stillCanRecord = video->record(videoFeedData);
			if(!stillCanRecord) {
				printf("Recording finished becasue it's maxed length\n");
				finishedRecording();
			}
		}
	}
	
	ofSetWindowTitle(ofToString(ofGetFrameRate(), 2));
}



//--------------------------------------------------------------
void testApp::draw(){

	
	// what do we scale to?
	
	float scale = ofGetHeight()/frame.getHeight();
	
	ofSetHexColor(0xFFFFFF);
	glPushMatrix();
	{
		glScalef(scale, scale, 1);
		// scale to the size of the video
		scale = (float)frame.getWidth() / (float)videoFeed.getWidth();
		float xx = ((float)videoFeed.getWidth() * (float)frame.getHeight()) / (float)frame.getWidth();
		float yOff = (xx - videoFeed.getHeight())/2.f;
		glPushMatrix();
		{
			glScalef(scale, scale, 1);
			glTranslatef(0, yOff, 0);
			dots.begin();
			ofClear(0,0,0,0);
			ofSetHexColor(0xFFFFFF);
			carousel.draw();
			dots.end();
		
			drawOverlays();
			if(drawDebug) {
				jumpDetector.drawDebug();
			}
		}
		glPopMatrix();
			
			
			
		glColor3f(1,1,1);
		frame.draw(0, 0);
	}
	glPopMatrix();
	
	
	string st = "";
	switch(state) {
		case WAITING_FOR_PERSON:
			st = "WAITING";
			break;
		case READY_WITH_PERSON:
			st = "READY!";
			break;
		case RECORDING:
			st = "RECORDING";
			break;
		case WAITING_FOR_PERSON_TO_GO:
			st = "THANKS, BYE";
			break;
	}

	if(drawDebug) {
		ofSetHexColor(0xDD33EE);
		debugFont.drawString(st, 20, ofGetHeight()-20);
	}

	if(gui.isOn()) {
		gui.draw();
	}
}


void testApp::drawOverlays() {
	jumpDetector.draw(carousel.amountOfVideoFeedShowing());
}

