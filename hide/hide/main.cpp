#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

namespace fs = std::filesystem;

constexpr char homeDir[] = "/home/houidini";

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        return 1;
    }
    fs::path hiddenDir = fs::path(homeDir) / ".hidden";

    std::cout << hiddenDir << std::endl;

    try {
        if (!fs::exists(hiddenDir)) {
            if (mkdir(hiddenDir.c_str(), 0311) == -1) {
                perror("Cant create hidden dir");
                return 1;
            }
        }

        fs::path filePath = argv[1];
        if (!fs::exists(filePath)) {
            std::cerr << "File not found: " << filePath << std::endl;
            return 1;
        }

        fs::path newFilePath = hiddenDir / filePath.filename();

        fs::rename(filePath, newFilePath);
        std::cout << "File moved to: " << newFilePath << std::endl;
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
