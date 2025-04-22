#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <string>

class Server {
    SOCKET _listenSocket;
    bool _isRunning;

public:
    Server();

    ~Server();

    void start(const char* ip, unsigned int port);

    void stop();

    SOCKET acceptClient() const;

    bool sendItemToClient(SOCKET clientSocket, const char* item, size_t itemLength) const;

    bool receiveItemFromClient(SOCKET clientSocket, char*& item, size_t& bufferSize) const;
};

#endif //SERVER_H
