//
//  RamVidoe.cpp
//  Jump
//
//  Created by Marek Bereza on 02/06/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "RamVideo.h"

RamVideo::RamVideo() {
	done = false;
}

RamVideo::~RamVideo() {
		clear();
	} 
	
void RamVideo::clear() {
	for(int i = 0; i < frames.size(); i++) {
		delete [] frames[i];
	}
	frames.clear();
}


void RamVideo::dump() {
	startThread();
}
void RamVideo::dumpBlocking() {
	threadedFunction();
}

bool RamVideo::doneDumping() {
	return done;
}
void RamVideo::threadedFunction() {
	if(done) return;
	string filename = ofGetTimestampString();
	save(filename);
	done = true;
	
}

int RamVideo::getLength() {
	return frames.size();
}
void RamVideo::setup(int width, int height, int maxLength) {
	done = false;
	tex.allocate(width, height, GL_RGBA);
	framePos = 0;
	this->height = height;
	this->width = width;
	this->maxLength = maxLength;
	this->needsToLoad = true;
}




void RamVideo::draw(float x, float y) {
	if(needsToLoad) {
		tex.loadData(frames[framePos], width, height, GL_RGBA);
		needsToLoad = false;
	}
	tex.draw(x, y);
}

void RamVideo::play() {
	framePos = 0;
}
void RamVideo::nextFrame() {
	if(frames.size()==0) return;
	

		
	framePos++;
	framePos %= frames.size();
	
	needsToLoad = true;

	
}





void RamVideo::createDir(string path) {
	ofDirectory dir;
	dir.open(path);
	if(!dir.exists()) {
		dir.create();
		printf("Creating directory\n");
	} else {
		printf("Clearing dir\n");
		dir.listDir();
		for(int i = 0; i < dir.size(); i++) {
			dir[i].remove(true);
		}
	}
}

void RamVideo::save(string path) {
	ofImage img;
	img.setUseTexture(false);
	img.allocate(width, height, OF_IMAGE_COLOR_ALPHA);
	createDir(path);
	
	for(int i = 0; i < frames.size(); i++) {
		printf(".");
		img.setFromPixels(frames[i], width, height, OF_IMAGE_COLOR_ALPHA);
		string num = ofToString(i);
		while(num.size()<5) {
			num = "0" + num;
		}
		img.saveImage(path + "/" + num + ".tiff");
	}
	printf("\n");
}


void RamVideo::load(string path) {
	ofImage img;
	img.allocate(width, height, OF_IMAGE_COLOR_ALPHA);
	ofDirectory dir;
	dir.open(path);
	dir.sort();
	dir.listDir();
	dir.sort();
	
	int len = width * height * 4;
	for(int i = 0; i < dir.size(); i++) {
		//			printf("Got %s\n", dir[i].path().c_str());
		img.loadImage(dir[i].path());
		printf(".");
		
		unsigned char *frame = new unsigned char[len];
		memcpy(frame, img.getPixels(), len);
		frames.push_back(frame);
		
	}
	printf("\n");
}

bool RamVideo::record(unsigned char *frame) {
	
	if(frames.size()>maxLength) return false;
	
	int len = width * height * 4;
	unsigned char *f = new unsigned char[len];
	memcpy(f, frame, len);
	frames.push_back(f);
	return true;
}



