
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdlib.h>
#include <cstdio>
#include <stdio.h>
#include <unistd.h>

#import <ApplicationServices/ApplicationServices.h>

#define COLS 8
#define ROWS 5

extern "C" {
    bool tapLeft() {

        FILE *file;
        file = fopen("/dev/cu.usbmodem1421","w");

        if (file == NULL)
            return false;

        sleep(1);
        fprintf(file, "%d", 0);
        
        fclose(file);

        return true;
    }
}

extern "C" {
    bool tapRight() {

        FILE *file;
        file = fopen("/dev/cu.usbmodem1421","w");

        if (file == NULL)
            return false;

        sleep(1);
        fprintf(file, "%d", 1);
        
        fclose(file);

        return true;
    }
}

float CalculateDistance(const cv::Point& pt1, const cv::Point& pt2) {
    float deltaX = pt1.x - pt2.x;
    float deltaY = pt1.y - pt2.y;
    return (deltaX * deltaX) + (deltaY * deltaY);
}

void markAdjacentSpaces(int g[ROWS][COLS], int r, int c) {

    //Mark top.
    if(r != 0) {
        if(g[r-1][c] == 0)
            g[r-1][c] = 3;
    }

    //Mark bottom.
    if(r != ROWS-1) {
        if(g[r+1][c] == 0)
            g[r+1][c] = 3;
    }

    //Mark left.
    if(c != 0) {
        if(g[r][c-1] == 0)
            g[r][c-1] = 3;
    }
    //Mark right.
    if(c != COLS-1) {
        if(g[r][c+1] == 0)
            g[r][c+1] = 3;
    }

}

bool searchLeft(int markedMap[ROWS][COLS], cv::vector<cv::vector<int> > spots, int pos[2], int &steps) {

    int r = pos[0];
    int c = pos[1];

    steps = 0;

    while(true) {
        //Check if currently at the spot.
        for(int i = 0; i < spots.size(); i++)
            if(spots.at(i).at(0) == r && spots.at(i).at(1) == c)
                return true;
        if(c != 0) {
            //Check left block.
            if(markedMap[r][c-1] == 3) {
                c--; steps++;
            }
            //If left block is occupied, check top left block.
            else if(markedMap[r][c-1] == 1) {
                if(r != 0) {
                    //Check top left.
                    if(markedMap[r-1][c-1] == 3) {
                        r--; c--; steps++;
                    }
                    //If top left is occupied, check top.
                    else if(markedMap[r-1][c-1] == 1) {
                        //Check top.
                        if(markedMap[r-1][c] == 3) {
                            r--; steps++;
                        }
                        //If top is occupied, something is wrong.
                        else {
                            std::cout << "Top block is occupied when it should not be." << std::endl;
                            return false;
                        }
                    }
                    else {
                        std::cout << "Unknown value for top left block. (" << markedMap[r-1][c-1] << ")" << std::endl;
                        return false;
                    }
                }
                else {
                    std::cout << "Row value is zero, cannot search left." << std::endl;
                    return false;
                }
            }
            //If left block is empty, check bottom.
            else if(markedMap[r][c-1] == 0) {
                if(r != ROWS-1) {
                    //Check bottom.
                    if(markedMap[r+1][c] == 3) {
                        r++; steps++;
                    }
                    //If bottom is occupied, check bottom left.
                    else if(markedMap[r+1][c] == 1) {
                        //Check bottom left.
                        if(markedMap[r+1][c-1] == 3) {
                            r++; c--; steps++;
                        }
                        else {
                            std::cout << "Bottom left block is occupied/empty when it should not be." << std::endl;
                            return false;
                        }
                    }
                    else {
                        std::cout << "Unknown value for bottom block. (" << markedMap[r+1][c] << ")" << std::endl;
                        return false;
                    }
                }
                else {
                    std::cout << "Row value is max, cannot turn left anymore." << std::endl;
                    return false;
                }
            }
            else {
                std::cout << "Unknown value for left block. (" << markedMap[r][c-1] << ")" << std::endl;
                return false;
            }
        }
        else {
            std::cout << "Column value is zero, cannot search left." << std::endl;
            return false;
        }
        
    }
}

