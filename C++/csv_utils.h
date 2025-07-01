#ifndef HEADER_H
#define HEADER_H

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

void removeDuplicates(const std::string& input_csv);

#endif