#ifndef HUST_LAB_NETWORK_STRING_UTILS_H
#define HUST_LAB_NETWORK_STRING_UTILS_H
#include <string>
std::string& trim(std::string& s, const std::string& d = " ");
std::string& upper(std::string& s);
std::string& lower(std::string& s);
std::string get_time_string();
#endif //HUST_LAB_NETWORK_STRING_UTILS_H
