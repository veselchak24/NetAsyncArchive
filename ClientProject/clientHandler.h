#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <memory>

#include <snappy.h>
#include <socketUtils.h>
#include "client/Client.h"

#ifdef LOG
#include <iostream>
#endif

int processingHandler(const Client& client) {
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

    return EXIT_SUCCESS;
}

#endif //CLIENTHANDLER_H
