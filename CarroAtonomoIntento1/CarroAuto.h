#pragma once

#include <iostream>
#include <opencv2\opencv.hpp>
#include <math.h>
#include <Windows.h>
#include "SerialClass.h"
#include "Puntos.h"
#include <vector>

using namespace std;
using namespace cv;

class cCarroAuto {

	vector<Puntos*> Pun, linea;
	Serial* Arduino;
	char outputChars[2];

	Mat img, img1, imgHSV, imgThresholded;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	VideoCapture cap,cap1;
	const uchar* table;

	////////////////////////////////////////////////////////////////////////////
	int iLowH;
	int iHighH;

	int iLowS;
	int iHighS;

	int iLowV;
	int iHighV;
	///////////////////////////////////////////////////////////////////////////
public:

	cCarroAuto() {
		////////////////////////////////////////////////////////////////////////
		iLowH = 115;//115
		iHighH = 179;//179

		iLowS = 0;//0
		iHighS = 255;//255

		iLowV = 0;//0
		iHighV = 255;//255
		////////////////////////////////////////////////////////////////////////

		Arduino = new Serial(L"COM3");
		outputChars[0] = 'S';
	}

	void Mover(Mat& img){
		uint8_t* pixelPtr = (uint8_t*)img.data;
		int cn = img.channels();
		Scalar_<uint8_t> bgrPixel;
		int contA = 0;
		int contD = 0;
		int contIzq = 0;
		int contDer = 0;

		for (int i = 0; i < img.rows; i++)
		{
			for (int j = 0; j < img.cols; j++)
			{
				bgrPixel.val[0] = pixelPtr[i*img.cols*cn + j*cn + 0]; // B
				bgrPixel.val[1] = pixelPtr[i*img.cols*cn + j*cn + 1]; // G
				bgrPixel.val[2] = pixelPtr[i*img.cols*cn + j*cn + 2]; // R
				if ((int)bgrPixel.val[0] == 255 && (int)bgrPixel.val[1] == 0 && (int)bgrPixel.val[2] == 0) {

				}
			}
		}
	}

	void MatImg(Mat& I) {

		uint8_t* pixelPtr = (uint8_t*)I.data;
		int cn = I.channels();
		Scalar_<uint8_t> bgrPixel;
	    int auxi=0,auxj=0;
		int contA = 0;
		int contD = 0;
		int contIzq = 0;
		int contDer = 0;

		for (int i = 0; i < I.rows; i++)
		{
			for (int j = 0; j < I.cols; j++)
			{
				bgrPixel.val[0] = pixelPtr[i*I.cols*cn + j*cn + 0]; // B
				bgrPixel.val[1] = pixelPtr[i*I.cols*cn + j*cn + 1]; // G
				bgrPixel.val[2] = pixelPtr[i*I.cols*cn + j*cn + 2]; // R

				if ((int)bgrPixel.val[0] == 0 && (int)bgrPixel.val[1] == 255 && (int)bgrPixel.val[2] == 0) {
					if (i<I.rows / 4 || i>I.rows - (I.rows / 4) && j > I.cols / 2) {
						contA = contA + 1;
						
						

						//Pun.push_back(new Puntos(i,j));
					}
					else if(j > I.cols / 2){
						contD = contD + 1;
						if (i > I.rows / 4 && i < I.rows / 2) {
							contIzq = contIzq + 1;
						}

						if (i>I.rows / 2 && i<I.rows - (I.rows / 4)) {
							contDer = contDer + 1;
						}
					}
		
				}

			}

			int aux = 0;
			//for (int k = 0; k < (int)Pun.size(); k++) {
				//if (Pun.at(k)->getJ() == Pun.at(k + 1)->getJ())
					//if (Pun.at(k)->getI() != Pun.at(k + 1)->getI())
						//linea.push_back(new Puntos((Pun.at(k)->getI() + Pun.at(k + 1)->getI())/2,Pun.at(k)->getJ()));
			//}

			//for (int i = 0; i < I.rows; i++)
			//{
			//	for (int j = 0; j < I.cols; j++)
			//	{
			//		for (int k = 0; k < (int)linea->size();k++)
			//			/*if ((int)linea->at(k)->getI() == i && (int)linea->at(k)->getJ() == j) {
			//				pixelPtr[i*I.cols*cn + j*cn + 0] = 255;
			//				pixelPtr[i*I.cols*cn + j*cn + 1] = 0;
			//				pixelPtr[i*I.cols*cn + j*cn + 2] = 0;
			//			}*/

			//	}
			//}
		}

		if (contA > contD) {
			cout << "abanza" << endl;
			outputChars[0] = 'W';
		}
		else {
			
			if (contDer > contIzq) {
				cout << "giro Izq" << endl;
				outputChars[0] = 'A';
				Arduino->WriteData(outputChars, sizeof(outputChars));
				Sleep(100);
			}
			else {
				cout << "giro Derecha" << endl;
				outputChars[0] = 'D';
				Arduino->WriteData(outputChars, sizeof(outputChars));
				Sleep(100);
			}
			cout << "Detente" << endl;
			outputChars[0] = 'S';
		}

		Arduino->WriteData(outputChars, sizeof(outputChars));

		I.data = pixelPtr;

	}

	void Color(Mat& img) {

		cvtColor(img, imgHSV, COLOR_BGR2HSV);
		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);

		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		Canny(imgThresholded, imgThresholded, 50, 200, 3);
		imshow("contornos", imgThresholded);
		DetecLineas(imgThresholded);
		
	}

	void DetecLineas(Mat& img){
		//cvtColor(img, img, CV_GRAY2BGR);
		/////////////////////////////////////////////////////////////////////////////////////////
		findContours(img, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		Mat drawing = Mat::zeros(img.size(), CV_8UC3);
		for (size_t i = 0; i< contours.size(); i++)
		{
			
			////////////////////////////////////////////////////////////////////////////
			double a = contourArea(contours[i], false);
			//if (a > 15) {
				//rectangle(drawing, boundingRect(contours[i]), Scalar(0, 255, 0), 2, 8, 0);
				Scalar color = CV_RGB(0, 255, 0);
				drawContours(drawing, contours, (int)i, color, 2, 8, hierarchy, 0, Point());
				//cout << "lineas" << endl;
				
			//}
			////////////////////////////////////////////////////////////////////////////
		}
		imshow("Cu", drawing);

		MatImg(drawing);

		////////////////////////////////////////////////////////////////////////////////////////
		
	}

	void Play() {
		cap.open(0);
		//cap1.open(2);
		for (;;) {
			cap.read(img);
			cap.read(img1);
			Color(img1);
			imshow("img1", img1);
			if (waitKey(1) >= 0)break;
		}
	}

};
