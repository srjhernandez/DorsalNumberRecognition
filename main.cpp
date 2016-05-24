/* 
 * File:   main.cpp
 * Author: jesus
 *
 * Created on 13 de mayo de 2015, 17:27
 */

#include "ExtractTexto.h"

int main(int argc, char * argv[]) {
            
    ExtractTexto text ;
    
    text.runExtract("images/foto6.jpg", Tipo_OCR::TESSERACT);
    
    return 0;
    
}

