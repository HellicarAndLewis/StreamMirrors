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
 *  ThanksSign.h, created by Marek Bereza on 15/06/2012.
 */

#pragma once

 
class ThanksSign {
public:
	void setup() {
		lastTimePinged = -10000;
		sign.loadImage("thanks.png");
		sign.setAnchorPercent(0.5, 0.5);
	}
	
	void ping() {
		lastTimePinged = ofGetElapsedTimef();
	}
	void draw() {
		
		float alpha = 0;
		float timeSince = ofGetElapsedTimef() - lastTimePinged;
		if(timeSince<2) alpha = 1;
		else alpha = ofMap(timeSince, 2, 3, 1, 0, true);

		if(alpha==0) return;

		glColor4f(1, 1, 1, alpha*0.7);
		glPushMatrix();
		glTranslatef(240, 240, 0);
		glScalef(0.3, 0.3, 1);
		sign.draw(0, 0);
		glPopMatrix();
	}

	float lastTimePinged;
	ofImage sign;
};
