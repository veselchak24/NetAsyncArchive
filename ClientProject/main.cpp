#include <iostream>
#include <memory>

#include "clientHandler.h"
#include "client/Client.h"

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

    int value1 = processingHandler(client);

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

    return EXIT_SUCCESS;
}
