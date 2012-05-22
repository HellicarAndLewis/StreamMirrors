#include "testApp.h"
#include "ofxSimpleGuiToo.h"
#include "RamVideo.h"
#define HOLD_COUNT 80
#define MAX_VIDEO_LENGTH 30
bool mouseIsDown = false;
//--------------------------------------------------------------
void testApp::setup(){
	
	lastTimeFinishedRecording = -1000;
	ofSetFrameRate(25);
	ofSetVerticalSync(true);
	
	video = new RamVideo();
	video->setup(480, 640, MAX_VIDEO_LENGTH);

	rotateClockwise = false;
	flipX = true;
	
	triggersY = 120;
	triggersX = 120;
	triggerRadius = 40;
	triggerDepth = 100;
	drawDebug = false;
	
	
	lastLeftDetected = leftDetected = false;
	lastRightDetected = rightDetected = false;
	
	cam.setup();
	VISION_WIDTH = cam.getWidth();
	VISION_HEIGHT = cam.getHeight();
	
	gui.addToggle("Draw Debug", drawDebug);
	
	gui.addToggle("Rotate Clockwise", rotateClockwise);
	gui.addToggle("Flip X", flipX);
	gui.addContent("Color", colorImg);
	gui.addContent("Depth", depthImg);

	
	gui.addSlider("Triggers Horizontal", triggersX, 0, VISION_HEIGHT);
	gui.addSlider("Triggers Vertical", triggersY, 0, VISION_WIDTH);
	gui.addSlider("Triggers Radius", triggerRadius, 0, 100);
	gui.addSlider("Trigger Depth", triggerDepth, 0, 255);
	gui.setAutoSave(true);
	gui.loadFromXML();
	
	recording = false;
	
	
	colorImg.allocate(VISION_HEIGHT, VISION_WIDTH);
	depthImg.allocate(VISION_HEIGHT, VISION_WIDTH);
	threshImg.allocate(VISION_HEIGHT, VISION_WIDTH);
	rgbRot = new unsigned char[VISION_WIDTH * VISION_HEIGHT * 3];
	//ofSetOrientation(OF_ORIENTATION_90_LEFT);
	
	
}


void testApp::finishedRecording() {
	recording = false;
	holdCount = 0;
	videos.push_back(video);
	video = new RamVideo();
	video->setup(480, 640, MAX_VIDEO_LENGTH);
	carousel.addToEnd(videos.back());
	lastTimeFinishedRecording = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(0);

    bool bNewFrame = false;
	
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
	
	
	threshImg = depthImg;
	threshImg.threshold(triggerDepth);
	detectPresence(leftDetected, rightDetected);
	
	threshImg.dilate_3x3();
	threshImg.dilate_3x3();
	threshImg.blur();
	threshImg.blur();

	
	bool leftSmoothed = leftDetected || lastLeftDetected;
	bool rightSmoothed = rightDetected || lastRightDetected;
	
	lastLeftDetected = leftDetected;
	lastRightDetected = rightDetected;
	if(mouseIsDown) {
		leftSmoothed = true;
		rightSmoothed = true;
		leftDetected = true;
		rightDetected = true;
	}
	
	if(!leftSmoothed || !rightSmoothed) {
		holdCount = 0;
	}
	if(!recording && leftSmoothed && rightSmoothed) {
		holdCount++;
		if(holdCount>HOLD_COUNT) {
			video->clear();
			recording = true;
		}
	}

	// if we're still recording, but the user has lost touch,
	// stop recording
	if(recording && !leftSmoothed && !rightSmoothed) {
		finishedRecording();
	}
	
	if(recording) {
		bool stillCanRecord = video->record(colorImg.getPixels());
		if(!stillCanRecord) {
			finishedRecording();
		}
	}
}


void testApp::detectPresence(bool &left, bool &right) {
	
	ofRectangle r;
	
	r.setFromCenter(depthImg.getWidth()/2 - triggersX, triggersY, triggerRadius*2, triggerRadius*2);
	int leftValue = threshImg.countNonZeroInRegion(r.x, r.y, r.width, r.height);
	
	
	r.setFromCenter(depthImg.getWidth()/2 + triggersX, triggersY, triggerRadius*2, triggerRadius*2);
	int rightValue = threshImg.countNonZeroInRegion(r.x, r.y, r.width, r.height);
	
	int totalPixels = triggerRadius * 2 * 2 * triggerRadius;
	
	float fillPercent = 0.1;
	
	if((float)leftValue/(float)totalPixels>fillPercent) {
		left = true;
	} else {
		left = false;
	}
	
	if((float)rightValue/(float)totalPixels>fillPercent) {
		right = true;
	} else {
		right = false;
	}
	
	ofSetWindowTitle(ofToString(ofGetFrameRate(), 2));

}

