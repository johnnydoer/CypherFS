#pragma once

#include "FileDataStore.h"
#include "UserDataStore.h"
#include "SharedDataStore.h"
#include <string>
#include <vector>

namespace CipherFS {

class DataStoreHelper {
public:
    DataStoreHelper();
    ~DataStoreHelper();

    // Delegated methods from FileDataStore
    std::vector<std::string> retrieveAllFilePaths() { return fileDataStore.retrieveAllFilePaths(); }
    std::string registerFile(const std::string& path) { return fileDataStore.registerFile(path); }
    std::string registerDirectory(const std::string& directoryPath) { return fileDataStore.registerDirectory(directoryPath); }
    void removeFile(const std::string& path) { fileDataStore.removeFile(path); }
    std::string pathFromHash(const std::string& hashValue) { return fileDataStore.pathFromHash(hashValue); }
    std::string hashFromPath(std::string& path) { return fileDataStore.hashFromPath(path); }
    bool checkFileExistence(const std::string& filePath) { return fileDataStore.checkFileExistence(filePath); }
    bool checkDirectoryExistence(const std::string& directoryPath) { return fileDataStore.checkDirectoryExistence(directoryPath); }
    

    // Delegated methods from UserDataStore
    bool userExists(const std::string& user) { return userDataStore.userExists(user); }
    std::string fetchPublicKey(const std::string& user) { return userDataStore.fetchPublicKey(user); }
    void setupAdminDataStore(const std::string& adminKey) { userDataStore.setupAdminDataStore(adminKey); }
    std::string identifyUserByPrivateKey(const std::string& key) { return userDataStore.identifyUserByPrivateKey(key); }
    std::string fetchUserInfoProperty(const std::string& user, const std::string& property)  { return userDataStore.fetchUserInfoProperty(user, property); }
    std::string fetchPrivateKey(const std::string& user) { return userDataStore.fetchPrivateKey(user); }

    // Delegated methods from SharedDataStore
    void shareFile(const std::string& sourcePath, const std::string& destinationPath) { sharedDataStore.shareFile(sourcePath, destinationPath); }

    // Custom methods
    void addUser(const std::string& username);
    std::vector<std::string> listSharedFiles(const std::string& sourcePath);
    bool checkInitialization();
    void setupDataStore();


private:
    FileDataStore fileDataStore;
    UserDataStore userDataStore;
    SharedDataStore sharedDataStore;

    void persistData();
};

}
