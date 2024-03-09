#include <CipherFS.h>

CipherFS::CipherFS::CipherFS(int argCount, char** argValues) {
  DataStoreHelper db;
  AccountHandler userMgr(&db);

  // Check if admin user exists; if not, create it
  if (!db.userExists("admin")) {
    userMgr.createUser("admin");
    std::cout << "Created admin User! Please store your key safely. Since this is the first run, exiting!" << std::endl;
    return;
  }

  // Prompt for keyfile if not provided
  if (argValues[1] == nullptr) {
    std::cout << "Please enter the keyfile name!" << std::endl;
    std::cout << "Example: ./fileserver <keyfile_name>" << std::endl;
    return;
  }

  // Retrieve the keyfile name
  std::string keyfileName = argValues[1];

  // Attempt user login
  auto loginResult = CipherFS::loginUser(keyfileName, db);
  std::string username = std::get<0>(loginResult);
  std::string privateKey = std::get<1>(loginResult);
  if (username.empty()) {
    std::cout << "Invalid keyfile" << std::endl;
    return;
  } else if (username == "admin") {
    db.setupAdminDataStore(privateKey);
  }

  std::cout << "Logged in as " + username << std::endl;

  // Initialize current directory
  std::string currentDirectory = "/" + username + "/";

  // Initialize CLI for the user
  CLI cli(&db, username, privateKey, currentDirectory);

  // Main loop
  while (true) {
    std::string commandInput;
    std::cout << "[" + username + "@CipherFS: " + cli.v_current_dir + "]$ ";
    std::getline(std::cin, commandInput);

    if (commandInput.empty()) continue; // Skip empty input

    // Split command into arguments
    std::vector<std::string> commandArgs;
    std::stringstream ss(commandInput);
    std::string tmp;
    while (getline(ss, tmp, ' ')) {
      commandArgs.push_back(tmp);
    }

    // Process commands
    if (commandArgs[0] == "cd") {
      if (commandArgs.size() != 2) {
        std::cout << "Incorrect number of arguments!\nUsage: cd <directory>\n";
        continue;
      }
      //std::map<std::string, std::string> argsMap = {{"targetDir", commandArgs[1]}};
      //cli.executeCommand(commandArgs[0], argsMap);
      cli.cd(commandArgs[1]);
    } else if (commandArgs[0] == "pwd") {
      if (commandArgs.size() != 1) {
        std::cout << "Incorrect number of arguments!\nUsage: pwd\n";
        continue;
      }
      //std::map<std::string, std::string> argsMap;
      //cli.executeCommand(commandArgs[0], argsMap);
      cli.pwd();
    } else if (commandArgs[0] == "ls") {
      if (commandArgs.size() != 1) {
        std::cout << "Incorrect number of arguments!\nUsage: ls\n";
        continue;
      }
      //std::map<std::string, std::string> argsMap;
      //cli.executeCommand(commandArgs[0], argsMap);
      cli.ls();
    } else if (commandArgs[0] == "cat") {
      if (commandArgs.size() != 2) {
        std::cout << "Incorrect number of arguments!\nUsage: cat <filename>\n";
        continue;
      }
      //std::map<std::string, std::string> argsMap = {{"fileName", commandArgs[1]}};
      //cli.executeCommand(commandArgs[0], argsMap);
      cli.cat(commandArgs[1]);
    } else if (commandArgs[0] == "share") {
      if (commandArgs.size() != 3) {
        std::cout << "Incorrect number of arguments!\nUsage: share <filename> <username>\n";
        continue;
      }
      std::map<std::string, std::string> argsMap;
      argsMap["filename"] = commandArgs[1];
      argsMap["target_user"] = commandArgs[2]; 
      //cli.executeCommand(commandArgs[0], argsMap);
      cli.share(commandArgs[1], commandArgs[2]);
    } else if (commandArgs[0] == "adduser") {
      if (commandArgs.size() != 2) {
        std::cout << "Incorrect number of arguments!\nUsage: adduser <username>\n";
        continue;
      }
      if(username != "admin") {
        std::cout << "Forbidden. Only admin can add user. \n";
        continue;
      }
      std::map<std::string, std::string> argsMap;
      argsMap["username"] = commandArgs[1]; 
      //cli.executeCommand(commandArgs[0], argsMap);
      cli.adduser(commandArgs[1]);
    } else if (commandArgs[0] == "mkdir") {
      if (commandArgs.size() != 2) {
        std::cout << "Incorrect number of arguments!\nUsage: mkdir <directory>\n";
        continue;
      }
      std::map<std::string, std::string> argsMap;
      argsMap["dirname"] = commandArgs[1]; 
      //cli.executeCommand(commandArgs[0], argsMap);
      cli.mkdir(commandArgs[1]);
    } else if (commandArgs[0] == "mkfile") {
      if (commandArgs.size() < 3) {
        std::cout << "Incorrect number of arguments!\nUsage: mkfile <filename> <contents>\n";
        continue;
      }
      std::vector<std::string> fileContents(commandArgs.begin() + 2, commandArgs.end());
      std::string content = Utils::join(fileContents, " ");
      std::map<std::string, std::string> argsMap;
      argsMap["filename"] = commandArgs[1];
      argsMap["contents"] = content;
      //cli.executeCommand(commandArgs[0], argsMap);
      cli.mkfile(commandArgs[1], content);
    } else if (commandArgs[0] == "exit") {
      std::cout << "Exiting CipherFS" << std::endl;
      return;
    } else {
      std::cout << "Invalid command. \n";
    }
  }
}


