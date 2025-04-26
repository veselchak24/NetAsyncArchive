#ifndef SERVER_H
#define SERVER_H

#include <cstdint>
#include <winsock2.h>

/**
 * @class Server Server.h
 *
 * @brief Server for communication with client
 */
class Server {
    /// Server socket
    SOCKET _listenSocket;

    /// Flag of server running
    bool _isRunning;

public:
    /**
     * @brief Construct a new Server object
     *
     * @throws std::runtime_error If server can't be initialized(WSAStartup)
     */
    Server();

    ~Server() noexcept;

    /**
     *@brief Start listening server on created socket
     *
     * @param ip ip for server
     * @param port port for server
     *
     * @throws std::runtime_error: If server can't be started or initialization failed
     */
    void start(const char* ip, uint16_t port);

    /**
     * @brief Stop listening server and close the socket
     *
     * @throws std::runtime_error If server can't be stopped
     */
    void stop();

    /**
     * @brief Accept the next client connection. Block thread until connection
     *
     * @return accepted client socket
     * @throws std::runtime_error: If server not running
     */
    SOCKET acceptClient() const;

    /**
     * @brief Send data as array of char to client
     *
     * @param clientSocket socket of client to send
     * @param item array of char to send
     * @param itemLength size of item
     *
     * @return status of sending
     *
     * @throws std::runtime_error: If server not running
     * @throws std::invalid_argument: If put incorrect parameters
     */
    bool sendItemToClient(SOCKET clientSocket, const char* item, int itemLength) const;

    /**
     * @brief Receive data as array of char from client
     *
     * @param clientSocket socket of client to receive
     * @param item array of char to receive. Must be initialized or be nullptr
     * @param bufferSize size of buffer
     *
     * @return status of receiving
     *
     * @throws std::runtime_error: If put incorrect parameters
     */
    bool receiveItemFromClient(SOCKET clientSocket, char*& item, int& bufferSize) const;

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(Server&&) = delete;
};

#endif //SERVER_H
