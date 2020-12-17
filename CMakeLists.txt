cmake_minimum_required(VERSION 3.11.0)
project(RxCppExtras VERSION 0.1.0)

add_library(RxCppExtras INTERFACE)
target_include_directories(RxCppExtras INTERFACE rxcppextras)

include(FetchContent)
FetchContent_Declare(
        rxcpp
        GIT_REPOSITORY  https://github.com/ReactiveX/RxCpp
        GIT_TAG         HEAD
)

FetchContent_GetProperties(rxcpp)
if(NOT rxcpp_POPULATED)
    FetchContent_Populate(rxcpp)
    add_subdirectory(${rxcpp_SOURCE_DIR} ${rxcpp_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
