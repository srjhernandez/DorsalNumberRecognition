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

enum class Tipo_OCR { TESSERACT, OCRBEAMSEARCH, OCRHMM };


/*   Clase general para los OCR */

class OBJECT_OCR {
    
    public:

        virtual std::string ReconocerDorsal(cv::Mat &imgBinaria, std::vector<std::vector<cv::Point>> &contornoCaracteres) = 0;

        virtual ~OBJECT_OCR(){};

};


   
//  Clase Template 

template <class T> class OCR_DORSAL : public OBJECT_OCR  {
      
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
       
        int charFault = 0;
        
        std::string get(){            
             
            std::string numero;                        
            for(number num : digitos){              
                numero += num.c;            
            }             
            return numero;
        };   
        
        void add(std::string &caracter, int pos){  
                      
            if ( caracter.empty() || std::string(listNumbers).find(caracter) == std::string::npos ){                
                charFault++;            
            } 
            else{            

                digitos.push_back( number(caracter, pos) );
            } 
            caracter.clear();
        };
              
        void ordenar(){            
            std::sort(digitos.begin(), digitos.end(), [](const number &left, const number &right){          
                return (left.pos < right.pos); });
        };
    };
    
    
public:
    
    OCR_DORSAL(Ptr<T> _ocr);
    
    std::string ReconocerDorsal(cv::Mat &imgBinaria, std::vector<std::vector<cv::Point>> &contornoCaracteres);
    
    bool carga_OCR();
    
    virtual ~OCR_DORSAL();

private:

    void NumberRecognition(cv::Mat segmento, std::string &caracter); 
       
    Ptr<T> *OCR;
    
    std::vector<Rect> boxes;
    
    std::vector<std::string> words;
        
    std::vector<float> confidences;
    
};

#include "OCR_DORSAL.cpp"

#endif /* OCR_DORSAL_H */

