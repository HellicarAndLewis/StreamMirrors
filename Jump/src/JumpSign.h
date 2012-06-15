/**     ___           ___           ___                         ___           ___     
 *     /__/\         /  /\         /  /\         _____         /  /\         /__/|    
 *    |  |::\       /  /::\       /  /::|       /  /::\       /  /::\       |  |:|    
 *    |  |:|:\     /  /:/\:\     /  /:/:|      /  /:/\:\     /  /:/\:\      |  |:|    
 *  __|__|:|\:\   /  /:/~/::\   /  /:/|:|__   /  /:/~/::\   /  /:/  \:\   __|__|:|    
 * /__/::::| \:\ /__/:/ /:/\:\ /__/:/ |:| /\ /__/:/ /:/\:| /__/:/ \__\:\ /__/::::\____
 * \  \:\~~\__\/ \  \:\/:/__\/ \__\/  |:|/:/ \  \:\/:/~/:/ \  \:\ /  /:/    ~\~~\::::/
 *  \  \:\        \  \::/          |  |:/:/   \  \::/ /:/   \  \:\  /:/      |~~|:|~~ 
 *   \  \:\        \  \:\          |  |::/     \  \:\/:/     \  \:\/:/       |  |:|   
 *    \  \:\        \  \:\         |  |:/       \  \::/       \  \::/        |  |:|   
 *     \__\/         \__\/         |__|/         \__\/         \__\/         |__|/   
 *
 *  Description: 
 *				 
 *  JumpSign.h, created by Marek Bereza on 15/06/2012.
 */

#pragma once

#include "ofMain.h"

class JumpSign {
public:
	void setup() {
		alpha = 0;
		sign.loadImage("jump.png");
		sign.setAnchorPercent(0.5, 0.5);
	}
	
	void update(bool showSign) {
		if(showSign) alpha += 0.05;
		else alpha -= 0.05;
		alpha = ofClamp(alpha, 0, 1);
	}
	void draw() {
		if(alpha==0) return;
		glColor4f(1, 1, 1, alpha*0.7);
		glPushMatrix();
		glTranslatef(240, 270, 0);
		glScalef(0.3, 0.3, 1);
		sign.draw(0, 0);
		glPopMatrix();
	}
	float alpha;
	ofImage sign;
};
