/* 
 * File:   ExtractTexto.h
 * Author: jesus
 *
 * Created on 6 de abril de 2016, 17:50
 */ 

#ifndef EXTRACTTEXTO_H
#define EXTRACTTEXTO_H

#include "RegionesDorsal.h"
//#include "OBJECTCLASS.h"


template <class T> class OCR_DORSAL;

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
   
    void dibujarRectangulo(cv::Mat & imagen, cv::Rect const &detecciones, Scalar color);   
    
    void imprimirDetecciones(cv::Mat &imagen, RegionesDorsal reg , Scalar color );
       
    void RedimensionarImagen(cv::Mat const &source, int height, cv::Mat &dst);
    
    void PreprocesarImagen(cv::Mat const &source, cv::Mat &dst);
    
    void groups_draw(cv::Mat &src, std::vector<Rect> &groups);
    
    void Segmentar(cv::Mat const & source, std::vector<cv::Rect> &groups_boxes );
    
    void findSuperRect(std::vector<cv::Rect> const &locations, std::vector<cv::Rect> & new_locations);
    
    void BinarizarRegion(cv::Mat const &src, cv::Mat &imagenRegion, cv::Rect region);
    
    void DetectarCaracteres(cv::Mat const &caracteresROI, std::vector<std::vector<cv::Point>> &contornoCaracteres);
    
    void putEtiqueta(cv::Mat& im, std::vector<std::string> const labels, cv::Rect const ROI);
        
    void ReconocimientoDorsal(cv::Mat const & source, std::vector<cv::Rect> const &groups_boxes, std::vector<std::string> & dorsal, OBJECT_OCR * _ocr );

    void InicializarOCR();    
    
    OBJECT_OCR * setTipoOCR( Tipo_OCR _tipoOCR );

    private:      
               
        cv::Mat imagenEntrada;
        
        cv::Mat imagenSalida;
                
        std::vector<cv::Rect> cajasDeTextos;
        
        std::vector<std::string> dorsales; 
        
        RegionesDorsal DNR_Regiones;
               
        OCR_DORSAL<OCRTesseract> *TESSERACT_DNR; 
        
        OCR_DORSAL<OCRBeamSearchDecoder> *OCRBeamSearchDecoder_DNR;
               
        OCR_DORSAL<OCRHMMDecoder> *OCRHMMDecoder_DNR; 
               
};

#endif /* EXTRACTTEXTO_H */

