/* 
 * File:   DetectorImagen.h
 * Author: jesus
 *
 * Created on 13 de mayo de 2015, 17:32
 */

#ifndef DETECTORIMAGEN_H
#define	DETECTORIMAGEN_H

#include "opencv2/objdetect.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/core/mat.hpp"
#include "TesseractOCR.h"
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace cv;

class DetectorImagen {    
    
    public:

        DetectorImagen();

        Mat leerImagen(String pathToImg);

        void eqHistogramaGris( Mat imgSource , Mat imgDest );

        std::vector<Rect>  aplicarClasificador(const cv::Mat &Imagen, std::string PathXML, double scale, int minNeighbors , Size minSize , Size maxSize );

        void dibujarRectangulo(const cv::Mat & imagen, std::vector<Rect> detecciones, Scalar color);

        cv::Rect calcularDimensionROI(int maxFilas, int maxColumnas, cv::Rect ROI);

        std::vector<cv::Rect> setUpperbodyROI(const cv::Mat &imagen, const std::vector<cv::Rect> detecciones);

        std::vector<cv::Rect> setFaceROI(const cv::Mat &imagen, const std::vector<cv::Rect> detecciones);

        void runDetector(std::string fImagen , String pathOut);

        virtual ~DetectorImagen();
        
        static const Scalar ROJO;

        static const Scalar VERDE;

        static const Scalar AZUL;
        
        static const string CV_FACE; 

        static const string CV_UPPERBODY;


    private:

        double scaleFace;

        int minNeighborsFace;

        Size minSizeFace;

        Size maxSizeFace;

        double scaleUpper;

        int minNeighborsUpper;

        Size minSizeUpper;

        Size maxSizeUpper;

    };

#endif	/* DETECTORIMAGEN_H */

