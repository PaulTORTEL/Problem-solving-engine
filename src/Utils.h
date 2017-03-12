#pragma once

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <sstream>
#include <vector>

std::string concatString(const std::string& str1, const std::string& str2);
bool parseNumber(const char* s, int* i);
std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
