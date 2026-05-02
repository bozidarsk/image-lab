#include "Properties.h"

Properties::Properties() {}
Properties::Properties(std::initializer_list<std::pair<const std::string, std::any>> data) : data(data) {}
