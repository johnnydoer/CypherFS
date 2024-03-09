#pragma once
#include "ICommand.h"
#include "DataStoreHelper.h"
#include <string>
#include <map>

namespace CipherFS {
class CdCommand : public ICommand {
public:
    void execute(std::map<std::string, std::string>& params, 
        DataStoreHelper* database, StorageService* filesystem, 
        std::string& username, std::string& privateKey, std::string& currentDir) override;
private:
    // Moves to the parent directory, updating currentDir accordingly
    void moveToParentDirectory(const std::string& username, std::string& currentDir);

    // Changes the current directory to targetDir, performing necessary checks
    void changeDirectory(const std::string& targetDir, const std::string& username, 
                         DataStoreHelper* database, std::string& currentDir);

    // Builds a temporary directory path for validation purposes
    std::string buildTempDir(const std::string& targetDir, const std::string& currentDir);
};

}
