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
std::string operator+(std::string str1, int n);

std::string operator+(std::string str1, std::string str2);

std::string operator+(std::string str1, const char* str2);

std::string operator"" _s(const char* text, std::size_t len);
