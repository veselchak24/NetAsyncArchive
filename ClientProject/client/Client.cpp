#include "Client.h"

#include <stdexcept>

#ifdef DEBUG
#include <iostream>
#endif

Client::Client() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        throw std::runtime_error("WSAStartup failed");

    this->_socket = INVALID_SOCKET;
    this->m_isInitialized = true;
}

Client::~Client() {
    if (this->isConnected())
        this->disconnectFromServer();

    if (this->m_isInitialized)
        WSACleanup();
}

void Client::connectToServer(const char* ip, const uint16_t port) {
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

void Client::disconnectFromServer() {
    if (!this->isConnected())
        throw std::runtime_error("Client is not connected to server");

    if (closesocket(this->_socket) == SOCKET_ERROR)
        throw std::runtime_error("Failed to close socket");

    this->_socket = INVALID_SOCKET;
}
