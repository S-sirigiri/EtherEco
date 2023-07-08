#include "fileIO.hpp"

/*
 * Class FileIO
 * Encapsulates methods to handle file IO
 */
class FileIO {

public:
    static bool createDirectoryIfNotExists(const std::string &dirPath)
    {
        if (!fs::exists(dirPath))
        {
            if (fs::create_directory(dirPath))
            {
                std::cout << "Directory created successfully." << std::endl;
                return true;
            }
            else
            {
                std::cout << "Failed to create directory." << std::endl;
                std::cerr << "Failed to create directory." << std::endl;
                exit(-1);
            }
        }
        else
        {
            std::cout << "Directory already exists." << std::endl;
            return false;
        }
    }

    static void readFile(const char* filename)
    {
        char path[124] = "chats/";
        strcat(path, filename);
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open chat: " << filename << std::endl;
            return;
        }

        std::string content;
        std::string line;
        while (std::getline(file, line))
        {
            content += line + "\n";
        }

        file.close();

        if (!content.empty())
        {
            std::cout << content << std::endl;
        }
    }

    static void writeToFile(const char* filename, const std::string& content)
    {
        std::string path = "chats/";
        path += filename;
        std::ofstream file(path, std::ios::app); // Open the file in append mode
        if (file.is_open())
        {
            file << content; // Write the string to the file
            file.close();
        }
        else
        {
            std::cerr << "Failed to open file: " << path << std::endl;
        }
    }
};