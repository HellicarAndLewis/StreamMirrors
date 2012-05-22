//
//  Clip.h
//  opencvExample
//
//  Created by Marek Bereza on 21/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef opencvExample_Clip_h
#define opencvExample_Clip_h

class Clip: public ofRectangle {
public:
	RamVideo *video;
	
	void draw() {
		video->nextFrame();
		video->draw(x, y, width);
	}
	void draw(const ofRectangle &r) {
		video->nextFrame();
		video->draw(r);
	}
};

#endif
