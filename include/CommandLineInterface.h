// CLI.h
#pragma once

#include <unordered_map>
#include <map>
#include <string>
#include "ICommand.h"
#include <DataStoreHelper.h>
#include <Utils.h>
#include <StorageService.h>

namespace CipherFS {
class CommandLineInterface {
    std::unordered_map<std::string, ICommand*> commandMap;

public:
    std::string& currentDir;
    std::string& privateKey;
    CommandLineInterface(DataStoreHelper* database, std::string username, std::string privateKey, std::string currentDir);
    ~CommandLineInterface();
    void executeCommand(const std::string& commandName, std::map<std::string, std::string>& params);

private:
    StorageService filesystem;
    DataStoreHelper* database;
    std::string username;
};

}
