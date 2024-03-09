#include "LsCommand.h"
#include <iostream>

namespace CipherFS {

void LsCommand::execute(std::map<std::string, std::string>& params, 
    DataStoreHelper* database, StorageService* filesystem, 
    std::string& username, std::string& privateKey, std::string& currentDir) {
    
    // Retrieve all file paths from the database
    std::vector<std::string> tempEntries = database->retrieveAllFilePaths();
    std::vector<std::string> entries;

    for (const auto& tempEntry : tempEntries) {
        std::string path = tempEntry;

        if (path.find(currentDir) == 0) { // Path starts with currentDir
            path.erase(0, currentDir.length()); // Remove the prefix

            // Keep only the first level under currentDir
            size_t slashPos = path.find('/');
            if (slashPos != std::string::npos) {
                path = path.substr(0, slashPos + 1);
            }

            // Avoid adding empty paths
            if (!path.empty()) {
                entries.push_back(path);
            }
        }
    }

    // Remove duplicate and sort entries
    std::sort(entries.begin(), entries.end());
    entries.erase(std::unique(entries.begin(), entries.end()), entries.end());

    // Print current and parent directory indicators
    std::cout << "d -> .\n";
    std::cout << "d -> ..\n";

    // Print out entries
    for (const auto& entry : entries) {
        char fileType = entry.back() == '/' ? 'd' : 'f';
        std::cout << fileType << " -> " << entry << std::endl;
    }
}

} // namespace CipherFS
