#include "MkfileCommand.h"
#include <iostream>

namespace CipherFS {

void MkfileCommand::execute(std::map<std::string, std::string>& params, 
                            DataStoreHelper* database, StorageService* filesystem, 
                            std::string& username, std::string& privateKey, std::string& currentDir) {
    std::string filename = params["filename"];
    std::string contents = params["contents"];
    std::string filepath = currentDir + filename;
    std::string public_key = database->fetchPublicKey(username);

    if (!validateFilenameAndPath(filename, filepath, database, currentDir, username)) {
        return;
    }

    if (database->checkFileExistence(filepath)) {
        overwriteAndShareFile(filepath, contents, database, filesystem, privateKey);
    } else {
        createNewFile(filepath, contents, database, filesystem, public_key);
    }
}

bool MkfileCommand::validateFilenameAndPath(const std::string& filename, const std::string& filepath, 
                                            DataStoreHelper* database, const std::string& currentDir, 
                                            const std::string& username) {
    if (!Utils::checkPersonal(currentDir, username) || !Utils::checkName(filename) || 
        database->checkDirectoryExistence(filepath + '/')) {
        std::cout << (Utils::checkPersonal(currentDir, username) ? std::string("") : "Forbidden\n") +
                     (Utils::checkName(filename) ? std::string("") : "Invalid filename\n") + 
                     (database->checkDirectoryExistence(filepath + '/') ? "A directory with the same name already exists. Not creating.\n" : "")
                  << std::endl;
        return false;
    }
    return true;
}

void MkfileCommand::overwriteAndShareFile(const std::string& filepath, const std::string& contents, 
                                          DataStoreHelper* database, StorageService* filesystem, 
                                          const std::string& privateKey) {
    std::cout << "File already exists, overwriting" << std::endl;
    filesystem->generateFile(filepath, contents, database->fetchPublicKey(Utils::fetchOwner(filepath)));
    std::vector<std::string> dst_filepaths = database->listSharedFiles(filepath);
    for (const std::string& dst_filepath : dst_filepaths) {
        std::string dst_public_key = database->fetchPublicKey(Utils::fetchOwner(dst_filepath));
        filesystem->replicateFile(filepath, dst_filepath, privateKey, dst_public_key);
    }
}

void MkfileCommand::createNewFile(const std::string& filepath, const std::string& contents, 
                                  DataStoreHelper* database, StorageService* filesystem, 
                                  const std::string& public_key) {
    try {
        database->registerFile(filepath);
        filesystem->generateFile(filepath, contents, public_key);
        std::cout << "Created file!" << std::endl;
    } catch (StorageService::FileCreationError &ex) {
        std::cout << "Internal error: Unable to create file" << std::endl;
    }
}

} // namespace CipherFS
