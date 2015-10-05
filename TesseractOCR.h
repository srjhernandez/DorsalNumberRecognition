/* 
 * File:   TesseractOCR.h
 * Author: jesus
 *
 * Created on 18 de mayo de 2015, 16:35
 */

#ifndef TESSERACTOCR_H
#define	TESSERACTOCR_H

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/text.hpp"
#include <opencv2/photo/photo.hpp>
#include <tesseract/baseapi.h>


#include <iostream>
#include <vector>

using namespace std;
using namespace cv::text;

class TesseractOCR {  
    
    struct number{
        char c;
        int pos;  
        
        number(char &caracter, int &p ){
            
            c = caracter;
            pos = p;
        
        };        
    };
    
    struct vecNumber{
        
        std::vector<number> digitos; 
        
        void show(){            
            for(number num : digitos){        
                cout<<num.c;
            }  
            cout<<endl;
        };
        
        void ordenar(){            
             std::sort(digitos.begin(), digitos.end(), [](const number &left, const number &right){          
                 return (left.pos < right.pos); });
        };
    };
     
public:
    
    TesseractOCR();
    TesseractOCR(const TesseractOCR& orig);
    
    void rgb2cmyk(const cv::Mat& img, std::vector<cv::Mat>& cmyk);
    
    void brillo(cv::Mat& src, cv::Mat &dst);
    
    void runTesseractOCR(cv::Mat &imgROI);
    
    void runTOCR(cv::Mat &im0, const double factor, const int umbralV1,const int umbralV2);
    
    void runOCR(cv::Mat &im0, const double factor, const int umbralV1,const int umbralV2);
    
    void run(cv::Mat &imagen);
    
    void run2(cv::Mat &imagen);
    
    char * recognition(cv::Mat dst);
    
    std::vector<cv::Rect> detectBox(cv::Mat &imagen);
            
    virtual ~TesseractOCR();
    
private:
           
    void binarizarImagen(const cv::Mat &source, cv::Mat &dst);
    
    void detectorTextos( const cv::Mat &source, std::vector<cv::Rect> &groups_boxes );
    
    void reconocedorTextos(const cv::Mat &source, const std::vector<cv::Rect> &groups_boxes);
    
    void binarizarRegion(const cv::Mat &src, cv::Mat &imagenRegion, cv::Rect region);
    
    void detectarCaracteres(const cv::Mat &caracteresROI, std::vector<std::vector<cv::Point>> &contornoCaracteres);
    
    void reconocerCaracteres(const cv::Mat &imgBinaria, std::vector<std::vector<cv::Point>> &contornoCaracteres);
    
};

#endif	/* TESSERACTOCR_H */

