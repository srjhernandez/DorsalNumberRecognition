/* 
 * File:   RegionesDorsal.cpp
 * Author: jesus
 * 
 * Created on 29 de mayo de 2016, 15:58
 */

#include "RegionesDorsal.h"

RegionesDorsal::RegionesDorsal() {
    
    scaleFace = 1.15;
    
    minNeighborsFace = 2;
       
    scaleUpper = 1.20;
    
    minNeighborsUpper = 3;
    
    Size minSizeUpper();
    
    Size maxSizeUpper();
    
    Size minSizeFace();
    
    Size maxSizeFace();   
}

/*--------------------------------------------------------------------------------------*/
/*  Devuelte un objeto de la clase MatOfRect que contiene los patrones detectados en la 

 *  imagen pasada como primer parámetro.   */
/*--------------------------------------------------------------------------------------*/
    
void RegionesDorsal::aplicarClasificador(cv::Mat const &Imagen, std::vector<cv::Rect> &patronesDetectados ,std::string PathXML, double scale, int minNeighbors , Size minSize , Size maxSize){
          
    CascadeClassifier detector(PathXML);   
    
    detector.detectMultiScale(Imagen, patronesDetectados, scale, minNeighbors, 0 | CASCADE_SCALE_IMAGE, minSize , maxSize);
    
}


/*--------------------------------------------------------------------------------------*/
// Determina la región de interés a partir de la detección de caras/hombro.

// ROI. 
/*--------------------------------------------------------------------------------------*/

void RegionesDorsal::setUpperbodyROI(int const &rows, cv::Rect const & upperDetect, cv::Rect & ROIdetect){   

    double alto = 1.5;

    int dfHombros;

    double heightROI;
    
    int dfWidth;
    
    if ( upperDetect.height > 0 && upperDetect.width > 0 ){
    
        dfHombros = (upperDetect.height*0.25);

        heightROI = alto*upperDetect.height;

        dfWidth = upperDetect.width * 0.10;

        int newW = (upperDetect.width - 2*dfWidth);

        int wxPos = upperDetect.x + dfWidth;

        if ( newW <= 0 ) {

            newW = upperDetect.width;

            wxPos = upperDetect.x;
        }

        if(heightROI + upperDetect.height + upperDetect.y > rows){

            heightROI = rows - upperDetect.y - upperDetect.height + dfHombros - 3;

        }

        cv::Rect ROI = cv::Rect(wxPos, upperDetect.y + upperDetect.height - dfHombros, newW, heightROI);   

        if(ROI.height > 0 && ROI.width > 0){
            ROIdetect = cv::Rect(ROI);
        } 
        else{
            ROIdetect = cv::Rect();
        }
    }
    else{        
        ROIdetect = cv::Rect();        
    }
}


/*--------------------------------------------------------------------------------------*/
// Calcula las dimensiones de la región de interés en función del número máximo de filas y columnas

// de la imagen.
/*--------------------------------------------------------------------------------------*/
    
cv::Rect RegionesDorsal::calcularDimensionROI(const int &maxFilas, const int &maxColumnas, cv::Rect const & ROI){
        
    double xROI, yROI, heightROI, widthROI;

    double alto = 3.5;

    double anchoCuerpo = (ROI.width/2.5);

    double cuello = (ROI.height/2.5);

    xROI = ROI.x - anchoCuerpo;

    yROI = ROI.y + ROI.height + cuello;

    widthROI = ROI.width + 2*anchoCuerpo;

    heightROI = ROI.height*alto;   

    if(xROI < 0){

        xROI = 0;
    }
    if(yROI > maxFilas){

        yROI = maxFilas;

        heightROI = 0;

    }

    if((xROI +  widthROI) > maxColumnas){

        widthROI = maxColumnas - xROI;
    }
    if(yROI + heightROI > maxFilas){

        heightROI = maxFilas - yROI;
    }

    return cv::Rect(xROI,yROI,widthROI,heightROI);

}


/*--------------------------------------------------------------------------------------*/
// Determina la región de interés a partir de la detección de caras.

// ROI
/*--------------------------------------------------------------------------------------*/
    
void RegionesDorsal::setFaceROI(int rows, int cols, std::vector<cv::Rect> const &detecciones, std::vector<participante> &grupo){   
       
    long unsigned int ref = 0;
   
    for( std::vector<Rect>::const_iterator r = detecciones.begin(); r != detecciones.end(); r++ ){

        cv::Rect ROI = calcularDimensionROI( rows , cols , *r);     
        
        if(ROI.height > 0 && ROI.width > 0){
            
            grupo.push_back( participante(ref, ROI) );
                     
        } 
        ref++;
    }        
}

