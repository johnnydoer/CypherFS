#include <iostream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <fstream>

#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

#include <DataStoreHelper.h>
#include <Utils.h>
#include <AccountHandler.h>
#include <StorageService.h>
#include <string.h>

namespace CipherFS {
  class CLI {
    public:
      std::string v_current_dir;
      std::string private_key;

      /**
       * @brief Construct a new CLI object
       *
       * @param database database pointer
       */
      CLI(DataStoreHelper* database, std::string username, std::string private_key, std::string initial_dir);
      void pwd();
      void ls();
      void cd(std::string destination);
      void share(std::string filename, std::string target_user);
      void mkfile(std::string fileName, std::string fileContent);
      void cat(std::string fileName);
      void mkdir(std::string dirName);
      void adduser(std::string username);

    private:
      StorageService filesystem_service;
      DataStoreHelper* database;
      std::string username;
  };
}