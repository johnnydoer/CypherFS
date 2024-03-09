#include "AddUserCommand.h"
#include <iostream>

namespace CipherFS {

void AddUserCommand::execute(std::map<std::string, std::string>& params, 
                             DataStoreHelper* database, StorageService* filesystem, 
                             std::string& currentUser, std::string& privateKey, std::string& currentDir) {
    
    std::string newUsername = params["username"];
    AccountHandler userManager(database);

    if (AddUserCommand::isInvalidUsername(newUsername)) {
        const std::string symbols = "~`!@#$%^&*()-_+={}[]|\\:;\"'<>,.?/";
        std::cout << "Username cannot contain any of the following symbols: " << symbols << std::endl;
        return;
    }

    if (database->userExists(newUsername)) {
        std::cout << "User " << newUsername << " already exists" << std::endl;
        return;
    }

    userManager.createUser(newUsername);
    std::cout << "Created user: " << newUsername << std::endl;
}

bool AddUserCommand::isInvalidUsername(const std::string& username) {
    const std::string symbols = "~`!@#$%^&*()-_+={}[]|\\:;\"'<>,.?/";
    return username.find_first_of(symbols) != std::string::npos;
}

} // namespace CipherFS
