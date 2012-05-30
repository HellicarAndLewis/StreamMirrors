//
//  PresenceDetector.h
//  Chain
//
//  Created by Marek Bereza on 23/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

class PresenceDetector {
public:
	float triggersY;
	float triggersX;
	float triggerRadius;
	
	PresenceDetector() {
		triggersY = 120;
		triggersX = 120;
		triggerRadius = 40;
		
		lastLeftDetected = leftDetected = false;
		lastRightDetected = rightDetected = false;
		
	}
	
	
	
	
	void update(ofxCvGrayscaleImage &threshImg, bool force) {
		if(force) {
			forceOn();
		} else {
			detectPresence(leftDetected, rightDetected, threshImg);
			
			leftSmoothed = leftDetected || lastLeftDetected;
			rightSmoothed = rightDetected || lastRightDetected;
			
			lastLeftDetected = leftDetected;
			lastRightDetected = rightDetected;

		}
	}

	bool leftSmoothed;
	bool rightSmoothed;
	
	bool leftDetected;
	bool rightDetected;
	bool lastLeftDetected;
	bool lastRightDetected;
	
	
	void draw(float alpha) {
		if(alpha==0) return;
		ofNoFill();
		glLineWidth(5);
		
		if(leftSmoothed) {

			glColor4f(0,1,0, alpha);
		} else {
			glColor4f(1,0,0.6,alpha);
		}
		ofCircle(240 - triggersX, triggersY, triggerRadius);
		if(rightSmoothed) {
			glColor4f(0,1,0, alpha);
		} else {
			glColor4f(1,0,0.6,alpha);
		}
		ofCircle(240 + triggersX, triggersY, triggerRadius);
		glLineWidth(1);
		ofFill();
	}
	
	void forceOn() {
		leftSmoothed = true;
		rightSmoothed = true;
		leftDetected = true;
		rightDetected = true;
	}
	
	bool present() {
		return leftSmoothed && rightSmoothed;
	}
	
	bool eitherHandPresent() {
		return leftSmoothed || rightSmoothed;
	}
	
private:
	void detectPresence(bool &left, bool &right, ofxCvGrayscaleImage &threshImg) {
		ofRectangle r;
		
		r.setFromCenter(threshImg.getWidth()/2 - triggersX, triggersY, triggerRadius*2, triggerRadius*2);
		int leftValue = threshImg.countNonZeroInRegion(r.x, r.y, r.width, r.height);
		
		
		r.setFromCenter(threshImg.getWidth()/2 + triggersX, triggersY, triggerRadius*2, triggerRadius*2);
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
	}
};