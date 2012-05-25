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
#include "ofxCvGrayscaleImage.h"

class JumpDetector {
public:	
	JumpDetector() {
		isJumping = false;
	}
	bool isJumping;
	float triggerHeight;
	void update(ofxCvGrayscaleImage &img) {
		// jumping is true if there is pixels above the line, and none below (or not very many)
		
		int aboveLine = img.countNonZeroInRegion(0, 0, 480, triggerHeight);
		int belowLine = img.countNonZeroInRegion(0, triggerHeight, 480, 640 - triggerHeight);
		isJumping = aboveLine>1000 && belowLine<500;
	}
	
	
	bool jumping() {
		return isJumping;
	}
	void draw(float alpha) {
		glColor4f(1, 0, 0, alpha);
		
		ofLine(0, triggerHeight, 480, triggerHeight);
	}
};

#endif
