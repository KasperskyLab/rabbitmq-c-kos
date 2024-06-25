# © 2024 AO Kaspersky Lab
# Licensed under the MIT License

project(consumer)

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

# Generate the *.edl.h file using the NK compiler for the Consumer program.
nk_build_edl_files(consumer_edl_files
  NK_MODULE "amqp"
  EDL ${RESOURCES}/edl/Consumer.edl
  )

add_executable(Consumer
  src/main.cpp
  src/consumer.cpp
  ${UTILS}/utils.cpp
  )

target_compile_features(Consumer PUBLIC cxx_std_17)
target_compile_options(Consumer PUBLIC "-fexceptions")

add_dependencies(Consumer consumer_edl_files)
target_include_directories(Consumer PUBLIC ${UTILS})

target_link_libraries(Consumer
  ${vfs_CLIENT_LIB}
  ${RABBITMQ_LIB}
  ${CMAKE_THREAD_LIBS_INIT}
  )

set_target_properties(Consumer PROPERTIES ${vfs_ENTITY}_REPLACEMENT "")
