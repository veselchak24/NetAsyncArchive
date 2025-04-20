#ifndef SOCKETQUEUE_H
#define SOCKETQUEUE_H

#include <concurrentqueue/concurrentqueue.h>
#include <winsock2.h>

class SocketQueue {
    moodycamel::ConcurrentQueue<std::string> _queue;
    SOCKET _listenSocket;

    bool _isRunning;

    explicit SocketQueue();

public:
    template<typename It>
    explicit SocketQueue(It begin, size_t count) : SocketQueue() { this->_queue.enqueue_bulk(begin, count); }

    ~SocketQueue();

    void start(const char* ip, unsigned int port);

    void stop();

    SOCKET acceptClient() const;

    bool sendItemToClient(SOCKET clientSocket);
};


#endif //SOCKETQUEUE_H
