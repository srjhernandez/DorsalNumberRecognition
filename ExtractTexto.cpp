/* 
 * File:   ExtractTexto.cpp
 * Author: jesus
 * 
 * Created on 6 de abril de 2016, 17:50
 */

#include "ExtractTexto.h"

// Constructor de la clase ExtractTexto. Inicializació de las atributos.

ExtractTexto::ExtractTexto(){
    
    scaleFace = 1.15;
    
    minNeighborsFace = 2;
       
    scaleUpper = 1.20;
    
    minNeighborsUpper = 3;
    
    Size minSizeUpper();
    
    Size maxSizeUpper();
    
    Size minSizeFace();
    
    Size maxSizeFace();   
    
    Ptr<OCRTesseract> TESSERACT_OCR = OCRTesseract::create(NULL, "spa", listNumbers, tesseract::OEM_DEFAULT, tesseract::PSM_SINGLE_CHAR);
        
    std::string vocabulario = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    
    Mat transition_p;
    std::string filename = "OCRHMM_transitions_table.xml";
    FileStorage fs(filename, FileStorage::READ);
    fs["transition_probabilities"] >> transition_p;
    fs.release();

    Mat emission_p = Mat::eye(62,62,CV_64FC1);
    
    Ptr<OCRBeamSearchDecoder> OCRBeamSearch_Decoder = OCRBeamSearchDecoder::create(
                loadOCRBeamSearchClassifierCNN("OCRBeamSearch_CNN_model_data.xml.gz"),
                vocabulario, transition_p, emission_p, OCR_DECODER_VITERBI, 500);
        
    Ptr<OCRHMMDecoder> OCRHMMDec  = OCRHMMDecoder::create(
                                 loadOCRHMMClassifierNM("OCRHMM_knn_model_data.xml.gz"),
                                 vocabulario, transition_p, emission_p);
    
    TESSERACT_DNR = new OCR_DORSAL<OCRTesseract>( TESSERACT_OCR );  
    
    OCRBeamSearchDecoder_DNR = new OCR_DORSAL<OCRBeamSearchDecoder>( OCRBeamSearch_Decoder );
    
    OCRHMMDecoder_DNR = new OCR_DORSAL<OCRHMMDecoder>( OCRHMMDec );
   
}


// Liberación de los recursos asignados a las imágenes de entrada y salida

void ExtractTexto::liberarRecursosImagen(){
    
    imagenEntrada.release();
    imagenSalida.release();
    
}

/*--------------------------------------------------------------------------------------*/
// Inicializa la matriz de la imagen cargada y la imagen de salida.

// Devuelve si la imagen ha sido leída.
/*--------------------------------------------------------------------------------------*/

bool ExtractTexto::leerImagen(std::string const pathToImg){
        
       
    imagenEntrada = imread(pathToImg);
    
    if ( !imagenEntrada.empty() ){
        imagenSalida = imagenEntrada.clone();
        return true;
    }
    else{
        return false;
    }
}

/*--------------------------------------------------------------------------------------*/
/*  Devuelte un objeto de la clase MatOfRect que contiene los patrones detectados en la 

 *  imagen pasada como primer parámetro.   */
/*--------------------------------------------------------------------------------------*/
    
void ExtractTexto::aplicarClasificador(cv::Mat const &Imagen, std::vector<cv::Rect> &patronesDetectados ,std::string PathXML, double scale, int minNeighbors , Size minSize , Size maxSize){
    
    patronesDetectados.clear();
    
    CascadeClassifier detector(PathXML);   
    
    detector.detectMultiScale(Imagen.clone(), patronesDetectados, scale, minNeighbors, 0 | CASCADE_SCALE_IMAGE, minSize , maxSize);
    
}

/*--------------------------------------------------------------------------------------*/
//  Dibuja un rectángulo de color para cada una de las detecciones realizadas, en

//  función del clasificador aplicado.
/*--------------------------------------------------------------------------------------*/
    
void ExtractTexto::dibujarRectangulo(cv::Mat & imagen, std::vector<Rect> const &detecciones, Scalar color){

    for( std::vector<Rect>::const_iterator r = detecciones.begin(); r != detecciones.end(); r++ ){
        
        rectangle(imagen, cvPoint(r->x, r->y), cvPoint(r->x + r->width, r->y + r->height), color ,2);     
        
    }    
} 

