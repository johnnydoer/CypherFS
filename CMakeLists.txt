# Minimum cmake version
cmake_minimum_required(VERSION 3.10)

# Project name
project(CipherFS VERSION 1.0 LANGUAGES CXX)

# Setting contants
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")                 # This line needed to generate the comopile_commands.json for lower cmake versions
set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})

# --------------------------------------------------------------------------------
#                         Locate files (change as needed).
# --------------------------------------------------------------------------------
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Add subdir
add_subdirectory(dependencies)

# Find packages
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

# Default name for the library built from src/*.cpp (change if you wish)
set(LIBRARY_NAME CipherFS)

# --------------------------------------------------------------------------------
#                            Build! (Change as needed)
# --------------------------------------------------------------------------------
# Compile all sources into a library.
add_library(${LIBRARY_NAME} ${headers} ${sources})

# Add submodules from dependencies folder
set(EXTERNAL_LIB_HEADERS
    dependencies/nlohmann_json/single_include
)

target_include_directories(${LIBRARY_NAME} PRIVATE ${EXTERNAL_LIB_HEADERS})

# Lib needs its header files, and users of the library must also see these (PUBLIC). (No change needed)
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Add an executable for the file app/main.cpp.
# If you add more executables, copy these lines accordingly.
add_executable(fileserver app/main.cpp)   # Name of exec. and location of file.
target_link_libraries(fileserver PRIVATE OpenSSL::SSL)     # Link executable with OPENSSL
target_link_libraries(fileserver PRIVATE nlohmann_json::nlohmann_json) # Link with nlohmann:json
target_link_libraries(fileserver PRIVATE ${LIBRARY_NAME} -static-libgcc -static-libstdc++ -lrt -pthread)  # Link the executable to library (if it uses it).


# Set the properties you require, e.g. what C++ standard to use. Here applied to library and main (change as needed).
set_target_properties(
    ${LIBRARY_NAME} fileserver
      PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)