cmake_minimum_required(VERSION 3.11.0)
project(RxCppExtras VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(GNUInstallDirs)

add_library(${PROJECT_NAME} INTERFACE)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)

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

target_include_directories(
    ${PROJECT_NAME}
    INTERFACE $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/rxcppextras>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    INTERFACE ${rxcpp_SOURCE_DIR}/Rx/v2/src/)