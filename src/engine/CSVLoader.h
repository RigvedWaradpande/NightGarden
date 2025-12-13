#ifndef CSVLOADER_H
#define CSVLOADER_H

#include <vector>
#include <string>
#include "../include/types.h"

std::vector<Bar> LoadCSV(const std::string& filePath);

#endif