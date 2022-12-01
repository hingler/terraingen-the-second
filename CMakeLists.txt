cmake_minimum_required(VERSION 3.0.0)
project(terraingen-the-second VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# glm might be duplicated (if we need it)
# might have to look into a solution for that (shared lib??)

# i will just avoid it right now >:)

set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(GLM_INCLUDES ${LIB_DIR}/glm)


add_library(${PROJECT_NAME} terraingen-the-second.cpp
                                  ${SRC_DIR}/lod/lod_node.cpp
                                  ${SRC_DIR}/lod/LodTreeGenerator.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${INC_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${GLM_INCLUDES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
