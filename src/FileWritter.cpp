// FileWriter.cpp
#include "FileWritter.hpp"
#include <iostream>

FileWriter::FileWriter(const std::string& filePath) {
    fileStream.open(filePath, std::ofstream::out | std::ofstream::binary);
}

FileWriter::~FileWriter() {
    if (fileStream.is_open()) {
        fileStream.close();
    }
}

int FileWriter::write(char* data, int size) {
    if (!fileStream.is_open() || !fileStream.good()) {
        std::cerr << "File not opened" << std::endl;
        return -1; // Indicate error
    }
    fileStream.write(data, size);
    if (fileStream.fail()) {
        std::cerr << "Error writing to file" << std::endl;
        return -1; // Indicate error if the write operation failed
    }
    return size; // Return the number of bytes intended to be written
}

void FileWriter::close() {
    if (fileStream.is_open()) {
        fileStream.close();
    }
}