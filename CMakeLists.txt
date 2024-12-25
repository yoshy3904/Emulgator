cmake_minimum_required(VERSION 3.15.2)
set(CMAKE_CXX_STANDARD 17)

project(Emulgator)

# Define location of source and header files.
include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/imgui

        ${PROJECT_SOURCE_DIR}/external/imgui-docking
)
file(GLOB all_SRCS
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.c"
        "${PROJECT_SOURCE_DIR}/src/imgui/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/imgui/*.c"

        "${PROJECT_SOURCE_DIR}/external/imgui-docking/*.cpp"
        "${PROJECT_SOURCE_DIR}/external/imgui-docking/*.c"
        )
add_executable(${PROJECT_NAME} ${all_SRCS} main.cpp)

# Define the libraries to be used.
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "external/sfml/SFML_2.5.1-TDM_GCC_10.3.0-Mingw_MakeFiles-Static/lib/cmake/SFML")
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC sfml-graphics sfml-audio)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS -static) # Static linking the standard libraries (so we dont have to keep .dll's nearby)
    message("GNU compiler detected")
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../bin/${CMAKE_BUILD_TYPE}/")
elseif(MSVC)
    message("MSVC compiler detected")
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../bin/")
    # Set the MSVC Runtime-Library from "Multithreaded (/MD)" to "Multithreaded-DLL (/MT)", because otherwise linking errors occur.
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    message("Unrecognized compiler detected")
endif()