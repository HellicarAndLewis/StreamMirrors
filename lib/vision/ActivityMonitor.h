//
//  ActivityMonitor.h
//  Chain
//
//  Created by Marek Bereza on 23/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofxOpenCv.h"

class ActivityMonitor {
public:
	ActivityMonitor() {
		currPresence = false;
		prevPresence = false;
		timeOfLastActivity = 0;
	}
	float getTimeSinceLastMove() {
		return ofGetElapsedTimef() - timeOfLastActivity;
	}
	void update(ofxCvGrayscaleImage &image) {
		prevPresence = currPresence;
		int numPix = image.countNonZeroInRegion(0, 0, image.getWidth(), image.getHeight());
		if(numPix>image.getHeight()*2) {
			currPresence = true;
		} else {
			currPresence = false;
		}
		if(getPresence()) {
			timeOfLastActivity = ofGetElapsedTimef();
		}
	}
	
	bool getPresence() {
		return currPresence && prevPresence;
	}
	float timeOfLastActivity;
	bool currPresence;
	bool prevPresence;
};

