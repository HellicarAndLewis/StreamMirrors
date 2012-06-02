

#include "testApp.h"
#include "ofxSimpleGuiToo.h"
#include "RamVideo.h"





void testApp::setupGraphics() {
	ofSetFrameRate(25);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
}

void testApp::setupVision() {
	cam.setup();
	VISION_WIDTH = cam.getWidth();
	VISION_HEIGHT = cam.getHeight();
	
	videoFeedData = new unsigned char[VISION_WIDTH*VISION_HEIGHT*4];
	videoFeed.allocate(VISION_HEIGHT, VISION_WIDTH, GL_RGBA);
	colorImg.allocate(VISION_HEIGHT, VISION_WIDTH);
	depthImg.allocate(VISION_HEIGHT, VISION_WIDTH);
	threshImg.allocate(VISION_HEIGHT, VISION_WIDTH);
	bgImg.allocate(VISION_HEIGHT, VISION_WIDTH);
	rgbRot = new unsigned char[VISION_WIDTH * VISION_HEIGHT * 3];

	bgHysteresis = 6;
	jumpDetector.setup();
}

//--------------------------------------------------------------
void testApp::setup(){

	setupGraphics();
	setupVision();
	fbo.allocate(VISION_HEIGHT, VISION_WIDTH, GL_RGBA);
	syphon.setName("openframeworks");
	learnBackground = true;

	lastTimeFinishedRecording = -1000;


	video = new RamVideo();
	video->setup(480, 640, MAX_VIDEO_LENGTH);

	rotateClockwise = false;
	flipX = true;
	triggerDepth = 100;
	drawDebug = false;
	dilations = 0;
	blurs = 0;
	blurSize = 1;
	
	minRecordingInterval = 4;
	
	carousel.setVideoFeed(&videoFeed);
	
	
	gui.addToggle("Draw Debug", drawDebug);
	gui.addToggle("Rotate Clockwise", rotateClockwise);
	gui.addToggle("Flip X", flipX);
	gui.addContent("Color", colorImg);
	gui.addContent("Depth", depthImg);
	gui.addContent("Thresh", threshImg);
	gui.addToggle("Learn BG", learnBackground); 
	gui.addSlider("BG Hysteresis", bgHysteresis, 0, 20);
	
	gui.addTitle("Compositing");
	gui.addSlider("erosions", erosions, 0, 10);
	gui.addSlider("dilations", dilations, 0, 10);
	gui.addSlider("blurs", blurs, 0, 10);
	gui.addSlider("blur size", blurSize, 1, 10);
	gui.addSlider("Min Time between recordings", minRecordingInterval, 2, 20);
	gui.addSlider("Carousel video duration", carousel.frameDuration, 10, 200);
	gui.addSlider("Carousel inactivity delay", carouselDelay, 0, 5);
	gui.addSlider("Slide time pct", carousel.slideTime, 0.05, 1);
	gui.addTitle("Carousel");
	gui.addSlider("Overlap", carousel.overlap, 0, 100);
	gui.addTitle("Triggers");
	gui.addSlider("Trigger Height", jumpDetector.triggerHeight, 0, VISION_WIDTH);
	gui.addSlider("Trigger Depth", triggerDepth, 0, 255);

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


void testApp::doVision() {
	
	// rotate the pixels from the camera
	cam.update();
	unsigned char *rgb = cam.getPixels();
	unsigned char *depth = cam.getDepthPixels();
	if(rgb!=NULL) {
		rotateRgb90(rgb, rgbRot, rotateClockwise, flipX);
		colorImg.setFromPixels(rgbRot, VISION_HEIGHT, VISION_WIDTH);
	}
	if(depth!=NULL) {
		rotate90(depth, rgbRot, rotateClockwise, flipX);
		depthImg.setFromPixels(rgbRot, VISION_HEIGHT, VISION_WIDTH);
	}
	if(learnBackground) {
		learnBackground = false;
		bgImg = depthImg;
	}
	
	int numPix = VISION_WIDTH * VISION_HEIGHT;
	depth = depthImg.getPixels();
	unsigned char *bg = bgImg.getPixels();
	
	
	// remove any pixels that are smaller than the background
	for(int i = 0; i < numPix; i++) {
		if(depth[i]<bg[i]+bgHysteresis) {	// background hysteresis, you've got to be at least 
											// bgHysteresis away
											// from the background to count as foreground.
			depth[i] = 0;
		}
	}
	
	// do thresholding and tidying up on the depth data.
	threshImg = depthImg;
	threshImg.threshold(triggerDepth);
	for(int i = 0; i < erosions; i++) {
		threshImg.erode_3x3();
	}
	for(int i = 0; i < dilations; i++) {
		threshImg.dilate_3x3();
	}
	for(int i = 0; i < blurs; i++) {
		threshImg.blur(blurSize*2+1);
	}
}




// just alpha out the person and upload to texture
void testApp::doCompositing() {
	int numPix = VISION_WIDTH * VISION_HEIGHT;
	unsigned char *img	= colorImg.getPixels();
	unsigned char *t = threshImg.getPixels();
	for(int i = 0; i < numPix; i++) {
		videoFeedData[i*4] = (img[i*3]*t[i])/255;
		videoFeedData[i*4+1] = (img[i*3+1]*t[i])/255;
		videoFeedData[i*4+2] = (img[i*3+2]*t[i])/255;
		videoFeedData[i*4+3] = t[i];
	}
	videoFeed.loadData(videoFeedData, VISION_HEIGHT, VISION_WIDTH, GL_RGBA);
}


//--------------------------------------------------------------
void testApp::update(){
	ofBackground(255);

	
	doVision();
	doCompositing();
	activityMonitor.update(threshImg);
	jumpDetector.update(threshImg);// mouseIsDown);
	
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
	
	
	fbo.begin();
	ofClear(0,0,0,0);
	ofSetHexColor(0xFFFFFF);
	carousel.draw();
	drawOverlays();
	//jumpDetector.drawDebug();
	fbo.end();
	
	glPushMatrix();
	{
		// TODO:
		// this scales the image as big as it can go
		// but we still need to offset it so it
		// sits in the centre
		glScalef(scale, scale, 1);
	ofSetHexColor(0xFFFFFF);
		fbo.draw(0,0);
	}
	glPopMatrix();
	
	syphon.publishTexture(&fbo.getTextureReference(0));
	
	
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

