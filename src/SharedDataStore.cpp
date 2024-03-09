#include "SharedDataStore.h"
#include <fstream>

namespace CipherFS {

SharedDataStore::SharedDataStore() {
    this->setupDataStore();
    std::ifstream filemapping(this->sharedInfoFilename);
    this->sharedInfoData = nlohmann::json::parse(filemapping);
}

// Destructor
SharedDataStore::~SharedDataStore() {
    this->persistData();
}

bool SharedDataStore::checkInitialization() {
    return (
        std::filesystem::exists(this->sharedInfoFilename)
    );
}

void SharedDataStore::setupDataStore() {
    if (!this->checkInitialization()) {
        for (auto const& [file, json] : this->dataFile) {
            std::ofstream output(file);
            output << nlohmann::json::object();
            output.close();
        } 
    }
}

void SharedDataStore::shareFile(const std::string& sourcePath, const std::string& destinationPath) {
    std::string sourceHash = EncryptDecrypt::computeSha256(sourcePath);
    std::string destinationHash = EncryptDecrypt::computeSha256(destinationPath);

    if (!this->sharedInfoData.contains(sourceHash)) {
        this->sharedInfoData[sourceHash] = {destinationHash};
        return ;
    } 
    
    this->sharedInfoData[sourceHash].push_back(destinationHash);
}

void SharedDataStore::persistData() {
   for (auto const& [file, json] : this->dataFile) {
        std::ofstream output(file);
        output << std::setw(2) << *json << std::endl;
        output.close();
    }
}

} // namespace CipherFS
