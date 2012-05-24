//
//  testAppEventsLoadSave.cpp
//  Chain
//
//  Created by Marek Bereza on 23/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "testApp.h"
#include "ofxSimpleGuiToo.h"

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key==' ') {
		gui.toggleDraw();
	} else if(key=='f') {
		ofToggleFullscreen();
	} else if(key=='s') {
		saveEverything();
	} else if(key=='l') {
		loadEverything();
	} else if(key=='d') {
		drawDebug ^= true;
	}
}

void testApp::loadEverything() {
	for(int i = 0; i < videos.size(); i++) {
		delete videos[i];
	}
	videos.clear();
	
	ofDirectory dir;
	dir.open("vids");
	if(!dir.exists()) return;
	
	dir.sort();	
	dir.listDir();
	dir.sort();
	for(int i = 0; i < dir.size(); i++) {
		videos.push_back(new RamVideo());
		videos.back()->setup(480, 640, MAX_VIDEO_LENGTH);
		videos.back()->load(dir[i].path());
		carousel.addToEnd(videos.back());
	}
}

#include <sys/stat.h>

void testApp::saveEverything() {
	ofDirectory dir;
	dir.open("vids");
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
	
	for(int i = 0; i < videos.size(); i++) {
		videos[i]->save("vids/"+ofToString(i));
	}
}
//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	mouseIsDown = true;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	mouseIsDown = false;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
	
}
