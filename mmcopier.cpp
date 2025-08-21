#include <iostream>
#include <pthread.h>
#include <fstream>
class fileData
{
public:
    std::string fileName;
    std::string sourceDir;
    std::string destDir;

    fileData(const std::string &fName, const std::string &sDir, const std::string &dDir)
    {
        fileName = fName;
        sourceDir = sDir;
        destDir = dDir;
    }
};

void *copyFile(void *arg)
{
    fileData *file = (fileData *)arg;

    std::string sourcePath = file->sourceDir + "/" + file->fileName;
    std::string destPath = file->destDir + "/" + file->fileName;

    std::ifstream src(sourcePath, std::ios::binary);
    std::ofstream dst(destPath, std::ios::binary);

    if (!src)
    {
        std::cerr << "Error opening source file: " << sourcePath << std::endl;
        return nullptr;
    }
    if (!dst)
    {
        std::cerr << "Error opening destination file: " << destPath << std::endl;
        return nullptr;
    }

    dst << src.rdbuf();

    return arg;
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Error: requires three arguements\n";
        return 1;
    }
    std::string numFilesStr = argv[1];
    int numFiles = 0;

    try
    {
        numFiles = std::stoi(numFilesStr);
        if (numFiles < 2 || numFiles > 10)
        {
            std::cerr << "Error: number of files must be between 2 & 10\n";
            return 1;
        }
    }
    catch (const std::invalid_argument &)
    {
        std::cerr << "Error: first arguement must be a valid integer\n";
        return 1;
    }
    catch (const std::out_of_range &)
    {
        std::cerr << "Error: integer out of range\n";
        return 1;
    }

    char *sourceDir = argv[2]; // char arrays
    char *destDir = argv[3];   // char arrays

    std::string source = "source";
    std::string txt = ".txt";
    pthread_t threads[numFiles];
    for (int i = 0; i < numFiles; i++)
    {
        std::string name = source + std::to_string(i + 1) + txt;
        fileData *file = new fileData(name, sourceDir, destDir); // char arrays are implcitly changed to std::string
        pthread_create(&threads[i], nullptr, copyFile, file);
    }

    for (int i = 0; i < numFiles; ++i)
    {
        void *ptr;
        pthread_join(threads[i], &ptr);
        delete static_cast<fileData *>(ptr);
    }

    std::cout << "Main thread exiting." << std::endl;
    return 0;
}