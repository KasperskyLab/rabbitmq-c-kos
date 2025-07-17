# © 2024 AO Kaspersky Lab
# Licensed under the MIT License

project_header_default("STANDARD_GNU_17:YES" "STRICT_WARNINGS:YES")

find_package(OpenSSL REQUIRED)
find_package(GTest REQUIRED NO_CMAKE_PACKAGE_REGISTRY)

include(FetchContent)

FetchContent_Declare(rabbitmq SOURCE_DIR ${CMAKE_EXTERNAL_PREFIX})
FetchContent_MakeAvailable(rabbitmq)
set(rabbitmq-c_BUILD_PATH ${CMAKE_BINARY_DIR}/_deps/rabbitmq-build)

generate_edl_file(TestEntity PREFIX amqp)
nk_build_edl_files(multi_vfs_amqp_test NK_MODULE amqp EDL ${EDL_FILE})

add_executable(TestEntity src/main.cpp)

target_compile_features(TestEntity PUBLIC cxx_std_17)
target_compile_options(TestEntity PUBLIC -fexceptions)
target_compile_definitions(TestEntity PRIVATE HAVE_CONFIG_H)
target_link_libraries(TestEntity PRIVATE
  GTest::gtest
  GTest::gmock
  ${vfs_CLIENT_LIB}
  ${OPENSSL_LIBRARIES}
  rabbitmq::rabbitmq-static
  ${CMAKE_THREAD_LIBS_INIT}
  )

target_include_directories(TestEntity PRIVATE
  ${CMAKE_EXTERNAL_PREFIX}/librabbitmq
  ${rabbitmq-c_BUILD_PATH}/librabbitmq
  )

add_dependencies(TestEntity multi_vfs_amqp_test)

set_target_properties(TestEntity PROPERTIES ${vfs_ENTITY}_REPLACEMENT "")
