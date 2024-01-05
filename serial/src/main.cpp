//
// Created by Mahdi on 12/13/23.
//
#include "include/bmp.h"
#include "include/filters.h"

void executionTimeSheet(std::vector<double> times){
    std::cout << "Read Execution Time : " << times[0]/1000 << " ms" << std::endl;
    std::cout << "Flip Execution Time : " << times[1]/1000 << " ms" << std::endl;
    std::cout << "Blur Execution Time : " << times[2]/1000 << " ms" << std::endl;
    std::cout << "PurpleHaze Execution Time : " << times[3]/1000 << " ms" << std::endl;
    std::cout << "Hatch Execution Time : " << times[4]/1000 << " ms" << std::endl;
    std::cout << "Write Execution Time : " << times[5]/1000 << " ms" << std::endl;
    double total;
    for (int i = 0 ; i < times.size() ; i++)
        total += times[i];
    std::cout << "Total Execution Time : " << total/1000 << " ms" << std::endl;
    

}



int main(int argc, char* argv[]) {
    std::vector<char> fileBuffer;
    int bufferSize;
    if (!fillAndAllocate(fileBuffer, argv[1], rows, cols, bufferSize)) {
        std::cout << "File read error" << std::endl;
        return 1;
    }
    std::vector<double> executionTimes;

    // Read input file and fill the pixels vector
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::vector<Pixel>> pixels = getPixelsFromBMP24(bufferSize, rows, cols, fileBuffer);
    auto stop = std::chrono::high_resolution_clock::now();
    executionTimes.push_back(std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count());


    filters f = filters(pixels);
    std::vector<std::vector<Pixel>> modified = f.applyFilters(executionTimes);

    // Write output file
    start = std::chrono::high_resolution_clock::now();
    writeOutBmp24(fileBuffer, "output1.bmp", bufferSize , modified);
    stop = std::chrono::high_resolution_clock::now();
    executionTimes.push_back(std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count());

    executionTimeSheet(executionTimes);

    return 0;
}