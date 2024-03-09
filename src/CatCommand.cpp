#include "CatCommand.h"
#include <iostream>

namespace CipherFS {

void CatCommand::execute(std::map<std::string, std::string>& params, 
    DataStoreHelper* database, StorageService* filesystem, 
    std::string& username, std::string& privateKey, std::string& currentDir) {
    
    std::string filename = params["filename"];
    std::string filepath = currentDir + filename;

    // Verify file existence
    if (!database->checkFileExistence(filepath)) {
        std::cout << "File does not exist: " << filepath << std::endl;
        return;
    }

    // Determine the appropriate private key for file access
    std::string keyForReading = privateKey;
    if (username == "admin") {
        std::string owner = Utils::fetchOwner(filepath);
        keyForReading = database->fetchPrivateKey(owner);
    }

    // Attempt to read the file
    try {
        std::string fileContent = filesystem->fetchFileContent(filepath, keyForReading);
        std::cout << fileContent << std::endl;
    } catch (const StorageService::FileReadError& ex) {
        std::cout << ex.what() << std::endl;
    }
}

} // namespace CipherFS
