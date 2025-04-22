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

void handleClient(const Server& server, const SOCKET& client,
                  moodycamel::ConcurrentQueue<std::string>* const socketQueue) {
    char* buffer;
    size_t bufferSize;
    std::string path;

    while (true)
    {
        if (!socketQueue->try_dequeue(path))
        {
            server.sendItemToClient(client, {}, 0);
            break;
        } else
            buffer = getDataFile(path, bufferSize);

        if (!server.sendItemToClient(client, buffer, bufferSize))
            break;

        server.receiveItemFromClient(client, buffer, bufferSize);

        const auto indexBackSlash = path.find_last_of('\\');
        std::string newPath = path.substr(indexBackSlash + 1) + "Compressed" + path.substr(indexBackSlash + 1);
        createCompressedFile(newPath, buffer, bufferSize);
    };
}

char* getDataFile(const std::string& path, size_t& bufferSize) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file.is_open())
        throw std::runtime_error("Ошибка открытия файла");

    bufferSize = file.tellg();

    file.seekg(0, std::ios::beg);

    auto data = new char[bufferSize];
    file.read(data, bufferSize);

    return data;
}

void createCompressedFile(const std::string& path, const char* data, size_t size) {
    std::ofstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        throw std::runtime_error("Ошибка открытия файла");

    file.write(data, size);

    if (!file.good())
        throw std::runtime_error("Ошибка записи в файл");

    file.close();
}
