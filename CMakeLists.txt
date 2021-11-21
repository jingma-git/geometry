cmake_minimum_required(VERSION 3.5)

project(geometry)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_CXX_STANDARD  17)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/;${CMAKE_MODULE_PATH}")

find_package(Eigen3 REQUIRED)
if(EIGEN3_FOUND)
  message("-- Eigen @ ${EIGEN3_INCLUDE_DIR}")
  message("-- Eigen version: ${EIGEN3_VERSION}")
  include_directories(${EIGEN3_INCLUDE_DIR})
endif(EIGEN3_FOUND)

find_package(OpenMP REQUIRED)
if(OpenMP_FOUND)
    message(STATUS "Found OpenMP")
endif()


find_package(GMP REQUIRED)

# find_package(OpenCV)
# if(OpenCV_FOUND)
#     message("-- OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
#     message("-- OpenCV_LIBRARIES: ${OpenCV_LIBRARIES}")
#     include_directories(${OpenCV_INCLUDE_DIRS})
# endif()

# find_package(Boost)
# if(Boost_FOUND)
#     message("-- Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
#     message("-- Boost_LIBRARIES: ${Boost_LIBRARIES}")
#     include_directories(${Boost_INCLUDE_DIRS})
# endif()

############################################################
# My Head-only egl library: Verts are arranged as 3 by #V
############################################################
include_directories(external/CGAL/include) # 5.3, gcc 8.3 and higher
include_directories(external/egl/include) # egl relies on Eigen and CGAL

add_executable(laplace src/egl/laplace.cpp)
target_link_libraries(laplace Eigen3::Eigen OpenMP::OpenMP_CXX ${GMP_LIBRARIES})

add_executable(test src/egl/test_egl.cpp)
target_link_libraries(test Eigen3::Eigen)

add_executable(test_polyhedron src/egl/cgal/test_polyhedron.cpp)
target_link_libraries(test_polyhedron Eigen3::Eigen)

############################################################
# CGAL
############################################################
add_executable(test_euler src/cgal/test_euler.cpp)

############################################################
# Head-only Libigl library: Verts are arranged as #V by 3
############################################################
option(LIBIGL_USE_STATIC_LIBRARY "Use libigl as static library" ON)
# option(LIBIGL_WITH_TETGEN            "Use Tetgen"                   ON)
# option(LIBIGL_WITH_TRIANGLE          "Use Triangle"                 ON)
# option(LIBIGL_WITH_OPENGL            "Use OpenGL"                   ON)
# option(LIBIGL_WITH_OPENGL_GLFW       "Use GLFW"                     ON)
# option(LIBIGL_WITH_OPENGL_GLFW_IMGUI "Use ImGui"                    ON)

find_package(LIBIGL REQUIRED)
if(LIBIGL_FOUND)
   message("-- Found LIBIGL ${LIBIGL_INCLUDE_DIR}")
   include_directories(${LIBIGL_INCLUDE_DIR})
else()
   message("-- Not Found LIBIGL")
endif()


