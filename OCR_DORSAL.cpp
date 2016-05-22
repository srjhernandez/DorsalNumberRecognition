/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OCR_DORSAL.cpp
 * Author: jesus
 * 
 * Created on 18 de mayo de 2016, 19:49
 */

#include "OCR_DORSAL.h"

template <class T> 

void OCR_DORSAL<T>::recognition(cv::Mat segmento, std::string &caracter){
        
    OCR->run(segmento, caracter, &boxes, &words, &confidences, OCR_LEVEL_WORD); 
    
    caracter.erase(remove(caracter.begin(), caracter.end(), '\n'), caracter.end());   
       
}

/*-----------------------------------------------------------------------------*/

template <class T> 

std::string OCR_DORSAL<T>::ReconocerDorsal(cv::Mat &imgBinaria, std::vector<std::vector<cv::Point>> &contornoCaracteres)
{

    size_t numCaracteres = contornoCaracteres.size();
    
    OCR_DORSAL::vecNumber numero;
    
    std::vector<cv::Rect> boundRect(numCaracteres);   
    
    std::string digit_numero;
    
    int maxNumCharFault = (double)numCaracteres * RATECHARFAULT;
   
    size_t i;  
        
    for (i = 0; i < numCaracteres; i++ ){
              
        boundRect[i] = cv::boundingRect(cv::Mat(contornoCaracteres[i]));
                         
        cv::Mat newImagen =  cv::Mat::zeros( imgBinaria.size(), CV_8UC1 );

        cv::drawContours( newImagen, contornoCaracteres, i, Scalar(255,255,255), -1, 8, std::vector<Vec4i>(), 0, Point() );

        cv::Mat segmentacion = newImagen & imgBinaria;       

        cv::Mat imagenSegment( segmentacion, boundRect[i]);
        
        cv::copyMakeBorder(imagenSegment, imagenSegment,15,15,15,15,BORDER_CONSTANT,Scalar(0));       
        
        namedWindow("CARACTER", WINDOW_NORMAL);
         
        cv::imshow("CARACTER",imagenSegment);
        cv::waitKey(1200);
               
        recognition(imagenSegment, digit_numero);   
                
        numero.add(digit_numero, boundRect[i].x);
        
        if ( numero.charFault > maxNumCharFault ){                
            
            return std::string();
        }             
    }
            
    numero.ordenar();

    return numero.get();  
    
}


