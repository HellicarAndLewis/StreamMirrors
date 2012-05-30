//
//  testAppRotatePixels.cpp
//  opencvExample
//
//  Created by Marek Bereza on 21/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "testApp.h"

int *rgbRotTable = NULL;
bool oldFlipXRgb = false;
bool oldClockwiseRgb = false;

int *rotTable = NULL;
bool oldFlipX = false;
bool oldClockwise = false;

void testApp::rotateRgb90(unsigned char *a, unsigned char *b, bool clockwise, bool flipX) {
	if(flipX != oldFlipXRgb || clockwise!=oldClockwiseRgb || rgbRotTable==NULL) {
		oldFlipXRgb = flipX;
		oldClockwiseRgb = clockwise;
		if(rgbRotTable==NULL) {
			rgbRotTable = new int[640*480*3];
		}
		
		if(!flipX) {
			if(clockwise) {
				for(int i = 0; i < VISION_WIDTH; i++) {
					int ii = (VISION_WIDTH - i - 1) * VISION_HEIGHT;
					for(int j = 0; j < VISION_HEIGHT; j++) {
						int inPos = i + j * VISION_WIDTH;
						int outPos = (VISION_HEIGHT - j - 1) + ii;
						inPos *= 3;
						outPos *= 3;
						rgbRotTable[inPos] = outPos;
						rgbRotTable[inPos+1] = outPos+1;
						rgbRotTable[inPos+2] = outPos+2;
					}
				}
			} else {
				for(int i = 0; i < VISION_WIDTH; i++) {
					int ii = i * VISION_HEIGHT;
					for(int j = 0; j < VISION_HEIGHT; j++) {
						int inPos = i + j * VISION_WIDTH;
						int outPos = j + ii;
						inPos *= 3;
						outPos *= 3;
						rgbRotTable[inPos] = outPos;
						rgbRotTable[inPos+1] = outPos+1;
						rgbRotTable[inPos+2] = outPos+2;
					}
				}
			}
			
		} else {
			if(clockwise) {
				for(int i = 0; i < VISION_WIDTH; i++) {
					int ii = (VISION_WIDTH - i - 1) * VISION_HEIGHT;
					for(int j = 0; j < VISION_HEIGHT; j++) {
						int inPos = i + (VISION_HEIGHT - j - 1) * VISION_WIDTH;
						int outPos = (VISION_HEIGHT - j - 1) + ii;
						inPos *= 3;
						outPos *= 3;
						rgbRotTable[inPos] = outPos;
						rgbRotTable[inPos+1] = outPos+1;
						rgbRotTable[inPos+2] = outPos+2;
					}
				}
			} else {
				for(int i = 0; i < VISION_WIDTH; i++) {
					int ii = i * VISION_HEIGHT;
					for(int j = 0; j < VISION_HEIGHT; j++) {
						int inPos = i + (VISION_HEIGHT - j - 1) * VISION_WIDTH;
						int outPos = j + ii;
						inPos *= 3;
						outPos *= 3;
						rgbRotTable[inPos] = outPos;
						rgbRotTable[inPos+1] = outPos+1;
						rgbRotTable[inPos+2] = outPos+2;
					}
				}
			}
		}
	}
	
	int numPix = 640*480*3;
	for(int i = 0; i < numPix; i++) {
		b[rgbRotTable[i]] = a[i];
		
	}
}

void testApp::rotate90(unsigned char *a, unsigned char *b, bool clockwise, bool flipX) {
	if(flipX != oldFlipX || clockwise!=oldClockwise || rotTable==NULL) {
		oldFlipX = flipX;
		oldClockwise = clockwise;
		if(rotTable==NULL) {
			rotTable = new int[640*480];
		}
		if(!flipX) {
			if(clockwise) {
				for(int i = 0; i < VISION_WIDTH; i++) {
					int ii = (VISION_WIDTH - i - 1) * VISION_HEIGHT;
					for(int j = 0; j < VISION_HEIGHT; j++) {
						int inPos = i + j * VISION_WIDTH;
						int outPos = (VISION_HEIGHT - j - 1) + ii;
						rotTable[inPos] = outPos;
					}
				}
			} else {
				for(int i = 0; i < VISION_WIDTH; i++) {
					int ii = i * VISION_HEIGHT;
					for(int j = 0; j < VISION_HEIGHT; j++) {
						int inPos = i + j * VISION_WIDTH;
						int outPos = j + ii;
						rotTable[inPos] = outPos;
					}
				}
			}
			
		} else {
			if(clockwise) {
				for(int i = 0; i < VISION_WIDTH; i++) {
					int ii = (VISION_WIDTH - i - 1) * VISION_HEIGHT;
					for(int j = 0; j < VISION_HEIGHT; j++) {
						int inPos = i + (VISION_HEIGHT - j - 1) * VISION_WIDTH;
						int outPos = (VISION_HEIGHT - j - 1) + ii;
						rotTable[inPos] = outPos;
					}
				}
			} else {
				for(int i = 0; i < VISION_WIDTH; i++) {
					int ii = i * VISION_HEIGHT;
					for(int j = 0; j < VISION_HEIGHT; j++) {
						int inPos = i + (VISION_HEIGHT - j - 1) * VISION_WIDTH;
						int outPos = j + ii;
						rotTable[inPos] = outPos;
					}
				}
			}
		}
	}
	int numPix = 640*480;
	for(int i = 0; i < numPix; i++) {
		b[rotTable[i]] = a[i];
		
	}
}