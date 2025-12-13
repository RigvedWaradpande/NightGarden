#include "CSVLoader.h"
#include <fstream>
#include <sstream>

std::vector<Bar> LoadCSV(const std::string& filePath){
    std::vector<Bar> data;

    std::ifstream file(filePath);
    std::string line;

    std::getline(file, line); //consumes the first line
    int counter = 0;
    while(std::getline(file, line)){
        std::stringstream ss(line);
        std::string token;

        Bar bar;
        bar.timestamp = ++counter;
        std::getline(ss, token, ',');
        bar.date = token;
        std::getline(ss, token, ',');
        bar.close = std::stod(token);
        std::getline(ss, token, ',');
        bar.high = std::stod(token);
        std::getline(ss, token, ',');
        bar.low = std::stod(token);
        std::getline(ss, token, ',');
        bar.open = std::stod(token);
        std::getline(ss, token, ',');
        bar.volume = std::stol(token);

        data.push_back(bar);
    }
    file.close();
    return data;
}