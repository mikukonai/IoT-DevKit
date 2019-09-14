/*
 *  Opencv functionality validator
 *  By CSK
 *
 */

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <unistd.h>

#define uchar unsigned char

#include "SSD1306.h"

using namespace cv;


int main( int argc, char * argv[] ) {

    // grab one frame from the camera specified via arg[1]
    int camera_id = 0;
    unsigned char rot_buffer[1024];

    cv::VideoCapture cap(camera_id);

    if(!cap.isOpened())  // check if we succeeded
    {
        printf("failed to open the camera with id %d.\n", camera_id);
        return -1;
    }
    
    cv::Mat srcframe;
    cv::Mat destFrameRGB, destFrameGray_tmp;
    uint8_t oledfb[128*64];

    cap >> srcframe; // get a new frame from camera
    // convert the frame size to 128xN
    int srcWidth = srcframe.cols;

    float scale = srcWidth/128.0f;

    const int targetHeight = srcframe.rows / scale;
    const int targetWidth  = 128;
    
    printf("targetHeight = %d, targetWidth = %d\n", targetHeight,targetWidth);
    destFrameRGB.create(targetHeight,targetWidth,srcframe.type());
    destFrameGray_tmp.create(targetHeight,targetWidth,CV_8U);
    SSD1306_init();
    SSD1306_clearDisplay();
    SSD1306_setBrightness(255);

    while (1) {
        // capture one frame

        cap >> srcframe; // get a new frame from camera

        // resize the frame
        resize(srcframe, destFrameRGB, cvSize(targetWidth, targetHeight));

        // convert to gray scale
        cvtColor(destFrameRGB, destFrameGray_tmp, CV_RGB2GRAY);

#if 1
      //  blur( destFrameGray_tmp, destFrameGray_tmp, Size(2,2) );
        Canny( destFrameGray_tmp, destFrameGray_tmp, 60, 120, 3 );
#else
        // threshold
        cv::threshold(destFrameGray_tmp,destFrameGray_tmp,128,255,CV_THRESH_BINARY);

#endif
        // clip the frame to 128x64

        Mat_<uchar>& rawgray = (Mat_<uchar>&)destFrameGray_tmp;
        int pos = 0;
        for (int row= (targetHeight-64)/2; row <  (targetHeight-64)/2+64; ++row) {
            for (int col = 0; col < 128; ++col) {
                uint8_t current = rawgray(row, col);
                oledfb[pos++] = current;
            }
        }
        SSD1306_rotate((unsigned char *)oledfb, rot_buffer, 1024);
        SSD1306_fillData(rot_buffer);

        //usleep(1*1000); // frame limit

    }

    return 0;
}