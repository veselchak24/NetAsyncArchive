#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <fstream>
#include <functional>
#include <iosfwd>

#include <winsock2.h>

using buffer_size_type = std::ifstream::pos_type;

//**
// * @brief Send data in buffer to client socket.
// *
// * @param socket The socket to send data to
// * @param buffer The buffer to send data from
// * @param bufferSize The size of the buffer
// *
// * @throws std::invalid_argument If parameters are incorrect
// * @throws std::underflow_error If failed to send data
// * @throws std::runtime_error If socket is invalid
// */
void sendDataToSocket(SOCKET socket, const char* buffer, int bufferSize);

//**
// * @brief Receive data from client socket into buffer.
// *
// * @param clientSocket The socket to receive data from
// * @param buffer The buffer to receive data into
// * @param bufferSize The size of the buffer
// *
// * @return The number of bytes received
// *
// * @throws std::invalid_argument If parameters are incorrect
// * @throws std::underflow_error If failed to receive data
// * @throws std::runtime_error If socket is invalid or failed to receive bufferSize
// */
void recvDataFromSocket(SOCKET socket, char*& buffer, int& bufferSize);

#endif //SOCKET_UTILS_H
