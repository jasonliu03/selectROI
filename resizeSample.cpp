// An example program in which the  
// user can draw boxes on the screen.  
//  
//#include <cv.h>  
//#include <highgui.h>  

#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
//#include "opencv2/imgproc/imgproc.hpp"  
//#include "opencv2/highgui/highgui.hpp"  

#include "stdio.h"
#include "iostream"
#include "fstream"
using namespace std;
using namespace cv;  
// Define our callback which we will install for  
// mouse events.  
//  

string dir_path = "./dst_onlineEnv/";  
string dst_fp = "./posdata/";  


int main( int argc, char* argv[] ) {  
    Directory dir;  
    vector<string> fileNames = dir.GetListFiles(dir_path, "*", false);  
    cout << "file numbers:" << fileNames.size() << endl;

    for(int i=0; i < fileNames.size(); i++){
        string fileFullName = dir_path + fileNames[i];
        cout << "fileFullName:" << fileFullName << endl;

        IplImage* image_input = cvLoadImage(fileFullName.c_str());  

        CvSize size;  
        size.width = 20;  
        size.height = 20; 
        IplImage* image_output = cvCreateImage(size, image_input->depth, image_input->nChannels);  
        cvResize(image_input, image_output, CV_INTER_LINEAR);  
        char save_image_name[128];  
        sprintf(save_image_name, "%s%s", dst_fp.c_str(), fileNames[i].c_str()); 
        cvSaveImage(save_image_name, image_output);  

        cvReleaseImage( &image_input );  
    }
}  
  
