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
#include "animation.h"

class Carousel {
public:

	float overlap;
	
	deque<Clip> clips;	
	Clip videoFeed;

	int frameNum;
	bool paused;
	
	int prevVideo;
	int currVideo;
	int nextVideo;
	int nextNextVideo;
	
	int frameDuration;
	float slideTime;
	

	
	
	Carousel() {
		paused = false;
		overlap = 0;

		nextNextVideo = -1;
		prevVideo = -1;
		currVideo = 0;
		nextVideo = 0;
		frameNum = 0;
		slideTime = 0.75;
		frameDuration = 100;
		whatNextVideoWas = -1;
	}
	
	
	void setVideoFeed(ofTexture *vid) {
		videoFeed.setVideoFeed(vid);
	}
	
	int whatNextVideoWas;
	int nextVideoToVideoFeedStartTime;
	
	void scrollToVideoFeed() {
		
		
		if(nextVideo!=-1) {
			whatNextVideoWas = nextVideo;
			nextVideoToVideoFeedStartTime = ofGetElapsedTimef();
			nextVideo = -1;
			printf("Scrolling to video feed\n");
		}
	}
	
	void replaceVideoFeedWithVideo(RamVideo *vid) {

		clips.push_front(Clip());
		clips.front().video = vid;
		currVideo = 0;
		nextVideo = 1;
		if(clips.size()==1) nextVideo = 1;
		
		printf("Added video\n");
		
	}
	
	void addVideo(RamVideo *vid) {
		clips.push_back(Clip());
		clips.back().video = vid;
	}
	
	
	// this will return whether the video feed is present onscreen.
	// 0 if not at all, 1 if filling the screen, and numbers in between
	// to denote that its sliding on or off.
	float amountOfVideoFeedShowing() {
		if(clips.size()==0) return 1;
		
		if(nextVideo==-1) {
			return getOffset();
		} else if(currVideo==-1) {
			return 1.f - getOffset();
		} else {
			return 0;
		}
	}
	
	void autoScroll() {
		paused = false;
		printf("Autoscrolling\n");
		
		// make sure the next frame isn't the video.
		if(nextVideo==-1) {
			nextVideo = currVideo+1;
			if(nextVideo>=clips.size()) {
				nextVideo = 0;
			}
		}
	}
	
	
	bool isScrolling() {
		return !paused;
	}
	
	float getOffset() {
		return ofMap(frameNum, slideTime*frameDuration, frameDuration, 0, 1, true);
	}
	
	void draw() {
		
		
		if(clips.size()==0) {
			videoFeed.draw();
		} else if(clips.size()==1 && nextVideo!=-1 &&currVideo!=-1) {
			clips[0].draw();
		} else {

			if(!paused) {
				frameNum++;
			}
			
				
			if(frameNum>frameDuration) {
				// move to next frame
				prevVideo = currVideo;
				currVideo = nextVideo;
				nextVideo = nextVideo+1;

				if(nextVideo>=clips.size()) {
					nextVideo = 0;
				}
				nextNextVideo = nextVideo+1;
				if(nextNextVideo>=clips.size()) {
					nextNextVideo = 0;
				}
				
				if(currVideo==-1) {
					paused = true;
				}
				
				frameNum = 0;
			}
				
			float offset = getOffset();
			
			// slidey-slidey
			offset = tricks::animation::easeInOutQuad(offset);
			
			float currVideoX = ofMap(offset, 0, 1, 0, overlap-480);
			float nextVideoX = ofMap(offset, 0, 1, 480-overlap, 0);
			float prevVideoX = ofMap(offset, 0, 1, overlap-480, overlap-480*2);
			float nextNextVideoX = ofMap(offset, 0, 1, 480*2-overlap, 480-overlap);
			if(prevVideo!=-1 && prevVideoX>-480) {
				clips[prevVideo].draw(prevVideoX, 0);
			}
			
			if(currVideo>=0 && currVideo<clips.size()) {
				clips[currVideo].draw(currVideoX, 0);
			} else if(currVideo==-1) {
				videoFeed.draw(currVideoX, 0);
			}
			
			if(nextVideo>=0 && nextVideo<clips.size()) {
				clips[nextVideo].draw(nextVideoX, 0);
			} else if(nextVideo==-1) {
				
				// if we've just flipped into videoFeed mode, we want to fade it in
				// incase we're in the middle of a transition.
				float a = ofMap(ofGetElapsedTimef() - nextVideoToVideoFeedStartTime,
								0, 0.4, 0, 1, true);
				glColor4f(1,1,1,1.f-a);
				if(a<1 && whatNextVideoWas!=-1) clips[whatNextVideoWas].draw(nextVideoX, 0);
				glColor4f(1,1,1,a);
				videoFeed.draw(nextVideoX, 0);
			}
			
			if(nextNextVideo!=-1 && nextNextVideoX<480) {
				clips[nextNextVideo].draw(nextNextVideoX, 0);
			}
			
			
		}
	}
};

#endif
