

#include "testApp.h"
#include "ofxSimpleGuiToo.h"
#include "RamVideo.h"





void testApp::setupGraphics() {
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
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
	gui.setAlignRight(true);
	gui.setAutoSave(true);
	gui.loadFromXML();
	
	recording = false;
	
	
	//ofSetOrientation(OF_ORIENTATION_90_LEFT);
	
	
}


void testApp::finishedRecording() {
	recording = false;
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
}


//--------------------------------------------------------------
void testApp::update(){
	ofBackground(255);

	
	doVision();
	doCompositing();
	activityMonitor.update(threshImg); // don't know if we're using this one??
	jumpDetector.update(threshImg, depthImg);// mouseIsDown);
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
	
	/*
	// if there's been no movement for a while, (and we're not recording)
	// start spinning the carousel
	if(!carousel.isScrolling() && activityMonitor.getTimeSinceLastMove()>carouselDelay && !recording) {
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
		
		
		
		if(!recording && jumpDetector.jumping()) {
			video->clear();
			recording = true;
		}

		// if we're still recording, but the user has lost touch,
		// stop recording
		if(recording && !jumpDetector.jumping() && video->getLength()>MIN_VIDEO_LENGTH) {
			printf("Recording finished because user stepped away\n");
			finishedRecording();

		}
		
		if(recording) {
			// assemble composite
			
			bool stillCanRecord = video->record(videoFeedData);
			if(!stillCanRecord) {
				printf("Recording finished becasue it's maxed length\n");
				finishedRecording();
			}
		}
	}*/
	
	ofSetWindowTitle(ofToString(ofGetFrameRate(), 2));
}



//--------------------------------------------------------------
void testApp::draw(){

	
	// what do we scale to?
	float scale = (float)ofGetHeight()/(float)colorImg.getHeight();
	float newWidth = scale * (float) colorImg.getWidth();
	if(newWidth>ofGetWidth()) {
		scale = (float) ofGetWidth()/(float) colorImg.getWidth();
	}
	
	
	ofSetHexColor(0xFFFFFF);
	
	
	glPushMatrix();
	{
		// TODO:
		// this scales the image as big as it can go
		// but we still need to offset it so it
		// sits in the centre
		glScalef(scale, scale, 1);
	
		carousel.draw();
		drawOverlays();
		jumpDetector.drawDebug();
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
	ofSetHexColor(0xDD33EE);
	debugFont.drawString(st, 20, ofGetHeight()-20);
	jumpDetector.drawDebug();
	if(gui.isOn()) {
		gui.draw();
	}
}


void testApp::drawOverlays() {
	if(recording) {
		ofDrawBitmapString("Recording", 20, 20);
		ofSetHexColor(0xFF0000);
		ofRect(0, 630, ofMap(video->getLength(), 0, MAX_VIDEO_LENGTH, 0, 480), 10);
	}
	
	jumpDetector.draw(carousel.amountOfVideoFeedShowing());

}

