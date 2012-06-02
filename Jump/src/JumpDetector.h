//
//  JumpDetector.h
//  Jump
//
//  Created by Marek Bereza on 25/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Jump_JumpDetector_h
#define Jump_JumpDetector_h
#include "ofMain.h"
#include "ofxOpenCv.h"

class JumpDetector {
public:	
	JumpDetector() {
		isJumping = false;
	}
	void setup() {
		debugFont.loadFont("Impact.ttf", 60);
	}
	bool isJumping;
	float triggerHeight;
	
	void update(ofxCvGrayscaleImage &img) {
		// jumping is true if there is pixels above the line, and none below (or not very many)
		
		int aboveLine = img.countNonZeroInRegion(0, 0, 480, triggerHeight);
		int belowLine = img.countNonZeroInRegion(0, triggerHeight, 480, 640 - triggerHeight);
		isJumping = aboveLine>1000 && belowLine<500;
		int minArea = 100*50;
		int maxArea = 640 * 480;
		int nConsidered = 5;
		contours.findContours(img, minArea, maxArea, nConsidered, false);
	}
	
	
	bool jumping() {
		return isJumping;
	}
	
	void draw(float alpha) {
		glColor4f(1, 0, 0, alpha);
		
		ofLine(0, triggerHeight, 480, triggerHeight);
	}
	
	void drawDebug() {
		contours.draw(0,0);
	}
	ofTrueTypeFont debugFont;
	ofxCvContourFinder contours;
	
};

#endif
