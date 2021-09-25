cmake_minimum_required(VERSION 3.20)
project(profman)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_CXX_STANDARD 17)

if(CMAKE_INTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBNOTIFY REQUIRED IMPORTED_TARGET libnotify)

#
# Main profman binary
#
add_executable(profman
    main.cpp
    util.cpp util.h
)
target_link_libraries(profman PRIVATE ${LIBNOTIFY_LIBRARIES})
target_include_directories(profman PRIVATE ${LIBNOTIFY_INCLUDE_DIRS})
target_compile_definitions(profman PRIVATE -DPROFMAN_HELPER_BIN_PATH=\"${CMAKE_INSTALL_PREFIX}/share/profman/bin/profman_helper\")
install(TARGETS profman RUNTIME)

#
# Setuid helper
#
add_executable(profman_helper
    main_helper.cpp
    util.cpp util.h
)
set(HELPER_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE WORLD_READ WORLD_EXECUTE)
install(TARGETS profman_helper RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/share/profman/bin PERMISSIONS ${HELPER_PERMISSIONS} SETUID)

