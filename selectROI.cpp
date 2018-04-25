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

CvPoint pt1 = cvPoint(0,0);    
CvPoint pt2 = cvPoint(0,0);

string dir_path = "./src/";  
string PREFIX = "";
string dst_name = "./dst/" + PREFIX + "%s";
string roiFileName = "./roi_fileList.txt";

// Define our callback which we will install for  
// mouse events.  
//  
void my_mouse_callback(  
    int event, int x, int y, int flags, void* param  
);  
  
CvRect box;  
CvPoint mousePos;
bool drawing_box = false;  
bool isRectDrawn = false;  
  
// A litte subroutine to draw a box onto an image_copy  
void draw_box( IplImage* img, CvRect box ) {  
    cvRectangle (  
        img,  
        cvPoint(box.x,box.y),  
        cvPoint(box.x+box.width,box.y+box.height),  
        cvScalar(0x00,0x00,0xff) /* blue */  
    );  
}  
  
void draw_box_green( IplImage* img, CvRect box ) {  
    cvRectangle (  
        img,  
        cvPoint(box.x,box.y),  
        cvPoint(box.x+box.width,box.y+box.height),  
        cvScalar(0x00,0xff,0x00) /* green */  
    );  
}  
  
int main( int argc, char* argv[] ) {  
    if(argc == 2)
    {
        dir_path = argv[1];
    }
  
    box = cvRect(-1,-1,0,0);  

    ifstream infile;
    infile.open(roiFileName.c_str());
    if(!infile) cout<<"roi file not exist"<<endl;
    string str;
    vector<string> ve;
    vector<string>::iterator it;
    while(getline(infile,str))   //按行读取,遇到换行符结束
    {   
        char* buf = strdup(str.c_str());    
        char* tmpstr = strtok(buf, "\t");
        ve.push_back(tmpstr);
    }
    infile.close();
    cout << ve.size() << endl;

    Directory dir;  
    vector<string> fileNames = dir.GetListFiles(dir_path, "*", false);  

    for(int i=0; i < fileNames.size(); i++){
        string fileFullName = dir_path + fileNames[i];
        cout << "fileFullName:" << fileFullName << endl;

        it=find(ve.begin(),ve.end(),fileNames[i]);
         
        if (it!=ve.end())
        {
            continue;
        }

        IplImage* image_input = cvLoadImage(fileFullName.c_str());  
        IplImage* image = cvCloneImage( image_input );  
        IplImage* image_copy = cvCloneImage( image );  
        IplImage* temp = cvCloneImage( image_copy );  
        cvNamedWindow( "Src Image", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);  
        cvResizeWindow("Src Image", 1920, 1080);
        // Here is the crucial moment that we actually install  
        // the callback. Note that we set the value ‘param’ to  
        // be the image_copy we are working with so that the callback  
        // will have the image_copy to edit.  
        //  
        cvSetMouseCallback(  
            "Src Image",  
            my_mouse_callback,  
            (void*) image_copy  
        );  
        // The main program loop. Here we copy the working image_copy  
        // to the ‘temp’ image_copy, and if the user is drawing, then  
        // put the currently contemplated box onto that temp image_copy.  
        // display the temp image_copy, and wait 15ms for a keystroke,  
        // then repeat…  
        //  

        char text[80];    
        CvFont font;    
        cvInitFont(&font,CV_FONT_HERSHEY_PLAIN,1.0,1.0);

        ofstream of;
        of.open(roiFileName.c_str(), ios::app);
        if(!of) cout<<"open file error"<<endl;
        while( 1 ) {  
            //cvCopyImage( image_copy, temp );  
            cvCopy( image_copy, temp );  
            //cvRectangle(temp,pt1,pt2,cvScalar(255,255,255));
            sprintf(text,"roi = %d,%d,%d,%d",pt1.x,pt1.y,pt2.x,pt2.y);
            cvPutText(temp,text,cvPoint(10,20),&font,cvScalar(0,0,255));

            cvLine(temp, cvPoint(mousePos.x, 0), cvPoint(mousePos.x, temp->height), CV_RGB(0, 0, 255), 1, 8);  
            cvLine(temp, cvPoint(0, mousePos.y), cvPoint(temp->width, mousePos.y), CV_RGB(0, 0, 255), 1, 8);  
            if( drawing_box ) draw_box( temp, box );  
            cvShowImage( "Src Image", temp );  
            //if( cvWaitKey( 15 )==27 ) break;  
            char key = cvWaitKey(10);  
            if(key == 27) break;  
            if(isRectDrawn){  
                if(key == 's' || key == 'S'){  
                    // draw green box  
                    draw_box_green( image_copy, box );  
                    cvCopy( image_copy, image );  
                      
                    // save roi image  
                    char save_image_name[128];  
                    sprintf(save_image_name, dst_name.c_str(), fileNames[i].c_str());  
                    cvSetImageROI(image_input, box);  
                    cvSaveImage(save_image_name, image_input);  
                    cvResetImageROI(image_input);  

                    of << PREFIX+fileNames[i] << "\t" << pt1.x << "," << pt1.y << "," << pt2.x << "," << pt2.y << endl;
                    of.flush();
                    
  
                    isRectDrawn = false;  
                }  
  
                if(key == 'q' || key == 'Q'){  
                    cvCopy( image, image_copy );  
                    isRectDrawn = false;  
                }  
            }  
        }  
        // Be tidy  
        //  
        cvReleaseImage( &image_input );  
        cvReleaseImage( &image );  
        cvReleaseImage( &image_copy );  
        cvReleaseImage( &temp );  
        cvDestroyWindow( "Src Image" );  

        of.close();
    }
}  
  
// This is our mouse callback. If the user  
// presses the left button, we start a box.  
// when the user releases that button, then we  
// add the box to the current image_copy. When the  
// mouse is dragged (with the button down) we  
// resize the box.  
//  
void my_mouse_callback(  
int event, int x, int y, int flags, void* param  
) {  
    IplImage* image_copy = (IplImage*) param;  
    switch( event ) {  
        case CV_EVENT_MOUSEMOVE: {  
            mousePos.x = x;  
            mousePos.y = y;  
            if( drawing_box ) {  
                box.width = x-box.x;  
                box.height = y-box.y;  
                pt2 = cvPoint(x,y); 
            }  
        }  
        break;  
        case CV_EVENT_LBUTTONDOWN: {  
            pt1 = cvPoint(x,y);    
            pt2 = cvPoint(x,y); 
            drawing_box = true;  
            box = cvRect(x, y, 0, 0);  
        }  
        break;  
        case CV_EVENT_LBUTTONUP: {  
            drawing_box = false;  
            isRectDrawn = true;  
            pt2 = cvPoint(x,y); 
            if(box.width<0) {  
                box.x+=box.width;  
                box.width *=-1;  
            }  
            if(box.height<0) {  
                box.y+=box.height;  
                box.height*=-1;  
            }  
            draw_box(image_copy, box);  
        }  
        break;  
    }  
}  
