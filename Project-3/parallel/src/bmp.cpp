
#include "include/bmp.h"

int rows , cols;
bool fillAndAllocate(char*& buffer, const char* fileName, int& rows, int& cols, int& bufferSize) {
    std::ifstream file(fileName);
    if (!file) {
        std::cout << "File" << fileName << " doesn't exist!" << std::endl;
        return false;
    }

    file.seekg(0, std::ios::end);
    std::streampos length = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer = new char[length];
    file.read(&buffer[0], length);

    PBITMAPFILEHEADER file_header;
    PBITMAPINFOHEADER info_header;

    file_header = (PBITMAPFILEHEADER)(&buffer[0]);
    info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));
    rows = info_header->biHeight;
    cols = info_header->biWidth;
    bufferSize = file_header->bfSize;
    return true;
}

void writeOutBmp24(char*& fileBuffer, const char* nameOfFileToCreate, int bufferSize , std::vector<std::vector<Pixel>> pixels) {
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

    for (int i = 0; i < rows; i++) {
        count += extra;
        for (int j = 0; j < cols; j++) {
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







BMP::BMP(const BMP& other)
    : pixels_(other.pixels_),
      bitmap_file_header_(other.bitmap_file_header_),
      bitmap_info_header_(other.bitmap_info_header_){}



std::vector<std::vector<Pixel>> BMP::get_pixels()const{
    return pixels_;
}

void BMP::set_pixels(std::vector<std::vector<Pixel>> pixels){
    pixels_ = pixels;
}

Pixel& BMP::operator()(int row, int column) {
    return pixels_[row][column];
}

const Pixel& BMP::operator()(int row, int column) const {
    return pixels_[row][column];
}

std::istream& operator>>(std::istream& stream, BMP& bmp) {
    bmp.read_headers(stream);
    bmp.read_bitmap(stream);
    return stream;
}


void BMP::read_headers(std::istream& read_stream) {
    read_stream.read(
        (char*)(&bitmap_file_header_),
        sizeof(BITMAPFILEHEADER));

    if (bitmap_file_header_.bfType != 0x4D42)
        throw std::runtime_error("Not a bmp file.");

    read_stream.read(
        (char*)(&bitmap_info_header_),
        sizeof(BITMAPINFOHEADER));
}

void BMP::read_bitmap(std::istream& read_stream) {
    read_stream.seekg(bitmap_file_header_.bfOffBits, std::ios::beg);
    pixels_.assign(bitmap_info_header_.biHeight, std::vector<Pixel>(bitmap_info_header_.biWidth));
    unsigned char* img_buffer = new unsigned char[bitmap_info_header_.biSizeImage];
    read_stream.read(
        (char*)img_buffer,
        bitmap_info_header_.biSizeImage);

    auto do_partial = [img_buffer, this](int row_begin, int row_end) {
        BMP::DoPartialArgs* args = new BMP::DoPartialArgs;
        args->bmp = this;
        args->row_begin = row_begin;
        args->row_end = row_end;
        args->img_buffer = img_buffer;
        pthread_t t_id;
        pthread_create(&t_id, NULL, read_partial, (void*)args);
        return t_id;
    };

    std::vector<pthread_t> t_ids;
    int N_PARTS = 8;
    for (int i = 0; i < N_PARTS; ++i) {
        int row_begin = i * (bitmap_info_header_.biHeight / N_PARTS);
        int row_end = (i + 1) * (bitmap_info_header_.biHeight / N_PARTS);
        t_ids.push_back(do_partial(row_begin, row_end));
    }

    if (bitmap_info_header_.biHeight % N_PARTS) {
        int row_begin = (bitmap_info_header_.biHeight - (bitmap_info_header_.biHeight % N_PARTS));
        int row_end = bitmap_info_header_.biHeight;
        t_ids.push_back(do_partial(row_begin, row_end));
    }

    for (auto t_id : t_ids)
        pthread_join(t_id, NULL);

    delete[] img_buffer;
}

void BMP::write_headers(std::ostream& write_stream) {
    write_stream.write(
        (char*)(&bitmap_file_header_),
        sizeof(BITMAPFILEHEADER));
    write_stream.write(
        (char*)(&bitmap_info_header_),
        sizeof(BITMAPINFOHEADER));

    int cur_stream_point = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    while (cur_stream_point < bitmap_file_header_.bfOffBits) {
        write_stream << '\0';
        ++cur_stream_point;
    }
}

void BMP::write_bitmap(std::ostream& write_stream) {
    unsigned char* img_buffer = new unsigned char[bitmap_info_header_.biSizeImage];

    auto do_partial = [img_buffer, this](int row_begin, int row_end) {
        BMP::DoPartialArgs* args = new BMP::DoPartialArgs;
        args->bmp = this;
        args->row_begin = row_begin;
        args->row_end = row_end;
        args->img_buffer = img_buffer;
        pthread_t t_id;
        pthread_create(&t_id, NULL, write_partial, (void*)args);
        return t_id;
    };

    std::vector<pthread_t> t_ids;
    int N_PARTS = 8;
    for (int i = 0; i < N_PARTS; ++i) {
        int row_begin = i * (bitmap_info_header_.biHeight / N_PARTS);
        int row_end = (i + 1) * (bitmap_info_header_.biHeight / N_PARTS);
        t_ids.push_back(do_partial(row_begin, row_end));
    }

    if (bitmap_info_header_.biHeight % N_PARTS) {
        int row_begin = (bitmap_info_header_.biHeight - (bitmap_info_header_.biHeight % N_PARTS));
        int row_end = bitmap_info_header_.biHeight;
        t_ids.push_back(do_partial(row_begin, row_end));
    }

    for (auto t_id : t_ids)
        pthread_join(t_id, NULL);

    write_stream.write(
        (char*)(img_buffer),
        bitmap_info_header_.biSizeImage);

    delete[] img_buffer;
}

int BMP::get_data_pos_from_pixel_loc(int row, int col) {
    int row_len = std::ceil((3 * bitmap_info_header_.biWidth) / 4) * 4;
    int height = bitmap_info_header_.biHeight - row - 1;
    int width = col * 3;
    int result = height * row_len + width;
    return result;
}

void* BMP::read_partial(void* args) {
    BMP::DoPartialArgs* args_struct =
        static_cast<BMP::DoPartialArgs*>(args);

    auto& row_begin = args_struct->row_begin;
    auto& row_end = args_struct->row_end;
    auto& bmp = args_struct->bmp;
    auto& img_buffer = args_struct->img_buffer;

    for (int i = row_begin; i < row_end; i++) {
        for (int j = cols - 1; j >= 0; j--) {
            unsigned char* temp_pixels = &img_buffer[bmp->get_data_pos_from_pixel_loc(i, j)];
            bmp->pixels_[i][j] = {temp_pixels[2], temp_pixels[1], temp_pixels[0]};
        }
    }

    delete args_struct;
    return NULL;
}

void* BMP::write_partial(void* args) {
    BMP::DoPartialArgs* args_struct =
        static_cast<BMP::DoPartialArgs*>(args);

    auto& row_begin = args_struct->row_begin;
    auto& row_end = args_struct->row_end;
    auto& bmp = args_struct->bmp;
    auto& img_buffer = args_struct->img_buffer;

    for (int i = 0; i < bmp->bitmap_info_header_.biHeight; i++) {
        for (int j = 0; j <cols; j++) {
            unsigned char* temp_pixel = &img_buffer[bmp->get_data_pos_from_pixel_loc(i, j)];
            temp_pixel[0] = bmp->pixels_[i][j].green;
            temp_pixel[1] = bmp->pixels_[i][j].red;
            temp_pixel[2] = bmp->pixels_[i][j].blue;
        }
    }

    delete args_struct;
    return NULL;
}