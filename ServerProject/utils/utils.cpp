#include "utils.h"

#include <iostream>

#include <filesystem>
#include <fstream>
#include <functional>

#include <socketUtils.h>
#include "../Server/Server.h"

#ifdef LOG
#include <cmath>
#endif

// supported functions
namespace
{
    /**
     * @brief Read file from disk and convert to binary array
     *
     * @param[in] path path to file
     * @param[out] buffer the binary buffer of file
     * @param[out] bufferSize size of buffer file
     * @param[in] offset offset of file
     *
     * @return true if end of file
     *
     * @throws std::runtime_error if file does not exist
     */
    bool getPartDataFile(const std::string& path, char*& buffer, int& bufferSize,
                         const buffer_size_type& offset);

    /**
     * @brief Function that generates buffer of file by part of INT_MAX size.
     * @param[out] buffer buffer of  binary data
     * @param[out] bufferSize size of buffer
     * @param[in] offset offset of file
     * @return isEndOfFile
     */
    using GenBuf = bool(char*& buffer, int& bufferSize, const buffer_size_type& offset);

    /// functor of GenBuf
    using GenBufFunc = std::function<GenBuf>;

    /**
     *
     * @param socket the client socket
     * @param path the path to current file
     * @param generateBufferFunc function that generates buffer of file. Must be noexcept
     */
    void processFile(const SOCKET& socket, const std::string& path, const GenBufFunc& generateBufferFunc);

    /**
     * @brief Create file and write data in binary format
     * @param path Path to file
     * @param buffer Data of file
     * @param bufferSize Size of data array
     *
     * @throws std::runtime_error if file does not exist or if write error
     */
    void appendCompressedFile(const std::string& path, const char* buffer, int bufferSize);
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
                     concurrentQueue<std::string>& queue) {
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

#ifdef LOG
    std::cout << "Host: " << host << ", port: " << port << ", path: " << argv[3] << std::endl;

    if (files.empty())
        std::cout << "No files found" << std::endl;
    else
    {
        std::cout << "Found files:" << std::endl;
        for (const std::string& file : files)
            std::cout << '\t' << std::setw(14) << std::filesystem::file_size(file) << 'b' << '\t' << file << std::endl;
    }
#endif
}

void handleClient(const Server* const server, const SOCKET& client,
                  concurrentQueue<std::string>* const socketQueue, std::atomic<unsigned int>& clientsCount) {
    if (server == nullptr)
        throw std::invalid_argument("Server is null");
    if (client == INVALID_SOCKET)
        throw std::invalid_argument("Client is null");
    if (socketQueue == nullptr)
        throw std::invalid_argument("SocketQueue is null");

    std::string path;

    while (socketQueue->try_dequeue(path))
    {
        try
        {
            processFile(client, path,
                        [&path](char*& buffer, int& bufferSize, const buffer_size_type& offset) {
                            return getPartDataFile(path, buffer, bufferSize, offset);
                        });
        }
        catch (const std::underflow_error& ex)
        {
            std::cout << "Error: " << ex.what() << std::endl;

            socketQueue->enqueue(path);

            --clientsCount;
            break;
        }
    }

    closesocket(client);

#ifdef LOG
    std::cout << "Client disconnected. Thread: " << GetCurrentThreadId() << std::endl;
#endif
}

namespace
{
    bool getPartDataFile(const std::string& path, char*& buffer, int& bufferSize,
                         const buffer_size_type& offset) {
        std::ifstream file(path, std::ios::binary);

        if (!file.is_open())
            throw std::runtime_error("Failed to open file " + path);

        const uintmax_t fileSize = std::filesystem::file_size(path) - offset;
        bufferSize = fileSize > INT_MAX ? INT_MAX : fileSize;

        file.seekg(offset);

        delete[] buffer;
        buffer = new char[bufferSize];

        try
        {
            file.read(buffer, bufferSize);
        }
        catch (std::exception& ex)
        {
            std::cerr << "Failed to read file " << path << " with exception: " << ex.what() << std::endl;
        }
        file.close();

        return bufferSize != INT_MAX;
    }


    void processFile(const SOCKET& socket, const std::string& path, const GenBufFunc& generateBufferFunc) {
        if (!socket)
            throw std::runtime_error("Socket for sending data is invalid");

        buffer_size_type sentBytes = 0;
        char* buffer = nullptr;
        int bufferSize;

#ifdef LOG
        const uintmax_t fileSize = std::filesystem::file_size(path);
        const uintmax_t countParts = static_cast<uintmax_t>(ceil(
            static_cast<double>(fileSize) / static_cast<double>(INT_MAX)));
        uintmax_t currentPart = 1;

        std::cout << "Sending file: " << path << " Size: " << fileSize << "b" << std::endl;
#endif

        bool endOfBuf = false;
        while (!endOfBuf)
        {
            endOfBuf = generateBufferFunc(buffer, bufferSize, sentBytes);

#ifdef LOG
            std::cout << "\t Part " << currentPart << "/" << countParts << ':' << std::endl;
            std::cout << "\t\tSend part " << currentPart << "/" << countParts << ':' << ' ';
#endif

            sendDataToSocket(socket, buffer, bufferSize);
            sentBytes += bufferSize;

#ifdef LOG
            std::cout << "passed" << std::endl;
            std::cout << "\t\tReceive part " << currentPart << "/" << countParts << ':' << ' ';
#endif

            recvDataFromSocket(socket, buffer, bufferSize);

#ifdef LOG
            std::cout << "passed" << std::endl;
            ++currentPart;
#endif

            const auto indexBackSlash = path.find_last_of('\\');
            std::string newPath = path.substr(0, indexBackSlash + 1) + "Compressed " + path.substr(
                                      indexBackSlash + 1);

#ifdef LOG
            std::cout << "\t\tBuffer size: " << bufferSize << "b\tAppend to compressed file: " << newPath;
#endif

            appendCompressedFile(newPath, buffer, bufferSize);

#ifdef LOG
            std::cout << " passed" << std::endl;
#endif
        }
    }

    void appendCompressedFile(const std::string& path, const char* buffer, const int bufferSize) {
        std::ofstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open())
            throw std::runtime_error("Failed to open file " + path);

        try
        {
            file.write(buffer, bufferSize);
        }
        catch (const std::exception&)
        {
            file.close();

            throw std::runtime_error("Failed to write to file " + path);
        }

        file.close();
    }
}
