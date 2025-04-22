#include <iostream>

#include "Client.h"
#include <snappy.h>

int main() {
    Client client;

    client.connectToServer("127.0.0.1", 1234);

    while (true)
    {
        size_t size;
        char* file = client.receiveData(size);

        if (size == 0)
            break;

        std::string compressedFile;
        snappy::Compress(file, size, &compressedFile);

        client.sendData(compressedFile.c_str(), compressedFile.length());
    }

    client.disconnectFromServer();

    return 0;
}
