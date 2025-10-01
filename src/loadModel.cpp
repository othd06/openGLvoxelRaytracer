

#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "loadModel.h"


std::vector<std::vector<std::vector<unsigned char>>> getModelData(const Model& m, const std::vector<unsigned char>& data)
{
    std::vector<std::vector<std::vector<unsigned char>>> modelData;

    for (int x = 0; x < m.xDim; x++)
    {
        std::vector<std::vector<unsigned char>> plane = {};
        modelData.push_back(plane);
        for (int y = 0; y < m.yDim; y++)
        {
            std::vector<unsigned char> row = {};
            modelData[x].push_back(row);
            for (int z = 0; z < m.zDim; z++)
            {
                modelData[x][y].push_back(data[x*m.zDim*m.yDim + z*m.yDim + y]);
            }
        }
    }
    return modelData;
}

std::vector<unsigned char> raw(const Model& m, const std::vector<unsigned char>& data)
{
    std::vector<unsigned char> output;
    int i = 0;
    while (i < data.size())
    {
        for (int j = 0; j < data[i+1]; j++)
        {
            output.push_back(data[i]);
        }
        i += 2;
    }
    return output;
}

Model loadModel(std::string path)
{
    Model result;
    std::ifstream f(path, std::ios::binary);
    if (!f) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    std::string line;

    //parse ASCII header until data
    while (std::getline(f, line)) {
        if (line.empty()) continue;

        if (line.rfind("dim", 0) == 0) { // starts with "dim"
            std::istringstream iss(line);
            std::string keyword;
            iss >> keyword >> result.xDim >> result.yDim >> result.zDim;
        } else if (line == "data") {
            break;
        }
    }
     // Read the rest of the file as raw bytes
    std::vector<unsigned char> dataBytes(
        (std::istreambuf_iterator<char>(f)),
        std::istreambuf_iterator<char>()
    );

    std::vector<unsigned char> rawBytes = raw(result, dataBytes);
    result.data = getModelData(result, rawBytes);
    return result;
}


std::vector<unsigned char> flatData(Model m)
{
    std::vector<unsigned char> output = {};
    for (int z = 0; z < m.zDim; z++)
    {
        for (int y = 0; y < m.yDim; y++)
        {
            for (int x = 0; x < m.xDim; x++)
            {
                output.push_back(m.data[x][y][z]);
            }
        }
    }
    return output;
}

