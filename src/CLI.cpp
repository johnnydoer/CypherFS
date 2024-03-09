#include "CLI.h"

CipherFS::CLI::CLI(DataStoreHelper* database, std::string username, std::string private_key, std::string initial_dir) {
  this->database = database;
  this->username = username;
  this->private_key = private_key;
  this->v_current_dir = initial_dir;
}

void CipherFS::CLI::pwd() {
  std::cout << v_current_dir << std::endl;
}

void CipherFS::CLI::ls() {
    // Temporary container for paths from database
    std::vector<std::string> tempPaths;
    tempPaths = this->database->retrieveAllFilePaths();

    // Container for processed directory entries
    std::vector<std::string> directoryEntries;

    for (const auto& tempPath : tempPaths) {
        std::string processedPath = tempPath;

        // Check if path starts with the current directory
        if (processedPath.find(this->v_current_dir) == 0) {
            // Remove the current directory prefix
            processedPath.erase(0, this->v_current_dir.length());

            // Trim path to remove subdirectories or files under the current layer
            auto slashPos = processedPath.find('/');
            if (slashPos < processedPath.length() && !processedPath.empty()) {
                processedPath = processedPath.substr(0, slashPos + 1);
            }

            // Add the processed path to the list if it's not empty
            if (!processedPath.empty()) {
                directoryEntries.push_back(processedPath);
            }
        }
    }

    // Alphabetically sort the directory entries
    std::sort(directoryEntries.begin(), directoryEntries.end());
    // Remove duplicates
    directoryEntries.erase(std::unique(directoryEntries.begin(), directoryEntries.end()), directoryEntries.end());

    // Print current and parent directory symbols
    std::cout << "d -> . " << std::endl;
    std::cout << "d -> .." << std::endl;

    // Output the directory entries
    for (const auto& entry : directoryEntries) {
        char entryType = entry.back() == '/' ? 'd' : 'f';
        std::cout << entryType << " -> " << entry << std::endl;
    }
}

void CipherFS::CLI::cd(std::string destination) {
    // Navigate to root directory if target is "/"
    if (destination == "/") {
        this->v_current_dir = "/" + this->username + "/";
        return;
    }

    // Navigate up one directory if target is ".."
    if (destination == "..") {
        if (this->v_current_dir == "/" + this->username + "/") {
            // Restricted movement for non-admin users
            if (username != "admin") {
                std::cout << "Access denied" << std::endl;
                return;
            }
        } else if (this->v_current_dir == "/") {
            // At root, do nothing
            return;
        }

        bool slashAtEnd = this->v_current_dir.back() == '/';
        size_t prevSlashPos = this->v_current_dir.find_last_of("/", this->v_current_dir.length() - (slashAtEnd ? 2 : 1));

        this->v_current_dir = this->v_current_dir.substr(0, prevSlashPos + 1);
        return;
    } else if (destination == ".") {
        // Stay in the current directory
        return;
    } else {
        // Append trailing slash if missing
        if (destination.back() != '/') {
            destination += "/";
        }

        std::vector<std::string> pathSegments = Utils::split(destination, '/');

        std::string newPath = this->v_current_dir;
        for (const auto& segment : pathSegments) {
            if (segment == "..") {
                bool slashAtEnd = newPath.back() == '/';
                size_t prevSlashPos = newPath.find_last_of("/", newPath.length() - (slashAtEnd ? 2 : 1));
                newPath = newPath.substr(0, prevSlashPos + 1);
            } else {
                newPath += segment + "/";
            }
        }

        // Non-admin users have restricted directory access
        if (this->username != "admin") {
            if (Utils::fetchOwner(newPath) != this->username) {
                std::cout << "Access denied" << std::endl;
                return;
            }

            if (!database->checkDirectoryExistence(newPath)) {
                std::cout << "Directory not found" << std::endl;
                return;
            }

            this->v_current_dir = newPath;
            return;
        }

        // Admin specific checks
        if (this->username == "admin") {
            if (!database->checkDirectoryExistence(newPath) && newPath != "/") {
                std::cout << "Directory not found" << std::endl;
                return;
            }
            this->v_current_dir = newPath;
            return;
        }
    }
}

void CipherFS::CLI::cat(std::string fileName) {
    // Build full file path within the current directory
    std::string fullPath = this->v_current_dir + fileName;

    // Verify the file's existence in the database
    if (!this->database->checkFileExistence(fullPath)) {
        std::cout << "File not found: " << fullPath << std::endl;
        return;
    }

    // Read file content based on user privileges
    try {
        std::string userKey; // User's private key or the impersonated user's key for admin
        if (this->username == "admin") {
            std::string fileOwner = Utils::fetchOwner(fullPath); // Owner of the file
            userKey = this->database->fetchPrivateKey(fileOwner); // Get owner's private key if admin
        } else {
            userKey = this->private_key; // Use current user's private key
        }
        // Output the file's content
        std::cout << this->filesystem_service.fetchFileContent(fullPath, userKey) << std::endl;
    } catch (StorageService::FileReadError &exception) {
        std::cout << "Error reading file" << std::endl; // Handling read exceptions
    }
}

