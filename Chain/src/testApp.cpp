

#include "testApp.h"
#include "ofxSimpleGuiToo.h"
#include "RamVideo.h"





void testApp::setupGraphics() {
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	ofEnableNormalizedTexCoords();


	frame.loadImage("frame.png");
	progressBar.loadImage("bar.png");
	
	move.loadImage("move.png");
	next.loadImage("next.png");
	one.loadImage("1.png");
	two.loadImage("2.png");
	three.loadImage("3.png");
	ready.loadImage("getready.png");
	
	move.setAnchorPercent(0.5, 1);
	next.setAnchorPercent(0.5, 1);
	one.setAnchorPercent(0.5, 0.5);
	two.setAnchorPercent(0.5, 0.5);
	three.setAnchorPercent(0.5, 0.5);
	ready.setAnchorPercent(0.5, 1);
}


//--------------------------------------------------------------
void testApp::setup(){
	HOLD_COUNT = 200;
	setupGraphics();
	setupVision();
	presenceDetector.setup();
	

	lastTimeFinishedRecording = -1000;


	video = new RamVideo();
	video->setup(480, 640, MAX_VIDEO_LENGTH);

	triggerDepth = 100;
	drawDebug = false;
	
	
	minRecordingInterval = 4;
	
	carousel.setVideoFeed(&videoFeed);
	
	
	gui.addToggle("Draw Debug", drawDebug);
	
	gui.addSlider("Count down time", HOLD_COUNT, 80, 350);
	gui.addSlider("Min Time between recordings", minRecordingInterval, 2, 20);
	gui.addSlider("Carousel video duration", carousel.frameDuration, 10, 200);
	gui.addSlider("Carousel inactivity delay", carouselDelay, 0, 5);
	gui.addSlider("Slide time pct", carousel.slideTime, 0.05, 1);
	gui.addTitle("Carousel");
	gui.addSlider("Overlap", carousel.overlap, 0, 260);
	gui.addTitle("Triggers");
	gui.addSlider("Triggers Horizontal", presenceDetector.triggersX, 0, VISION_HEIGHT);
	gui.addSlider("Triggers Vertical", presenceDetector.triggersY, 0, VISION_WIDTH);
	gui.addSlider("Triggers Radius", presenceDetector.triggerRadius, 0, 100);
	gui.addSlider("Trigger Depth", farThreshold, 0, 255);
	gui.addToggle("save all videos", saveAllVideos);
	gui.addToggle("clear all videos", clearAllVideos);
	dots.setup(480, 640);
	gui.setAlignRight(true);
	gui.setAutoSave(true);
	gui.loadFromXML();
	
	recording = false;
	clearAllVideos = false;
	saveAllVideos = false;
	
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
	if(clearAllVideos) {
		carousel.clearVideos();
		clearAllVideos = false;
	}
	if(saveAllVideos) {
		carousel.saveVideos();
		saveAllVideos = false;
	}
	ofBackground(0);

	carousel.checkForSize();
	
	doVision();
	doCompositing();
	activityMonitor.update(threshImg);
	presenceDetector.update(threshImg, false);// mouseIsDown);
	
	
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
		
		if(!presenceDetector.present()) {
			holdCount = 0;
		}

		if(!recording && presenceDetector.present() && carousel.isOnVideoFeed()) {
			holdCount++;
			if(holdCount>HOLD_COUNT) {
				video->clear();
				recording = true;
			}
		}

		// if we're still recording, but the user has lost touch,
		// stop recording
		if(recording && !presenceDetector.eitherHandPresent() && video->getLength()>MIN_VIDEO_LENGTH) {
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
	}
	ofSetWindowTitle(ofToString(ofGetFrameRate(), 2));
}








//--------------------------------------------------------------
void testApp::draw(){
	
	ofBackground(150, 150, 150);
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
			carousel.draw();
			dots.end();
			//videoFeed.draw(0, 0);
			drawOverlays();
		}
		glPopMatrix();
	
	
	
		glColor3f(1,1,1);
		frame.draw(0, 0);
	}
	glPopMatrix();
	
	
	
	
	if(gui.isOn()) {
		gui.draw();
	}
}








void testApp::drawProgressBar(float v) {
	ofSetHexColor(0xFFFFFF);

	float yOff = 300;
	float y0 = yOff;
	float y1 = y0 + progressBar.getHeight();
	float c = videoFeed.getWidth()/2;
	float p = ofMap(v, 0, 1, 0, progressBar.getWidth()/2);
	float n = ofMap(v, 0, 1, 0.1, 0.5);
	float w = progressBar.getWidth();
	float h = progressBar.getHeight();
	progressBar.bind();
	
	//ofDisableNormalizedTexCoords();
	glBegin(GL_TRIANGLE_STRIP);
	
	glTexCoord2f(0, 0);
	glVertex2f(c-p, y0);
	
	glTexCoord2f(0, 1);
	glVertex2f(c-p, y1);
	
	glTexCoord2f(n, 0);
	glVertex2f(c, y0);
	
	glTexCoord2f(n, 1);
	glVertex2f(c, y1);
	
	
	
	
	
	glTexCoord2f(0, 0);
	glVertex2f(c+p, y0);
	
	
	
	glTexCoord2f(0, 1);
	glVertex2f(c+p, y1);
	
	
	
	glEnd();
	progressBar.unbind();
//	ofRect(0, 630, v*480, 10);
}

float countdownFade(float f) {
	float fadeInPoint = 0.15;
	float fadeOutPoint = 0.7;
	int i = f;
	f -= i;
	if(f<fadeInPoint) return ofMap(f, 0, fadeInPoint, 0, 1);
	else if(f>fadeOutPoint) return ofMap(f, fadeOutPoint, 1, 1, 0);
	else return 1;
}

void testApp::drawCountdown(float v) {
	v *= 8;
	int i = v;
	float x = videoFeed.getWidth()/2;
	float y = videoFeed.getHeight()/2;
	if(i<=2) v /= 3;
	if(i>=6) {
		v -= 6.f;
		v /= 2.f;
	}
	v = countdownFade(v);
	glColor4f(1, 1, 1, v);
	glPushMatrix();
	{
		glTranslatef(x, y, 0);
		glScalef(0.5, 0.5, 1);
		if(i<=2) {
			ready.draw(0,0);
		} else if(i==3) {
			one.draw(0,0);
		} else if(i==4) {
			two.draw(0,0);
		} else if(i==5) {
			three.draw(0,0);
		} else if(i>=6) {
			move.draw(0,0);
		}
	}
	glPopMatrix();
}
void testApp::drawOverlays() {
	if(recording) {
		drawProgressBar(ofMap(video->getLength(), 0, MAX_VIDEO_LENGTH, 0, 1));
		
	} else if(holdCount>0 && holdCount<HOLD_COUNT) {
		drawCountdown((float)holdCount/(float)HOLD_COUNT);
/*		if(holdCount<HOLD_COUNT/3) {
			ofSetHexColor(0xFF0000);
			ofDrawBitmapString("READY!!!", 20, 20);
		} else if(holdCount<2*HOLD_COUNT/3) {
			ofSetHexColor(0xFFCC00);
			ofDrawBitmapString("STEADY!!!", 20, 20);
			
		} else {
			ofSetHexColor(0x00FF00);
			ofDrawBitmapString("GO!!!", 20, 20);
		}
*/
	}
	
	presenceDetector.draw(carousel.amountOfVideoFeedShowing());

}

