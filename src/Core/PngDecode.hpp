#ifndef PNGDECODE_HPP_
#define PNGDECODE_HPP_
#include <vector>

bool decodePng(const char* filename, int& width, int& height, std::vector<unsigned char>& pixelIndices, std::vector<int>& palette);

#endif
