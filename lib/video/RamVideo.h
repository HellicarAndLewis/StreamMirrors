//
//  RamVideo.h
//  opencvExample
//
//  Created by Marek Bereza on 21/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef opencvExample_RamVideo_h
#define opencvExample_RamVideo_h


class RamVideo {
public:
	
	~RamVideo() {
		clear();
	} 
	
	void clear() {
		for(int i = 0; i < frames.size(); i++) {
			delete [] frames[i];
		}
		frames.clear();
	}
	
	int getLength() {
		return frames.size();
	}
	void setup(int width, int height, int maxLength) {

		tex.allocate(width, height, GL_RGBA);
		framePos = 0;
		this->height = height;
		this->width = width;
		this->maxLength = maxLength;
		this->needsToLoad = true;
	}

	void draw(float x, float y) {
		if(needsToLoad) {
			tex.loadData(frames[framePos], width, height, GL_RGBA);
			needsToLoad = false;
		}
		tex.draw(x, y);
	}
	
	void draw(float x, float y, float w) { // automatically calculates height
		if(needsToLoad) {
			tex.loadData(frames[framePos], width, height, GL_RGBA);
			needsToLoad = false;
		}
		float h = w*height/width;
		tex.draw(x, y, w, h);
	}
	
	void draw(const ofRectangle &r) {
		if(needsToLoad) {
			tex.loadData(frames[framePos], width, height, GL_RGBA);
			needsToLoad = false;
		}
		tex.draw(r);
	}
	
	void play() {
		framePos = 0;
	}
	void nextFrame() {
		if(frames.size()>0) {
			framePos++;
			framePos %= frames.size();

			needsToLoad = true;
		}
	}
	
	void createDir(string path) {
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
	
	void save(string path) {
		ofImage img;
		img.allocate(width, height, OF_IMAGE_COLOR_ALPHA);
		createDir(path);
		
		for(int i = 0; i < frames.size(); i++) {
			printf(".");
			img.setFromPixels(frames[i], width, height, OF_IMAGE_COLOR_ALPHA);
			string num = ofToString(i);
			while(num.size()<5) {
				num = "0" + num;
			}
			img.saveImage(path + "/" + num + ".png");
		}
		printf("\n");
	}
	
	
	void load(string path) {
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
	
	
	
	ofTexture tex;
	bool record(unsigned char *frame) {
		
		if(frames.size()>maxLength) return false;
		
		int len = width * height * 4;
		unsigned char *f = new unsigned char[len];
		memcpy(f, frame, len);
		frames.push_back(f);
		return true;
	}
	
	int framePos;
	bool needsToLoad;
	
	int width;
	int height;
	int maxLength;
	vector<unsigned char *> frames;

};
#endif
