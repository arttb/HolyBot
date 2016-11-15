#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#import <ApplicationServices/ApplicationServices.h>
#include <stdlib.h> 

using namespace cv;
using namespace std;

int main( int argc, char** argv) {

    CGImageRef screenshot = CGDisplayCreateImage(CGMainDisplayID());

    CFURLRef url = CFURLCreateWithFileSystemPath(NULL, CFSTR("../res/screenshot.png"), kCFURLPOSIXPathStyle, false);
    
    if (!url){
        cout << "failed to create URL\n";
        return 0;
    }

    CGImageDestinationRef idst = CGImageDestinationCreateWithURL(url, kUTTypePNG, 1, NULL);
    CGImageDestinationAddImage(idst, screenshot, NULL);
    CGImageDestinationFinalize(idst);

    return 0;
}