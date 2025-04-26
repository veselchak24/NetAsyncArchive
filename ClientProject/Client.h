#ifndef CLIENT_H
#define CLIENT_H

#include <cstdint>
#include <string>
#include <winsock2.h>

class Client {
    SOCKET _socket;
    bool _isInitialized;

public:
    Client();

    ~Client();

    void connectToServer(const char* ip, uint16_t port);

    char* receiveData(int& bufferSize) const;

    void sendData(const char* buffer, int bufferSize) const;

    void disconnectFromServer() const;

private:
    bool isConnected() const noexcept { return this->_socket != INVALID_SOCKET; }
};


#endif //CLIENT_H
