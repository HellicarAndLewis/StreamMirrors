//
//  RamVideo.h
//  opencvExample
//
//  Created by Marek Bereza on 21/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef opencvExample_RamVideo_h
#define opencvExample_RamVideo_h
#include "ofThread.h"


class RamVideo: public ofThread {
public:
	static int referenceCount;
	~RamVideo();	
	void clear();
	bool done;
	
	void dump();	
	bool doneDumping();
	
	void threadedFunction();	
	int getLength();
	
	void setup(int width, int height, int maxLength);

	void draw(float x, float y);
	
	void draw(float x, float y, float w);
	
	void draw(const ofRectangle &r);	
	void play();
	
	void nextFrame();
	
	void createDir(string path);
	
	void save(string path);	
	
	void load(string path);
	
	
	ofTexture tex;
	bool record(unsigned char *frame);
	
	int framePos;
	bool needsToLoad;
	
	int width;
	int height;
	int maxLength;
	vector<unsigned char *> frames;

};

#endif