void CipherFS::CLI::mkfile(std::string fileName, std::string fileContent) {
    if (!Utils::checkPersonal(this->v_current_dir, this->username)) {
        std::cout << "Forbidden" << std::endl;
        return;
    }

    std::string fileFullPath = this->v_current_dir + fileName;
    std::string userPublicKey = this->database->fetchPublicKey(this->username);

    if (!Utils::checkName(fileName)) {
        std::cout << "Invalid filename" << std::endl;
        return;
    }

    if (this->database->checkDirectoryExistence(fileFullPath + '/')) {
        std::cout << "A directory with the same name already exists. Not creating." << std::endl;
        return;
    }

    if (this->database->checkFileExistence(fileFullPath)) {
        std::cout << "File already exists, overwriting" << std::endl;
        this->filesystem_service.generateFile(fileFullPath, fileContent, userPublicKey);

        auto relatedFilesPaths = this->database->listSharedFiles(fileFullPath);
        for (auto &relatedFilePath : relatedFilesPaths) {
            auto relatedUser = Utils::fetchOwner(relatedFilePath);
            auto relatedUserPublicKey = this->database->fetchPublicKey(relatedUser);
            this->filesystem_service.replicateFile(fileFullPath, relatedFilePath, this->private_key, relatedUserPublicKey);
        }
    } else {
        try {
            this->database->registerFile(fileFullPath);
            this->filesystem_service.generateFile(fileFullPath, fileContent, userPublicKey);
            std::cout << "Created file!" << std::endl;
        } catch (StorageService::FileCreationError &e) {
            std::cout << "Internal error: Unable to create file" << std::endl;
        }
    }
}

void CipherFS::CLI::adduser(std::string username) {
    // Instantiating UserManager with the current database
    AccountHandler userManager(this->database);

    // Validate username existence
    if (this->database->userExists(username)) {
        std::cout << "User " << username << " already exists." << std::endl;
        return;
    }

    // Validate username for illegal characters
    if (username.find_first_of("~`!@#$%^&*()-_+={}[]|\\:;\"'<>,.?/") != std::string::npos) {
        std::cout << "Username cannot contain special characters." << std::endl;
        return;
    }

    // Attempt to create the user
    try {
        userManager.createUser(username);
        std::cout << "Created user: " << username << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Failed to create user: " << e.what() << std::endl;
    }
}


void CipherFS::CLI::mkdir(std::string dirName) {
    if (!Utils::checkPersonal(this->v_current_dir, this->username)) {
        std::cout << "Forbidden" << std::endl;
        return;
    }

    if (!Utils::checkName(dirName)) {
        std::cout << "Invalid directory name" << std::endl;
        return;
    }

    if (dirName.back() != '/') dirName += "/";

    std::string dirFullPath = this->v_current_dir + dirName;
    std::string fileEquivalentPath = dirFullPath.substr(0, dirFullPath.size() - 1);

    if (this->database->checkFileExistence(fileEquivalentPath)) {
        std::cout << "File with same name already exists. Abandoning." << std::endl;
        return;
    }

    if (this->database->checkDirectoryExistence(dirFullPath)) {
        std::cout << "Directory already exists" << std::endl;
        return;
    }

    try {
        this->database->registerDirectory(dirFullPath);
        this->filesystem_service.establishDirectory(dirFullPath);
        std::cout << "Created directory: " << dirFullPath << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Error creating directory" << std::endl;
    }
}

void CipherFS::CLI::share(std::string filename, std::string target_user) {

    // Disallow sharing of directories
    if (filename.back() == '/') {
        std::cout << "Cannot share directories." << std::endl;
        return;
    }

    std::string search_str = "/shared/";
    // Construct source file path
    const std::string src_filepath = this->v_current_dir + filename;

    if (src_filepath.find(search_str) != std::string::npos) {
        std::cout << "Forbidden. Only file owners can share their file." << std::endl;
        return;
    }

    // Ensure the file exists
    if (!this->database->checkFileExistence(src_filepath)) {
        std::cout << "File " + filename + " does not exist." << std::endl;
        return;
    }

    // Check if the target user exists
    if (!this->database->userExists(target_user)) {
        std::cout << "User " + target_user + " does not exist." << std::endl;
        return;
    }

    // Avoid sharing with self
    if (this->username == target_user) {
        std::cout << "Cannot share file with oneself." << std::endl;
        return;
    }

    // Construct the destination directory and file paths
    const std::string dst_dirpath = "/" + target_user + "/shared/" + this->username + "/";
    const std::string dst_filepath = dst_dirpath + filename;

    // Check for destination file existence to avoid overwriting
    if (this->database->checkFileExistence(dst_filepath)) {
        std::cout << "File already shared. Rename to share again." << std::endl;
        return;
    }

    // Ensure the target shared directory exists or create it
    if (!this->database->checkDirectoryExistence(dst_dirpath)) {
        this->filesystem_service.establishDirectory(dst_dirpath);
        this->database->registerDirectory(dst_dirpath);
    }

    // Retrieve keys for encryption/decryption
    const std::string src_private_key = this->private_key;
    const std::string dst_public_key = this->database->fetchPublicKey(target_user);

    // Copy and share the file
    try {
        this->filesystem_service.replicateFile(src_filepath, dst_filepath, src_private_key, dst_public_key);
        this->database->registerFile(dst_filepath);
        this->database->shareFile(src_filepath, dst_filepath);
        std::cout << "Shared file with " << target_user << " at " << dst_filepath << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Failed to share file: " << e.what() << std::endl;
    }
}




