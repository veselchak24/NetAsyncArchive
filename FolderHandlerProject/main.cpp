#include <iostream>

#include <concurrentqueue/concurrentqueue.h>

#include "utils.h"

int main(const int argc, const char** argv) {
    char* host;
    int port;

    moodycamel::ConcurrentQueue<std::string> queue{};

    try
    {
        processingInput(argc, argv, host, port, queue);
    }
    catch (const std::exception& ex)
    {
        std::cout << "Error: " << ex.what() << std::endl;
        std::cout << "Program needs 3 arguments: \"<host> <port> <path to folder>\"" << std::endl;
        return EXIT_FAILURE;
    }

    Server server;

    server.start(host, port);

    // vector of clients thread for joining in the end
    std::vector<std::thread> clientsThreads;

    // thread that will accept client for not blocking main thread
    std::thread acceptThread([&clientsThreads, &server, &queue] {
        while (queue.size_approx())
            if (SOCKET client = server.acceptClient(); client != INVALID_SOCKET)
            {
                clientsThreads.emplace_back(handleClient, &server, std::ref(client), &queue);
#ifdef DEBUG
                std::cout << "Client connected" << std::endl;
#endif
            }
    });

    // wait for  archiving all files
    while (queue.size_approx())
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // detach accept thread to avoid blocking
    acceptThread.detach();

    // join clients threads
    for (auto& clientThread : clientsThreads)
        clientThread.join();

    server.stop();
    return 0;
}
