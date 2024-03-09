#pragma once
#include "ICommand.h"
#include "DataStoreHelper.h"
#include <string>
#include <map>

namespace CipherFS {
class MkfileCommand : public ICommand {
public:
    void execute(std::map<std::string, std::string>& params, 
        DataStoreHelper* database, StorageService* filesystem, 
        std::string& username, std::string& privateKey, std::string& currentDir) override;
private:
    bool validateFilenameAndPath(const std::string& filename, const std::string& filepath, 
                                 DataStoreHelper* database, const std::string& currentDir, 
                                 const std::string& username);
    void overwriteAndShareFile(const std::string& filepath, const std::string& contents, 
                               DataStoreHelper* database, StorageService* filesystem, 
                               const std::string& privateKey);
    void createNewFile(const std::string& filepath, const std::string& contents, 
                       DataStoreHelper* database, StorageService* filesystem, 
                       const std::string& public_key);
};

}
