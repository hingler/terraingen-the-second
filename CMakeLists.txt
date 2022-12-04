cmake_minimum_required(VERSION 3.0.0)
project(terraingen-the-second VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-g -O3")

# glm might be duplicated (if we need it)
# might have to look into a solution for that (shared lib??)

# i will just avoid it right now >:)

set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

set(GLM_INCLUDES ${LIB_DIR}/glm)


add_library(${PROJECT_NAME} terraingen-the-second.cpp
                            ${SRC_DIR}/lod/lod_node.cpp
                            ${SRC_DIR}/terrain/Chunk.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${INC_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${GLM_INCLUDES})

set(TEST_PATHS ${TEST_DIR}/LodTreeGeneratorTest.cpp
               ${TEST_DIR}/HashListTest.cpp
               ${TEST_DIR}/LRUCacheTest.cpp
               ${TEST_DIR}/VertexGeneratorTest.cpp)

set(TEST_NAMES LodTreeGeneratorTest
               HashListTest
               LRUCacheTest
               VertexGeneratorTest)

find_package(gtest REQUIRED)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  foreach(test_path test_name IN ZIP_LISTS TEST_PATHS TEST_NAMES)
    add_executable(${test_name} ${test_path})
    target_link_libraries(${test_name} PRIVATE ${PROJECT_NAME} GTest::gtest_main)
    target_include_directories(${test_name} PUBLIC ${INC_DIR}
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
  endforeach()
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