/*-----------------------------------------------------------------------------------------------------------*/

void RegionesDorsal::setDorsalROI(cv::Mat const &img){
       
       
    Participantes.clear();  
    
    bool FaceInUpper;
    
    aplicarClasificador(img,  Participantes.detecciones_face_upper, CV_FACE, scaleFace, minNeighborsFace , minSizeFace, maxSizeFace);
       
    if ( Participantes.detecciones_face_upper.size() == 0 ){
        
        aplicarClasificador(img, Participantes.detecciones_face_upper, CV_UPPERBODY, scaleUpper, minNeighborsUpper , minSizeUpper, maxSizeUpper);             
                     
        //setUpperbodyROI( img.rows , Participantes.detecciones_face_upper, face_upper_ROI);
        
        for ( long unsigned int t = 0; t < Participantes.detecciones_face_upper.size(); t++){
            
            cv::Rect face_upper_ROI;
            
            setUpperbodyROI( img.rows , Participantes.detecciones_face_upper[t], face_upper_ROI);
            
            Participantes.grupo.push_back(participante(t, face_upper_ROI));
        }
    
    }
    else{
        
        std::vector<cv::Rect> detecciones_Upperbody;
        
        setFaceROI(img.rows, img.cols, Participantes.detecciones_face_upper, Participantes.grupo);
        
        aplicarClasificador(img, detecciones_Upperbody, CV_UPPERBODY, scaleUpper, minNeighborsUpper , minSizeUpper, maxSizeUpper); 
               
        if ( detecciones_Upperbody.size() > 0){            
            
            long unsigned int dimFace = Participantes.detecciones_face_upper.size();
            
            for( cv::Rect upper : detecciones_Upperbody ) {
                    
                FaceInUpper = false;

                for( long unsigned int i = 0; i < dimFace; i++) {

                    cv::Rect intersect = Participantes.detecciones_face_upper[i] & upper;

                    if ( intersect.area() >= Participantes.detecciones_face_upper[i].area()){

                        FaceInUpper = true;
                        break;
                    }  
                }
                
                if ( !FaceInUpper ){

                    Participantes.detecciones_face_upper.push_back(cv::Rect(upper));
                    
                    long unsigned int pos = Participantes.detecciones_face_upper.size();
                    
                    cv::Rect upper_ROI;
                    
                    setUpperbodyROI(img.rows, upper, upper_ROI);                    
                    
                    Participantes.grupo.push_back( participante(pos-1, upper_ROI) );
                }                
            }        
        
        }    
    
    }   
            
}

/*-----------------------------------------------------------------------------------------------------------*/

void RegionesDorsal::setDorsal(long unsigned int pos, std::vector<std::string> __dorsales){    
    
    if ( pos < Participantes.grupo.size()){
        
        Participantes.grupo[pos].dorsal = __dorsales;
        
    }
    
}

/*-----------------------------------------------------------------------------------------------------------*/

long unsigned int RegionesDorsal::size(){    
    
    return Participantes.grupo.size();
    
}

/*-----------------------------------------------------------------------------------------------------------*/

cv::Rect RegionesDorsal::getROI(long unsigned int num){
    
    if ( num < Participantes.grupo.size() ){
        
        return Participantes.grupo[num].ROI;
    }
    else{
        return cv::Rect();
    }
    
}

/*-----------------------------------------------------------------------------------------------------------*/

bool RegionesDorsal::regionHasDorsal(long unsigned int num){
    
    if ( num < Participantes.grupo.size() ){
        
        return (Participantes.grupo[num].dorsal.size() > 0 );
    }
    else{
        return false;
    }
    
}


/*-----------------------------------------------------------------------------------------------------------*/

std::vector<std::string> RegionesDorsal::getDorsalesOfROI(long unsigned int num){
    
    if ( num < Participantes.grupo.size() ){
        
        return Participantes.grupo[num].dorsal;
    }
    else{
        return std::vector<std::string>();
    }
    
}

/*-----------------------------------------------------------------------------------------------------------*/


cv::Rect RegionesDorsal::getFaceUpper(long unsigned int num){
    
    if ( num < Participantes.grupo.size() ){
        
        return Participantes.detecciones_face_upper[Participantes.grupo[num].detect];
    }
    else{
        return cv::Rect();
    }
    
}

/*-----------------------------------------------------------------------------------------------------------*/


RegionesDorsal::~RegionesDorsal() {
}

