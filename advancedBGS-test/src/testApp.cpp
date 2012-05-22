#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
 // ofSetVerticalSync(true);

  	#ifdef _USE_LIVE_VIDEO
        vidGrabber.setVerbose(true);
        vidGrabber.setDeviceID(0); // internal camera / USB camera
        vidGrabber.initGrabber(W,H);

	#else
        vidPlayer.loadMovie("fingers.mov");
        W = vidPlayer.width;
        H = vidPlayer.height;
        vidPlayer.setLoopState(OF_LOOP_NORMAL);
        vidPlayer.setVolume(0);
        vidPlayer.play();

	#endif

// -------- INITIAL ALGORITHM PARAMETERS:
//
//           PLAY WITH THESE SETTINGS
//
//-----------------------------------------
// For AdaptiveMedianBGS ------------------------------
		bgsArray[0].bgsPtr = &A_bgs;
        bgsArray[0].paramsPtr = &A_params;
        bgsArray[0].name = "ADAPTIVE MEDIAN";
        A_params.SetFrameSize(W, H);
		A_params.LowThreshold() = 70; // default 40
		A_params.HighThreshold() = 2*A_params.LowThreshold();
		A_params.SamplingRate() = 2; // default 7;
		A_params.LearningFrames() = 30;// default 30;
 		A_bgs.Initalize(A_params);
// For Eigenbackground ----------------------------
        bgsArray[1].bgsPtr = &E_bgs;
        bgsArray[1].paramsPtr = &E_params;
        bgsArray[1].name = "EIGENBACKGROUND";
        E_params.SetFrameSize(W, H);
		E_params.LowThreshold() = 100;//15*15; // default 15*15;
		E_params.HighThreshold() = 2*E_params.LowThreshold();
		E_params.HistorySize() = 40;// default 100;
		E_params.EmbeddedDim() = 20;// default 20;
 		E_bgs.Initalize(E_params);
// For GrimsonGMM ------------------------------
        bgsArray[2].bgsPtr = &G_bgs;
        bgsArray[2].paramsPtr = &G_params;
        bgsArray[2].name = "GRIMSON";
        G_params.SetFrameSize(W, H);
		G_params.LowThreshold() = 3.0f*3.0f; // default 3.0f*3.0f;
		G_params.HighThreshold() =  2*G_params.LowThreshold();
		G_params.Alpha() = 0.01f;// default 0.001f;
		G_params.MaxModes() = 3; // default 3;
 		G_bgs.Initalize(G_params);
// For MeanBGS ----------------------------
        bgsArray[3].bgsPtr = &M_bgs;
        bgsArray[3].paramsPtr = &M_params;
        bgsArray[3].name = "MEAN";
        M_params.SetFrameSize(W, H);
		M_params.LowThreshold() = 3*30*30; // default 3*30*30;
		M_params.HighThreshold() = 2*M_params.LowThreshold();
		M_params.Alpha() = 1e-6f; // default 1e-6f;
		M_params.LearningFrames() = 30;// default 30;
 		M_bgs.Initalize(M_params);
// For PratiMediodBGS ----------------------------
        bgsArray[4].bgsPtr = &P_bgs;
        bgsArray[4].paramsPtr = &P_params;
        bgsArray[4].name = "PRATI";
        P_params.SetFrameSize(W, H);
		P_params.LowThreshold() = 40;// default 30;
		P_params.HighThreshold() = 2*P_params.LowThreshold();
		P_params.SamplingRate() = 5;// default 5;
		P_params.HistorySize() = 16;// default 16;
		P_params.Weight() = 2;// default 5;
 		P_bgs.Initalize(P_params);
// For WrenGA ----------------------------
        bgsArray[5].bgsPtr = &W_bgs;
        bgsArray[5].paramsPtr = &W_params;
        bgsArray[5].name = "WREN GA";
        W_params.SetFrameSize(W, H);
		W_params.LowThreshold() = 50;// default 3.5f*3.5f;
		W_params.HighThreshold() = 2*W_params.LowThreshold();
		W_params.Alpha() = 0.5f;// default 0.005f;
		W_params.LearningFrames() = 30;// default 30
 		W_bgs.Initalize(W_params);
