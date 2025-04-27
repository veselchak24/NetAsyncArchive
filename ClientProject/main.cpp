#include <iostream>
#include <memory>

#include <snappy.h>
#include <socketUtils.h>
#include "Client.h"

int main(const int argc, char* argv[]) {
    if (argc != 3)
        throw std::invalid_argument(
            "Wrong number of arguments. Expected 2: (server ip, port). But was: " + std::to_string(argc - 1) + ".");

    const char* server_ip = argv[1];
    const uint16_t server_port = std::stoi(argv[2]);

    Client client;

#ifdef LOG
    std::cout << "Connecting to server...";
#endif
    try
    {
        client.connectToServer(server_ip, server_port);
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

#ifdef LOG
    std::cout << " passed" << std::endl;
#endif

    bool isProcessing = true;
    int bufferSize;
    std::unique_ptr<char> buffer = nullptr;

    while (isProcessing)
    {
        do
        {
            char* buf = buffer.release();

#ifdef LOG
            std::cout << "\tReceiving data... ";
#endif
            try
            {
                recvDataFromSocket(client.getSocket(), buf, bufferSize);
            }
            catch (const std::exception& ex)
            {
                delete[] buf;
                std::cerr << ex.what() << std::endl;
                return EXIT_FAILURE;
            }
            buffer.reset(buf);
#ifdef LOG
            std::cout << " passed. Size: " << bufferSize << std::endl;
#endif

            // end of processing
            if (bufferSize == 0)
            {
                // end of file processing
                if (buffer == nullptr)
                    isProcessing = false;
                break;
            }

#ifdef LOG
            std::cout << "\tCompressing data...";
#endif
            std::string compressedFile;
            snappy::Compress(buffer.get(), bufferSize, &compressedFile);
#ifdef LOG
            std::cout << " passed. Size: " << compressedFile.length() << std::endl;
            std::cout << "Sending data...";
#endif
            try
            {
                sendDataToSocket(client.getSocket(), compressedFile.c_str(), compressedFile.length());
            }
            catch (const std::exception& ex)
            {
                std::cerr << ex.what() << std::endl;
                return EXIT_FAILURE;
            }
#ifdef LOG
            std::cout << " passed." << std::endl;
#endif
        } while (bufferSize > 0);
    }

    try
    {
        client.disconnectFromServer();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

#ifdef LOG
    std::cout << "Disconnected from server." << std::endl;
#endif

    return 0;
}
