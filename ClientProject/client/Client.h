#ifndef CLIENT_H
#define CLIENT_H

#include <cstdint>
#include <winsock2.h>

/**
 * @class Client Client.h
 *
 * @brief Client for communication with server
 */
class Client {
    SOCKET _socket;

    /// flag initialized WSAStartup
    bool m_isInitialized = false;

    /// true if connection to server
    bool isConnected() const noexcept { return this->_socket != INVALID_SOCKET; }

public:
    /// constructor
    /// @throws std::runtime_error Fail WSAStartup
    Client();

    ~Client();

    /**
     * @brief Connect to server
     *
     * @param ip address of server
     * @param port port of server. 0-65535
     *
     * @throws std::runtime_error Failed to connect to server
     */
    void connectToServer(const char* ip, uint16_t port);

    /// @brief Disconnect from server
    /// @throws std::runtime_error Failed to disconnect
    void disconnectFromServer();

    SOCKET getSocket() const { return _socket; }
};


#endif //CLIENT_H
