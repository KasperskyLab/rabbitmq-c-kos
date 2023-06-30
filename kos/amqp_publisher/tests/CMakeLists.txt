# © 2023 AO Kaspersky Lab. All Rights Reserved
# Licensed under the MIT License

include (platform/nk)

project_header_default ("STANDARD_GNU_11:YES" "STRICT_WARNINGS:NO")

set (EXECUTABLE_NAME "TestEntity")

find_package (OpenSSL REQUIRED)

add_definitions (-DHAVE_CONFIG_H)

nk_build_edl_files (multi_vfs_amqp_test
                    NK_MODULE "amqp"
                    EDL ${CMAKE_SOURCE_DIR}/../resources/edl/TestEntity.edl)

set (rabbitmq-c_BUILD_PATH
    ${CMAKE_BINARY_DIR}/publisher/rabbitmq-c/src/rabbitmq-c-build)

set (rabbitmq-c_LIBRARIES 
    ${rabbitmq-c_BUILD_PATH}/librabbitmq/librabbitmq${CMAKE_STATIC_LIBRARY_SUFFIX})

add_executable (
    ${EXECUTABLE_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_compile_features (Publisher PUBLIC cxx_std_17)
target_compile_options (Publisher PUBLIC "-fexceptions")

target_link_libraries (${EXECUTABLE_NAME} PRIVATE 
                       gtest
                       gmock
                       ${vfs_CLIENT_LIB}
                       ${OPENSSL_LIBRARIES}
                       ${rabbitmq-c_LIBRARIES}
                       ${CMAKE_THREAD_LIBS_INIT})

target_include_directories (${EXECUTABLE_NAME} PRIVATE 
                            ${CMAKE_EXTERNAL_PREFIX}/include
                            ${CMAKE_EXTERNAL_PREFIX}/librabbitmq
                            ${rabbitmq-c_BUILD_PATH}/include)

add_dependencies (${EXECUTABLE_NAME} multi_vfs_amqp_test rabbitmq-c)

set_target_properties (${EXECUTABLE_NAME} PROPERTIES ${vfs_ENTITY}_REPLACEMENT "")
