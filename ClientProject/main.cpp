#include <iostream>

#include "Client.h"
#include <snappy.h>

int main() {
    Client client;

    client.connectToServer("127.0.0.1", 1234);

    while (true)
    {
        std::string file = client.receiveData();

        if (file.empty())
            break;

        std::string compressedFile;
        snappy::Compress(file.c_str(), file.length(), &compressedFile);

        client.sendData(compressedFile.c_str(), compressedFile.length());
    }

    client.disconnectFromServer();

    return 0;
}