bool searchRight(int markedMap[ROWS][COLS], cv::vector<cv::vector<int> > spots, int pos[2], int &steps) {

    int r = pos[0];
    int c = pos[1];

    steps = 0;

    while(true) {
        //Check if currently at the spot.
        for(int i = 0; i < spots.size(); i++)
            if(spots.at(i).at(0) == r && spots.at(i).at(1) == c)
                return true;
        if(c != COLS-1) {
            //Check right block.
            if(markedMap[r][c+1] == 3) {
                c++; steps++;
            }
            //If right block is occupied, check top right.
            else if (markedMap[r][c+1] == 1) {
                if(r != 0) {
                    //Check top right.
                    if(markedMap[r-1][c+1] == 3) {
                        c++; r--; steps++;
                    }
                    //If top right block is occupied, check top.
                    else if (markedMap[r-1][c+1] == 1) {
                        //Check top block.
                        if(markedMap[r-1][c] == 3) {
                            r--; steps++;
                        }
                        else {
                            std::cout << "Top block is occupied/empty when it should not be." << std::endl;
                            return false;
                        }
                    }
                    else {
                        std::cout << "Unknown value for top right block. (" << markedMap[r-1][c+1] << ")" << std::endl;
                        return false;
                    }
                }
                else {
                    std::cout << "Row value is zero, cannot search upwards." << std::endl;
                    return false;
                }
            }
            //If right block is empty, check bottom block.
            else if (markedMap[r][c+1] == 0) {
                if (r != ROWS-1) {
                    //Check bottom block.
                    if (markedMap[r+1][c] == 3) {
                        r++; steps++;
                    }
                    //If bottom block is occupied, check bottom right block.
                    else if (markedMap[r+1][c] == 1) {
                        //Check bottom right block.
                        if (markedMap[r+1][c+1] == 3) {
                            r++; c++; steps++;
                        }
                        else {
                            std::cout << "Bottom right block is occupied/empty when it should not be." << std::endl;
                            return false;
                        }
                    }
                    else {
                        std::cout << "Unknown value for bottom block. (" << markedMap[r+1][c] << ")" << std::endl;
                        return false;
                    }
                }
                else {
                    std::cout << "Row value at max, cannot search right." << std::endl;
                    return false;
                }
            }
            else {
                std::cout << "Unknown value for right block. (" << markedMap[r][c+1] << ")" << std::endl;
                return false;
            }
        }
        else {
            std::cout << "Column value is max, cannot search right." << std::endl;
            return false;
        }
    }
}

int findShortestPath(int given[ROWS][COLS], cv::vector<cv::vector<int> > spots) {

    //Steps.
    int steps;

    //Player position.
    int playerpos[2];

    //Make a copy of the given array.
    int gc[ROWS][COLS];
    for(int i = 0; i < ROWS; i++)
        for(int j = 0; j < COLS; j++) {
            gc[i][j] = given[i][j];
            if(gc[i][j] == 2) {
                playerpos[0] = i;
                playerpos[1] = j;
            }
        }
    
    //Mark empty spots on the bottom row as possible.
    for(int j = 0; j < COLS; j++)
        if(gc[ROWS-1][j] == 0)
            gc[ROWS-1][j] = 3;

    for(int i = 0; i < ROWS; i++)
        for(int j = 0; j < COLS; j++)
            if(gc[i][j] == 1)
                markAdjacentSpaces(gc, i, j);
    
    //Pathfinding - left.
    if(searchLeft(gc, spots, playerpos, steps))
        return steps;

    //Pathfinding - right.
    if(searchRight(gc, spots, playerpos, steps))
        return steps;
    
    //Not a possible number of steps so you know something's wrong.
    return 1000;

}

