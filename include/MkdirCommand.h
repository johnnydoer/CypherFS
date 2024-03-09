#pragma once
#include "ICommand.h"
#include "DataStoreHelper.h"
#include <string>
#include <map>

namespace CipherFS {
class MkdirCommand : public ICommand {
public:
    void execute(std::map<std::string, std::string>& params, 
        DataStoreHelper* database, StorageService* filesystem, 
        std::string& username, std::string& privateKey, std::string& currentDir) override;
private:
    void ensureTrailingSlash(std::string& dirname);
    std::string constructDirpath(const std::string& currentDir, const std::string& dirname);
    bool isNameConflict(const std::string& dirpath, DataStoreHelper* database);
    void createDirectory(const std::string& dirpath, DataStoreHelper* database, StorageService* filesystem);
};

}
