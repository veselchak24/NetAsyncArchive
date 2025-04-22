#include <iostream>
#include <vector>

#include "pathUtils.h"
#include "SocketQueue.h"

int main(const int argc, const char** argv) {
    if (argc != 2)
    {
        std::string errorMessage = "Incorrect number of arguments. Expected 1. But was:" + std::to_string(argc - 1);
#ifdef DEBUG
        throw std::runtime_error(errorMessage);
#else
        std::cout << errorMessage << std::endl;
        return -1;
#endif
    }

    std::vector<std::string> files = getAllFiles(argv[1]);

#ifdef DEBUG
    if (files.empty())
        std::cout << "No files found" << std::endl;
    else
        for (const auto& file : files)
            std::cout << file << ' ';
#endif

    SocketQueue queue(files.begin(), files.size());
    queue.start("127.0.0.1", 1234);

    std::this_thread::sleep_for(std::chrono::seconds(3));

    queue.stop();
    return 0;
}
