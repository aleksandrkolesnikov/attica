cmake_minimum_required(VERSION 3.16)

project(PackageTest CXX)

set(TARGET_NAME "AtticaTestPackage")

find_package(KF5Attica 5.96.0 REQUIRED)

add_executable(${TARGET_NAME} "example.cpp")
target_link_libraries(${TARGET_NAME} KF5::Attica)
