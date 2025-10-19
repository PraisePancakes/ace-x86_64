cmake_minimum_required(VERSION 3.14)  # FetchContent is best with 3.14+

set(PROJECT_NAME ACE)

project(${PROJECT_NAME} VERSION 0.0.1 DESCRIPTION "Ace Compiler" LANGUAGES CXX)

message("Your C++ compiler supports these C++ features:")
foreach(i ${CMAKE_CXX_COMPILE_FEATURES})
  message("${i}")
endforeach()

# Main project executable
add_executable(${PROJECT_NAME} 
    main.cpp 
)

message("INFO :")
message("  C: '${CMAKE_C_COMPILER}'")
message("  C++: '${CMAKE_CXX_COMPILER}'")
message("  Source: ${PROJECT_SOURCE_DIR}")
message("  Binary: ${PROJECT_BINARY_DIR}")
message("  Version: ${PROJECT_VERSION}")
message("  Version (alt): ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-unused-variable -Wno-parentheses)
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)


# ----------------------------
# Add Doctest test executable
# ----------------------------

include(FetchContent)
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/doctest/doctest.git
  GIT_TAG        v2.4.11 # or latest stable tag
)
FetchContent_MakeAvailable(doctest)

add_executable(testing
  test/test_main.cpp
  test/dtest_test_lexer.cpp
)
target_link_libraries(testing PRIVATE doctest)
target_compile_features(testing PUBLIC cxx_std_20)

if(MSVC)
  target_compile_options(testing PRIVATE /W4 /WX)
else()
  target_compile_options(testing PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
# Optional: enable CTest
enable_testing()
add_test(NAME test_all COMMAND testing)



