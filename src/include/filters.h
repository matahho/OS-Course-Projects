//
// Created by Mahdi on 12/13/23.
//

#ifndef PROJECT3_FILTERS_H
#define PROJECT3_FILTERS_H
#include "vector"
#include "bmp.h"


class filters {
public:
    filters(std::vector<std::vector<Pixel>>& pixels_ );
    void mirror();
    std::vector<std::vector<Pixel>> applyFilters();
private:
    std::vector<std::vector<Pixel>> pixels;
};




#endif //PROJECT3_FILTERS_H
