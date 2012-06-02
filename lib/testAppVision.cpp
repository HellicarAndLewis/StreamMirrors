/**
 *  testAppVision.cpp
 *
 *  Created by Marek Bereza on 02/06/2012.
 */

#include "testApp.h"
#include "ofxSimpleGuiToo.h"


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
	
	learnBackground = true;
	
	rotateClockwise = false;
	flipX = true;
	dilations = 0;
	blurs = 0;
	blurSize = 1;
	
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
	threshImg.threshold(farThreshold);
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