// For ZivkovicAGMM ----------------------------
        bgsArray[6].bgsPtr = &Z_bgs;
        bgsArray[6].paramsPtr = &Z_params;
        bgsArray[6].name = "ZIVCOVIK";
        Z_params.SetFrameSize(W, H);
		Z_params.LowThreshold() = 5.0f*5.0f;// default 5.0f*5.0f;
		Z_params.HighThreshold() = 2*Z_params.LowThreshold();
		Z_params.Alpha() = 0.01f;// default 0.001f;
		Z_params.MaxModes() = 3;// default 3
 		Z_bgs.Initalize(Z_params);
//  ----------------------------

    for (int i = 0; i < 7; i++) {
        bgsArray[i].On = 1;
        bgsArray[i].low = cvCreateImage(cvSize(W,H),IPL_DEPTH_8U,1);
        bgsArray[i].high = cvCreateImage(cvSize(W,H),IPL_DEPTH_8U,1);
    }

    colorImg.allocate(W, H);


    bLearnBakground = true;

    lastTime = ofGetElapsedTimeMillis();
	currFrameNum = 0;
	videoFPS = 0;
}

/*****************************************************************
                            UPDATE
******************************************************************/
void testApp::update(){


    ofBackground(100,100,100);

    bool bNewFrame = false;

	#ifdef _USE_LIVE_VIDEO
       vidGrabber.grabFrame();
	   bNewFrame = vidGrabber.isFrameNew();
    #else
        vidPlayer.idleMovie();
        bNewFrame = vidPlayer.isFrameNew();
	#endif

	if (bNewFrame){
	    // Calculate video framerate
	    int currTime = ofGetElapsedTimeMillis();
	    videoFPS = 1000/(currTime-lastTime);
	    lastTime = currTime;

		#ifdef _USE_LIVE_VIDEO
            // set current frame number
	        currFrameNum++;
            colorImg.setFromPixels(vidGrabber.getPixels(), W, H);
	    #else
	        // get current frame number
	        currFrameNum = vidPlayer.getCurrentFrame();
            colorImg.setFromPixels(vidPlayer.getPixels(), W, H);
        #endif

        // Initialize / Update background models
        IplImage* img=cvCloneImage(colorImg.getCvImage()); // clone incoming frame
        cvCvtColor(img,img,CV_RGB2BGR); // convert from RGB to BGR
        RgbImage  rgbImage(img); // create RgbImage


        for (int i = 0; i < 7; i++) {
          if (bLearnBakground == true){
            // Initialize the background model. Typically, the background model is initialized using the first
            // frame of the incoming video stream, but alternatives are possible.
            bgsArray[i].bgsPtr->InitModel(rgbImage);
          }
          else if (bgsArray[i].On) {
            // Subtract the current frame from the background model and produce a binary foreground mask using
            // both a low and high threshold value.
            bgsArray[i].bgsPtr->Subtract(currFrameNum, rgbImage, bgsArray[i].low, bgsArray[i].high);
            // Update the background model. Only pixels set to background in update_mask are updated.
            bgsArray[i].bgsPtr->Update(currFrameNum, rgbImage, bgsArray[i].low );
          }
        }
        bLearnBakground = false;

    }
}

