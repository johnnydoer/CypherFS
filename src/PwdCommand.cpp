#include "PwdCommand.h"
#include <iostream>

namespace CipherFS {

void PwdCommand::execute(std::map<std::string, std::string>& params, 
    DataStoreHelper* database, StorageService* filesystem, 
    std::string& username, std::string& privateKey, std::string& currentDir) {
    
    std::cout << currentDir << std::endl;
}

} // namespace CipherFS
