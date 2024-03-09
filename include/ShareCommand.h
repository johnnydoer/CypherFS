#pragma once
#include "ICommand.h"
#include "DataStoreHelper.h"
#include <string>
#include <map>

namespace CipherFS {
class ShareCommand : public ICommand {
public:
    void execute(std::map<std::string, std::string>& params, 
        DataStoreHelper* database, StorageService* filesystem, 
        std::string& username, std::string& privateKey, std::string& currentDir) override;
private:
    bool validateShareConditions(const std::string& filename, const std::string& targetUser, 
                                 DataStoreHelper* database, const std::string& username, const std::string& srcFilePath);

    void ensureDestinationDirectoryExists(const std::string& dstDirPath, 
                                          StorageService* filesystem, DataStoreHelper* database);

    void shareFile(const std::string& srcFilePath, const std::string& dstFilePath, 
                   DataStoreHelper* database, StorageService* filesystem, 
                   const std::string& privateKey, const std::string& targetUser);
};

}