############################################################
# Create ACAM library
############################################################
#Generate the static library from the library sources
include_directories(external/acam/include/)
file(GLOB SRC_ACAM external/acam/src/*.cpp)
add_library(ACAM_mesh STATIC ${SRC_ACAM})
target_include_directories(ACAM_mesh 
        PUBLIC 
        external/acam/include
)


############################################################
# Create an executable
############################################################

# Add an executable with the above sources
add_executable(ACAM_example 
    src/acam/main.cpp
)

add_executable(test_acam
    src/acam/test.cpp
)

# link the new ACAM_example target with the ACAM_mesh target
target_link_libraries( ACAM_example
    PRIVATE 
        ACAM_mesh
)

target_link_libraries( test_acam
    PRIVATE 
        ACAM_mesh
)


############################################################
# Project Homework 1
############################################################

# file (GLOB HW1_FILE
#     src/acam/hw1/*.cpp
# )

# # Add an executable with the above sources
# add_executable(${PROJECT_NAME}_HW1 ${HW1_FILE})
# target_include_directories(${PROJECT_NAME}_HW1
#     PRIVATE 
#         ${PROJECT_SOURCE_DIR}/include
# )

# target_link_libraries(${PROJECT_NAME}_HW1
#     PRIVATE 
#         ACAM_mesh
# )

############################################################
# Project Homework 2: Curvature Estimation and Visualization
############################################################

# file (GLOB HW2_FILE
#     src/acam/hw2/*.cpp
# )

# # Add an executable with the above sources
# add_executable(${PROJECT_NAME}_HW2 ${HW2_FILE})
# target_include_directories(${PROJECT_NAME}_HW2
#     PRIVATE 
#         ${PROJECT_SOURCE_DIR}/include
# )

# target_link_libraries(${PROJECT_NAME}_HW2
#     PRIVATE 
#         ACAM_mesh
# )

add_executable(igl_curvature src/igl/curvature.cpp)
target_link_libraries(igl_curvature igl::core)

############################################################
# Project Homework 3
############################################################

# file (GLOB HW3_FILE
#     src/acam/hw3/*.cpp
# )

# # Add an executable with the above sources
# add_executable(${PROJECT_NAME}_HW3 ${HW3_FILE})
# target_include_directories(${PROJECT_NAME}_HW3
#     PRIVATE 
#         ${PROJECT_SOURCE_DIR}/include
# )

# target_link_libraries(${PROJECT_NAME}_HW3
#     PRIVATE 
#         ACAM_mesh
# )

############################################################
# Project Homework 4
############################################################

# file (GLOB HW4_FILE
#     src/acam/hw4/*.cpp
# )

# Add an executable with the above sources
# add_executable(${PROJECT_NAME}_HW4 ${HW4_FILE})
# target_include_directories(${PROJECT_NAME}_HW4
#     PRIVATE 
#         ${PROJECT_SOURCE_DIR}/include
#         ${EIGEN_PATH}
# )

# target_link_libraries(${PROJECT_NAME}_HW4
#     PRIVATE 
#         ACAM_mesh
# )

############################################################
# Project Homework 5
############################################################

# file (GLOB HW5_FILE
#     src/acam/hw5/*.cpp
# )

# # Add an executable with the above sources
# add_executable(${PROJECT_NAME}_HW5 ${HW5_FILE})
# target_include_directories(${PROJECT_NAME}_HW5
#     PRIVATE 
#         ${PROJECT_SOURCE_DIR}/include
#         ${EIGEN_PATH}
# )

# target_link_libraries(${PROJECT_NAME}_HW5
#     PRIVATE 
#         ACAM_mesh
# )

############################################################
# Project Homework 6
############################################################

# file (GLOB HW6_FILE
#     src/acam/hw6/*.cpp
# )

# # Add an executable with the above sources
# add_executable(${PROJECT_NAME}_HW6 ${HW6_FILE})
# target_include_directories(${PROJECT_NAME}_HW6
#     PRIVATE 
#         ${PROJECT_SOURCE_DIR}/include
#         ${EIGEN_PATH}
# )

# target_link_libraries(${PROJECT_NAME}_HW6
#     PRIVATE 
#         ACAM_mesh
# )

############################################################
# Project Homework 7
############################################################

# file (GLOB HW7_FILE
#     src/acam/hw7/*.cpp
# )

# # Add an executable with the above sources
# add_executable(${PROJECT_NAME}_HW7 ${HW7_FILE})
# target_include_directories(${PROJECT_NAME}_HW7
#     PRIVATE 
#         ${PROJECT_SOURCE_DIR}/include
#         ${EIGEN_PATH}
# )

# target_link_libraries(${PROJECT_NAME}_HW7
#     PRIVATE 
#         ACAM_mesh
# )

############################################################
# Project Homework 8
############################################################

# file (GLOB HW8_FILE
#     src/acam/hw8/*.cpp
# )

# # Add an executable with the above sources
# add_executable(${PROJECT_NAME}_HW8 ${HW8_FILE})
# target_include_directories(${PROJECT_NAME}_HW8
#     PRIVATE 
#         ${PROJECT_SOURCE_DIR}/include
#         ${EIGEN_PATH}
# )

# target_link_libraries(${PROJECT_NAME}_HW8
#     PRIVATE 
#         ACAM_mesh
# )

############################################################
# Project Homework 9
############################################################

# file (GLOB HW9_FILE
#     src/acam/hw9/*.cpp
# )

# Add an executable with the above sources
# add_executable(hw9 ${HW9_FILE})
# target_include_directories(hw9
#     PRIVATE 
#         ${PROJECT_SOURCE_DIR}/include
#         ${EIGEN_PATH}
# )

# target_link_libraries(hw9
#     PRIVATE 
#         ACAM_mesh
# )

############################################################
# Project Homework 10
############################################################

# file (GLOB HW10_FILE
#     src/acam/hw10/*.cpp
# )

# # Add an executable with the above sources
# add_executable(${PROJECT_NAME}_HW10 ${HW10_FILE})
# target_include_directories(${PROJECT_NAME}_HW10
#     PRIVATE 
#         ${PROJECT_SOURCE_DIR}/include
#         ${EIGEN_PATH}
# )

# target_link_libraries(${PROJECT_NAME}_HW10
#     PRIVATE 
#         ACAM_mesh
# )

############################################################
# Project Homework 11
############################################################

# file (GLOB HW11_FILE
#     src/acam/hw11/*.cpp
# )

# # Add an executable with the above sources
# add_executable(${PROJECT_NAME}_HW11 ${HW11_FILE})
# target_include_directories(${PROJECT_NAME}_HW11
#     PRIVATE 
#         ${PROJECT_SOURCE_DIR}/include
# )

# target_link_libraries(${PROJECT_NAME}_HW11
#     PRIVATE 
#         ACAM_mesh
# )

############################################################
# Project Homework 12
############################################################

# file (GLOB HW12_FILE
#     src/acam/hw12/*.cpp
# )

# Add an executable with the above sources
# add_executable(hw12 src/acam/hw12/main.cpp)
# target_include_directories(hw12
#     PRIVATE 
#         ${PROJECT_SOURCE_DIR}/include
# )

# target_link_libraries(hw12
#     PRIVATE 
#         ACAM_mesh
# )

############################################################
# Project Homework 13
############################################################

# file (GLOB HW13_FILE
#     src/acam/hw13/*.cpp
# )

# # Add an executable with the above sources
# add_executable(${PROJECT_NAME}_HW13 ${HW13_FILE})
# target_include_directories(${PROJECT_NAME}_HW13
#     PRIVATE 
#         ${PROJECT_SOURCE_DIR}/include
#         ${EIGEN_PATH}
# )

# target_link_libraries(${PROJECT_NAME}_HW13
#     PRIVATE 
#         ACAM_mesh
# )


############################################################
# Project 14 
############################################################

# file (GLOB HW14_FILE
#     src/mesh_contract/*.cpp
# )

# Add an executable with the above sources
# add_executable(contract ${HW14_FILE})
# target_include_directories(contract
#     PRIVATE 
#         ${PROJECT_SOURCE_DIR}/include
#         ${EIGEN_PATH}
# )

# target_link_libraries(contract
#     PRIVATE 
#         ACAM_mesh
# )



## -------------- Project Skeletonization-----------------------------------------
# add_executable(repair src/mesh_contract/repair.cpp)
# target_link_libraries(repair ACAM_mesh)


## -------------- Project Voronoi --------------------------------------------------
# add_executable(voro src/voronoi/main.cpp)
# target_link_libraries(voro Eigen3::Eigen ${OpenCV_LIBRARIES})
# target_compile_definitions(voro PUBLIC -DWITH_INLINE)


## -------------- Project Straigth Skeleton ---------------------------------------
# add_executable(sskel src/sskel/main.cpp)
# target_link_libraries(sskel Eigen3::Eigen ${OpenCV_LIBRARIES} ${Boost_LIBRARIES})
# target_compile_definitions(sskel PUBLIC -DWITH_INLINE)

## -------------- Project Spline ----------------------------------------------------
# add_executable(spline src/spline/main.cpp src/spline/Spline.cpp)
# target_link_libraries(spline Eigen3::Eigen ${OpenCV_LIBRARIES} ${Boost_LIBRARIES})

