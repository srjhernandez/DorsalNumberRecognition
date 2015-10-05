/* 
 * File:   main.cpp
 * Author: jesus
 *
 * Created on 13 de mayo de 2015, 17:27
 */

#include <cstdlib>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <tesseract/baseapi.h>
#include <vector>
#include "DetectorImagen.h"
#include "FiltroImagenes.h"
#include "TesseractOCR.h"

using namespace std;


int main(int argc, char** argv) {

    /*DetectorImagen detector;
        
    FiltroImagenes fimagenes(vector<std::string> {"jpg"});
    
    vector<std::string> Fimagenes = fimagenes.getFicherosImagenes("images");
    
    for( vector<string>::const_iterator r = Fimagenes.begin(); r != Fimagenes.end(); r++ ){
        
        detector.runDetector( *r,"prueba");
                
    }*/
    
    
   /*
    cv::Mat imgROI = cv::imread("images/scratchcard.png");
    
    TesseractOCR numberDetector;
   
    //numberDetector.runTesseractOCR(imgROI);        
    
    //numberDetector.runTOCR(imgROI, 0.25, 200, 100);
    
    cv::Mat imgROI1 = cv::imread("images/p1.png");
    //numberDetector.runTOCR(imgROI1, 0.5, 140, 5);
   //numberDetector.runTesseractOCR(imgROI1);;
    
    cv::Mat imgROI2 = cv::imread("images/p2.png");
    //numberDetector.runTOCR(imgROI2, 0.35, 225, 155);
    
    cv::Mat imgROI3 = cv::imread("images/p3.png");
    numberDetector.runTOCR(imgROI3, 0.2, 200, 200);
    
    cv::Mat imgROI4 = cv::imread("images/p4.png");
    //numberDetector.runTOCR(imgROI4, 0.75, 140, 0);
    
    cv::Mat tmp;
    
    
    cv::Mat imgROI5 = cv::imread("images/foto2.JPG");
    
    tmp = imgROI5.clone();
    cv::namedWindow("ORIGINAL");
    cv::imshow("ORIGINAL",tmp);
    
    cout<<tmp.size()<<endl;
    
    //numberDetector.runTOCR(imgROI5,  0.7, 30, 10);
    
    
    cv::Mat imgROI6 = cv::imread("images/p6.png");
    //numberDetector.runTOCR(imgROI6,  0.3, 150, 10);
    
    cv::Mat imgROI7 = cv::imread("images/p7.png");
    //numberDetector.runTOCR(imgROI7,  0.2, 110, 5);
    
    cv::Mat imgROI8 = cv::imread("images/p8.png");
    //numberDetector.runTOCR(imgROI8,  0.40, 130, 10);*/
   
    TesseractOCR numberDetector;
    
    cv::Mat img = cv::imread("imgpruebas/original.png");    
    cv::Mat img1 = cv::imread("imgpruebas/j1.png");
    cv::Mat img2 = cv::imread("imgpruebas/j2.png");
    cv::Mat img3 = cv::imread("imgpruebas/j3.png");
    cv::Mat img4 = cv::imread("imgpruebas/j4.png");
    cv::Mat img5 = cv::imread("imgpruebas/j5.png");
    cv::Mat img6 = cv::imread("imgpruebas/j6.png");
    cv::Mat img7 = cv::imread("imgpruebas/j7.png");
  
    cv::Mat imgROI1 = cv::imread("images/p1.png");
    
    //numberDetector.runTOCR(imgROI1, 0.5, 140, 5);
       
    //numberDetector.run(img);
    
    //numberDetector.run2(img3);
         
   
      
    DetectorImagen detector;
    
    
    detector.runDetector("foto2.JPG","prueba");
    
    
    return 0;
    
}

