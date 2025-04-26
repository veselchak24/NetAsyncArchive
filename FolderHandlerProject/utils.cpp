#include "utils.h"

#include <iostream>

#include <filesystem>
#include <fstream>

// supported functions
namespace
{
    /**
     * @brief Read file from disk and convert to binary array
     *
     * @param path path to file
     * @param bufferSize size of buffer file
     *
     * @return The binary buffer of file
     *
     * @throws std::runtime_error if file does not exist
     */
    char* getDataFile(const std::string& path, int& bufferSize);

    /**
     * @brief Create file and write data in binary format
     * @param path Path to file
     * @param data Data of file
     * @param size Size of data array
     *
     * @throws std::runtime_error if file does not exist or if write error
     */
    void createCompressedFile(const std::string& path, const char* data, int size);
}

std::vector<std::string> getAllFiles(const std::string& path) {
    std::vector<std::string> files;

    if (!std::filesystem::exists(path))
        throw std::invalid_argument("Path does not exist");

    for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
        if (!entry.is_directory())
            files.push_back(entry.path().string());

    return files;
}

void processingInput(const int argc, const char** const argv, char*& host, int& port,
                     moodycamel::ConcurrentQueue<std::string>& queue) {
    if (argc != 4) // command, host, port, path
        throw std::invalid_argument(
            "Incorrect number of arguments. Expected 3 (port,host,path). But was:" + std::to_string(argc - 1));

    host = const_cast<char*>(argv[1]);

    port = std::stoi(argv[2]);
    if (port <= 0 || port > 65535) // check for valid port number
        throw std::invalid_argument("Invalid port number");

    std::vector<std::string> files = getAllFiles(argv[3]);

    if (!files.empty())
        queue.enqueue_bulk(files.begin(), files.size());

#ifdef DEBUG
    std::cout << "Host: " << host << ", port: " << port << ", path: " << argv[3] << std::endl;

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
    if (server == nullptr)
        throw std::invalid_argument("Server is null");
    if (client == INVALID_SOCKET)
        throw std::invalid_argument("Client is null");
    if (socketQueue == nullptr)
        throw std::invalid_argument("SocketQueue is null");

    std::string path;

    while (socketQueue->try_dequeue(path))
    {
        int bufferSize;
        char* buffer(getDataFile(path, bufferSize));

        if (!server->sendItemToClient(client, buffer, bufferSize))
        {
            socketQueue->enqueue(path);
            break;
        }

        server->receiveItemFromClient(client, buffer, bufferSize);

        const auto indexBackSlash = path.find_last_of('\\');
        std::string newPath = path.substr(0, indexBackSlash + 1) + "Compressed " + path.substr(indexBackSlash + 1);
        createCompressedFile(newPath, buffer, bufferSize);

        delete[] buffer;
    }

    closesocket(client);
}

namespace
{
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
}
