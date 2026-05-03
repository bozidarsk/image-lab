#include "Properties.h"

std::unordered_map<std::string, std::any>::iterator Properties::begin() { return data.begin(); }
std::unordered_map<std::string, std::any>::iterator Properties::end() { return data.end(); }
std::unordered_map<std::string, std::any>::const_iterator Properties::cbegin() const { return data.cbegin(); }
std::unordered_map<std::string, std::any>::const_iterator Properties::cend() const { return data.cend(); }

Properties::Properties() {}
Properties::Properties(std::initializer_list<std::pair<const std::string, std::any>> data) : data(data) {}
