/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OCR_DORSAL.h
 * Author: jesus
 *
 * Created on 18 de mayo de 2016, 19:49
 */

#ifndef OCR_DORSAL_H
#define OCR_DORSAL_H

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/text.hpp>
#include "opencv2/core/utility.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <tesseract/baseapi.h>
#include <iostream>

#define listNumbers ("0123456789")

#define RATECHARFAULT 0.50

using namespace cv;

using namespace cv::text;

template <class T> class OCR_DORSAL {
    
    struct number{
        std::string c;
        int pos;  
        
        number( std::string caracter, int p ){
            
            c = caracter;
            pos = p;
        
        };        
    };
    
    struct vecNumber{
        
        std::vector<number> digitos; 
        
        std::string numero;
        
        int charFault = 0;
        
        std::string get(){            
             
            numero.clear();
                        
            for(number num : digitos){              
                numero.append(num.c);            
            }    
            std::cout<<"EL NUMERO ES:  --> "<<numero<<std::endl;
            return numero;
        };   
        
        void add(std::string caracter, int pos){  
                      
            if ( std::string(listNumbers).find(caracter) == std::string::npos ){    
               
                charFault++;            
            } 
            else{            

                digitos.push_back( number(caracter, pos) );
            } 
        };
              
        void ordenar(){            
            std::sort(digitos.begin(), digitos.end(), [](const number &left, const number &right){          
                return (left.pos < right.pos); });
        };
    };
    
    
public:
    
    OCR_DORSAL(Ptr<T> _ocr){
    
        OCR = _ocr;
    };
    
    std::string ReconocerDorsal(cv::Mat &imgBinaria, std::vector<std::vector<cv::Point>> &contornoCaracteres);
     
    virtual ~OCR_DORSAL(){};

private:

    void recognition(cv::Mat segmento, std::string &caracter); 
       
    Ptr<T> OCR;
    
    std::vector<Rect> boxes;
    
    std::vector<std::string> words;
        
    std::vector<float> confidences;
    
};

#include "OCR_DORSAL.cpp"

#endif /* OCR_DORSAL_H */

