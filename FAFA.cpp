/* 
 * File:   TesseractOCRDorsal.cpp
 * Author: jesus
 * 
 * Created on 7 de abril de 2016, 19:51
 */

#include "SADDF.h"
#include <tesseract/baseapi.h>

OCR

OCR_DORSAL::OCR_DORSAL() {    
    
}

/*-----------------------------------------------------------------------------*/

void OCR_DORSAL::recognition(cv::Mat &dst){     
    
    /*tesseract::TessBaseAPI tess; 
    tess.Init(NULL, "eng", tesseract::OEM_DEFAULT);
    tess.SetVariable("tessedit_char_whitelist", listNumbers);
    tess.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
    tess.SetImage((uchar*)dst.data, dst.cols, dst.rows, 1, dst.cols);    
    return (tess.GetUTF8Text());  */
    
    /*std::string vocabulary = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; 
    
     Ptr<OCRHMMDecoder::ClassifierCallback> ocr = loadOCRHMMClassifierCNN("OCRBeamSearch_CNN_model_data.xml");

    std::vector<int> out_classes;
    std::vector<double> out_confidences;

    ocr->eval(dst, out_classes, out_confidences);

    std::cout << "OCR output = \"" << vocabulary[out_classes[0]] << "\" with confidence "
         << out_confidences[0] << ". Evaluated in "
         << std::endl << std::endl;
    
     */
    
     Ptr<OCRTesseract> ocr = OCRTesseract::create(NULL, "eng", listNumbers, tesseract::OEM_DEFAULT, tesseract::PSM_SINGLE_CHAR);
     
    std::string output;
    std::vector<Rect> boxes;
    std::vector<std::string> words;
    std::vector<float> confidences;
    
    ocr->run(dst, output, &boxes, &words, &confidences, OCR_LEVEL_WORD);

    output.erase(remove(output.begin(), output.end(), '\n'), output.end());
    std::cout<<output;
     
}
/*-----------------------------------------------------------------------------*/

std::string OCR_DORSAL::ReconocerDorsal(const cv::Mat &imgBinaria, const std::vector<std::vector<cv::Point>> &contornoCaracteres)
{

    size_t numCaracteres = contornoCaracteres.size();
    
    OCR_DORSAL::vecNumber numero;
    
    std::vector<cv::Rect> boundRect(numCaracteres);    
    
    int maxNumCharFault = (double)numCaracteres * RATECHARFAULT;
   
    size_t i;   
    
    for (i = 0; i < numCaracteres; i++ ){
            
        char *caracter;
        
        boundRect[i] = cv::boundingRect(cv::Mat(contornoCaracteres[i]));
                         
        cv::Mat newImagen =  cv::Mat::zeros( imgBinaria.size(), CV_8UC1 );

        cv::drawContours( newImagen, contornoCaracteres, i, Scalar(255,255,255), -1, 8, std::vector<Vec4i>(), 0, Point() );

        cv::Mat segmentacion = newImagen & imgBinaria;       

        cv::Mat imagenSegment( segmentacion, boundRect[i]);
        
        cv::copyMakeBorder(imagenSegment.clone(), imagenSegment,15,15,15,15,BORDER_CONSTANT,Scalar(0));       
        
        cv::imshow("CARACTER",imagenSegment);
        cv::waitKey(1200);
        
        recognition(imagenSegment);   
        
        char c = 'c';
        
        caracter = &c;
        numero.add(caracter, boundRect[i].x);
        
        //if ( numero.charFault > maxNumCharFault ){                
            
         //   return std::string();
        //}             
    }
            
    numero.ordenar();

    return numero.get();  
    
}

/*-----------------------------------------------------------------------------*/

