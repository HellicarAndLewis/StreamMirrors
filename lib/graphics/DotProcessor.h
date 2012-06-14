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
        pixScale = 80;
        
        gui.addTitle("Shader");
        
        gui.addSlider("sharpness", sharpness, 0, 100);
        gui.addSlider("brightness scale", brightnessScale, -1, 1.5);
        gui.addSlider("pixScale", pixScale, 5, 200);
		fbo.allocate(width, height, GL_RGBA);
    }
    
    void draw(ofTexture &tex) {
        shader.begin();
        shader.setUniform1f("sharpness", sharpness);
        shader.setUniform1f("brightnessScale", brightnessScale);
        shader.setUniform1f("pixScale", pixScale);
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
    float pixScale;
};
