#include "SocketQueue.h"

SocketQueue::SocketQueue() : _listenSocket(INVALID_SOCKET), _isRunning(false) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        throw std::runtime_error("WSAStartup failed");
}

SocketQueue::~SocketQueue() {
    if (this->_isRunning)
    {
        this->stop();
        WSACleanup();
    }
}

void SocketQueue::start(const char* ip, const unsigned int port) {
    if (this->_isRunning)
#ifdef DEBUG
        throw std::runtime_error("SocketQueue is already running");
#else
        return;
#endif

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

void SocketQueue::stop() {
    if (!this->_isRunning)
#ifdef DEBUG
        throw std::runtime_error("SocketQueue is not running");
#else
        return;
#endif

    if (closesocket(this->_listenSocket) == SOCKET_ERROR)
        throw std::runtime_error("Failed to close socket");
    this->_isRunning = false;
}

SOCKET SocketQueue::acceptClient() const {
    if (!this->_isRunning)
#ifdef DEBUG
        throw std::runtime_error("SocketQueue is not running");
#else
        return INVALID_SOCKET;
#endif

    return accept(this->_listenSocket, nullptr, nullptr);
}

bool SocketQueue::sendItemToClient(const SOCKET clientSocket) {
    if (this->_queue.size_approx() == 0)
    {
        if (send(clientSocket, "", 0, 0) == SOCKET_ERROR)
            return false;
        return true;
    }

    std::string item;
    if (!this->_queue.try_dequeue(item))
#ifdef DEBUG
        throw std::runtime_error("Failed to dequeue item");
#else
        return false;
#endif

    if (send(clientSocket, item.c_str(), item.size(), 0) == SOCKET_ERROR)
    {
        this->_queue.enqueue(item);
        return false;
    }

    return true;
}
