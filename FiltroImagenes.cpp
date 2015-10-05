/* 
 * File:   FiltroImagenes.cpp
 * Author: jesus
 * 
 * Created on 15 de mayo de 2015, 19:10
 */

#include <algorithm>
#include "FiltroImagenes.h"


/*-----------------------------------------------------------------------------*/
std::vector<std::string> FiltroImagenes::toMinusculas(std::vector<std::string> ext) const{
    
    char caracter;
    
    std::string minusculas;
    
    std::vector<std::string> cadena;    
        
    for(auto texto : ext){
        
        for(auto c : texto){
            
            caracter = tolower(c);
        
            minusculas.push_back(caracter);
            
        }
        
        cadena.push_back(minusculas);
        
        cout<<minusculas;
        
        minusculas.clear();
        
    }
                
    return cadena;
    
}

/*-----------------------------------------------------------------------------*/
std::vector<std::string> FiltroImagenes::getFicherosImagenes(const char *pathToDir){
    
    std::vector<std::string> ficheros;
           
    DIR *carpetaImagenes;
        
    carpetaImagenes = opendir(pathToDir);
    
    struct dirent *sd;
        
    if (carpetaImagenes != NULL){
        
        while ( (sd =readdir(carpetaImagenes))!= NULL ){
          
            std::string fileIMG(sd->d_name);
            
            std::size_t found = fileIMG.find(".jpg");
 
             if (found!=std::string::npos){
                 
                ficheros.push_back(fileIMG); 
            }
                       
        }
        
    }
    return ficheros;   
    
}


FiltroImagenes::~FiltroImagenes() {
}

