cmake_minimum_required(VERSION 3.14...3.28)

project(order_book
        VERSION 1.2
        DESCRIPTION "Example of Order Book for given instrument"
        LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED True)
    set(CMAKE_CXX_EXTENSIONS OFF)

    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# The compiled library code is here
add_subdirectory(src)

# The executable code is here
add_subdirectory(app)
