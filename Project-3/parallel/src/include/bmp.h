//
// Created by Mahdi on 12/13/23.
//

#ifndef BMP_H
#define BMP_H

#include <vector>
#include <fstream>
#include <iostream>
#include <cmath>



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



#include <iostream>
#include <vector>

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;


bool fillAndAllocate(char*& buffer, const char* fileName, int& rows, int& cols, int& bufferSize);
void writeOutBmp24(char*& fileBuffer, const char* nameOfFileToCreate, int bufferSize , std::vector<std::vector<Pixel>> pixels);



#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
#pragma pack(pop)


class BMP {

public:


    BMP() = default;
    BMP(const BMP& other);


    std::vector<std::vector<Pixel>> get_pixels () const ;
    void set_pixels (std::vector<std::vector<Pixel>> pixels);
    Pixel& operator()(int row, int column);
    const Pixel& operator()(int row, int column) const;
    friend std::istream& operator>>(std::istream& stream, BMP& bmp);


private:
    std::vector<std::vector<Pixel>> pixels_;
    BITMAPFILEHEADER bitmap_file_header_;
    BITMAPINFOHEADER bitmap_info_header_;

    void read_headers(std::istream& read_stream);
    void read_bitmap(std::istream& read_stream);

    void write_headers(std::ostream& write_stream);
    void write_bitmap(std::ostream& write_stream);

    int get_data_pos_from_pixel_loc(int row, int col);

    struct DoPartialArgs {
        BMP* bmp;
        int row_begin;
        int row_end;
        unsigned char* img_buffer;
    };

    static void* read_partial(void* args);
    static void* write_partial(void* args);
};


#endif