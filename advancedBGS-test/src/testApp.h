#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "./BGS/AdvancedBGS.hpp"


#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
								// otherwise, we'll use a movie file
#define SPACE 6

#define  W 160
#define  H 120
//#define ZOOM 1
#define ZOOM 250/(float)W
#define DRAW_W W*ZOOM
#define DRAW_H H*ZOOM


class testApp : public ofBaseApp{

	public:
        struct bgs_struct {
            Algorithms::BackgroundSubtraction::Bgs          *bgsPtr;
            Algorithms::BackgroundSubtraction::BgsParams    *paramsPtr;
            int                                             On;
            string                                          name;
            BwImage                                         low;
            BwImage                                         high;
        };

		void setup();
		void update();
		void draw();
		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
        void DrawIplImage(IplImage *image, int x, int y, GLfloat xZoom, GLfloat yZoom);
        void DrawBGR(int algNum,bgs_struct *bgs, int x, int y);

        #ifdef _USE_LIVE_VIDEO
		  ofVideoGrabber 		vidGrabber;
		#else
		  ofVideoPlayer 		vidPlayer;
		#endif

        ofxCvColorImage		    colorImg;

		Algorithms::BackgroundSubtraction::BgsParams A_params;
		Algorithms::BackgroundSubtraction::AdaptiveMedianBGS A_bgs;

		Algorithms::BackgroundSubtraction::BgsParams G_params;
		Algorithms::BackgroundSubtraction::GrimsonGMM G_bgs;

		Algorithms::BackgroundSubtraction::BgsParams M_params;
		Algorithms::BackgroundSubtraction::MeanBGS M_bgs;

        Algorithms::BackgroundSubtraction::BgsParams E_params;
		Algorithms::BackgroundSubtraction::Eigenbackground E_bgs;

        Algorithms::BackgroundSubtraction::BgsParams P_params;
		Algorithms::BackgroundSubtraction::PratiMediodBGS P_bgs;

        Algorithms::BackgroundSubtraction::BgsParams W_params;
		Algorithms::BackgroundSubtraction::WrenGA W_bgs;

		Algorithms::BackgroundSubtraction::BgsParams Z_params;
		Algorithms::BackgroundSubtraction::ZivkovicAGMM Z_bgs;

        bgs_struct          bgsArray[7];

		bool				bLearnBakground;

		int                 currFrameNum;
		float               videoFPS;
		int                 lastTime;

};

#endif
