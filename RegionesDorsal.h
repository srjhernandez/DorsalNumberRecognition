/* 
 * File:   RegionesDorsal.h
 * Author: jesus
 *
 * Created on 29 de mayo de 2016, 15:58
 */

#ifndef REGIONESDORSAL_H
#define REGIONESDORSAL_H

#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/core/utility.hpp"
#include "opencv2/objdetect/objdetect.hpp"

using namespace cv;


#define CV_FACE std::string("haarcascade_frontalface_alt.xml")

#define CV_UPPERBODY std::string("haarcascade_mcs_upperbody.xml")


class RegionesDorsal {
       
    struct participante{ 
        
        unsigned long int detect;
        cv::Rect ROI;
        std::vector<std::string> dorsal;
        
        participante(unsigned long int face, cv::Rect _roi){            
            detect = face;
            ROI = cv::Rect(_roi);
            dorsal.clear();           
        };     
        
    }; 
    
    struct Participantes_Foto{ 
        
        std::vector<cv::Rect> detecciones_face_upper;
        
        std::vector<participante> grupo;
        
        void clear(){
            detecciones_face_upper.clear();
            grupo.clear();
        }
        
    }; 
    
    
    public:

        RegionesDorsal();
        
        void setDorsalROI(cv::Mat const &img);
        
        long unsigned int size();
        
        bool regionHasDorsal(long unsigned int num);
        
        cv::Rect getROI(long unsigned int num);  
        
        cv::Rect getFaceUpper(long unsigned int num); 
        
        std::vector<std::string> getDorsalesOfROI(long unsigned int num);
        
        void setDorsal(long unsigned int po, std::vector<std::string> __dorsales_detect);
        
        virtual ~RegionesDorsal();

    private:
        
        void setUpperbodyROI(const int &rows, cv::Rect const & upperDetect, cv::Rect & ROIdetect);
                
        void setFaceROI(int rows, int cols, std::vector<cv::Rect> const &detecciones, std::vector<participante> &grupo);
         
        cv::Rect calcularDimensionROI(const int &rows, const int &cols, cv::Rect const & ROI);  
   
        void aplicarClasificador(cv::Mat const &Imagen, std::vector<cv::Rect> &patronesDetectados ,std::string PathXML, double scale, int minNeighbors , Size minSize , Size maxSize);
            
        Participantes_Foto Participantes;
        
        double scaleFace;

        int minNeighborsFace;

        Size minSizeFace;

        Size maxSizeFace;

        double scaleUpper;

        int minNeighborsUpper;

        Size minSizeUpper;

        Size maxSizeUpper;       

};

#endif /* REGIONESDORSAL_H */

