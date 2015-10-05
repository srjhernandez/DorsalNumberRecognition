/* 
 * File:   DetectorImagen.cpp
 * Author: jesus
 * 
 * Created on 13 de mayo de 2015, 17:32
 */

#include "DetectorImagen.h"

const string DetectorImagen::CV_FACE = "haarcascade_frontalface_alt.xml";

const string DetectorImagen::CV_UPPERBODY  = "haarcascade_mcs_upperbody.xml";

const Scalar DetectorImagen::ROJO = Scalar(255,0,0);

const Scalar DetectorImagen::VERDE = Scalar(0,255,0);

const Scalar DetectorImagen::AZUL = Scalar(0,0,255);

DetectorImagen::DetectorImagen() {
    
    scaleFace = 1.15;
    
    minNeighborsFace = 2;
       
    scaleUpper = 1.20;
    
    minNeighborsUpper = 3;
    
    Size minSizeUpper();
    
    Size maxSizeUpper();
    
    Size minSizeFace();
    
    Size maxSizeFace();
}

 /* Devuelve la imagen que lee desde la ruta pasada por parámetro. */

Mat DetectorImagen::leerImagen(String pathToImg){
        
    return imread(pathToImg);
}

/* Aplicar el histograma de nivelación a la imagen fuente, creando la de destino

 * sin eliminar la fuente. */
    
void DetectorImagen::eqHistogramaGris( Mat imgSource , Mat imgDest ){

    cvtColor(imgSource, imgDest, COLOR_BGR2GRAY);

    //Imgproc.equalizeHist(imgDest, imgDest);

}   

/*  Devuelte un objeto de la clase MatOfRect que contiene los patrones detectados en la 

 * imagen pasada como primer parámetro.   */
    
std::vector<Rect>  DetectorImagen::aplicarClasificador(const cv::Mat &Imagen, std::string PathXML, double scale, int minNeighbors , Size minSize , Size maxSize){

    CascadeClassifier detector(PathXML);

    std::vector<cv::Rect>  patronesDetectados;
    
    detector.detectMultiScale(Imagen, patronesDetectados, scale, minNeighbors, 0 | CASCADE_SCALE_IMAGE, minSize , maxSize);
           
    return patronesDetectados;
}

//Dibuja un rectángulo de color para cada una de las detecciones realizadas, en

//función del clasificador aplicado.
    
void DetectorImagen::dibujarRectangulo(const cv::Mat & imagen, std::vector<Rect> detecciones, Scalar color){

    for( vector<Rect>::const_iterator r = detecciones.begin(); r != detecciones.end(); r++ ){
        
        rectangle(imagen, cvPoint(r->x, r->y), cvPoint(r->x + r->width, r->y + r->height), color ,2);     
        
    }
    
} 

/*----------------------------------------------------------------------*/
    
cv::Rect DetectorImagen::calcularDimensionROI(int maxFilas, int maxColumnas, cv::Rect ROI){
        
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
    
/* ------------------------------------------------------------------------*/

std::vector<cv::Rect> DetectorImagen::setUpperbodyROI(const cv::Mat &imagen, const std::vector<cv::Rect> detecciones){   

    cv::Rect ROI; 
    
    std::vector<cv::Rect> ROIdetect;

    double alto = 1.5;

    int dfHombros;

    double heightROI;

    int maxFilas = imagen.rows;
    
    ROIdetect.clear();

    for( vector<Rect>::const_iterator r = detecciones.begin(); r != detecciones.end(); r++ ){
    
        dfHombros = (r->height*0.25);

        heightROI = alto*r->height;            

        if(heightROI + r->height + r->y > maxFilas){

            heightROI = maxFilas - r->y - r->height + dfHombros;

        }

        ROI = cv::Rect(r->x, r->y + r->height - dfHombros,r->width, heightROI);   
        
        if(ROI.height > 0 && ROI.width > 0){
            ROIdetect.push_back(ROI);
        }
        
    }
    
    return ROIdetect;

}

/* --------------------------------------------------------------------------*/
    
std::vector<cv::Rect> DetectorImagen::setFaceROI(const cv::Mat &imagen, const std::vector<cv::Rect> detecciones ){   

    cv::Rect ROI; 
    
    std::vector<cv::Rect> ROIdetect;
    
    ROIdetect.clear();
        
    for( vector<Rect>::const_iterator r = detecciones.begin(); r != detecciones.end(); r++ ){

        ROI = calcularDimensionROI( imagen.rows , imagen.cols , *r );     
        
        if(ROI.height > 0 && ROI.width > 0){
            
            ROIdetect.push_back(ROI);
            
        }
        
    }  
    
    return ROIdetect;
}
    
/* -----------------------------------------------------------------------*/

void DetectorImagen::runDetector(std::string fImagen , String pathOut){
    
    // Paso 1. Leer la imagen que va a ser tratada por el clasificador.

    Mat imagen = leerImagen("images/"+fImagen);             
    
    // Paso 2. Detectar objetos o patrones en la imagen cargada.

    vector<cv::Rect> detecciones_face = aplicarClasificador(imagen, CV_FACE, scaleFace, minNeighborsFace , minSizeFace, maxSizeFace);

    //vector<cv::Rect> detecciones_upperbody = aplicarClasificador(imagen, CV_UPPERBODY, scaleUpper, minNeighborsUpper , minSizeUpper, maxSizeUpper);             
    
    vector<cv::Rect> ROI_face = setFaceROI(imagen,detecciones_face);
    
    //vector<cv::Rect> ROI_upperbody = setUpperbodyROI(imagen,detecciones_upperbody);
    
    //dibujarRectangulo( imagen , detecciones_upperbody, ROJO );
    
    //dibujarRectangulo( imagen , ROI_upperbody, AZUL );
    
    // Paso 3. Dibujar rectángulos de color en la imagen con los patrones detectados.

    //dibujarRect(imagen, detecciones_face, VERDE);
    
    //dibujarRectangulo(imagen, detecciones_upperbody, ROJO);

   
    // Paso 4. Identificar el número de dorsal en cada ROI obtenido.
    TesseractOCR numberDetector;
    
    for(cv::Rect ROI: ROI_face){
        
        cv::Mat imagenROI(imagen,ROI);
        
        imshow("prueba",imagenROI);
        
        numberDetector.run2(imagenROI);
        
    }
    
     // Paso 5. Mostrar las imágenes           
    
    cv::waitKey(-1);
    
    imwrite("images/fp_"+fImagen, imagen); 
            
}

DetectorImagen::~DetectorImagen() {
}

