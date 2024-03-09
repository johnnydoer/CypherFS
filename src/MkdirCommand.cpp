#include "MkdirCommand.h"
#include <iostream>

namespace CipherFS {

void MkdirCommand::execute(std::map<std::string, std::string>& params, 
                           DataStoreHelper* database, StorageService* filesystem, 
                           std::string& username, std::string& privateKey, std::string& currentDir) {
    std::string dirname = params["dirname"];

    if (!Utils::checkPersonal(currentDir, username)) {
        std::cout << "Forbidden" << std::endl;
        return;
    }

    if (!Utils::checkName(dirname)) {
        std::cout << "Invalid directory name" << std::endl;
        return;
    }

    ensureTrailingSlash(dirname);

    std::string dirpath = constructDirpath(currentDir, dirname);

    if (isNameConflict(dirpath, database)) {
        // Error message is printed inside isNameConflict
        return;
    }

    createDirectory(dirpath, database, filesystem);
}

void MkdirCommand::ensureTrailingSlash(std::string& dirname) {
    if (dirname.back() != '/') {
        dirname += "/";
    }
}

std::string MkdirCommand::constructDirpath(const std::string& currentDir, const std::string& dirname) {
    return currentDir + dirname;
}

bool MkdirCommand::isNameConflict(const std::string& dirpath, DataStoreHelper* database) {
    std::string filepath = dirpath.substr(0, dirpath.size() - 1);
    if (database->checkFileExistence(filepath)) {
        std::cout << "File with same name already exists. Abandoning." << std::endl;
        return true;
    }

    if (database->checkDirectoryExistence(dirpath)) {
        std::cout << "Directory already exists" << std::endl;
        return true;
    }

    return false;
}

void MkdirCommand::createDirectory(const std::string& dirpath, DataStoreHelper* database, StorageService* filesystem) {
    try {
        database->registerDirectory(dirpath);
        filesystem->establishDirectory(dirpath);
        std::cout << "Created directory: " << dirpath << std::endl;
    } catch (StorageService::DirectoryCreationError &ex) {
        std::cout << "Error creating directory" << std::endl;
    }
}

} // namespace CipherFS
