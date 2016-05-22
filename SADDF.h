/* 
 * File:   OCR_DORSAL.h
 * Author: jesus
 *
 * Created on 7 de abril de 2016, 19:51
 */

#ifndef OCR_DORSAL_H
#define OCR_DORSAL_H

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/text.hpp>
#include "opencv2/core/utility.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <iostream>
#define listNumbers ("0123456789")

#define RATECHARFAULT 0.50

using namespace cv;

using namespace cv::text;

template <class T>

class OCR_DORSAL {
    
    struct number{
        char c;
        int pos;  
        
        number(char caracter, int p ){
            
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
                numero.push_back(num.c);            
            }    
            std::cout<<"EL NUMERO ES:  --> "<<numero<<std::endl;
            return numero;
        };   
        
        void add(char *caracter, int pos){            
            
            if ( caracter == NULL ){
                charFault++;
            }
            else{            
                if ( std::string(listNumbers).find(caracter[0]) == std::string::npos ){    
                    charFault++;            
                } 
                else{               
                    
                    digitos.push_back( number(*caracter, pos) );
                }      
            }     
            
        };
              
        void ordenar(){            
            std::sort(digitos.begin(), digitos.end(), [](const number &left, const number &right){          
                return (left.pos < right.pos); });
        };
    };
    
    public:
       
        OCR_DORSAL();
       
        std::string ReconocerDorsal(const cv::Mat &imgBinaria, const std::vector<std::vector<cv::Point>> &contornoCaracteres);
       
        virtual ~OCR_DORSAL(){};
        
    private:
        
        void recognition(cv::Mat &dst); 
        
        std::vector<Mat> &detections;
        
        std::vector<std::string> &outputs;
        
        std::vector< std::vector<Rect> > &boxes;
        
        std::vector< std::vector<std::string> > &words;
        
        std::vector< std::vector<float> > &confidences;
        
        std::vector< Ptr<T> > &ocrs;
               
};

#endif /* TESSERACTOCRDORSAL_H */

