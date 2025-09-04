#include <iostream>
#include <pthread.h>
#include <fstream>
#include <cstdlib>
#include <sys/stat.h>

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
    std::ofstream dst(destPath, std::ios::binary); // creates file if it doesnt exist

    if (!src)
    {
        std::cerr << "Error opening source dir: " << sourcePath << std::endl;
        return nullptr;
    }
    if (!dst)
    {
        std::cerr << "Error opening destination dir: " << destPath << std::endl;
        return nullptr;
    }

    dst << src.rdbuf(); // writes to destination file

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

    char *endptr;
    numFiles = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || numFiles < 2 || numFiles > 10)
    {
        std::cerr << "Error: number of files must be a valid integer between 2 & 10\n";
        return 1;
    }

    char *sourceDir = argv[2];
    char *destDir = argv[3];

    struct stat st = {0};

    if (stat(sourceDir, &st) != 0 || !S_ISDIR(st.st_mode))
    {
        std::cerr << "Error: source directory does not exist: " << sourceDir << std::endl;
        return 1;
    }

    if (stat(destDir, &st) != 0 || !S_ISDIR(st.st_mode))
    {
        std::cerr << "Error: destination directory does not exist: " << destDir << std::endl;
        return 1;
    }

    std::string source = "source";
    std::string txt = ".txt";
    pthread_t threads[numFiles];
    int returnValue;

    for (int i = 0; i < numFiles; i++)
    {

        std::string name = source + std::to_string(i + 1) + txt;
        fileData *file = new fileData(name, sourceDir, destDir);
        returnValue = pthread_create(&threads[i], nullptr, copyFile, file);

        if (returnValue != 0)
        {
            delete file;
            std::cerr << "Error when making thread" << std::endl;
            return 1;
        }
    }

    for (int i = 0; i < numFiles; ++i)
    {
        void *ptr;
        returnValue = pthread_join(threads[i], &ptr);
        if (returnValue != 0)
        {
            std::cerr << "Error when making thread" << std::endl;
            return 1;
        }
        delete static_cast<fileData *>(ptr);
    }

    return 0;
}