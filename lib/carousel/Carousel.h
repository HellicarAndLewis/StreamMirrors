//
//  Carousel.h
//  opencvExample
//
//  Created by Marek Bereza on 21/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef opencvExample_Carousel_h
#define opencvExample_Carousel_h
#include "Clip.h"
#include "RamVideo.h"
#include "Tween.h"

class Carousel {
public:
	int xOffset;
	float zoom;
	float overlap;
	float speed;
	bool active;

	bool scrollingToEnd;
	
	vector<Clip> clips;
	Tween spaceTween;
	
	Carousel() {
		scrollingToEnd = false;
		speed = 20;
		xOffset = 0;
		zoom = 1;
		overlap = 20;
		active = false;
		spaceTween = 0;
	}
	void addToEnd(RamVideo *vid) {
		spaceTween = 0;
		clips.push_back(Clip());
		clips.back().video = vid;
		clips.back().width = vid->width;
		clips.back().height = vid->height;
		clips.back().y = 0;
		if(clips.size()>1) {
			clips.back().x = clips[clips.size()-2].x + clips[clips.size()-2].width - overlap;
		}

	}
	
	
	bool rectOverlapsRect(const ofRectangle &a, const ofRectangle &b) {
		if(a.x>b.x+b.width) return false;
		if(a.x+a.width<b.x) return false;
		if(a.y>b.y+b.height) return false;
		if(a.y+a.height<b.y) return false;
		return true;
	}
	
	int getWidth() {
		float s = clips.size() + getEndSpace();
		if(clips.size()==0) return 1;
		return s*(clips[0].width - overlap);
	}
	
	bool isActive() {
		return active;
	}
	
	void activate() {
		active = true;
		scrollingToEnd = false;
		spaceTween.tween(0, 1);

	}
	
	float getEndSpace() {
		return spaceTween.getValue();
	}
	
	void createSpaceAndGoToEnd() {
		spaceTween.tween(1, 1);
		active = false;
		scrollingToEnd = true;
	}
	
	void draw() {
		if(scrollingToEnd) {
			xOffset += (getWidth() - xOffset)/3.f;
		} else {
			xOffset += speed;
		}
		
		while(xOffset<0) xOffset += getWidth();
		//printf("Offset: %d\n", xOffset);
		xOffset %= getWidth();
		glPushMatrix();
		
		glScalef(zoom, zoom, zoom);
		glTranslatef(-xOffset, 0, 0);
		ofRectangle screen = ofRectangle(xOffset, 0, (float) ofGetWidth()  / (zoom), 
									(float) ofGetHeight() / zoom);
		

			
		ofSetHexColor(0xFFFFFF);
		
		for(int i = 0; i < clips.size(); i++) {
			int clipX = clips[i].x;
			clipX %= getWidth();
			clips[i].x = clipX;
			
			if(rectOverlapsRect(clips[i], screen)) {
				clips[i].draw();
			} else {
				ofRectangle r = clips[i];
				r.x += getWidth();
				if(rectOverlapsRect(r, screen)) {
					
					clips[i].draw(r);
				}
			}
		}
		ofFill();
		glPopMatrix();
	}
};

#endif
