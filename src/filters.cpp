#include "include/filters.h"


filters::filters(std::vector <std::vector<Pixel>> &pixels_) {
    pixels = pixels_;
}
void filters::mirror() {
    for (int r=0 ; r < rows/2 ; r++){
        std::swap(pixels[r] , pixels[rows-r-1]);
    }
}
std::vector<std::vector<Pixel>> filters::applyFilters() {
    mirror();
    return pixels;
}

