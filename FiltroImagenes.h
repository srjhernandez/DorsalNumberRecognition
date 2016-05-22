/* 
 * File:   FiltroImagenes.h
 * Author: jesus
 *
 * Created on 15 de mayo de 2015, 19:10
 */

#ifndef FILTROIMAGENES_H
#define	FILTROIMAGENES_H

#include <sys/types.h>
#include <dirent.h>
#include <cstdlib>
#include <iostream>
#include <libgen.h>
#include <locale> 
#include <vector>
#include <cctype>
#include <math.h>

using namespace std;

class FiltroImagenes {
public:
   
    FiltroImagenes( std::vector<std::string> filtros ): f_filtros(toMinusculas(filtros)){};
       
    std::vector<std::string> getFicherosImagenes(const char *pathToDir);
    
    ~FiltroImagenes();
        
private:
    
    std::vector<std::string> toMinusculas(std::vector<std::string> c ) const;
    
    const std::vector<std::string> f_filtros;

};

#endif	/* FILTROIMAGENES_H */

