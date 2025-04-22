#include <iostream>

#include <concurrentqueue/concurrentqueue.h>

#include "utils.h"

int main(const int argc, const char** argv) {
    moodycamel::ConcurrentQueue<std::string> queue{};

    createTestFiles();

    try
    {
        processingInput(argc, argv, queue);
    } catch (const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    Server server;

    server.start("127.0.0.1", 1234);

    while (queue.size_approx())
    {
        SOCKET client = server.acceptClient();
        if (client != INVALID_SOCKET)
            std::thread(handleClient, server, client, &queue).detach();
    }

    server.stop();
    return 0;
}
