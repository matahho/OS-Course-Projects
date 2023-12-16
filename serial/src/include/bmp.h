//
// Created by Mahdi on 12/13/23.
//

#ifndef PROJECT3_BMP_H
#define PROJECT3_BMP_H

#include <vector>
#include <fstream>
#include <iostream>



typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

struct Pixel {
    unsigned int red;
    unsigned int green;
    unsigned int blue;
    
    Pixel(unsigned int r, unsigned int g, unsigned int b) : red(r), green(g), blue(b) {} 
    Pixel() : red(0), green(0), blue(0) {}

};

extern int rows ;
extern int cols ;

bool fillAndAllocate(std::vector<char>& buffer, const char* fileName, int& rows, int& cols, int& bufferSize);
std::vector<std::vector<Pixel>> getPixelsFromBMP24(int end, int rows, int cols, const std::vector<char>& fileReadBuffer);
void writeOutBmp24(const std::vector<char>& fileBuffer, const char* nameOfFileToCreate, int bufferSize , std::vector<std::vector<Pixel>> pixels);
#endif //PROJECT3_BMP_H
