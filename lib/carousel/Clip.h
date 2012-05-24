//
//  Clip.h
//  opencvExample
//
//  Created by Marek Bereza on 21/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef opencvExample_Clip_h
#define opencvExample_Clip_h

class Clip {
public:
	RamVideo *video;
	ofTexture *videoFeed;
	bool isVideoFeed;
	Clip() {
		isVideoFeed = false;
	}
	void setVideoFeed(ofTexture *videoFeed) {
		isVideoFeed = true;
		this->videoFeed = videoFeed;
	}
	
	void draw() {
		if(isVideoFeed) {
			videoFeed->draw(0, 0);
		} else {
			video->nextFrame();
			video->draw(0, 0);
		}
	}
	
	void draw(float x, float y) {
		if(isVideoFeed) {
			videoFeed->draw(x, y);
		} else {
			video->nextFrame();
			video->draw(x, y);
		}
	}
};

#endif
