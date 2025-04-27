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
    bool m_isInitialized = false;

    inline bool isRunning() const { return _listenSocket != INVALID_SOCKET; }

public:
    /**
     * @brief Construct a new Server object
     *
     * @throws std::runtime_error If server can't be initialized(WSAStartup)
     */
    Server();

    ~Server() noexcept;

public:
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

    Server(const Server&) = delete;

    Server& operator=(const Server&) = delete;

    Server(Server&&) = delete;

    Server& operator=(Server&&) = delete;
};

#endif //SERVER_H
