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
	ofImage hurdle;
	
	JumpDetector() {
		
		isPersonReady = false;
		isPersonJumping = false;
	}
	
	void setup() {
		hurdle.loadImage("hurdle.png");
		hurdle.setAnchorPercent(0.5,0.5);
	}
	float triggerHeight;
	float nearThreshold;
	
	void update(ofxCvGrayscaleImage &threshImg, ofxCvGrayscaleImage &depthImg) {
		this->depthImg = &depthImg;
		int minArea = 100*50;
		int maxArea = 640 * 480;
		int nConsidered = 5;
		contours.findContours(threshImg, minArea, maxArea, nConsidered, false);
		isPersonReady = detectPersonReady();
		isPersonJumping = detectPersonJumping();
	}
	
	bool personReady() {
		return isPersonReady;
	}
	
	bool personJumping() {
		return isPersonJumping;
	}
		
	bool personPresent() {
		return personReady() || personJumping();
	}
	
	void draw(float alpha) {
		glColor4f(1, 1, 1, alpha);
		glPushMatrix();
		glTranslatef(240, triggerHeight,0);
		float scale = 480.f/1080.f;
		glScalef(scale, scale, scale);
		hurdle.draw(0, 0);
		glPopMatrix();
		//ofLine(0, triggerHeight, 480, triggerHeight);
	}
	
	void drawDebug() {
		contours.draw(0,0);
	}
	
	
	
private:
	bool isPersonReady;
	bool isPersonJumping;
	ofxCvGrayscaleImage *depthImg;
	
	int blobDepth(ofxCvBlob &b) {
		
		int largest = 0;
		//int secondLargest = 0;
		//int thirdLargest = 0;
		unsigned char *depth = depthImg->getPixels();
		
		
		
		// find (roughly) the third brightest pixel 
		for(int j = b.boundingRect.y; j < b.boundingRect.y+b.boundingRect.height; j+=3) {
			int yOffset = j;
			yOffset *= depthImg->getWidth();
			int endX = b.boundingRect.x+b.boundingRect.width;
			
			for(int i = b.boundingRect.x; i < endX; i+=3) {
				int offset = i + yOffset;
				if(depth[offset]>largest) {
					// shuffle
					//thirdLargest = secondLargest;
					//secondLargest = largest;
					largest = depth[offset];
				}
			}
		}
		return largest;
	}
	
	bool blobWithinThreshold(ofxCvBlob &b) {
		return blobDepth(b)>nearThreshold;
	}
	
	bool detectPersonReady() {
		for(int i = 0; i < contours.blobs.size(); i++) {
			ofRectangle &r = contours.blobs[i].boundingRect;
			
			// we need the blob to be of a reasonable height
			if(r.height>640.f/4.f) {
				// and also, the blob needs to be crossing the line
				if(r.y<triggerHeight && r.y+r.height>triggerHeight) {

					// and also, the blob needs to be close enough to the kinect
					if(blobWithinThreshold(contours.blobs[i])) {
						return true;
					}
				}
			}
		}
		return false;
	}
	
	bool detectPersonJumping() {
		for(int i = 0; i < contours.blobs.size(); i++) {
			ofRectangle &r = contours.blobs[i].boundingRect;
			
			// we need the blob to be of a reasonable height
			if(r.height>640.f/4.f) {
				// and also, the blob needs to above the line  
				if(r.y+r.height<triggerHeight) {
					// and also, the blob needs to be close enough to the kinect
					if(blobWithinThreshold(contours.blobs[i])) {
						return true;
					}
				}
			}
		}
		
		return false;
	}

	ofxCvContourFinder contours;
	
};

#endif
