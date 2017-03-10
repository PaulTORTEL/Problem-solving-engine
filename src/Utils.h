#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <cstdlib>
#include <sstream>
#include <vector>

std::string concatString(const std::string& str1, const std::string& str2);
bool parseNumber(const char* s, int* i);
std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);

#endif // UTILS_H_INCLUDED
