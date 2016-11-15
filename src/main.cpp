#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#import <ApplicationServices/ApplicationServices.h>
#include <stdlib.h>

int main( int argc, char** argv) {

    //Creating a window to display everything
    cv::namedWindow("Display window", CV_WINDOW_AUTOSIZE); // Create a window for display.

    while (true) {

        //Taking a screenshot of the main display
        CGImageRef screenshot = CGDisplayCreateImage(CGMainDisplayID());

        //Creating URL to save the image (because I am not sure how to convert CGImageRef to cv::Mat)
        CFURLRef url = CFURLCreateWithFileSystemPath(NULL, CFSTR("../res/screenshot.png"), kCFURLPOSIXPathStyle, false);
        
        //Failed to create URL
        if (!url){
            std::cout << "failed to create URL\n";
            return 0;
        }

        //Saving the screenshot
        CGImageDestinationRef idst = CGImageDestinationCreateWithURL(url, kUTTypePNG, 1, NULL);
        CGImageDestinationAddImage(idst, screenshot, NULL);
        CGImageDestinationFinalize(idst);
        CGImageRelease(screenshot);

        std::cout << "Took a screenshot\n";

        //Read the image to cv::Mat
        cv::Mat screen = cv::imread("../res/screenshot.png");
        if (screen.empty()){
            std::cout << "Failed imread(): image not found\n";
        }

        //Rect for cropping the image
        double xPos = screen.cols * 3 / 4;
        double yPos = 0;
        double width = screen.cols - xPos;
        double height = screen.rows * 2 / 3 - yPos;

        cv::Rect cropRect(xPos, yPos, width, height);

        //Crop the image
        cv::Mat croppedImage = screen(cropRect);

        //Convert Image to Gray
        cv::Mat grayImage;
        cv::cvtColor(croppedImage, grayImage, cv::COLOR_BGR2GRAY);

        //Blur the Image
        cv::Mat blurredImage;
        cv::GaussianBlur(grayImage, blurredImage, cv::Size(3, 3), 0, 0 );

        //Canny Edge Detection
		cv::Mat cannyImage;
		Canny(blurredImage, cannyImage, 200, 100);

        //Find contours after using canny edge detection
        cv::vector<cv::vector<cv::Point> > contours;
        cv::findContours(cannyImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

        //Find Polygons
        for (int i = 0; i < contours.size(); i++){
            
            //Approximate contour with accuracy proportional to the contour perimeter
            cv::vector<cv::Point> approx;
            approxPolyDP(cv::Mat(contours[i]), approx, arcLength(cv::Mat(contours[i]), true) * 0.02, true);

            //Skip convex objects
            if (!cv::isContourConvex(approx)) continue;

            //Skip small objects
            if (fabs(contourArea(contours[i])) < 200) continue;

            //Skip if not a quadrilateral
            if (approx.size() != 4) continue;

            //Paint points
            for (int i = 0; i < approx.size(); i++) {
             	circle(croppedImage, approx[i], 1, cv::Scalar(0, 0, 255), 8, 1, 0);
            }

        }

        //Displaying the screenshot
        cv::imshow("Display window", croppedImage); 

        //It needs time to process images
        cv::waitKey(1);
    }

    cv::waitKey(0);
    return 0;
}