/*****************************************************************
                            DRAW
******************************************************************/
void testApp::draw(){

	int yPos = SPACE;
	int xPos = SPACE;

    ofSetColor(0x888888);
    ofRect( 0, 0, DRAW_W+SPACE, DRAW_H*2+SPACE);

    yPos = SPACE/2;
    xPos = SPACE/2;
    //-------------------  INPUT FRAME
	ofSetColor(0xffffff);
	colorImg.draw(xPos,yPos, DRAW_W, DRAW_H);
    ofDrawBitmapString("INCOMING FRAME", xPos, yPos+15);
    ofDrawBitmapString(ofToString(currFrameNum), SPACE, yPos + DRAW_H-10);

    //------------ 1st row on screen
    //--------------------- ADAPTIME_MEDIAN_BGS
    xPos+=(DRAW_W+SPACE);
    DrawBGR(0, &bgsArray[0], xPos, yPos);
    if (A_params.LearningFrames() > currFrameNum)
       ofDrawBitmapString( "LEARNING "+ofToString(A_params.LearningFrames()-currFrameNum),
                          xPos+50, yPos + DRAW_H/2);

    //--------------------- EIGENBACKROUND
    xPos+=(DRAW_W+SPACE);
    DrawBGR(1,&bgsArray[1], xPos, yPos);
    if (E_params.HistorySize() > currFrameNum)
      ofDrawBitmapString( "LEARNING "+ofToString(E_params.HistorySize()-currFrameNum),
                          xPos+50, yPos + DRAW_H/2);
    //--------------------- GRIMSON GMM
    xPos+=(DRAW_W+SPACE);
    DrawBGR(2,&bgsArray[2], xPos, yPos);

    //---------- 2nd row on screen
    yPos = (DRAW_H+SPACE)*2;
    //--------------------- MEAN
    xPos = SPACE/2;
    DrawBGR(3,&bgsArray[3], xPos, yPos);
   if (M_params.LearningFrames() > currFrameNum)
      ofDrawBitmapString( "LEARNING "+ofToString(M_params.LearningFrames()-currFrameNum),
                          xPos+50, yPos + DRAW_H/2);
    //--------------------- PRATI
    xPos+=(DRAW_W+SPACE);
    DrawBGR(4,&bgsArray[4], xPos, yPos);
    if (P_params.HistorySize() > currFrameNum)
      ofDrawBitmapString( "LEARNING "+ofToString(P_params.HistorySize()-currFrameNum),
                          xPos+50, yPos + DRAW_H/2);
    //--------------------- WREN GA
    xPos+=(DRAW_W+SPACE);
    DrawBGR(5,&bgsArray[5], xPos, yPos);
    if (W_params.LearningFrames() > currFrameNum)
      ofDrawBitmapString( "LEARNING "+ofToString(W_params.LearningFrames()-currFrameNum),
                          xPos+50, yPos + DRAW_H/2);
    //--------------------- ZIVKOVIC
    xPos+=(DRAW_W+SPACE);
    DrawBGR(6,&bgsArray[6], xPos, yPos);
    //--------------------- report
    xPos = SPACE;
    yPos = SPACE+DRAW_H+15;
	ofSetColor(0xffffff);
	char reportStr[1024];
	sprintf(reportStr, "BACKGROUND\nSUBTRACTION"
         "\n\nscreen fps: %.0f\nvideo fps: %.0f"
         "\n\n' ' : reset BGR\n1..7 : switch on/off", ofGetFrameRate(),videoFPS);
    ofDrawBitmapString(reportStr, xPos, yPos);

}
/************************************************
*
*    DrawBGR - draw background and mask of a given algorithm
*
**************************************************/
void testApp::DrawBGR(int algNum, bgs_struct *bgs, int x = 0, int y = 0)
{
    char numStr;

    numStr = char(algNum+49);
    ofSetColor(0xffffff);

    if (bgs->On) {
        DrawIplImage(bgs->bgsPtr->Background()->Ptr(), x,y, ZOOM, ZOOM);
        DrawIplImage(bgs->low.Ptr(), x,y+DRAW_H+2, ZOOM, ZOOM);
    }
    ofSetColor(0x000000);
    ofDrawBitmapString(&numStr, x+1, y+15+1);
    ofDrawBitmapString(bgs->name, x+15+1,y+15+1);
    ofDrawBitmapString("BACKGROUND", x,y+DRAW_H+1-15);
    ofDrawBitmapString("MASK", x,y+DRAW_H+1+15);
    if (bgs->On) ofSetColor(0xff0000);
    else ofSetColor(0x0000ff);
    ofDrawBitmapString(&numStr, x, y+15);
    ofDrawBitmapString(bgs->name, x+15,y+15);
    ofDrawBitmapString("BACKGROUND", x,y+DRAW_H-15);
    ofDrawBitmapString("MASK", x,y+DRAW_H+15);

}
/************************************************
*
*    DrawIplImage code
*    found at http://programanddesign.com/cpp/draw-iplimage/
*
**************************************************/
void testApp::DrawIplImage(IplImage *image, int x = 0, int y = 0, GLfloat xZoom = 1.0f, GLfloat yZoom = 1.0f)
{
    GLenum format;
        switch(image->nChannels) {
            case 1:
                format = GL_LUMINANCE;
                break;
            case 2:
                format = GL_LUMINANCE_ALPHA;
                break;
            case 3:
                format = GL_BGR;
                break;
            default:
                return;
        }

    glRasterPos2i(x, y);
    glPixelZoom(xZoom, -yZoom);
    glDrawPixels(image->width, image->height, format, GL_UNSIGNED_BYTE, image->imageData);
    glPixelZoom(1.0f, 1.0f);

}
//--------------------------------------------------------------
void testApp::keyPressed  (int key){

    if ((key >= 49) && (key <= 57)) bgsArray[key-49].On = !(bgsArray[key-49].On);
	else switch (key){
		case ' ':
			currFrameNum = 0;
			bLearnBakground = true;
			break;
     }
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

