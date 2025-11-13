AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/DP_loader.cpp")
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/DisconnectProtection.cpp")

AC_ADD_SCRIPT_LOADER("Disconnect_Protection" "${CMAKE_CURRENT_LIST_DIR}/src/DP_loader.cpp")

CU_SET_PATH("CMAKE_DISCONNECT_PROTECTION_DIR" "${CMAKE_CURRENT_LIST_DIR}")

# Find CURL
find_package(CURL REQUIRED)

# Add include directories
target_include_directories(worldserver PRIVATE ${CURL_INCLUDE_DIRS})

# Link CURL library
target_link_libraries(worldserver PRIVATE ${CURL_LIBRARIES})
