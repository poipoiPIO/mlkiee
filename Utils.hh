#pragma once

#include <string>

bool isNum(const std::string& val) {
  try {std::stoi(val); } catch (...) {
    return false;
  } return true; 
}
