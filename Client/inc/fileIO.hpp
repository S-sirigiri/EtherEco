#ifndef FILE_H
#define FILE_H

#include <filesystem>
#include <fstream>
#include <iostream>

#include <string.h>

namespace fs = std::filesystem;



///////////////////////
//ClASS DECLARATIONS//
/////////////////////


/*
 * Class FileIO
 * Encapsulates methods to handle file IO
 */
class FileIO {

public:
    static bool createDirectoryIfNotExists(const std::string &dirPath);
    static void readFile(const char* filename);
    static void writeToFile(const char* filename, const std::string& content);
};

#endif /* FILE_H */
