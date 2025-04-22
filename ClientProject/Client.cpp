#include "Client.h"

#include <stdexcept>

Client::Client() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        throw std::runtime_error("WSAStartup failed");

    this->_socket = INVALID_SOCKET;
    this->_isInitialized = true;
}

Client::~Client() {
    if (this->isConnected())
        this->disconnectFromServer();

    if (this->_isInitialized)
        WSACleanup();
}

void Client::connectToServer(const char* ip, const unsigned int port) {
    if (this->isConnected())
        throw std::runtime_error("Client already connected to server");

    // ??? WSASocket multithread
    this->_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (this->_socket == INVALID_SOCKET)
        throw std::runtime_error("Failed to create socket");

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(ip);

    if (connect(this->_socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == SOCKET_ERROR)
        throw std::runtime_error("Failed to connect to server");
}

std::string Client::receiveData() const {
    if (!this->isConnected())
        throw std::runtime_error("Client is not connected to server");

    // receive buffer size - 4 bytes int
    int bufferSize;
    recv(this->_socket, reinterpret_cast<char*>(&bufferSize), sizeof(bufferSize), 0);

    char buffer[bufferSize];
    const size_t bytes = recv(this->_socket, buffer, bufferSize, 0);
    if (bytes > 0)
        return {buffer, bytes};
    else
        return {};
}

void Client::sendData(const char* buffer, const int bufferSize) const {
    if (!this->isConnected())
        throw std::runtime_error("Client is not connected to server");

    if (bufferSize <= 0)
        throw std::runtime_error("Buffer size must be greater than zero");

    if (send(this->_socket, buffer, bufferSize, 0) == SOCKET_ERROR)
        throw std::runtime_error("Failed to send data");
}

void Client::disconnectFromServer() const {
    if (!this->isConnected())
        throw std::runtime_error("Client is not connected to server");

    closesocket(this->_socket);
}
