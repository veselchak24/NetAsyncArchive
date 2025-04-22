#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <winsock2.h>

class Client {

    SOCKET _socket;
    bool _isInitialized;
public:

    Client();

    ~Client();

    void connectToServer(const char *ip, unsigned int port);

    std::string receiveData() const;

    void sendData(const char *buffer, int bufferSize) const;

    void disconnectFromServer() const;
private:
    bool isConnected() const noexcept {return this->_socket!=INVALID_SOCKET;}
};


#endif //CLIENT_H
