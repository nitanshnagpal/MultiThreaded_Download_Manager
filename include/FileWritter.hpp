// FileWriter.hpp
#ifndef FILEWRITER_HPP
#define FILEWRITER_HPP

#include <fstream>
#include <string>

class FileWriter {
private:
    std::ofstream fileStream;

public:
    FileWriter(const std::string& filePath);
    ~FileWriter();
    int write(char* data, int size);
    void close();
};

#endif // FILEWRITER_HPP