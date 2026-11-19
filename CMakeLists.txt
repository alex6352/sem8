cmake_minimum_required(VERSION 3.16)
project(sem8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(sem8 main.cpp)
if(MSVC)
    target_compile_options(sem8 PRIVATE /W4 /WX)
else()
    target_compile_options(sem8 PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

include(CTest)
if(BUILD_TESTING)
    foreach(task RANGE 1 5)
        add_test(NAME task_${task} COMMAND sem8 ${task})
        set_tests_properties(task_${task} PROPERTIES
            PASS_REGULAR_EXPRESSION "check OK" TIMEOUT 10)
    endforeach()
endif()
