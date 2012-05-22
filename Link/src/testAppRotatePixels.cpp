//
//  testAppRotatePixels.cpp
//  opencvExample
//
//  Created by Marek Bereza on 21/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "testApp.h"


void testApp::rotateRgb90(unsigned char *a, unsigned char *b, bool clockwise, bool flipX) {
	if(!flipX) {
		if(clockwise) {
			for(int i = 0; i < VISION_WIDTH; i++) {
				for(int j = 0; j < VISION_HEIGHT; j++) {
					int inPos = i + j * VISION_WIDTH;
					int outPos = (VISION_HEIGHT - j - 1) + (VISION_WIDTH - i - 1) * VISION_HEIGHT;
					inPos *= 3;
					outPos *= 3;
					b[outPos] = a[inPos];
					b[outPos+1] = a[inPos+1];
					b[outPos+2] = a[inPos+2];
				}
			}
		} else {
			for(int i = 0; i < VISION_WIDTH; i++) {
				for(int j = 0; j < VISION_HEIGHT; j++) {
					int inPos = i + j * VISION_WIDTH;
					int outPos = j + i * VISION_HEIGHT;
					inPos *= 3;
					outPos *= 3;
					b[outPos] = a[inPos];
					b[outPos+1] = a[inPos+1];
					b[outPos+2] = a[inPos+2];
				}
			}
		}
		
	} else {
		if(clockwise) {
			for(int i = 0; i < VISION_WIDTH; i++) {
				for(int j = 0; j < VISION_HEIGHT; j++) {
					int inPos = i + (VISION_HEIGHT - j - 1) * VISION_WIDTH;
					int outPos = (VISION_HEIGHT - j - 1) + (VISION_WIDTH - i - 1) * VISION_HEIGHT;
					inPos *= 3;
					outPos *= 3;
					b[outPos] = a[inPos];
					b[outPos+1] = a[inPos+1];
					b[outPos+2] = a[inPos+2];
				}
			}
		} else {
			for(int i = 0; i < VISION_WIDTH; i++) {
				for(int j = 0; j < VISION_HEIGHT; j++) {
					int inPos = i + (VISION_HEIGHT - j - 1) * VISION_WIDTH;
					int outPos = j + i * VISION_HEIGHT;
					inPos *= 3;
					outPos *= 3;
					b[outPos] = a[inPos];
					b[outPos+1] = a[inPos+1];
					b[outPos+2] = a[inPos+2];
				}
			}
		}
	}
}

void testApp::rotate90(unsigned char *a, unsigned char *b, bool clockwise, bool flipX) {
	if(!flipX) {
		if(clockwise) {
			for(int i = 0; i < VISION_WIDTH; i++) {
				for(int j = 0; j < VISION_HEIGHT; j++) {
					int inPos = i + j * VISION_WIDTH;
					int outPos = (VISION_HEIGHT - j - 1) + (VISION_WIDTH - i - 1) * VISION_HEIGHT;
					b[outPos] = a[inPos];
				}
			}
		} else {
			for(int i = 0; i < VISION_WIDTH; i++) {
				for(int j = 0; j < VISION_HEIGHT; j++) {
					int inPos = i + j * VISION_WIDTH;
					int outPos = j + i * VISION_HEIGHT;
					b[outPos] = a[inPos];
				}
			}
		}
		
	} else {
		if(clockwise) {
			for(int i = 0; i < VISION_WIDTH; i++) {
				for(int j = 0; j < VISION_HEIGHT; j++) {
					int inPos = i + (VISION_HEIGHT - j - 1) * VISION_WIDTH;
					int outPos = (VISION_HEIGHT - j - 1) + (VISION_WIDTH - i - 1) * VISION_HEIGHT;
					b[outPos] = a[inPos];
				}
			}
		} else {
			for(int i = 0; i < VISION_WIDTH; i++) {
				for(int j = 0; j < VISION_HEIGHT; j++) {
					int inPos = i + (VISION_HEIGHT - j - 1) * VISION_WIDTH;
					int outPos = j + i * VISION_HEIGHT;
					b[outPos] = a[inPos];
				}
			}
		}
	}
}