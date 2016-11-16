#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#import <ApplicationServices/ApplicationServices.h>
#include <stdlib.h>

int main( int argc, char** argv) {

    //Creating a window to display everything
    cv::namedWindow("Display window", CV_WINDOW_AUTOSIZE); // Create a window for display.

    int n = 0;

    cv::Vec3b testPixel1;
    cv::Vec3b testPixel2;
    cv::Vec3b testPixel3;
    cv::Vec3b testPixel4;

    while (true) {
        n++;
        //if (n++ % 10 != 0) continue;


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
        double xPos = screen.cols * 2 / 3;
        double yPos = 0;
        double width = screen.cols - xPos;
        double height = screen.rows - yPos;

        cv::Rect cropRect(xPos, yPos, width, height);

        //Crop the image
        cv::Mat croppedImage = screen(cropRect);

        //Skip the frame if we are not playing
        cv::Vec3b bgrPixel1 = croppedImage.at<cv::Vec3b>(cv::Point(croppedImage.cols / 9, croppedImage.rows / 23));
        cv::Vec3b bgrPixel2 = croppedImage.at<cv::Vec3b>(cv::Point(croppedImage.cols / 2.8, croppedImage.rows / 28.5));
        //circle(croppedImage, cv::Point(croppedImage.cols / 9, croppedImage.rows / 23), 1, cv::Scalar(0, 255, 0), 8, 1, 0);
      	//circle(croppedImage, cv::Point(croppedImage.cols / 2.8, croppedImage.rows / 28.5), 1, cv::Scalar(0, 255, 0), 8, 1, 0);

        if (n == 1) {
            testPixel1 = bgrPixel1;
            testPixel2 = bgrPixel2;
            continue;
        }

        if (testPixel1 == bgrPixel1 && testPixel2 == bgrPixel2) continue;

        //Skip all frames except for the one when the wall is at certain position
        cv::Vec3b bgrPixel3 = croppedImage.at<cv::Vec3b>(cv::Point(croppedImage.cols / 3, croppedImage.rows / 3));
        cv::Vec3b bgrPixel4 = croppedImage.at<cv::Vec3b>(cv::Point(croppedImage.cols * 2 / 3.3, croppedImage.rows / 3));

        if (static_cast<int>(bgrPixel3[0]) != 223 && static_cast<int>(bgrPixel3[1]) != 223 && 
            static_cast<int>(bgrPixel3[2]) != 223 && static_cast<int>(bgrPixel4[0]) != 223 && 
            static_cast<int>(bgrPixel4[1]) != 223 && static_cast<int>(bgrPixel4[2]) != 223) {
                continue;
        }

        //Convert Image to Gray
        cv::Mat grayImage;
        // Convert the original image from BGR to HSV
        cv::cvtColor(croppedImage, grayImage, cv::COLOR_BGR2HSV);

        // Convert HSV image Gray
        cv::Mat hsvChannels[3];
        cv::split(grayImage, hsvChannels);
        grayImage = hsvChannels[2];

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
            //if (!cv::isContourConvex(approx)) continue;

            //Skip big objects
            //if (fabs(contourArea(contours[i])) > 400) continue;

            //Skip tiny objects
            if (fabs(contourArea(contours[i])) < 200) continue;

            //Skip if not a quadrilateral
            //if (approx.size() != 4) continue;

            //Paint points
            for (int i = 0; i < approx.size(); i++) {
                circle(croppedImage, approx[i], 1, cv::Scalar(0, 0, 255), 8, 1, 0);
            }

            //Draw polygon
			// for (int i = 0; i < approx.size() - 1; i++) {
			// 	cv::line(croppedImage, approx[i], approx[i + 1], cv::Scalar(255, 0, 0), 4);
			// }
            	
            // cv::line(croppedImage, approx[approx.size() - 1], approx[0], cv::Scalar(255, 0, 0), 4);

        }

        bool grid[6][6];

        for (int i = 0; i < 6; i++) {
            std::cout << "-------------------\n|";
            for (int l = 0; l < 6; l++) {
                std::cout << grid[i][l] << " |";
            }
            std::cout << "\n";
        }
        std::cout << "-------------------\n\n\n";

        //Displaying the screenshot
        cv::imshow("Display window", croppedImage); 
        break;
        //Waitkey delay for HighGUI to process event loops. (Important for the display window)
		if (cv::waitKey(1) >= 0) break;

    }

    cv::waitKey(0);
    return 0;
}