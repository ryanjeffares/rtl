cmake_minimum_required(VERSION 3.27)
project(rtl-tests)

include(CTest)

if (MSVC)
    set(RTL_OPTIONS /W4 /WX /std:c++latest)
else ()
    set(RTL_OPTIONS -Wall -Wextra -Wpedantic -Wconversion -Werror -std=c++23)
endif ()

add_executable(rtl-tests tests/rtl_tests.cpp)
target_compile_options(rtl-tests PRIVATE ${RTL_OPTIONS})
target_include_directories(rtl-tests PRIVATE include)

add_test(NAME RtlTestAll COMMAND rtl-tests)