/*--------------------------------------------------------------------------------------*/
// Determina la región de interés a partir de la detección de caras.

// ROI
/*--------------------------------------------------------------------------------------*/
    
void ExtractTexto::setFaceROI(cv::Mat const &imagen, std::vector<cv::Rect> const &detecciones, std::vector<cv::Rect> & ROIdetect){   
     
    ROIdetect.clear();   
        
    for( std::vector<Rect>::const_iterator r = detecciones.begin(); r != detecciones.end(); r++ ){

        cv::Rect ROI = calcularDimensionROI( imagen.rows , imagen.cols , *r);     
        
        if(ROI.height > 0 && ROI.width > 0){
            
            ROIdetect.push_back(ROI);            
        }                
    }        
}

/*--------------------------------------------------------------------------------------*/
// Determina la región de interés a partir de la detección de caras/hombro.

// ROI
/*--------------------------------------------------------------------------------------*/

void ExtractTexto::setUpperbodyROI(cv::Mat const &imagen, std::vector<cv::Rect> const &detecciones, std::vector<cv::Rect> & ROIdetect){   

    double alto = 1.5;

    int dfHombros;

    double heightROI;

    int maxFilas = imagen.rows;
    
    int dfWidth;
     
    ROIdetect.clear();

    for( std::vector<Rect>::const_iterator r = detecciones.begin(); r != detecciones.end(); r++ ){
    
        dfHombros = (r->height*0.25);

        heightROI = alto*r->height;
        
        dfWidth = r->width * 0.10;
        
        int newW = (r->width - 2*dfWidth);
       
        int wxPos = r->x + dfWidth;
       
        if ( newW <= 0 ) {
           
            newW = r->width;
           
            wxPos = r->x;
        }

        if(heightROI + r->height + r->y > maxFilas){

            heightROI = maxFilas - r->y - r->height + dfHombros - 3;

        }

        cv::Rect ROI = cv::Rect(wxPos, r->y + r->height - dfHombros, newW, heightROI);   
        
        if(ROI.height > 0 && ROI.width > 0){
            ROIdetect.push_back(ROI);
        }        
    } 
}

/*--------------------------------------------------------------------------------------*/
// Calcula las dimensiones de la región de interés en función del número máximo de filas y columnas

// de la imagen.
/*--------------------------------------------------------------------------------------*/
    
