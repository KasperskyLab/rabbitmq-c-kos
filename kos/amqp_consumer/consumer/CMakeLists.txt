# © 2023 AO Kaspersky Lab. All Rights Reserved
# Licensed under the MIT License

cmake_minimum_required (VERSION 3.12)

include (platform/nk)

find_package (OpenSSL REQUIRED)
find_package (Threads REQUIRED)

include (ExternalProject)

ExternalProject_Add (rabbitmq-c
  PREFIX         rabbitmq-c
  INSTALL_COMMAND ""
  SOURCE_DIR     ${CMAKE_EXTERNAL_PREFIX}
  CMAKE_ARGS     -DBUILD_EXAMPLES:BOOL=OFF
                 -DBUILD_TOOLS:BOOL=OFF
                 -DBUILD_SHARED_LIBS:BOOL=OFF
                 -DBUILD_STATIC_LIBS:BOOL=ON
                 -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE})

set (rabbitmq-c_BUILD_PATH
      ${CMAKE_CURRENT_BINARY_DIR}/rabbitmq-c/src/rabbitmq-c-build)

set (rabbitmq-c_LIBRARIES 
      ${rabbitmq-c_BUILD_PATH}/librabbitmq/librabbitmq${CMAKE_STATIC_LIBRARY_SUFFIX})

add_executable (Consumer 
                src/main.cpp
                src/consumer.cpp
                ${CMAKE_SOURCE_DIR}/../utils/utils.cpp)

target_compile_features (Consumer PUBLIC cxx_std_17)
target_compile_options (Consumer PUBLIC "-fexceptions")

add_dependencies (Consumer consumer_edl_files rabbitmq-c)
target_include_directories (Consumer PUBLIC
                            ${CMAKE_SOURCE_DIR}/../utils
                            ${CMAKE_EXTERNAL_PREFIX}/include
                            ${rabbitmq-c_BUILD_PATH}/include)

target_link_libraries(Consumer
                      ${vfs_CLIENT_LIB}
                      ${rabbitmq-c_LIBRARIES} 
                      ${CMAKE_THREAD_LIBS_INIT})

set_target_properties (Consumer PROPERTIES ${vfs_ENTITY}_REPLACEMENT "")
