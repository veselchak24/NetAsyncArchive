#include "Server.h"

#include <stdexcept>
#include <thread>

#ifdef LOG
#include <iostream>
#endif

Server::Server() : _listenSocket(INVALID_SOCKET) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        throw std::runtime_error("WSAStartup failed");

    this->m_isInitialized = true;
}

Server::~Server() {
    if (this->isRunning())
        this->stop();

    if (this->m_isInitialized)
        WSACleanup();
}

void Server::start(const char* ip, const uint16_t port) {
    if (this->isRunning())
        throw std::runtime_error("Server is already running");

    this->_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = inet_addr(ip);

    if (bind(this->_listenSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) ==
        SOCKET_ERROR)
        throw std::runtime_error("Failed to bind socket");

    if (listen(this->_listenSocket, SOMAXCONN) == SOCKET_ERROR)
        throw std::runtime_error("Failed to listen on socket");
}

void Server::stop() {
    if (!this->isRunning())
        throw std::runtime_error("Server is not running");

    if (closesocket(this->_listenSocket) == SOCKET_ERROR)
        throw std::runtime_error("Failed to close socket with error code: " + std::to_string(WSAGetLastError()));

    this->_listenSocket = INVALID_SOCKET;

#ifdef LOG
    std::cout << "SERVER STOPPED" << std::endl;
#endif
}

SOCKET Server::acceptClient() const {
    if (!this->isRunning())
        throw std::runtime_error("Server is not running");

    return accept(this->_listenSocket, nullptr, nullptr);
}
