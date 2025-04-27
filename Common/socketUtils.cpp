#include "socketUtils.h"

#include <stdexcept>

void sendDataToSocket(const SOCKET socket, const char* buffer, const int bufferSize) {
    if (socket == INVALID_SOCKET)
        throw std::runtime_error("Socket for sending data is invalid");

    if (!buffer)
        throw std::invalid_argument("Item cannot be nullptr");

    if (bufferSize <= 0)
        throw std::invalid_argument("Buffer size must be greater than zero");

    // send buffer size
    if (send(socket, reinterpret_cast<const char*>(&bufferSize), sizeof(bufferSize), 0) != sizeof(bufferSize))
        throw std::underflow_error("Failed to send bufferSize to server");

    // send buffer
    if (send(socket, buffer, bufferSize, 0) != bufferSize)
        throw std::underflow_error("Failed to send buffer to server");
}

void recvDataFromSocket(const SOCKET socket, char*& buffer, int& bufferSize) {
    if (socket == INVALID_SOCKET)
        throw std::runtime_error("Socket for sending data is invalid");

    // receive buffer size - 4 bytes int
    const int bytes = recv(socket, reinterpret_cast<char*>(&bufferSize), sizeof(bufferSize), MSG_WAITALL);

    if (bytes == SOCKET_ERROR)
        throw std::runtime_error("Failed to receive bufferSize from server");

    // data ended
    if (bytes == 0)
    {
        bufferSize = 0;

        delete[] buffer;
        buffer = nullptr;

        return;
    }

    if (bufferSize == 0)
        throw std::runtime_error("Buffer size cannot be zero");

    delete[] buffer;
    buffer = new char[bufferSize];

    if (recv(socket, buffer, bufferSize, MSG_WAITALL) != bufferSize)
        throw std::underflow_error("Failed to receive buffer from server");
}
