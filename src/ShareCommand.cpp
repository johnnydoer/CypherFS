#include "ShareCommand.h"
#include <iostream>
#include <string>

namespace CipherFS {

void ShareCommand::execute(std::map<std::string, std::string>& params, 
                           DataStoreHelper* database, StorageService* filesystem, 
                           std::string& username, std::string& privateKey, std::string& currentDir) {
    std::string filename = params["filename"];
    std::string targetUser = params["target_user"];

    // Pre-validation checks
    if (!validateShareConditions(filename, targetUser, database, username, currentDir + filename)) {
        return; // Error message is printed in validateShareConditions
    }

    // Construct destination path
    std::string srcFilePath = currentDir + filename;
    std::string dstDirPath = "/" + targetUser + "/shared/" + username + "/";
    std::string dstFilePath = dstDirPath + filename;

    // Ensure destination directory exists
    ensureDestinationDirectoryExists(dstDirPath, filesystem, database);

    // Avoid overwriting an existing file
    if (database->checkFileExistence(dstFilePath)) {
        std::cout << "A file with the same name was already shared. Please rename the file to share." << std::endl;
        return;
    }

    // Perform the file sharing operation
    shareFile(srcFilePath, dstFilePath, database, filesystem, privateKey, targetUser);

    std::cout << "Shared file with " << targetUser << " at " << dstFilePath << std::endl;
}

bool ShareCommand::validateShareConditions(const std::string& filename, const std::string& targetUser, 
                             DataStoreHelper* database, const std::string& username, const std::string& srcFilePath) {
    if (!database->userExists(targetUser)) {
        std::cout << "User does not exist." << std::endl;
        return false;
    }
    if (filename.back() == '/') {
        std::cout << "Cannot share a directory." << std::endl;
        return false;
    }
    if (username == targetUser) {
        std::cout << "Target user cannot be the current user." << std::endl;
        return false;
    }
    if (!database->checkFileExistence(srcFilePath)) {
        std::cout << "File does not exist." << std::endl;
        return false;
    }
    return true;
}

void ShareCommand::ensureDestinationDirectoryExists(const std::string& dstDirPath, StorageService* filesystem, DataStoreHelper* database) {
    if (!database->checkDirectoryExistence(dstDirPath)) { // Assuming this checks directory existence
        filesystem->establishDirectory(dstDirPath);
        database->registerDirectory(dstDirPath);
    }
}

void ShareCommand::shareFile(const std::string& srcFilePath, const std::string& dstFilePath, 
               DataStoreHelper* database, StorageService* filesystem, 
               const std::string& privateKey, const std::string& targetUser) {
    std::string dstPublicKey = database->fetchPublicKey(targetUser);
    filesystem->replicateFile(srcFilePath, dstFilePath, privateKey, dstPublicKey);
    database->registerFile(dstFilePath);
    database->shareFile(srcFilePath, dstFilePath);
}

} // namespace CipherFS