cv::vector<cv::vector<int> > findAvailableHoles(int wall[][COLS], int hero[][COLS]) {
    cv::vector<cv::vector<int> > availableHoles;

    for (int i = 0; i < ROWS; i++) {
        for (int l = 0; l < COLS; l++) {

            if (wall[i][l] == 0 && hero[i][l] == 0) {
                
                if ((i < ROWS - 1 && wall[i + 1][l] == 1) || 
                    (i > 0 && wall[i - 1][l] == 1) || 
                    (l < COLS - 1 && wall[i][l + 1] == 1) || 
                    (l > 0 && wall[i][l - 1] == 1)) {

                    cv::vector<int> coordinates;
                    coordinates.push_back(i);
                    coordinates.push_back(l);
                    availableHoles.push_back(coordinates);

                }

            }

        }
    }

    return availableHoles;

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

        //Crop the hero wall
        cv::Point h1(croppedImage.cols / 7, croppedImage.rows * 4.4 / 6);
        cv::Point h2(croppedImage.cols * 5.65 / 7, croppedImage.rows * 4.4 / 6);

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

        int gridWall[ROWS][COLS];

        double wallDistHorizontal = (cr3.x - cr1.x) / static_cast<double>(COLS);
        double wallDistVertical = (cr2.y - cr1.y) / static_cast<double>(ROWS);

        double xWallPos = cr1.x - wallDistHorizontal / 2.0;
        double yWallPos = cr1.y + wallDistVertical / 2.0 - wallDistVertical / 15;

        for (int i = 0; i < ROWS; i++) {
            for (int l = 0; l < COLS; l++) {
                xWallPos += wallDistHorizontal;
                cv::Vec3b bgrPixel = croppedImage.at<cv::Vec3b>(cv::Point(xWallPos, yWallPos));
                for(int z = 0; z < 3; z++) std::cout << "(" << i << ", " << l << ", " << z << ") BGR Value: " << static_cast<int>(bgrPixel[z]) << std::endl;
                circle(croppedImage, cv::Point(xWallPos, yWallPos), 1, cv::Scalar(255, 0, 255), 8, 1, 0);
                gridWall[i][l] = (static_cast<int>(bgrPixel[0]) > 200 && static_cast<int>(bgrPixel[1]) > 200 && static_cast<int>(bgrPixel[2]) > 200);
            }
            xWallPos = cr1.x - wallDistHorizontal / 2.0;
            yWallPos = cr1.y + wallDistVertical / 2.0 + wallDistVertical * (i + 1) - wallDistVertical / 25 * (ROWS - i);
        }

        std::cout << "Wall grid:\n";
        for (int i = 0; i < ROWS; i++) {
            std::cout << "---------------------------------\n|";
            for (int l = 0; l < COLS; l++) {
                std::cout << " " << gridWall[i][l] << " |";
            }
            std::cout << "\n";
        }
        std::cout << "---------------------------------\n\n\n";

        int gridHero[ROWS][COLS];

        double heroDistHorizontal = (h2.x - h1.x) / static_cast<double>(COLS - 2);
        double heroDistVertical = croppedImage.rows / 17;

        double xHeroPos = h1.x - heroDistHorizontal / 2.0;
        double yHeroPos = h1.y - heroDistVertical / 2.0;

        for (int i = 0; i < ROWS; i++) {
            for (int l = 1; l < COLS - 1; l++) {
                xHeroPos += heroDistHorizontal;
                cv::Vec3b bgrPixel = croppedImage.at<cv::Vec3b>(cv::Point(xHeroPos, yHeroPos));

                if ((static_cast<int>(bgrPixel[0]) > 200 && static_cast<int>(bgrPixel[1]) > 200 && static_cast<int>(bgrPixel[2]) > 200)) {
                    gridHero[ROWS - i - 1][l] = 1;
                }else if ((static_cast<int>(bgrPixel[0]) > 45 && static_cast<int>(bgrPixel[0]) < 65 && static_cast<int>(bgrPixel[1]) > 185 && static_cast<int>(bgrPixel[1]) < 195 && static_cast<int>(bgrPixel[2]) > 245 && static_cast<int>(bgrPixel[2]) < 255)) {
                    gridHero[ROWS - i - 1][l] = 2;
                }else{
                    gridHero[ROWS - i - 1][l] = 0;
                }

                //circle(croppedImage, cv::Point(xHeroPos, yHeroPos), 1, cv::Scalar(255, 0, 255), 8, 1, 0);
            }
            xHeroPos = h1.x - heroDistHorizontal / 2.0;
            yHeroPos = h1.y - heroDistVertical / 2.0 - heroDistVertical * (i + 1);
        }

        std::cout << "Hero grid:\n";
        for (int i = 0; i < ROWS; i++) {
            std::cout << "---------------------------------\n|";
            for (int l = 0; l < COLS; l++) {
                std::cout << " " << gridHero[i][l] << " |";
            }

            std::cout << "\n";
        }
        std::cout << "---------------------------------\n\n\n";


        cv::vector<cv::vector<int> > availableHoles = findAvailableHoles(gridWall, gridHero);

        for (int i = 0; i < (int)availableHoles.size(); i++) {
            std::cout << availableHoles.at(i).at(0) << " ";
            std::cout << availableHoles.at(i).at(1) << std::endl;
        }

        std::cout << "Number of steps: " << findShortestPath(gridHero, availableHoles) << std::endl;

        //Displaying the screenshot
        cv::imshow("Display window", croppedImage); 
        break;
        //Waitkey delay for HighGUI to process event loops. (Important for the display window)
		if (cv::waitKey(1) >= 0) break;

    }

    cv::waitKey(0);
    return 0;
}