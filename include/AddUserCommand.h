#pragma once
#include "ICommand.h"
#include <string>
#include <map>

namespace CipherFS {
class AddUserCommand : public ICommand {
public:
    void execute(std::map<std::string, std::string>& params, 
        DataStoreHelper* database, StorageService* filesystem, 
        std::string& username, std::string& privateKey, std::string& currentDir) override;
    
    static bool isInvalidUsername(const std::string& username);
};

}
