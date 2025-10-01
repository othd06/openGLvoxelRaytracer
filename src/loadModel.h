

#pragma once
#ifndef LOADMODEL
#define LOADMODEL

#include <vector>


struct Model
{
    std::vector<std::vector<std::vector<unsigned char>>> data;
    int xDim;
    int yDim;
    int zDim;
};

Model loadModel(std::string path);

std::vector<unsigned char> flatData(Model m);


#endif
