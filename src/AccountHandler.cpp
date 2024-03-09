#include <AccountHandler.h>

CipherFS::AccountHandler::AccountHandler(DataStoreHelper* db) {
  this->database = db;
}

std::string CipherFS::AccountHandler::createUser(const std::string& userName) {
  std::string homeDirectory = "/" + userName + "/";
  
  if (!this->database->userExists(userName)) {
    this->database->addUser(userName);
    this->storage.establishDirectory(homeDirectory);
    this->storage.establishDirectory(homeDirectory + "personal/");
    this->storage.establishDirectory(homeDirectory + "shared/");
  }

  return homeDirectory;
}
