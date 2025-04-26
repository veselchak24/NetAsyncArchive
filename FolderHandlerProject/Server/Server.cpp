#include "Server.h"

#include <stdexcept>

#ifdef DEBUG
#include <iostream>
#endif

Server::Server() : _listenSocket(INVALID_SOCKET), _isRunning(false) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        throw std::runtime_error("WSAStartup failed");
}

Server::~Server() {
    if (this->_isRunning)
        this->stop();

    WSACleanup();
}

void Server::start(const char* ip, const uint16_t port) {
    if (this->_isRunning)
        throw std::runtime_error("Server is already running");

    this->_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = inet_addr(ip);

    if (bind(this->_listenSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) ==
        SOCKET_ERROR)
        throw std::runtime_error("Failed to bind socket");

    if (listen(this->_listenSocket, SOMAXCONN) == SOCKET_ERROR)
        throw std::runtime_error("Failed to listen on socket");

    this->_isRunning = true;
}

void Server::stop() {
    if (!this->_isRunning)
        throw std::runtime_error("Server is not running");

    if (closesocket(this->_listenSocket) == SOCKET_ERROR)
        throw std::runtime_error("Failed to close socket");

    this->_isRunning = false;

#ifdef DEBUG
    std::cout << "Server stopped" << std::endl;
#endif
}

SOCKET Server::acceptClient() const {
    if (!this->_isRunning)
        throw std::runtime_error("Server is not running");

    return accept(this->_listenSocket, nullptr, nullptr);
}

bool Server::sendItemToClient(const SOCKET clientSocket, const char* item, const int itemLength) const {
    if (!this->_isRunning)
        throw std::runtime_error("Server is not running");

    if (!item)
        throw std::invalid_argument("Item cannot be nullptr");

    if (itemLength < 0)
        throw std::invalid_argument("Item length must be greater than 0");

    // send item length
    const int bytes = send(clientSocket, reinterpret_cast<const char*>(&itemLength), sizeof(itemLength), 0);

    if (bytes == SOCKET_ERROR)
        return false;
    if (itemLength == 0)
        return true;

    return send(clientSocket, item, itemLength, 0) != SOCKET_ERROR;
}

bool Server::receiveItemFromClient(const SOCKET clientSocket, char*& item, int& bufferSize) const {
    if (!this->_isRunning)
        throw std::runtime_error("Server is not running");

    recv(clientSocket, reinterpret_cast<char*>(&bufferSize), sizeof(bufferSize), MSG_WAITALL);

    delete[] item;
    item = new char[bufferSize];

    if (bufferSize == 0)
        throw std::runtime_error("Failed to receive item from client");

    return recv(clientSocket, item, bufferSize, MSG_WAITALL) != SOCKET_ERROR;
}
