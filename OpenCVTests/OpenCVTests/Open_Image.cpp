#include "opencv2\highgui.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\objdetect\objdetect.hpp"
#include "opencv2/video/tracking.hpp"
#include <vector>
#include <stdio.h>
#include <Windows.h>
#include <iostream>


using namespace cv;
using namespace std;

int numAOIs = 0;
bool showGrid = false;

struct circleAoi {
	int centerX;
	int centerY;
	int radius;
};

int main(int argc, const char** argv)
{
	circleAoi list[1];
	numAOIs = 1;
	list[0].centerX = 375;
	list[0].centerY = 225;
	list[0].radius = 18;


	// add your file name
	VideoCapture cap("C:/Users/Cameron/Desktop/OpenCVTests/OpenCVTests/Red_Kettle.avi");


	Mat flow, frame;
	// some faster than mat image container
	Mat  flowUmat, prevgray;

	Mat img;
	Mat original;
	namedWindow("Testing", WINDOW_AUTOSIZE);

	for (;;)
	{

		bool Is = cap.grab();
		if (Is == false) {
			// if video capture failed
			cout << "Video Capture Fail" << endl;
			break;
		}
		else {



			

			// capture frame from video file
			cap.retrieve(img, CV_CAP_OPENNI_BGR_IMAGE);
			resize(img, img, Size(640, 480));

			// save original for later
			img.copyTo(original);

			// just make current frame gray
			cvtColor(img, img, COLOR_BGR2GRAY);


			// For all optical flow you need a sequence of images.. Or at least 2 of them. Previous                           //and current frame
			//if there is no current frame
			// go to this part and fill previous frame
			//else {
			// img.copyTo(prevgray);
			//   }
			// if previous frame is not empty.. There is a picture of previous frame. Do some                                  //optical flow alg. 

			if (prevgray.empty() == false) {

				// calculate optical flow 
				calcOpticalFlowFarneback(prevgray, img, flowUmat, 0.5, 3, 15, 3, 5, 1.2, 0);
				// copy Umat container to standard Mat
				flowUmat.copyTo(flow);


				if (showGrid)
				{
					// By y += 5, x += 5 you can specify the grid 
					for (int y = 0; y < original.rows; y += 5) {
						for (int x = 0; x < original.cols; x += 5)
						{
							// get the flow from y, x position * 10 for better visibility
							const Point2f flowatxy = flow.at<Point2f>(y, x) * 1;
							// draw line at flow direction
							line(original, Point(x, y), Point(cvRound(x + flowatxy.x), cvRound(y + flowatxy.y)), Scalar(255, 0, 0));
							// draw initial point
							circle(original, Point(x, y), 1, Scalar(0, 0, 0), -1);

						}

					}
				}

				//move aois
				for (int i = 0; i<numAOIs; i++)
				{
					const Point2f flowatxy = flow.at<Point2f>(list[i].centerY, list[i].centerX) * 1;
					list[i].centerX = cvRound(list[i].centerX + flowatxy.x);
					list[i].centerY = cvRound(list[i].centerY + flowatxy.y);
					circle(original, Point(list[i].centerX, list[i].centerY), list[i].radius, Scalar(255, 0, 0));
				}

				//if yes, move that aoi by the vector

				//OR, if using multiple points to define an aoi, move each point by the closest vector


				// draw the results
				
				imshow("Testing", original);

				// fill previous image again
				img.copyTo(prevgray);

			}
			else {

				// fill previous image in case prevgray.empty() == true
				img.copyTo(prevgray);

			}


			int key1 = waitKey(20);

		}
	}
}