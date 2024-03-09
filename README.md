

# CipherFS
**Encrypted secure file system**

The encrypted file system will reside on the host file system as a regular file but with added security. The filesystem is designed to be used simultaneously by different users to maximize storage efficiency.

## User Features

- `cd <directory>`  
  Changes the current directory. Accepts `.` and `..` for current and parent directories respectively. `cd /` moves to the current user’s root directory. If the directory doesn't exist, the user remains in the current directory.

- `pwd`  
  Prints the current directory.

- `ls`  
  Lists the files and directories in the current directory, including `.` and `..`, each on a new line.

- `cat <file_name>`  
  Reads the actual contents of the file.

- `share <file_name> <target_user>`  
  Shares the specified file with the target user, appearing in the `/shared` directory of the target user with read-only permissions. If the file doesn't exist, prints "File doesn't exist". If the user doesn't exist, prints "User doesn't exist".

- `mkdir <directory_name>`  
  Creates a new directory. Prints "Directory already exists" if it already exists.

- `mkfile <file_name> <contents>`  
  Creates a new file with the specified contents.

- `exit`  
  Terminates the program.

## Admin Features

Admins have access to read the entire file system in addition to all user features.

- `adduser <username>`  
  Creates a keyfile named `username_keyfile` on the host, which is used by the user to access the filesystem. Prints "User already exists" if the username is taken.

## How to Use the File System

In the project directory, follow these steps:

```bash
git submodule update --init --recursive
mkdir -p build
cmake . -B build
cmake --build build
cd build
./fileserver
./fileserver admin_private.pem
```

--- 
