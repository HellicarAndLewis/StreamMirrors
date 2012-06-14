//
//  DotProcessor.h
//  Chain
//
//  Created by Joel Gethin Lewis on 14/06/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "ofMain.h"
#include "ofxSimpleGuiToo.h"
class DotProcessor {
public:
    void setup(float width, float height) {
        shader.load("dots.vert", "dots.frag");
        sharpness = 50;
        brightnessScale = 0.5;
        pixScaleX = 80;
        pixScaleY = 80;
        enable = true;
		gui.addTitle("Shader");
		
        gui.addToggle("Enable", enable);
        gui.addSlider("sharpness", sharpness, 0, 100);
        gui.addSlider("brightness scale", brightnessScale, -1, 1.5);
        gui.addSlider("pixScaleX", pixScaleX, 5, 400);
        gui.addSlider("pixScaleY", pixScaleY, 5, 400);
		fbo.allocate(width, height, GL_RGBA);
    }
    
    void draw(ofTexture &tex) {
		if(enable) {
			shader.begin();
			shader.setUniform1f("sharpness", sharpness);
			shader.setUniform1f("brightnessScale", brightnessScale);
			shader.setUniform1f("pixScaleX", pixScaleX);
			shader.setUniform1f("pixScaleY", pixScaleY);
			shader.setUniform1f("width", fbo.getWidth());
			shader.setUniform1f("height",fbo.getHeight());
			shader.setUniformTexture("tex", tex, 0);
			glBegin(GL_TRIANGLE_STRIP);
			{
				glTexCoord2f(0, 0);
				glVertex2f(0, 0);
				glTexCoord2f(1, 0);
				glVertex2f(tex.getWidth(), 0);
				glTexCoord2f(0, 1);
				glVertex2f(0, tex.getHeight());
				glTexCoord2f(1, 1);
				glVertex2f(tex.getWidth(), tex.getHeight());
			}
			glEnd();
			shader.end();
		} else {
			fbo.draw(0, 0);
		}
        
    }
    
    void begin() {
        fbo.begin();
		ofClear(0, 0, 0, 0);
    }
    void end() {
        fbo.end();
		draw(fbo.getTextureReference(0));
    }
    
private:
	ofFbo fbo;
    ofShader shader;
    float sharpness;
    float brightnessScale;
    float pixScaleX;
	float pixScaleY;
	bool enable;
};
