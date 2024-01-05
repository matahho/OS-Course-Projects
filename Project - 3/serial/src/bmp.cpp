
#include "include/bmp.h"


// std::vector<std::vector<Pixel>> pixels; Vector of vectors to store pixels


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

int rows , cols;

bool fillAndAllocate(std::vector<char>& buffer, const char* fileName, int& rows, int& cols, int& bufferSize) {
    std::ifstream file(fileName, std::ios::binary);
    if (!file) {
        std::cout << "File " << fileName << " doesn't exist!" << std::endl;
        return false;
    }

    file.seekg(0, std::ios::end);
    std::streampos length = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer.resize(length);
    file.read(&buffer[0], length);

    PBITMAPFILEHEADER file_header;
    PBITMAPINFOHEADER info_header;

    file_header = (PBITMAPFILEHEADER)(&buffer[0]);
    info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));
    rows = info_header->biHeight;
    cols = info_header->biWidth;
    bufferSize = static_cast<int>(length);

    return true;
}

std::vector<std::vector<Pixel>> getPixelsFromBMP24(int end, int rows, int cols, const std::vector<char>& fileReadBuffer) {
    std::vector<std::vector<Pixel>> pixels ;
    int count = 1;
    int extra = cols % 4;

    for (int i = 0; i < rows; i++) {
        count += extra;
        std::vector<Pixel> rowPixels;
        for (int j = cols - 1; j >= 0; j--) {
            Pixel pixel;
            for (int k = 0; k < 3; k++) {
                switch (k) {
                    case 0:
                        pixel.red = static_cast<unsigned char>(fileReadBuffer[end - count]);
                        break;
                    case 1:
                        pixel.green = static_cast<unsigned char>(fileReadBuffer[end - count]);
                        break;
                    case 2:
                        pixel.blue = static_cast<unsigned char>(fileReadBuffer[end - count]);
                        break;
                }
                // go to the next position in the buffer
                count++;
            }
            rowPixels.push_back(pixel);
        }
        pixels.push_back(rowPixels);
    }

    return pixels;


}
void writeOutBmp24(const std::vector<char>& fileBuffer, const char* nameOfFileToCreate, int bufferSize , std::vector<std::vector<Pixel>> pixels) {
    std::ofstream write(nameOfFileToCreate, std::ios::binary);
    if (!write) {
        std::cout << "Failed to write " << nameOfFileToCreate << std::endl;
        return;
    }

    // Write BITMAPFILEHEADER
    BITMAPFILEHEADER fileHeader;
    fileHeader.bfType = 0x4D42; // BM
    fileHeader.bfSize = bufferSize;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    write.write(reinterpret_cast<const char*>(&fileHeader), sizeof(BITMAPFILEHEADER));

    // Write BITMAPINFOHEADER
    BITMAPINFOHEADER infoHeader;
    infoHeader.biSize = sizeof(BITMAPINFOHEADER);
    infoHeader.biWidth = cols;
    infoHeader.biHeight = rows;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 24; // 24 bits per pixel (RGB)
    infoHeader.biCompression = 0; // No compression
    infoHeader.biSizeImage = bufferSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;

    write.write(reinterpret_cast<const char*>(&infoHeader), sizeof(BITMAPINFOHEADER));

    // Write pixel data
    int count = 1;
    int extra = cols % 4;

    for (int i = rows-1; i > 0; i--) {
        count += extra;
        for (int j = cols - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
                switch (k) {
                    case 1:
                        write.put(static_cast<char>(pixels[i][j].red));
                        break;
                    case 0:
                        write.put(static_cast<char>(pixels[i][j].green));
                        break;
                    case 2:
                        write.put(static_cast<char>(pixels[i][j].blue));
                        break;
                }
                count++;
            }
        }
    }
}





