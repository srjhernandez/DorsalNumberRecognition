/* 
 * File:   OCR_DORSAL.cpp
 * Author: jesus
 * 
 * Created on 18 de mayo de 2016, 19:49
 */
  
/*-----------------------------------------------------------------------------*/
template <class T> 

OCR_DORSAL<T>::OCR_DORSAL(Ptr<T> _ocr){
    
    OCR =  new Ptr<T>( _ocr );
}

/*-----------------------------------------------------------------------------*/

template <class T> 

void OCR_DORSAL<T>::NumberRecognition(cv::Mat segmento, std::string &caracter){
   
    OCR->get()->run(segmento, caracter, &boxes, &words, &confidences, OCR_LEVEL_WORD); 
    
    caracter.erase(remove(caracter.begin(), caracter.end(), '\n'), caracter.end());
           
}

/*-----------------------------------------------------------------------------*/

template <class T> 

bool OCR_DORSAL<T>::carga_OCR(){
    
    return ( this->OCR->get() || !this->OCR->empty() );
    
}

/*-----------------------------------------------------------------------------*/

template <class T> 

std::string OCR_DORSAL<T>::ReconocerDorsal(cv::Mat &imgBinaria, std::vector<std::vector<cv::Point>> &contornoCaracteres)
{
    
    if ( carga_OCR() ){
        
        size_t numCaracteres = contornoCaracteres.size();
    
        OCR_DORSAL::vecNumber numero;

        std::vector<cv::Rect> boundRect(numCaracteres);   

        std::string digit_numero;

        int maxNumCharFault = (double)numCaracteres * RATECHARFAULT;

        for (size_t i = 0; i < numCaracteres; i++ ){

            boundRect[i] = cv::boundingRect(cv::Mat(contornoCaracteres[i]));

            cv::Mat newImagen =  cv::Mat::zeros( imgBinaria.size(), CV_8UC1 );

            cv::drawContours( newImagen, contornoCaracteres, i, Scalar(255,255,255), -1, 8, std::vector<Vec4i>(), 0, Point() );

            cv::Mat segmentacion = newImagen & imgBinaria;       

            cv::Mat imagenSegment( segmentacion, boundRect[i]);

            cv::copyMakeBorder(imagenSegment.clone(), imagenSegment,15,15,15,15,BORDER_CONSTANT,Scalar(0));       

            NumberRecognition(imagenSegment, digit_numero); 
            
            numero.add(digit_numero, boundRect[i].x);       
           
            if ( numero.charFault > maxNumCharFault ){                

                return std::string();
            }           
        }

        numero.ordenar();
        
        std::string Num = numero.get();
        
        std::cout<<"DORSAL: "<<Num<<std::endl;
        return Num;       
    }
    else{
        
        return std::string();
    }
    
}

/*-----------------------------------------------------------------------------*/

template <class T> 

OCR_DORSAL<T>::~OCR_DORSAL(){
    
    if ( OCR )
        delete OCR;
    
    OCR = NULL;
    
}

