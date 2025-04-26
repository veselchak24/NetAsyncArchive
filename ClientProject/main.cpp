#include <iostream>

#include "Client.h"
#include <snappy.h>

int main(const int argc, char* argv[]) {
    if (argc != 3)
        throw std::invalid_argument(
            "Wrong number of arguments. Expected 2: (server ip, port). But was: " + std::to_string(argc-1) + ".");

    const char* server_ip = argv[1];
    const uint16_t server_port = std::stoi(argv[2]);

    Client client;

    client.connectToServer(server_ip, server_port);

    while (true)
    {
        int size;
        const char* file = client.receiveData(size);

        std::cout << "Received file of size " << size << std::endl;

        if (file == nullptr)
            break;

        std::string compressedFile;
        snappy::Compress(file, size, &compressedFile);
        delete[] file;

        std::cout << "Sending file of size " << compressedFile.length() << " bytes..." << std::endl;

        client.sendData(compressedFile.c_str(), compressedFile.length());
    }

    client.disconnectFromServer();

    return 0;
}
