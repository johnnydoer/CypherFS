#include "FileDataStore.h" // Adjust the include path as necessary

namespace CipherFS {

// Constructor
FileDataStore::FileDataStore() {
    this->setupDataStore();
    std::ifstream filemapping(this->mappingsFilename);
    this->fileMappingsData = nlohmann::json::parse(filemapping);
}

// Destructor
FileDataStore::~FileDataStore() {
    this->persistData();
}

bool FileDataStore::checkInitialization() {
    return (
        std::filesystem::exists(this->mappingsFilename)
    );
}

// This method creates a file if it doesn't exist
void FileDataStore::setupDataStore() {
    if (!this->checkInitialization()) {
        for (auto const& [file, json] : this->dataFile) {
            std::ofstream output(file);
            output << nlohmann::json::object();
            output.close();
        } 
    }
}

std::vector<std::string> FileDataStore::retrieveAllFilePaths() {
    std::vector<std::string> result;
    for (auto& element : this->fileMappingsData.items()) {
        result.push_back(element.value());
    }
    return result;
}

std::string FileDataStore::registerFile(const std::string& path) {
    return this->encryptPathAndStore(path);
}

std::string FileDataStore::registerDirectory(const std::string& directoryPath) {
    return this->encryptPathAndStore(directoryPath);
}

void FileDataStore::removeFile(const std::string& path) {
    std::string encryptedPath = EncryptDecrypt::computeSha256(path);
    //std::string encryptedPath = path;
    this->fileMappingsData.erase(encryptedPath);
    this->persistData();
}

std::string FileDataStore::pathFromHash(const std::string& hashValue) {
    // Verify if the hash is present in the database
    if (!this->fileMappingsData.count(hashValue)) {
       return std::string();
    }
    std::string pathResult;
    try {
        // Attempt to retrieve the path associated with the hash
        pathResult = this->fileMappingsData.at(hashValue).get<std::string>();
    } catch (const nlohmann::json::exception& e) {
        // If retrieval fails, handle quietly
        return std::string();
    }
    return pathResult;
}

std::string FileDataStore::hashFromPath(std::string& path) {
    if (std::filesystem::is_directory(path)) {
        path.push_back('/');
    }
    return EncryptDecrypt::computeSha256(path);
}

bool FileDataStore::checkFileExistence(const std::string& filePath) {
    std::string hash = EncryptDecrypt::computeSha256(filePath);
    //std::string hash = filePath;
    return !this->pathFromHash(hash).empty();
}

bool FileDataStore::checkDirectoryExistence(const std::string& directoryPath) {
    std::string hash = EncryptDecrypt::computeSha256(directoryPath);
    return !this->pathFromHash(hash).empty();
}

void FileDataStore::persistData() {
    for (auto const& [file, json] : this->dataFile) {
        std::ofstream output(file);
        output << std::setw(2) << *json << std::endl;
        output.close();
    }
}

std::string FileDataStore::encryptPathAndStore(const std::string& path) {
    std::string encryptedPath = EncryptDecrypt::computeSha256(path);
    this->fileMappingsData[encryptedPath] = path;
    this->persistData();
    return encryptedPath;
}

} // namespace CipherFS
