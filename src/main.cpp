#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#import <ApplicationServices/ApplicationServices.h>
#include <stdlib.h> 

using namespace cv;
using namespace std;

int main( int argc, char** argv) {

    CGImageRef screenShot = CGDisplayCreateImage(CGMainDisplayID());//screencapture(1);//CGWindowListCreateImage(CGRectInfinite, kCGWindowListOptionOnScreenOnly, kCGNullWindowID, kCGWindowImageDefault);

    /*CFStringRef file = CFSTR("/Users/artemiygalkin/Programming/images.jpeg");
    CFStringRef type = CFSTR("../res/scrn.jpeg");
    CFURLRef urlRef = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, file, kCFURLPOSIXPathStyle, false);
    CGImageDestinationRef idst = CGImageDestinationCreateWithURL(urlRef, type, 1, NULL);
    CGImageDestinationAddImage(idst, screenShot, NULL );
    CGImageDestinationFinalize(idst);*/

}