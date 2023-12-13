//
// Created by Mahdi on 12/13/23.
//
#include "include/bmp.h"

int main(int argc, char* argv[]) {
    std::vector<char> fileBuffer;
    int bufferSize;
    if (!fillAndAllocate(fileBuffer, argv[1], rows, cols, bufferSize)) {
        std::cout << "File read error" << std::endl;
        return 1;
    }

    // Read input file and fill the pixels vector
    std::vector<std::vector<Pixel>> pixels = getPixelsFromBMP24(bufferSize, rows, cols, fileBuffer);


    // Write output file
    writeOutBmp24(fileBuffer, "output1.bmp", bufferSize , pixels);

    return 0;
}