# This file builds the PHI firmware

# include files
include_directories (${PHI_SOURCE_DIR}/include) 

# for any libs we might have
# link_directories (${PHI_BINARY_DIR}/my_lib_name) 

# added some extra options to gcc
# 
# Note: "-pthread" option affects compile and link
#
# Note: -Wno-unused-result is to allow ignoring function retvals if
#       you want to.  Sometimes you don't care if it works or not.
#

set (CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-pthread -Wextra -Wdouble-promotion -Wno-unused-result")

# Specify we're building an executable and the files that compose it (no src lib files)

add_executable ( phi
  main.c
  motor.c
  phi_utils.c
  uart.c
  webadmin.c
  webjson.c
  jsmn.c
  spi.c
)

# no libs to link
# target_link_libraries (phi my_lib_name) 
