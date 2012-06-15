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


// this equates to approximately 2GB's
#define MAX_NUM_FRAMES 1500 

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
	

	RamVideo *videoBeingRemoved;
	
	Carousel() {
		init();
		
		slideTime = 0.75;
		frameDuration = 100;
		overlap = 0;
	}
	void init() {
		videoBeingRemoved = NULL;
		paused = false;
		
		
		nextNextVideo = -1;
		prevVideo = -1;
		currVideo = 0;
		nextVideo = 0;
		frameNum = 0;
		
		whatNextVideoWas = -1;
	}
	
	void setVideoFeed(ofTexture *vid) {
		videoFeed.setVideoFeed(vid);
	}
	
	int whatNextVideoWas;
	int nextVideoToVideoFeedStartTime;
	
	void autoScroll() {
		paused = false;
		printf("Autoscrolling\n");
		
		// start sliding straight away if we're not already sliding
		if(frameNum < slideTime*frameDuration) frameNum = slideTime*frameDuration;
		
		// make sure the next frame isn't the video.
		if(nextVideo==-1) {
			nextVideo = currVideo+1;
			if(nextVideo>=clips.size()) {
				nextVideo = 0;
			}
		}
	}
	
	void scrollToVideoFeed() {
		
		// start sliding straight away if we're not already sliding
		if(frameNum < slideTime*frameDuration) frameNum = slideTime*frameDuration;
		
		// if the next slide is not the video feed, change it to the video feed.
		if(nextVideo!=-1) {
			whatNextVideoWas = nextVideo;
			nextVideoToVideoFeedStartTime = ofGetElapsedTimef();
			nextVideo = -1;
			printf("Scrolling to video feed\n");
		}
	}
	
	void replaceVideoFeedWithVideo(RamVideo *vid) {

		clips.push_back(Clip());
		clips.back().video = vid;
		currVideo = clips.size()-1;
		nextVideo = 0;
//		if(clips.size()==1) nextVideo = 1;
		
		printf("Added video\n");
		
	}
	
	void addVideo(RamVideo *vid) {
		clips.push_back(Clip());
		clips.back().video = vid;
	}
	
	void clearVideos() {
		while(clips.size()>0) {
			RamVideo *v = clips.front().video;
			clips.pop_front();
			delete v;
		}
		init();
	}
	void saveVideos() {
		for(int i = 0; i < clips.size(); i++) {
			clips[i].video->dumpBlocking();
		}
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
	
	void checkForSize() {
		if(videoBeingRemoved!=NULL) {
			if(videoBeingRemoved->doneDumping()) {
				printf("Video dumped - erasing\n");
				delete videoBeingRemoved;
				videoBeingRemoved = NULL;
			}
		}
		
		if(videoBeingRemoved==NULL) {
			if(RamVideo::numFrames>MAX_NUM_FRAMES) {
				if(currVideo!=0 && nextVideo!=0 && nextNextVideo!=0 && !clips.front().isAVideoFeed()) {	
									// the beginning of the clip deque
									// is the first one that was recorded.
									// we want to delete it if it's not currently
									// being played.
					videoBeingRemoved = clips.front().video;
					clips.pop_front();
					// start the dump here.
					videoBeingRemoved->dump();
					printf("Started dumping a video\n");
					if(currVideo>-1) currVideo--;
					if(nextVideo>-1) nextVideo--;
					if(nextNextVideo>-1) nextNextVideo--;
					if(prevVideo>-1) prevVideo--;
					
				}
			}
		}
	}
	
	
	bool isScrolling() {
		return !paused;
	}
	
	float getOffset() {

		return ofMap(frameNum, slideTime*frameDuration, frameDuration, 0, 1, true);
	}
	
	bool isOnVideoFeed() {
		if(clips.size()==0) return true;
		if(currVideo==-1) return true;
		if(currVideo<clips.size()) {
			return clips[currVideo].isAVideoFeed();
		}
		return false;
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
				if(a<1 && whatNextVideoWas>-1 &&  whatNextVideoWas<clips.size()) clips[whatNextVideoWas].draw(nextVideoX, 0);
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
