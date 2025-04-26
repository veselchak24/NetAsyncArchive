#include <iostream>

#include "Client.h"
#include <snappy.h>

int main() {
    Client client;

    client.connectToServer("127.0.0.1", 1234);

    while (true)
    {
        int size;
        const char* file = client.receiveData(size);

        std::cout << "Received file of size " << size << std::endl;

        if (file == nullptr)
            break;

        std::string compressedFile;
        snappy::Compress(file, size, &compressedFile);

        std::cout << "Sending file of size " << compressedFile.length() << " bytes..." << std::endl;

        client.sendData(compressedFile.c_str(), compressedFile.length());
    }

    client.disconnectFromServer();

    return 0;
}
