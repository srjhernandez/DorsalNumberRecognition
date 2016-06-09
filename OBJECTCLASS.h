
/* 
 * File:   OBJECTCLASS.h
 * Author: jesus
 *
 * Created on 2 de junio de 2016, 20:59
 */

#ifndef OBJECTCLASS_H
#define OBJECTCLASS_H

#include <opencv2/objdetect/objdetect.hpp>

using namespace cv;

enum class Tipo_OCR { TESSERACT, OCRBEAMSEARCH, OCRHMM };

/*   Clase general para los OCR */

class OBJECT_OCR {
    
    public:

        virtual std::string ReconocerDorsal(cv::Mat &imgBinaria, std::vector<std::vector<cv::Point>> &contornoCaracteres) = 0;

        virtual ~OBJECT_OCR(){};

};

#define MAX_GROUP_BOXES 3

#define ROJO Scalar(0,0,255)

#define VERDE Scalar(0,255,0)

#define AZUL Scalar(255,0,0)

#define __FACTOR_UPPER__ 0.999

#define listNumbers ("0123456789")

#define RATECHARFAULT 0.50

#define CV_FACE std::string("haarcascade_frontalface_alt.xml")

#define CV_UPPERBODY std::string("haarcascade_mcs_upperbody.xml")

#endif /* OBJECTCLASS_H */

