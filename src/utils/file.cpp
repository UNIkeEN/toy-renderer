#include "file.h"
#include <filesystem>
#include <stdexcept>

std::string findFile(const std::string& filename, int maxLevels) {
    // Search for a file in the current directory and up to maxLevels parent directories.
    namespace fs = std::filesystem;
    
    fs::path currentPath = fs::current_path();
    for (int i = 0; i <= maxLevels; ++i) {
        fs::path filePath = currentPath / filename;
        if (fs::exists(filePath)) {
            return filePath.string();
        }
        currentPath = currentPath.parent_path();
    }

    throw std::runtime_error("File not found: " + filename);
}
