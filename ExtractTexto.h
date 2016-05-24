/* 
 * File:   ExtractTexto.h
 * Author: jesus
 *
 * Created on 6 de abril de 2016, 17:50
 */

#ifndef EXTRACTTEXTO_H
#define EXTRACTTEXTO_H

#include "OCR_DORSAL.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/text.hpp>
#include "opencv2/core/utility.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <iostream>
#include <vector>

#define MAX_GROUP_BOXES 3

#define CV_FACE std::string("haarcascade_frontalface_alt.xml")

#define CV_UPPERBODY std::string("haarcascade_mcs_upperbody.xml")

#define ROJO Scalar(0,0,255)

#define VERDE Scalar(0,255,0)

#define AZUL Scalar(255,0,0)

using namespace cv;
using namespace cv::text;

class ExtractTexto {
       
    struct region{ 
        cv::Rect Reg;
        int numRect;
        int prioridad;  // En función del número de Rect superpuestos en una misma región
                        // donde es más probable localizar el dorsal. Se debe al análisis 
                        // del ROI en los distintos canales RGB.
        
        region(cv::Rect r , int n, int p){            
            Reg = r;
            numRect = n;
            prioridad = p;           
        };     
        
    };   
    
    struct regions{
        
        std::vector<region> reg;   
        
        void ordenarPrioritarios(){            
            std::sort(reg.begin(), reg.end(), [](const region &left, const region &right){          
                if( left.numRect == right.numRect ){
                    return (left.prioridad > right.prioridad);
                } 
                else{
                    return (left.numRect > right.numRect); 
                }             
            
            });
        };
              
        void copiarRegiones(std::vector<Rect> &vec ){
            vec.clear();
            for(unsigned i=0; i<reg.size(); i++){               
                vec.push_back(reg[i].Reg);                
            }
        };
    };
    
public:
    
    ExtractTexto();   
   
    void runExtract( std::string const pathToImg,  Tipo_OCR TIPO ); 

    virtual ~ExtractTexto();    
    
private:
    
    bool leerImagen( std::string const pathToImg );
    
    void liberarRecursosImagen();
         
    void aplicarClasificador(cv::Mat const &Imagen, std::vector<cv::Rect> &patronesDetectados ,std::string PathXML, double scale, int minNeighbors , Size minSize , Size maxSize);
    
    void setFaceROI(cv::Mat const &imagen, std::vector<cv::Rect> const &detecciones, std::vector<cv::Rect> & ROIdetect);
    
    void setUpperbodyROI(cv::Mat const &imagen, std::vector<cv::Rect> const &detecciones, std::vector<cv::Rect> & ROIdetect);

    cv::Rect calcularDimensionROI(int maxFilas, int maxColumnas, cv::Rect const & ROI);  
    
    void dibujarRectangulo(cv::Mat & imagen, std::vector<Rect> const &detecciones, Scalar color);   
       
    void RedimensionarImagen(cv::Mat const &source, int height, cv::Mat &dst);
    
    void PreprocesarImagen(cv::Mat const &source, cv::Mat &dst);
    
    void groups_draw(cv::Mat &src, std::vector<Rect> &groups);
    
    void Segmentar(cv::Mat const & source, std::vector<cv::Rect> &groups_boxes );
    
    void findSuperRect(std::vector<cv::Rect> const &locations, std::vector<cv::Rect> & new_locations);
    
    void BinarizarRegion(cv::Mat const &src, cv::Mat &imagenRegion, cv::Rect region);
    
    void DetectarCaracteres(cv::Mat const &caracteresROI, std::vector<std::vector<cv::Point>> &contornoCaracteres);
    
    void putEtiqueta(cv::Mat& im, std::vector<std::string> const labels, cv::Rect const ROI);
        
    void ReconocimientoDorsal(cv::Mat const & source, std::vector<cv::Rect> const &groups_boxes, std::vector<std::string> & dorsal );
       

    private:

        double scaleFace;

        int minNeighborsFace;

        Size minSizeFace;

        Size maxSizeFace;

        double scaleUpper;

        int minNeighborsUpper;

        Size minSizeUpper;

        Size maxSizeUpper;       
               
        cv::Mat imagenEntrada;
        
        cv::Mat imagenSalida;

        std::vector<cv::Rect> ROI_face;
        
        std::vector<cv::Rect> detecciones_face;

        std::vector<cv::Rect> ROI_upperbody;   
        
        std::vector<cv::Rect> detecciones_upperbody;
        
        std::vector<cv::Rect> cajasDeTextos;
        
        std::vector<std::string> dorsales; 
               
        OCR_DORSAL<OCRTesseract> *TESSERACT_DNR; 
        
        OCR_DORSAL<OCRBeamSearchDecoder> *OCRBeamSearchDecoder_DNR;
               
        OCR_DORSAL<OCRHMMDecoder> *OCRHMMDecoder_DNR; 
        
};

#endif /* EXTRACTTEXTO_H */

