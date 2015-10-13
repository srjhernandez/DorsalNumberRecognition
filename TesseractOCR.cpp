/* 
 * File:   TesseractOCR.cpp
 * Author: jesus
 * 
 * Created on 18 de mayo de 2015, 16:35
 */

#include <iostream>
#include "TesseractOCR.h"
#include "DetectorImagen.h"


TesseractOCR::TesseractOCR() {
}

TesseractOCR::TesseractOCR(const TesseractOCR& orig) {
}


void TesseractOCR::rgb2cmyk(const cv::Mat& src, std::vector<cv::Mat>& cmyk)
{
    CV_Assert(src.type() == CV_8UC3);

    cmyk.clear();
    for (int i = 0; i < 4; ++i)
        cmyk.push_back(cv::Mat(src.size(), CV_32F));

    for (int i = 0; i < src.rows; ++i)
    {
        for (int j = 0; j < src.cols; ++j)
        {
            cv::Vec3b p = src.at<cv::Vec3b>(i,j);

            float r = p[2] / 255.;
            float g = p[1] / 255.;
            float b = p[0] / 255.;
            float k = (1 - std::max(std::max(r,g),b));

            cmyk[0].at<float>(i,j) = (1 - r - k) / (1 - k); 
            cmyk[1].at<float>(i,j) = (1 - g - k) / (1 - k);
            cmyk[2].at<float>(i,j) = (1 - b - k) / (1 - k);
            cmyk[3].at<float>(i,j) = k;
        }
    }
}
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/


void TesseractOCR::brillo(cv::Mat& src, cv::Mat &dst){
    
    cv::Mat hsv;
    
    std::vector<cv::Mat> hsv_planes;
    
    cv::cvtColor(src, hsv, CV_BGR2HSV);
    
    cv::split(hsv,hsv_planes);
    
    cv::equalizeHist(hsv_planes[2],hsv_planes[2]);  
    
    cv::merge(hsv_planes,dst);
    
    cv::cvtColor(dst,dst,CV_HSV2BGR);
    
    cv::imshow("BRILLO",dst);
    
    cv::waitKey(-1);
    
}

