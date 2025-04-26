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

    std::vector<std::thread> clientsThreads;

    std::thread acceptThread([&] {
        while (queue.size_approx())
            if (SOCKET client = server.acceptClient(); client != INVALID_SOCKET)
                clientsThreads.push_back(std::thread(handleClient, &server, client, &queue));
    });

    while (queue.size_approx())
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

    acceptThread.detach();

    for (auto& clientThread : clientsThreads)
        clientThread.join();

    server.stop();
    return 0;
}
