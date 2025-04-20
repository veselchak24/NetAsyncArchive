#include "pathUtils.h"

std::vector<std::string> getAllFiles(const std::string& path) {
    std::vector<std::string> files;
    if (!std::filesystem::exists(path))
        throw std::invalid_argument("Path does not exist");

    for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
        if (!entry.is_directory())
            files.push_back(entry.path().string());

    return files;
}