void TesseractOCR::runTesseractOCR(cv::Mat &im0){
    
    if (!im0.data)
        return;

    std::vector<cv::Mat> cmyk;
    
    rgb2cmyk(im0, cmyk);

    cv::Mat im1;
    im1 = cmyk[3].mul(1 - cmyk[1]) > 0.25;
    
    cv::Mat im2;
    im1.convertTo(im2, CV_8U);
        
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(im2, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    double max_area = 0;
    int max_idx = 0;

    for (size_t i = 0; i < contours.size(); i++)
    {
        
        double area = cv::contourArea(contours[i]);
        max_idx  = area > max_area ? i : max_idx;
        max_area = area > max_area ? area : max_area;
    }

    im2.setTo(cv::Scalar(0));
        
    cv::drawContours(im2, contours, max_idx, cv::Scalar(255), -1);
    
    cv::Mat im3;
    cv::cvtColor(im0, im3, CV_BGR2GRAY);
    
    cv::equalizeHist(im3, im3);
    
    im3 = ((255 - im3) & im2) > 200;   
     
    cv::Mat dst = im3.clone();   
        
    cv::findContours(dst.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    
    for (size_t i = 0; i < contours.size(); i++)
    {
        if (cv::contourArea(contours[i]) < 100)
            cv::drawContours(dst, contours, i, cv::Scalar(0), -1);
    }
    
    tesseract::TessBaseAPI tess;
    tess.Init(NULL, "spa", tesseract::OEM_DEFAULT);
    tess.SetVariable("tessedit_char_whitelist", "0123456789");
    tess.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
    tess.SetImage((uchar*)dst.data, dst.cols, dst.rows, 1, dst.cols);

    char* out = tess.GetUTF8Text();
    std::cout << out << std::endl;
    
    cv::imshow("src", im0);
    cv::imshow("dst", dst);
    cv::waitKey();
    
}
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/

char * TesseractOCR::recognition(cv::Mat dst){
        
    
    tesseract::TessBaseAPI tess;
    tess.Init(NULL, "eng", tesseract::OEM_DEFAULT);
    tess.SetVariable("tessedit_char_whitelist", "0123456789");
    tess.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
    tess.SetImage((uchar*)dst.data, dst.cols, dst.rows, 1, dst.cols);
    
    return (tess.GetUTF8Text());
    
}


/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/
void TesseractOCR::runTOCR(cv::Mat &im0, const double factor, const int umbralV1,const int umbralV2){
    
    if (!im0.data)
        return;
       
    std::vector<cv::Mat> cmyk;
        
    rgb2cmyk(im0, cmyk);

    cv::Mat im1,im11(1-cmyk[3]);    
    
    im1 = im11.mul(1-cmyk[1]) > factor;
    
    
    cv::Mat im2;
    im1.convertTo(im2, CV_8U);
    
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(im2, contours,  CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    double max_area = 0;
    int max_idx = 0;

    for (size_t i = 0; i < contours.size(); i++)
    {
        
        double area = cv::contourArea(contours[i]);
        max_idx  = area > max_area ? i : max_idx;
        max_area = area > max_area ? area : max_area;
    }

    im2.setTo(cv::Scalar(0));
        
    cv::drawContours(im2, contours, max_idx, cv::Scalar(255), -1);
    
    cv::Mat im3;
    cv::cvtColor(im0, im3, CV_BGR2GRAY);
    
    cv::equalizeHist(im3, im3);
    
    im3 = ((255 - im3) & im2) > umbralV1;   
     
    cv::Mat dst = im3.clone();   
        
    cv::findContours(dst.clone(), contours,  CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    
    for (size_t i = 0; i < contours.size(); i++)
    {
        if (cv::contourArea(contours[i]) < umbralV2)
            cv::drawContours(dst, contours, i, cv::Scalar(0), -1);
    }
    
    tesseract::TessBaseAPI tess;
    tess.Init(NULL, "eng", tesseract::OEM_DEFAULT);
    tess.SetVariable("tessedit_char_whitelist", "0123456789");
    tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
    tess.SetImage((uchar*)dst.data, dst.cols, dst.rows, 1, dst.cols);

    char* out = tess.GetUTF8Text();
    std::cout << out << std::endl;
    
    cv::imshow("src", im0);
    cv::imshow("dst", dst);
    cv::waitKey(-1);
    
}

/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/

void TesseractOCR::runOCR(cv::Mat &im0, const double factor, const int umbralV1,const int umbralV2){
    
    if (!im0.data)
        return;

    std::vector<cv::Mat> cmyk;
    
    rgb2cmyk(im0, cmyk);

    cv::Mat im1;    
    
    im1 = (1 - cmyk[3]).mul(1 - cmyk[1]) > factor;
    
    cv::Mat im2;
    im1.convertTo(im2, CV_8U);
    
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(im2, contours,  CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    double max_area = 0;
    int max_idx = 0;

    for (size_t i = 0; i < contours.size(); i++)
    {
        
        double area = cv::contourArea(contours[i]);
        max_idx  = area > max_area ? i : max_idx;
        max_area = area > max_area ? area : max_area;
    }

    im2.setTo(cv::Scalar(0));
        
    cv::drawContours(im2, contours, max_idx, cv::Scalar(255), -1);
    
    cv::Mat im3;
    cv::cvtColor(im0, im3, CV_BGR2GRAY);
    
    //cv::equalizeHist(im3,im3);
    
    im3 = ((255 - im3) & im2) > umbralV1;   
     
    cv::Mat dst = im3.clone();   
        
    cv::findContours(dst.clone(), contours,  CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    
    for (size_t i = 0; i < contours.size(); i++)
    {
        if (cv::contourArea(contours[i]) < umbralV2)
            cv::drawContours(dst, contours, i, cv::Scalar(0), -1);
    }
    
    tesseract::TessBaseAPI tess;
    tess.Init(NULL, "eng", tesseract::OEM_DEFAULT);
    tess.SetVariable("tessedit_char_whitelist", "0123456789");
    tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
    tess.SetImage((uchar*)dst.data, dst.cols, dst.rows, 1, dst.cols);

    char* out = tess.GetUTF8Text();
    std::cout << out << std::endl;
    
    cv::imshow("src", im0);
    cv::imshow("dst", dst);
    cv::waitKey(-1);
    
}

/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/

void TesseractOCR::run(cv::Mat &imagen){
        
    cv::Mat imgcan, im1, im2;
    
    std::vector<cv::Mat> cmyk;
    
    int thresh = 200;       
    
    //brillo(imagen, img);
        
    rgb2cmyk(imagen, cmyk);
    
    im1 = (1 - cmyk[3]).mul(1 - cmyk[1])>0.4;
          
    cv::imshow("CONTORNOS",im1);
    
    cv::waitKey(-1);
    
    im1.convertTo(im2, CV_8UC3);    
    
    cv::equalizeHist(im2,im2);
       
    cv::Canny(im2, imgcan, 0, thresh, 5, true);
       
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(imgcan.clone(), contours,  CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
    
            
    for (size_t i = 0; i < contours.size(); i++)
    {
        if (cv::contourArea(contours[i]) < 100){
            cv::drawContours(imgcan, contours, i, cv::Scalar(0), -1);
        }
    }
      
    cvtColor(imgcan, imgcan, CV_GRAY2BGR);
    
    std::vector<cv::Rect> letterBBoxes1 = detectBox(imgcan);
    
    for(size_t i=0; i< letterBBoxes1.size(); i++){
        
        cv::rectangle(imgcan,letterBBoxes1[i],cv::Scalar(100,255,40),2,2,0);
    }
    
    cv::imshow("CONTORNOS4",imgcan);
    
    cv::waitKey(-1);
}

void groups_draw(Mat &src, vector<Rect> &groups)
{
    for (int i=(int)groups.size()-1; i>=0; i--)
    {
        if (src.type() == CV_8UC3)
            rectangle(src,groups.at(i).tl(),groups.at(i).br(),Scalar( 0, 255, 255 ), 3, 8 );
        else
            rectangle(src,groups.at(i).tl(),groups.at(i).br(),Scalar( 255 ), 3, 8 );
    }
}

void er_show(vector<Mat> &channels, vector<vector<ERStat> > &regions)
{
    for (int c=0; c<(int)channels.size(); c++)
    {
        Mat dst = Mat::zeros(channels[0].rows+2,channels[0].cols+2,CV_8UC1);
        for (int r=0; r<(int)regions[c].size(); r++)
        {
            ERStat er = regions[c][r];
            if (er.parent != NULL) // deprecate the root region
            {
                int newMaskVal = 255;
                int flags = 4 + (newMaskVal << 8) + FLOODFILL_FIXED_RANGE + FLOODFILL_MASK_ONLY;
                floodFill(channels[c],dst,Point(er.pixel%channels[c].cols,er.pixel/channels[c].cols),
                          Scalar(255),0,Scalar(er.level),Scalar(0),flags);
            }
        }
        char buff[10]; char *buff_ptr = buff;
        sprintf(buff, "channel %d", c);
        imshow(buff_ptr, dst);
    }
    waitKey(-1);
}

/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/

void TesseractOCR::binarizarImagen(const cv::Mat &source, cv::Mat &dst){
      
    dst = source.clone();
}

/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/

void TesseractOCR::detectorTextos( const cv::Mat &source, std::vector<cv::Rect> &groups_boxes ){
    
    std::vector<cv::Mat> channels;
          
    cv::text::computeNMChannels(source, channels);
    
    groups_boxes.clear();
    
    int cn = (int)channels.size();
    // Append negative channels to detect ER- (bright regions over dark background)
    for (int c = 0; c < cn-1; c++)
        channels.push_back(channels[c]);    
   
    Ptr<cv::text::ERFilter> er_filter1 = createERFilterNM1(loadClassifierNM1("trained_classifierNM1.xml"),16,0.00015f,0.13f,0.2f,true,0.1f);
    Ptr<cv::text::ERFilter> er_filter2 = createERFilterNM2(loadClassifierNM2("trained_classifierNM2.xml"),0.5);
    
    std::vector< std::vector<cv::text::ERStat> > regions(channels.size());
    
    for (int c=0; c<(int)channels.size(); c++){
        er_filter1->run(channels[c], regions[c]);
        er_filter2->run(channels[c], regions[c]);
    }
    
    std::vector< std::vector<Vec2i>> region_groups;    
    
    cv::text::erGrouping(source, channels, regions, region_groups, groups_boxes, ERGROUPING_ORIENTATION_HORIZ);
    
    cout<<" HAY "<<groups_boxes.size();
    
    cv::Mat imagClone = source.clone();
        
    groups_draw(imagClone, groups_boxes);
    
    cv::imshow("Regiones de Textos",imagClone);
            
    cv::waitKey(-1);    
    
}

/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/

void TesseractOCR::binarizarRegion(const cv::Mat &src, cv::Mat &imagenRegion, cv::Rect region){
    
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
   
    cv::Mat original(src, region), gris, componentes, originalRZ;  
            
    cv::resize(original, originalRZ, Size(550, 200));    
         
    cv::imshow("original",originalRZ);
                   
    cv::waitKey(1200);   
       
    cv::cvtColor(originalRZ, gris, CV_BGR2GRAY);
    
    cout<< "Hay estos componentes :"<<cv::connectedComponents(gris, componentes)<<endl;
        
    //cv::bilateralFilter(gris.clone(), gris, 20, 190, 255);
               
    cv::imshow("GRIS IMAGEN", gris);       
   
    cv::adaptiveThreshold(gris.clone(), imagenRegion, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 401, 30);     
    
    cv::imshow("imagenRegion", imagenRegion);
        
    cv::waitKey(-1);
}

/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/

void TesseractOCR::detectarCaracteres(const cv::Mat &caracteresROI, std::vector<std::vector<cv::Point>> &contornoCaracteres){
    
    std::vector<std::vector<cv::Point>> contornos;  
       
    contornoCaracteres.clear();
   
    findContours( caracteresROI.clone(), contornos, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );

    for( size_t i = 0; i < contornos.size(); i++ ){ 
        if( cv::contourArea(contornos[i]) > 400 ){           
            contornoCaracteres.push_back(contornos[i]);       
        }
    }
       
    cout<< "Se han identificado " <<contornoCaracteres.size()<< " caracteres."<<endl;
}

/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/

void TesseractOCR::reconocerCaracteres(const cv::Mat &imgBinaria, std::vector<std::vector<cv::Point>> &contornoCaracteres){

    size_t numCaracteres = contornoCaracteres.size();
    
    TesseractOCR::vecNumber numero; 
    
    std::vector<cv::Rect> boundRect(numCaracteres);
    
    for (size_t i = 0; i < numCaracteres; i++ ){
            
        char *caracter;
        
        boundRect[i] = cv::boundingRect(cv::Mat(contornoCaracteres[i]));
        
        cout<<"este contorno tiene area: "<<cv::contourArea(contornoCaracteres[i])<<endl;
                        
        cv:: Mat newImagen =  cv::Mat::zeros( imgBinaria.size(), CV_8UC1 );

        cv::drawContours( newImagen, contornoCaracteres, i, Scalar(255,255,255), -1, 8, vector<Vec4i>(), 0, Point() );

        cv::Mat segmentacion = newImagen & imgBinaria;       

        cv::Mat imagenSegment( segmentacion, boundRect[i]);

        cv::copyMakeBorder(imagenSegment,imagenSegment,15,15,15,15,BORDER_CONSTANT,Scalar(0));
        
        cv::imshow("Regiones de Textos",imagenSegment);
            
        cv::waitKey(1200);  
        
        caracter = recognition(imagenSegment);    
        
        numero.digitos.push_back( number(*caracter, boundRect[i].x) );

    }
    
    numero.ordenar();
    
    numero.show();
       
}


/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/

void TesseractOCR::reconocedorTextos( const cv::Mat &source, const std::vector<cv::Rect> &groups_boxes ){
            
    for( cv::Rect region : groups_boxes){
        
        cv::Mat regionMat;
        
        std::vector<std::vector<cv::Point>> caracteres;
        
        binarizarRegion(source, regionMat, region);   
        
        detectarCaracteres( regionMat, caracteres );
        
        reconocerCaracteres( regionMat, caracteres);        
        
    }
    
}

/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/


void TesseractOCR::run2(cv::Mat &imagen){
    
    cv::Mat imagenBinaria, nuevaImagen;    
    
    double p = ( (double)imagen.cols / imagen.rows );
   
    int height = 550 ;   
    
    int width = height * p;    
   
    cv::resize(imagen, nuevaImagen, Size(  width ,  height ));   
   
    std::vector<cv::Rect> cajasDeTextos;
    
    binarizarImagen( nuevaImagen, imagenBinaria );    
        
    detectorTextos( imagenBinaria, cajasDeTextos );   
    
    reconocedorTextos( nuevaImagen, cajasDeTextos );    
      
 }


/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/
std::vector<cv::Rect> TesseractOCR::detectBox(cv::Mat &img){
    
    std::vector<cv::Rect> boundRect;
    cv::Mat img_gray, img_sobel, img_threshold, element;
    cvtColor(img, img_gray, CV_BGR2GRAY);
    cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    cv::threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
    element = getStructuringElement(cv::MORPH_RECT, cv::Size(17, 3) );
    cv::morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element); //Does the trick
    std::vector< std::vector< cv::Point> > contours;
    cv::findContours(img_threshold, contours, 0, 1); 
    std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
    for( size_t i = 0; i < contours.size(); i++ )
        if (contours[i].size()>100)
        { 
            cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
            cv::Rect appRect( boundingRect( cv::Mat(contours_poly[i]) ));
            if (appRect.width>appRect.height) 
                boundRect.push_back(appRect);
        }
    return boundRect;
    
}




TesseractOCR::~TesseractOCR() {
}

