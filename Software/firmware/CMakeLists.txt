
# This file is used by "CMake" to create the makefile for PHI

# The name of our project is "PHI". CMakeLists files in this project can 
# refer to the root source directory of the project as ${PHI_SOURCE_DIR} and 
# to the root binary directory of the project as ${PHI_BINARY_DIR}. 

cmake_minimum_required (VERSION 2.6) 
project (PHI) 

 # Recurse into the src subdirectory. This does not actually 
 # cause another cmake executable to run. The same process will walk through 
 # the project's entire directory structure. 

add_subdirectory (src)


##########################
# ANDRE useful info below
##########################

# if there are other subdirectories, just keep adding them
# Note: each must have its own CMakeLists.txt file

#add_subdirectory (HelloLib) 
#add_subdirectory (Demo) 

######################

# to create a library, use this in the CMakeLists.txt file of the library subdir

# Create a library called "HelloLib" which includes the source file "hello.cxx". 
# The extension is already found. Any number of sources could be listed here. 

# add_library (HelloLib hello.cxx) 

######################

# simple project that uses the library above

# Make sure the compiler can find include files from our Hello library. 
# include_directories (${HELLO_SOURCE_DIR}/HelloLib) 

# Make sure the linker can find the Hello library once it is built. 
# link_directories (${HELLO_BINARY_DIR}/HelloLib) 

# Add executable called "helloDemo" that is built from the source files 
# "demo.cxx" and "demo_b.cxx". The extensions are automatically found. 

# add_executable (helloDemo demo.cxx demo_b.cxx) 

# Link the executable to the Hello library. 
# target_link_libraries (helloDemo HelloLib) 

######################