//--------------------------------------------------------------
void testApp::draw(){

	// draw the incoming, the grayscale, the bg and the thresholded difference
	ofSetHexColor(0xffffff);
	

	
	// what do we scale to?
	float scale = (float)ofGetHeight()/(float)colorImg.getHeight();
	float newWidth = scale * (float) colorImg.getWidth();
	if(newWidth>ofGetWidth()) {
		scale = (float) ofGetWidth()/(float) colorImg.getWidth();
	}
	
	
	ofEnableAlphaBlending();
	
	glPushMatrix();
	{
		//glTranslatef(ofGetWidth()/2, ofGetHeight()/2, 0);
		//glScalef(scale, scale, 1);
		//glTranslatef(-colorImg.getWidth()/2, -colorImg.getHeight()/2, 0);
		
		if(drawDebug) {
			depthImg.draw(0, 0);
			
		} else {
			threshImg.draw(0, 0);
			ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
			colorImg.draw(0, 0);
			ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		}
		
		ofSetHexColor(0xFFFFFF);

		if(!recording) {
			carousel.draw();
		}
		
		drawOverlays();

	}
	glPopMatrix();
	
	
	
	if(gui.isOn()) {
		gui.draw();
	}
}


void testApp::drawOverlays() {
	if(recording) {
		ofDrawBitmapString("Recording", 20, 20);
		ofSetHexColor(0xFF0000);
		ofRect(0, 630, ofMap(video->getLength(), 0, MAX_VIDEO_LENGTH, 0, 480), 10);
	} else if(holdCount>0 && holdCount<HOLD_COUNT) {
		if(holdCount<HOLD_COUNT/3) {
			ofSetHexColor(0xFF0000);
			ofDrawBitmapString("READY!!!", 20, 20);
		} else if(holdCount<2*HOLD_COUNT/3) {
			ofSetHexColor(0xFFCC00);
			ofDrawBitmapString("STEADY!!!", 20, 20);
			
		} else {
			ofSetHexColor(0x00FF00);
			ofDrawBitmapString("GO!!!", 20, 20);
		}
	}
	
	
	drawTriggers();
}
void testApp::drawTriggers() {
	ofNoFill();
	glLineWidth(5);
	
	if(leftDetected) {
		ofSetHexColor(0x00FF00);
	} else {
		ofSetHexColor(0xFF00BB);
	}
	ofCircle(colorImg.getWidth()/2 - triggersX, triggersY, triggerRadius);
	if(rightDetected) {
		ofSetHexColor(0x00FF00);
	} else {
		ofSetHexColor(0xFF00BB);
	}
	ofCircle(colorImg.getWidth()/2+triggersX, triggersY, triggerRadius);
	glLineWidth(1);
	ofFill();

}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key==' ') {
		gui.toggleDraw();
	} else if(key=='f') {
		ofToggleFullscreen();
	} else if(key=='s') {
		saveEverything();
	} else if(key=='l') {
		loadEverything();
	} else if(key=='d') {
		drawDebug ^= true;
	}
}

void testApp::loadEverything() {
	for(int i = 0; i < videos.size(); i++) {
		delete videos[i];
	}
	videos.clear();
	
	ofDirectory dir;
	dir.open("vids");
	if(!dir.exists()) return;

	dir.sort();	
	dir.listDir();
	dir.sort();
	for(int i = 0; i < dir.size(); i++) {
		videos.push_back(new RamVideo());
		videos.back()->setup(480, 640, MAX_VIDEO_LENGTH);
		videos.back()->load(dir[i].path());
		carousel.addToEnd(videos.back());
	}
}

#include <sys/stat.h>

void testApp::saveEverything() {
	ofDirectory dir;
	dir.open("vids");
	if(!dir.exists()) {
		dir.create();
		printf("Creating directory\n");
	} else {
		printf("Clearing dir\n");
		dir.listDir();
		for(int i = 0; i < dir.size(); i++) {
			dir[i].remove(true);
		}
	}
	
	for(int i = 0; i < videos.size(); i++) {
		videos[i]->save("vids/"+ofToString(i));
	}
}
//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	carousel.speed = (x - ofGetWidth()/2)/5;
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	mouseIsDown = true;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	mouseIsDown = false;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
