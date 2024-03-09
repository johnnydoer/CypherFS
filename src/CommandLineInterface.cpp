#include "CommandLineInterface.h"
#include "CdCommand.h"
#include "PwdCommand.h"
#include "CatCommand.h"
#include "LsCommand.h"
#include "ShareCommand.h"
#include "MkdirCommand.h"
#include "MkfileCommand.h"
#include "AddUserCommand.h"


namespace CipherFS {

CommandLineInterface::CommandLineInterface(DataStoreHelper* database, std::string username, std::string privateKey, std::string currentDir)
    : database(database), username(username), privateKey(privateKey), currentDir(currentDir) {

    commandMap["cd"] = new CdCommand();
    commandMap["pwd"] = new PwdCommand();
    commandMap["cat"] = new CatCommand();
    commandMap["ls"] = new LsCommand();
    commandMap["share"] = new ShareCommand();
    commandMap["mkdir"] = new MkdirCommand();
    commandMap["mkfile"] = new MkfileCommand();
    commandMap["adduser"] = new AddUserCommand();
}

CommandLineInterface::~CommandLineInterface() {
    // Clean up dynamically allocated command objects
    for (auto& command : commandMap) {
        delete command.second;
    }
}

void CommandLineInterface::executeCommand(const std::string& commandName, std::map<std::string, std::string>& params) {
    auto it = commandMap.find(commandName);
    if (it != commandMap.end()) {
        it->second->execute(params, this->database, &this->filesystem, this->username, this->privateKey, this->currentDir);
    } else {
        std::cout << "Command not found: " << commandName << std::endl;
    }
}

} // namespace CipherFS