cv::Rect ExtractTexto::calcularDimensionROI(int maxFilas, int maxColumnas, cv::Rect const & ROI){
        
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

// La imagen pasada como primer parámetro se redimensiona a la altura pasada como segundo 

// parámetro, preservando su ratio, almacenada en el tercer parámetro de salida.

/*--------------------------------------------------------------------------------------*/

void ExtractTexto::RedimensionarImagen(cv::Mat const &source, int height, cv::Mat &dst){
    
    double p = ( (double)source.cols / source.rows );
      
    int width = height * p;
   
    cv::resize(source, dst, Size(  width ,  height ));  
}


/*--------------------------------------------------------------------------------------*/
// Preprocesamiento de la imagen de entrada para su posterior tratamiento, como paso previo

// a la etapa de segmentación.
/*--------------------------------------------------------------------------------------*/

void ExtractTexto::PreprocesarImagen(cv::Mat const &source, cv::Mat &dst){
   
    dst = source.clone();
}

/* --------------------------------------------------------------------------*/

void ExtractTexto::groups_draw(Mat &src, std::vector<Rect> &groups)
{
    for (int i=(int)groups.size()-1; i>=0; i--)
    {
        if (src.type() == CV_8UC3)
            rectangle(src,groups.at(i).tl(),groups.at(i).br(),Scalar( 0, 255, 255 ), 3, 8 );
        else
            rectangle(src,groups.at(i).tl(),groups.at(i).br(),Scalar( 255 ), 3, 8 );
    }
}


/*--------------------------------------------------------------------------------------*/
// Etapa de segmentación de la imagen con el fin de extraer localizaciones de la misma

// que cuenten con mayor probabilidad de hallar un dorsal.
/*--------------------------------------------------------------------------------------*/

void ExtractTexto::Segmentar( cv::Mat const & source, std::vector<cv::Rect> &groups_boxes ){
    
    std::vector<cv::Mat> channels;
          
    cv::text::computeNMChannels(source, channels);
    
    groups_boxes.clear();
    
    int cn = (int)channels.size();
        
    for (int c = 0; c < cn-1; c++)
        channels.push_back(channels[c]);    
   
    Ptr<cv::text::ERFilter> er_filter1 = createERFilterNM1(loadClassifierNM1("trained_classifierNM1.xml"), 11, 0.00005, 0.2, 0.8, true, 0.5);
    Ptr<cv::text::ERFilter> er_filter2 = createERFilterNM2(loadClassifierNM2("trained_classifierNM2.xml"), 0.5);
    
    std::vector< std::vector<cv::text::ERStat> > regions(channels.size());
    
    for (int c=0; c<(int)channels.size(); c++){
        er_filter1->run(channels[c], regions[c]); 
        er_filter2->run(channels[c], regions[c]);
    }
    
    std::vector< std::vector<Vec2i>> region_groups;    
    
    cv::text::erGrouping(source, channels, regions, region_groups, groups_boxes, ERGROUPING_ORIENTATION_HORIZ);
        
    cv::Mat imagClone = source.clone();
       
    findSuperRect(groups_boxes, groups_boxes);
    
    groups_draw(imagClone, groups_boxes);  
    
    namedWindow("FILTERING", WINDOW_NORMAL);
    
    imshow("FILTERING", imagClone);
    cv::waitKey(-1);
    
    er_filter1.release();
    er_filter2.release();
    regions.clear();
    
}


/*-----------------------------------------------------------------------------------------------------*/

// Determinación de prioridad de los segmentos hallados para establecer un criterio de discriminación.

// La probabilidd de los segmentos viene dada por el números de regiones coincidentes y contenidas 

// por otras. Devuelve un subconjunto de localizaciones ordenados por criterio de prioridad.

/*-----------------------------------------------------------------------------------------------------*/

void ExtractTexto::findSuperRect(std::vector<cv::Rect> const &locations, std::vector<cv::Rect> & new_locations) {
   
    unsigned j, k;
    
    ExtractTexto::regions regiones;
    
    int pos, NumRect, prioridad;
    
    int areaOriginal, areaRegion, areaIntersect;    
            
    for (j = 0; j < locations.size(); j++) {
             
        unsigned regionSize = regiones.reg.size();
        
        areaOriginal = locations[j].area();
        
        pos = -1;
        
        NumRect = 1;
        
        prioridad = 1;
        
        for ( k = 0; k < regionSize; k++ ){
               
            cv::Rect rectIntersect = locations[j] & regiones.reg[k].Reg;
                
            areaRegion = regiones.reg[k].Reg.area();

            areaIntersect = rectIntersect.area();
            
            if ( areaIntersect > 0.80 * areaRegion ){  //Original contiene a Reg
                   
                regiones.reg[k].prioridad++;
                
                if ( areaIntersect > 0.80 * areaOriginal && pos < 0 ){  //Reg contiene a Original                
                    pos = k;  
                    regiones.reg[k].numRect++;
                }                 
            } 
            else if( areaIntersect > 0.80 * areaOriginal ){ //Reg contiene a Original 
                
                prioridad +=  regiones.reg[k].numRect;
            }            
        }
        
        if ( pos >= 0 ) {          
            
            if ( regiones.reg[pos].Reg.area() < areaOriginal ){
                
                regiones.reg[pos].Reg = locations[j];
            
            }              
        }
        else{
            regiones.reg.push_back(region(locations[j], NumRect, prioridad));
        }           
    }
    
    regiones.ordenarPrioritarios();  
    
    regiones.copiarRegiones(new_locations);
    
}

/*-----------------------------------------------------------------------------------------------------*/

// Etapa de binarización de la región segmentada.

/*-----------------------------------------------------------------------------------------------------*/

void ExtractTexto::BinarizarRegion(cv::Mat const &src, cv::Mat &imagenRegion, cv::Rect region){
    
    if( region.x < 0 ){
        region.x = 0;
    }
    
    if( region.x > src.cols ){
        region.x = src.cols;
    }
    
    if( region.y < 0 ){
        region.y = 0;
    }
    
    if( region.y > src.rows ){
        region.y = src.rows;
    }
    
    if( region.x + region.width > src.cols ){
        region.width = src.cols - region.x;
    }
    
    if( region.y + region.height > src.rows ){
        region.y = src.rows - region.height;
    }
      
    cv::Mat original(src, region), gris, originalRZ, adaptative; 
    
    RedimensionarImagen(original, 300, originalRZ);
            
    cv::imshow("Original", originalRZ);
                   
    cv::cvtColor(originalRZ, gris, CV_BGR2GRAY);
                
    cv::imshow("gris",gris);
                   
    cv::adaptiveThreshold(gris, adaptative, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 401, 30);     
    
    RedimensionarImagen(adaptative, 200, imagenRegion);
    
    cv::imshow("Imagen Region",imagenRegion);
    cv::waitKey(-1);
}


/*-----------------------------------------------------------------------------------------------------*/

// Etapa de detección y extracción de caracteres.

// Se extrae el contorno de cada carácter para ser tratado individualmente.

/*-----------------------------------------------------------------------------------------------------*/

void ExtractTexto::DetectarCaracteres(cv::Mat const &caracteresROI, std::vector<std::vector<cv::Point>> &contornoCaracteres){
    
    std::vector<std::vector<cv::Point>> contornos;  

    contornoCaracteres.clear();

    findContours( caracteresROI.clone(), contornos, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );

    for( size_t i = 0; i < contornos.size(); i++ ){ 
        if( cv::contourArea(contornos[i]) > 400 ){           
            contornoCaracteres.push_back(contornos[i]);       
        }
    } 
}


/*-----------------------------------------------------------------------------------------------------*/

// Reconocimiento del dorsal. Analiza cada región de interés (ROI) con el fin de capturar el dorsal.

// Devuelve un vector de hasta un máximo MAX_GROUP_BOXES de capturas ordenadas prioritariamente.

/*-----------------------------------------------------------------------------------------------------*/

void ExtractTexto::ReconocimientoDorsal(cv::Mat const & source, std::vector<cv::Rect> const &groups_boxes, std::vector<std::string> & dorsal ){
      
    dorsal.clear(); 
    
    std::vector<std::vector<cv::Point>> regionCaracteres;
    
    cv::Mat imagenBinaria;
   
    int j=0;       
    
    for( cv::Rect region : groups_boxes){
        
        if ( j++ >= MAX_GROUP_BOXES ){           
            break; 
        }            
                              
        BinarizarRegion(source, imagenBinaria, region); 
      
        DetectarCaracteres( imagenBinaria, regionCaracteres );          
                    
        std::string new_dorsal = OCRHMMDecoder_DNR->ReconocerDorsal(imagenBinaria, regionCaracteres);
              
        size_t t;
        
        if( !new_dorsal.empty() ){
                        
            for ( t=0; t< dorsal.size(); t++){
                
                if( dorsal[t] == new_dorsal){
                    break;
                }                
            }  
            if( t == dorsal.size()){            
                dorsal.push_back(new_dorsal);
            } 
        }               
    }  
}


/*-----------------------------------------------------------------------------------------------------*/

// Añade etiquetas a la imagen con los dorsales detectados. Imprime como máximo MAX_GROUP_BOXES etiquetas

// ordenadas prioritariamente en función de la mayor probabilidad de éxito en cuanto al reconocimiento del dorsal.

// El borde rojo de la etiqueta indica que el número obtenido tiene una mayor probabilidad de coincidir con el dorsal.

/*-----------------------------------------------------------------------------------------------------*/

void ExtractTexto::putEtiqueta(cv::Mat& im, const std::vector<std::string> labels, const cv::Rect ROI)
{
    int fontface = cv::FONT_HERSHEY_SIMPLEX;
    int baseline = 2; 
    int thickness = 1;
    
    const int borde = 4;
    const int alturaLabel = (double)ROI.height*0.06;
    const int alturaRectangulo =  alturaLabel + 2*borde;    
    const int anchoRectangulo = ROI.width - 6;
    int NewWidth, NewHeight; 
    int alfa, pos;
    double scale;
    
    for ( long unsigned i = 0; i < labels.size(); i++ ){
        
        cv::Size text = cv::getTextSize(labels[i], fontface, 1, thickness, &baseline);
                
        scale = (double)alturaLabel/text.height;
        
        NewWidth = scale *text.width;        
        
        if ( NewWidth > anchoRectangulo ){
            
            scale = (double)anchoRectangulo/text.width;
            
            NewHeight = scale*text.height;
            
            alfa = (alturaLabel - NewHeight)/2.0;
            
            NewWidth = anchoRectangulo; 
            
            pos = 0;
        }
        else{
            NewHeight = alturaLabel;
            alfa = 0;
            pos = (anchoRectangulo - NewWidth)/2.0;
        }
                
        cv::Point origin(ROI.x + 3, ROI.y + ROI.height - 3 - (alturaRectangulo+2)*i);   
        
        if ( origin.y - alturaRectangulo < 0)
            break;

        cv::rectangle(im, origin, origin + cv::Point(anchoRectangulo, -alturaRectangulo), AZUL, CV_FILLED);
        
        if (i==0)
            cv::rectangle(im, origin + cv::Point(1,-1), origin + cv::Point(anchoRectangulo, -alturaRectangulo) + cv::Point(-1,1), ROJO, 2);

        cv::putText(im, labels[i], origin - cv::Point(-pos, borde + alfa), fontface, scale, CV_RGB(255,255,255), thickness, CV_AA);
       
    }
    
}

/* --------------------------------------------------------------------------*/

void ExtractTexto::runExtract(std::string const path){
    
    // Paso 1. Leer la imagen objetivo
     
    if ( !leerImagen(path) ) 
        return;
     
    
    // Paso 2.  Obtener Regiones de Interés en la imagen cargada, donde es probable localizar
    //          dorsales.
    
    aplicarClasificador(imagenEntrada,  detecciones_face, CV_FACE, scaleFace, minNeighborsFace , minSizeFace, maxSizeFace);
   
    setFaceROI(imagenEntrada, detecciones_face, ROI_face); 
    
    aplicarClasificador(imagenEntrada, detecciones_upperbody, CV_UPPERBODY, scaleUpper, minNeighborsUpper , minSizeUpper, maxSizeUpper);             
    
    setUpperbodyROI(imagenEntrada, detecciones_upperbody, ROI_upperbody);
        
    
    // Paso 3.  Segmentar las ROIs obteniendo posibles cajas de texto conteniendo el número del dorsal
           
    for(cv::Rect ROI: ROI_upperbody){        
                  
        cv::Mat imagenROI(imagenEntrada, ROI), imagenPreprocesada;        
                
        PreprocesarImagen( imagenROI, imagenPreprocesada ); 
               
        Segmentar( imagenPreprocesada, cajasDeTextos );        
       
        ReconocimientoDorsal( imagenPreprocesada, cajasDeTextos, dorsales);
        
        putEtiqueta(imagenSalida, dorsales, ROI);
        
    }   
    
    
    // Paso 4. Dibujar rectángulos de color en la imagen con los patrones detectados.

    //dibujarRectangulo(imagenSalida, ROI_face, AZUL);
    
    //dibujarRectangulo(imagenSalida, detecciones_face, AZUL); 
    
    dibujarRectangulo(imagenSalida, ROI_upperbody, VERDE);
    
    dibujarRectangulo(imagenSalida, detecciones_upperbody, VERDE); 
      
    namedWindow("test", WINDOW_NORMAL);
        
    cv::imshow("test", imagenSalida);
    
    cv::waitKey(-1); 
    
    
    // Paso 5. Liberar la memoria asignada a las imágenes para su lectura.
    
    liberarRecursosImagen();
    
}


// Destructor de la clase

ExtractTexto::~ExtractTexto(){
    
    /*if ( TesseractDNR )
        delete TesseractDNR;
    
    TesseractDNR = NULL;*/
    
}