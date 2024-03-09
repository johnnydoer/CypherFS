#include <filesystem>
#include <fstream>
#include <iostream>
#include "StorageService.h"  // Assuming the refactored class name

namespace CipherFS {

StorageService::StorageService() {
    // Check and create the root directory for our storage system if it doesn't exist
    if (!std::filesystem::exists(basePath)) {
        std::filesystem::create_directory(basePath);
    }
}

std::string StorageService::fetchFileContent(const std::string& virtualPath, const std::string& decryptionKey) {
    try {
        auto physicalPath = translateVirtualToPhysicalPath(virtualPath);
        return EncryptDecrypt::retrieveFile(decryptionKey, physicalPath);
    } catch (const std::exception& error) {
        std::cerr << "Error reading file: " << error.what() << '\n';
        throw FileReadError("Unable to fetch file content.");
    }
}

bool StorageService::establishDirectory(const std::string& virtualDirectoryPath) {
    try {
        auto physicalPath = translateVirtualToPhysicalPath(virtualDirectoryPath);
        if (!std::filesystem::exists(physicalPath)) {
            std::filesystem::create_directories(physicalPath);
            return true;
        }
        return false;
    } catch (const std::exception& error) {
        std::cerr << "Failed to establish directory: " << error.what() << '\n';
        throw DirectoryCreationError("Directory establishment failed.");
    }
}

bool StorageService::generateFile(const std::string& virtualFilePath, const std::string& data, const std::string& encryptionKey) {
    try {
        auto physicalPath = translateVirtualToPhysicalPath(virtualFilePath);
        std::ofstream file(physicalPath);
        file << data;
        file.close();
        EncryptDecrypt::secureFile(encryptionKey, physicalPath);
        return true;
    } catch (const std::exception& error) {
        std::cerr << "File generation failed: " << error.what() << '\n';
        throw FileCreationError("Unable to generate file.");
    }
}

bool StorageService::replicateFile(const std::string& sourceVirtualPath, const std::string& destinationVirtualPath, const std::string& sourceDecryptionKey, const std::string& destinationEncryptionKey) {
    try {
        std::string data = fetchFileContent(sourceVirtualPath, sourceDecryptionKey);
        removeFile(destinationVirtualPath); // Clears the destination
        return generateFile(destinationVirtualPath, data, destinationEncryptionKey);
    } catch (const std::exception& error) {
        std::cerr << "Replicating file encountered an error: " << error.what() << '\n';
        throw FileCopyError("File replication was unsuccessful.");
    }
}

bool StorageService::removeFile(const std::string& virtualFilePath) {
    try {
        auto physicalPath = translateVirtualToPhysicalPath(virtualFilePath);
        return std::filesystem::remove(physicalPath);
    } catch (const std::exception& error) {
        std::cerr << "Deletion of file failed: " << error.what() << '\n';
        throw FileDeletionError("Unable to remove file.");
    }
}

std::filesystem::path StorageService::translateVirtualToPhysicalPath(const std::string& virtualPath) {
    std::filesystem::path physicalPath = basePath;
    for (const auto& segment : Utils::split(virtualPath, '/')) {
        if (!segment.empty()) {
            physicalPath /= EncryptDecrypt::computeSha256(segment);
        }
    }
    return physicalPath;
}

} // namespace CipherSpace
