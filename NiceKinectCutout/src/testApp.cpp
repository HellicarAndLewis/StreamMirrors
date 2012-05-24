

#include "testApp.h"
#include "ofxSimpleGuiToo.h"
int dilations = 4;
int blurSize = 1;
int blurs = 0;
//--------------------------------------------------------------
void testApp::setup(){
	ofSetFrameRate(25);
	ofSetVerticalSync(true);
	
	
	cam.setup();
	int VISION_WIDTH = cam.getWidth();
	int VISION_HEIGHT = cam.getHeight();
	
//	gui.addContent("Color", colorImg);
//	gui.addContent("Depth", depthImg);
//	gui.addContent("Thresh", threshImg);
	gui.addSlider("dilations", dilations, 0, 10);
	gui.addSlider("blurs", blurs, 0, 10);
	gui.addSlider("blur size", blurSize, 1, 10);
	
	gui.addSlider("threshold", threshold, 0, 255);
	gui.setAutoSave(true);
	gui.loadFromXML();
	
	
	
	colorImg.allocate(VISION_WIDTH, VISION_HEIGHT);
	depthImg.allocate(VISION_WIDTH, VISION_HEIGHT);
	threshImg.allocate(VISION_WIDTH, VISION_HEIGHT);
	
	
	
}





//--------------------------------------------------------------
void testApp::update(){
	ofBackground(0);

	
	cam.update();
	unsigned char *rgb = cam.getPixels();
	unsigned char *depth = cam.getDepthPixels();
	if(rgb!=NULL) {
		colorImg.setFromPixels(rgb, cam.getWidth(), cam.getHeight());
	}
	if(depth!=NULL) {
		depthImg.setFromPixels(depth, cam.getWidth(), cam.getHeight());
	}
	
	
	threshImg = depthImg;
	threshImg.threshold(threshold);
	for(int i = 0; i < dilations; i++) {
		threshImg.dilate_3x3();
	}
	for(int i = 0; i < blurs; i++) {
		threshImg.blur(blurSize*2+1);
	}
}



//--------------------------------------------------------------
void testApp::draw(){

	// draw the incoming, the grayscale, the bg and the thresholded difference
	ofSetHexColor(0xffffff);
	

	
	ofEnableAlphaBlending();
	

	threshImg.draw(0, 0);
	ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
	colorImg.draw(0, 0);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);

	
	
	if(gui.isOn()) {
		gui.draw();
	}
}


void testApp::keyPressed(int key) {
	if(key==' ') {
		gui.toggleDraw();
	}
}