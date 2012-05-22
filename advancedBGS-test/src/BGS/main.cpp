#include <iostream>

#pragma warning ( disable : 4800 ) 

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <stdlib.h>

int main(int argc, const char* argv[])
{
    cvNamedWindow( "AVI Video", CV_WINDOW_AUTOSIZE );
    CvVideoWriter *writer = 0;
    int isColor = 1;
    int fps     = 5;  // or 30
    IplImage* img = 0; 
    img=cvLoadImage("E:/tasks/video_recognition/data/IMG_0383.jpg");
    int frameW  = img->width; //640; // 744 for firewire cameras
    int frameH  = img->height; //480; // 480 for firewire cameras

    writer=cvCreateVideoWriter("out2.avi",-1,fps,cvSize(frameW,frameH),1);

    cvWriteFrame(writer, img);      // add the frame to the file

    char *FirstFile,fF[20]="",*fileNoStr,fns[4]="";
    fileNoStr=fns;
    for(int fileNo = 0;fileNo<100;fileNo++){
            FirstFile=fF;   
            itoa(fileNo,fileNoStr,10);
            FirstFile=strcat ( FirstFile,"animTest_");
            FirstFile=strcat ( FirstFile,fileNoStr);
            FirstFile=strcat ( FirstFile,".bmp");

            printf(" \n%s .",FirstFile);
            img=cvLoadImage(FirstFile);
            cvShowImage( "AVI Video", FirstFile );
            cvWriteFrame(writer, img);

    }
    cvReleaseVideoWriter(&writer);
    cvDestroyWindow ("AVI Video");
    return 0;

}
