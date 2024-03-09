#pragma once
#include <map>
#include <string>
#include <DataStoreHelper.h>
#include <Utils.h>
#include <StorageService.h>
#include <AccountHandler.h>

namespace CipherFS {

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void execute(std::map<std::string, std::string>& params, 
        DataStoreHelper* database, StorageService* filesystem, 
        std::string& username, std::string& privateKey, std::string& currentDir) = 0;
};

}