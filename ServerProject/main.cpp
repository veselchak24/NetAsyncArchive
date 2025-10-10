#include <iostream>

#include "SwitchConcurrentQueue.h"
#include "utils/utils.h"

int main(const int argc, const char** argv) {
    setlocale(LC_ALL, ".UTF8");

    char* host;
    int port;

    concurrentQueue<std::string> queue{};

    try {
        processingInput(argc, argv, host, port, queue);
    } catch (const std::exception& ex) {
        std::cout << "Error: " << ex.what() << std::endl;
        std::cout << "Program needs 3 arguments: \"<host> <port> <path to folder>\"" << std::endl;
        return EXIT_FAILURE;
    }

    if (!queue.size_approx()) {
        std::cout << "WARNING: No files to archive in folder!" << std::endl;
        return 0;
    }

    const unsigned int cores = std::thread::hardware_concurrency();

    if (!cores) {
        std::cout << "Error: Could not get number of cores!" << std::endl;
        return EXIT_FAILURE;
    }

    Server server;

    server.start(host, port);

    // vector of clients thread for joining in the end
    std::vector<std::thread> clientsThreads;

    // thread that will accept client for not blocking main thread
    std::thread acceptThread([&clientsThreads, &server, &queue, &cores] {
#ifdef LOG
        std::cout << "Started accepting clients" << std::endl;
#endif

        // counting clients to limit number of clients by cores
        std::atomic<unsigned int> countClients{0};

        while (queue.size_approx())
            if (countClients.load() >= cores)
                continue;

        if (SOCKET client = server.acceptClient(); client != INVALID_SOCKET) {
            clientsThreads.emplace_back(handleClient, &server, std::ref(client), &queue, std::ref(countClients));
            ++countClients;
#ifdef LOG
            std::cout << '(' << countClients.load() << ')' << " Client connected. Thread: "
                    << (--clientsThreads.end())->get_id() << std::endl;
#endif
        }

        std::cout << "Stopped accepting clients" << std::endl;
    });

    // detach accept thread to avoid blocking
    acceptThread.detach();

    // wait for  archiving all files
    while (queue.size_approx())
        std::this_thread::sleep_for(std::chrono::milliseconds(200));


    // join clients threads
    for (auto& clientThread: clientsThreads)
        clientThread.join();

    server.stop();
    return 0;
}
