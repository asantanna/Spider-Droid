# This file builds the PHI firmware

# include files
include_directories (${PHI_SOURCE_DIR}/include) 

# for any libs we might have
# link_directories (${PHI_BINARY_DIR}/my_lib_name) 

# say we want the "-pthread" option during the build
# (affects compile and link)

set (CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-pthread")

# Specify we're building an executable and the files that compose it (no src lib files)

add_executable ( phi
  main.c
  motor.c
  phi_utils.c
  uart.c
  webadmin.c
)

# no libs to link
# target_link_libraries (phi my_lib_name) 
