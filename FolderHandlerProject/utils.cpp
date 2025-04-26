#include "utils.h"

#include <iostream>

std::vector<std::string> getAllFiles(const std::string& path) {
    std::vector<std::string> files;
    if (!std::filesystem::exists(path))
        throw std::invalid_argument("Path does not exist");

    for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
        if (!entry.is_directory())
            files.push_back(entry.path().string());

    return files;
}

void processingInput(const int argc, const char** argv, moodycamel::ConcurrentQueue<std::string>& queue) {
    if (argc != 2)
    {
        const std::string errorMessage = "Incorrect number of arguments. Expected 1. But was:" +
                                         std::to_string(argc - 1);
        throw std::runtime_error(errorMessage);
    }

    std::vector<std::string> files = getAllFiles(argv[1]);

    if (!files.empty())
        queue.enqueue_bulk(files.begin(), files.size());

#ifdef DEBUG
    if (files.empty())
        std::cout << "No files found" << std::endl;
    else
    {
        std::cout << "Found files:" << std::endl;
        for (const auto& file : files)
            std::cout << '\t' << file << std::endl;
    }
#endif
}

void handleClient(const Server* const server, const SOCKET& client,
                  moodycamel::ConcurrentQueue<std::string>* const socketQueue) {
    std::string path;

    while (true)
    {
        if (!socketQueue->try_dequeue(path))
        {
            closesocket(client);
            break;
        }

        int bufferSize;
        char* buffer(getDataFile(path, bufferSize));

        if (!server->sendItemToClient(client, buffer, bufferSize))
        {
            socketQueue->enqueue(path);
            closesocket(client);
            break;
        }

        server->receiveItemFromClient(client, buffer, bufferSize);

        const auto indexBackSlash = path.find_last_of('\\');
        std::string newPath = path.substr(0, indexBackSlash + 1) + "Compressed" + path.substr(indexBackSlash + 1);
        createCompressedFile(newPath, buffer, bufferSize);

        delete[] buffer;
    }
}

char* getDataFile(const std::string& path, int& bufferSize) {
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("Failed to open file " + path);

    bufferSize = static_cast<int>(std::filesystem::file_size(path));

    char* data = new char[bufferSize];
    file.read(data, bufferSize);

    file.close();
    return data;
}

void createCompressedFile(const std::string& path, const char* data, const int size) {
    std::ofstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        throw std::runtime_error("Failed to open file " + path);

    file.write(data, size);

    if (!file.good())
        throw std::runtime_error("Failed to write to file " + path);

    file.close();

#ifdef DEBUG
    std::cout << "Created compressed file: " << path << std::endl;
#endif
}
