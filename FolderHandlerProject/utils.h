#ifndef PATHUTILS_H
#define PATHUTILS_H

#include <string>
#include <vector>

#include <concurrentqueue/concurrentqueue.h>
#include "Server/Server.h"

#include <winsock2.h>

std::vector<std::string> getAllFiles(const std::string& path);

void processingInput(int argc, const char** argv, moodycamel::ConcurrentQueue<std::string>& queue);

void handleClient(const Server& server, const SOCKET& client,
                  moodycamel::ConcurrentQueue<std::string>* const socketQueue);

char* getDataFile(const std::string& path, size_t& bufferSize);

void createCompressedFile(const std::string& path, const char* data, size_t size);

#ifdef DEBUG
#include <fstream>
#include <filesystem>

inline void WriteAllText(const std::string& path, const std::string& text) {
    if (std::ofstream file(path); file.is_open())
    {
        file << text;
        file.close();
    } else
        throw std::runtime_error("Can't create file");
}

inline void createTestFiles() {
    if (std::filesystem::exists("testFolder"))
        std::filesystem::remove_all("testFolder");

    std::filesystem::create_directory("testFolder");
    for (int i = 0; i < 10; i++)
    {
        std::ofstream file("testFolder/file" + std::to_string(i) + ".txt");
        for (int j = 0; j < 1000000; j++)
            file << static_cast<char>('A' + rand() % 26);
        file.close();
    }
}

#endif
#endif //PATHUTILS_H