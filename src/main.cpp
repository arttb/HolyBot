#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#import <ApplicationServices/ApplicationServices.h>
#include <stdlib.h>

#define COLS 8
#define ROWS 5

float CalculateDistance(const cv::Point& pt1, const cv::Point& pt2){
    float deltaX = pt1.x - pt2.x;
    float deltaY = pt1.y - pt2.y;
    return (deltaX * deltaX) + (deltaY * deltaY);
}

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
        //circle(croppedImage, cv::Point(croppedImage.cols / 3, croppedImage.rows / 3), 1, cv::Scalar(0, 255, 0), 8, 1, 0);
        //circle(croppedImage, cv::Point(croppedImage.cols * 2 / 3.3, croppedImage.rows / 3), 1, cv::Scalar(0, 255, 0), 8, 1, 0);
        
        /*
        std::cout << static_cast<int>(bgrPixel3[0]) << std::endl;
        std::cout << static_cast<int>(bgrPixel3[1]) << std::endl;
        std::cout << static_cast<int>(bgrPixel3[2]) << std::endl;

        std::cout << static_cast<int>(bgrPixel4[0]) << std::endl;
        std::cout << static_cast<int>(bgrPixel4[1]) << std::endl;
        std::cout << static_cast<int>(bgrPixel4[2]) << std::endl;
        */

        if (static_cast<int>(bgrPixel3[0]) < 222 && static_cast<int>(bgrPixel3[1]) < 222 && 
            static_cast<int>(bgrPixel3[2]) < 222 && static_cast<int>(bgrPixel4[0]) < 222 && 
            static_cast<int>(bgrPixel4[1]) < 222 && static_cast<int>(bgrPixel4[2]) < 222) {
                continue;
        }

        //Crop the wall
        cv::Point c1(croppedImage.cols / 4, croppedImage.rows / 4.8);
        cv::Point c2(croppedImage.cols / 4, croppedImage.rows / 2.8);
        cv::Point c3(croppedImage.cols * 2.8 / 4, croppedImage.rows / 4.8);
        cv::Point c4(croppedImage.cols * 2.8 / 4, croppedImage.rows / 2.8);

        circle(croppedImage, c1, 1, cv::Scalar(0, 255, 255), 8, 1, 0);
        circle(croppedImage, c2, 1, cv::Scalar(0, 255, 255), 8, 1, 0);
        circle(croppedImage, c3, 1, cv::Scalar(0, 255, 255), 8, 1, 0);
        circle(croppedImage, c4, 1, cv::Scalar(0, 255, 255), 8, 1, 0);

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

        /*//Detect circles
        cv::vector<cv::Vec3f> circles;
        HoughCircles(grayImage, circles, CV_HOUGH_GRADIENT, 1, 25, 200, 100);

        for( size_t i = 0; i < circles.size(); i++ ){

            cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));

            circle(croppedImage, center, 1, cv::Scalar(0, 255, 0), 8, 1, 0);

        }*/

        //Canny Edge Detection
		cv::Mat cannyImage;
		Canny(blurredImage, cannyImage, 200, 100);

        //Find contours after using canny edge detection
        cv::vector<cv::vector<cv::Point> > contours;
        cv::findContours(cannyImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

        cv::Point cr1(0, 0);
        cv::Point cr2(0, 0);
        cv::Point cr3(0, 0);
        cv::Point cr4(0, 0);

        float distance1 = 999999;
        float distance2 = 999999;
        float distance3 = 999999;
        float distance4 = 999999;

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

            for (int i = 0; i < approx.size(); i++) {
                                circle(croppedImage, approx[i], 1, cv::Scalar(255, 0, 255), 8, 1, 0);
                float distance11 = CalculateDistance(c1, approx[i]);
                float distance22 = CalculateDistance(c2, approx[i]);
                float distance33 = CalculateDistance(c3, approx[i]);
                float distance44 = CalculateDistance(c4, approx[i]);

                if (distance11 < distance1) {
                    distance1 = distance11;
                    cr1 = approx[i];
                }

                if (distance22 < distance2) {
                    distance2 = distance22;
                    cr2 = approx[i];
                }

                if (distance33 < distance3) {
                    distance3 = distance33;
                    cr3 = approx[i];
                }

                if (distance44 < distance4) {
                    distance4 = distance44;
                    cr4 = approx[i];
                }

            }

            //Draw polygon
			// for (int i = 0; i < approx.size() - 1; i++) {
			// 	cv::line(croppedImage, approx[i], approx[i + 1], cv::Scalar(255, 0, 0), 4);
			// }
            	
            // cv::line(croppedImage, approx[approx.size() - 1], approx[0], cv::Scalar(255, 0, 0), 4);

        }

        bool gridWall[ROWS][COLS];

        double distHorizontal = (cr3.x - cr1.x) / static_cast<double>(COLS);
        double distVertical = (cr2.y - cr1.y) / static_cast<double>(ROWS);

        double xWallPos = cr1.x - distHorizontal / 2.0;
        double yWallPos = cr1.y + distVertical / 2.0 - distVertical / 15;

        for (int i = 0; i < ROWS; i++) {
            for (int l = 0; l < COLS; l++) {
                xWallPos += distHorizontal;
                cv::Vec3b bgrPixel = croppedImage.at<cv::Vec3b>(cv::Point(xWallPos, yWallPos));

                gridWall[i][l] = (static_cast<int>(bgrPixel[0]) > 200 && static_cast<int>(bgrPixel[1]) > 200 && static_cast<int>(bgrPixel[2]) > 200);

                circle(croppedImage, cv::Point(xWallPos, yWallPos), 1, cv::Scalar(255, 0, 255), 8, 1, 0);
            }
            xWallPos = cr1.x - distHorizontal / 2.0;
            yWallPos = cr1.y + distVertical / 2.0 + distVertical * (i + 1) - distVertical / 25 * (ROWS - i);
        }

        circle(croppedImage, cr1, 1, cv::Scalar(0, 0, 255), 8, 1, 0);
        circle(croppedImage, cr2, 1, cv::Scalar(0, 0, 255), 8, 1, 0);
        circle(croppedImage, cr3, 1, cv::Scalar(0, 0, 255), 8, 1, 0);
        circle(croppedImage, cr4, 1, cv::Scalar(0, 0, 255), 8, 1, 0);

        std::cout << "Wall grid:\n";
        for (int i = 0; i < ROWS; i++) {
            std::cout << "---------------------------------\n|";
            for (int l = 0; l < COLS; l++) {
                std::cout << " " << gridWall[i][l] << " |";
            }
            std::cout << "\n";
        }
        std::cout << "---------------------------------\n\n\n";

        //Displaying the screenshot
        cv::imshow("Display window", croppedImage); 
        break;
        //Waitkey delay for HighGUI to process event loops. (Important for the display window)
		if (cv::waitKey(1) >= 0) break;

    }

    cv::waitKey(0);
    return 0;
}