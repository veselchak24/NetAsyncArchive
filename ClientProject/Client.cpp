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

void Client::connectToServer(const char* ip, const uint16_t port) {
    if (this->isConnected())
        throw std::runtime_error("Client already connected to server");

    if (port <= 0 || port > 65535)
        throw std::runtime_error("Invalid port number");

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

char* Client::receiveData(int& bufferSize) const {
    if (!this->isConnected())
        throw std::runtime_error("Client is not connected to server");

    // receive buffer size - 4 bytes int
    if (!recv(this->_socket, reinterpret_cast<char*>(&bufferSize), sizeof(bufferSize), 0))
        return nullptr;

    if (bufferSize <= 0)
        throw std::runtime_error("Client received empty buffer");

    char* buffer = new char[bufferSize];
    if (recv(this->_socket, buffer, bufferSize, MSG_WAITALL) != bufferSize)
        throw std::runtime_error("Failed to receive data from server");

    return buffer;
}

void Client::sendData(const char* buffer, const int bufferSize) const {
    if (!this->isConnected())
        throw std::runtime_error("Client is not connected to server");

    if (bufferSize <= 0)
        throw std::runtime_error("Buffer size must be greater than zero");

    if (send(this->_socket, reinterpret_cast<const char*>(&bufferSize), sizeof(bufferSize), 0) != sizeof(bufferSize))
        throw std::runtime_error("Failed to send bufferSize to server");

    if (send(this->_socket, buffer, bufferSize, 0) != bufferSize)
        throw std::runtime_error("Failed to send data");
}

void Client::disconnectFromServer() const {
    if (!this->isConnected())
        throw std::runtime_error("Client is not connected to server");

    closesocket(this->_socket);
}
