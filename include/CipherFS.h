#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <assert.h>
#include <fstream>
#include <filesystem>
#include <map>

#include <DataStoreHelper.h>
#include <EncryptDecrypt.h>
#include <AccountHandler.h>
#include <StorageService.h>
//#include <CommandLineInterface.h>
#include <CLI.h>

namespace CipherFS{

  class CipherFS {
    public:
      std::string m_currentDir;
  
      static std::tuple<std::string, std::string> loginUser(const std::string& keyfile_name, DataStoreHelper& database) {
        std::ifstream infile(keyfile_name);

        if (!infile) {
            std::cerr << "Error: Could not open key file '" << keyfile_name << "'" << std::endl;
            return std::make_tuple("", "");
        }

        std::string privateKeyStr((std::istreambuf_iterator<char>(infile)),
                                  std::istreambuf_iterator<char>());
        if (infile.fail() && !infile.eof()) {
            std::cerr << "Error: Failed to read data from file '" << keyfile_name << "'" << std::endl;
            return std::make_tuple("", "");
        }

        return std::make_tuple(database.identifyUserByPrivateKey(privateKeyStr), privateKeyStr);
      }


      CipherFS(int argc, char** argv);
  };
}