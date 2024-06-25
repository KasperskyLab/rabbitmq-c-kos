# © 2024 AO Kaspersky Lab
# Licensed under the MIT License

project(publisher)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

include(FetchContent)

FetchContent_Declare(rabbitmq SOURCE_DIR ${CMAKE_EXTERNAL_PREFIX})
FetchContent_MakeAvailable(rabbitmq)

if(PLATFORM_SUPPORTS_DYNAMIC_LINKING)
  set(RABBITMQ_LIB rabbitmq::rabbitmq)
else()
  set(RABBITMQ_LIB rabbitmq::rabbitmq-static)
endif()

nk_build_edl_files(publisher_edl_files
  NK_MODULE "amqp"
  EDL ${RESOURCES}/edl/Publisher.edl
  )

add_executable(Publisher
  src/main.cpp
  src/publisher.cpp
  ${UTILS}/utils.cpp
  )

target_compile_features(Publisher PUBLIC cxx_std_17)
target_compile_options(Publisher PUBLIC -fexceptions)

add_dependencies(Publisher publisher_edl_files)
target_include_directories(Publisher PUBLIC ${UTILS})

target_link_libraries(Publisher
  ${vfs_CLIENT_LIB}
  ${RABBITMQ_LIB}
  ${CMAKE_THREAD_LIBS_INIT}
  )

set_target_properties(Publisher PROPERTIES ${vfs_ENTITY}_REPLACEMENT "")
