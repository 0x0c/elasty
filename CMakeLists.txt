cmake_minimum_required(VERSION 3.1)
project(elasty)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------------------------
# Find prerequisites
# ------------------------------------------------------------------------------

find_package(Eigen3 REQUIRED)
if((NOT TARGET Eigen3::Eigen) AND (DEFINED EIGEN3_INCLUDE_DIR))
	# Eigen 3.0--3.3 do not provide the target named Eigen3::Eigen
	add_library(AliasEigen3 INTERFACE)
	target_include_directories(AliasEigen3 INTERFACE ${EIGEN3_INCLUDE_DIR})
	add_library(Eigen3::Eigen ALIAS AliasEigen3)
endif()

# ------------------------------------------------------------------------------
# Build external libraries
# ------------------------------------------------------------------------------

set(ALEMBIC_BUILD_LIBS     ON  CACHE INTERNAL "" FORCE)
set(ALEMBIC_LIB_USES_BOOST OFF CACHE INTERNAL "" FORCE)
set(ALEMBIC_LIB_USES_TR1   OFF CACHE INTERNAL "" FORCE)
set(ALEMBIC_SHARED_LIBS    OFF CACHE INTERNAL "" FORCE)
set(USE_ARNOLD             OFF CACHE INTERNAL "" FORCE)
set(USE_BINARIES           ON  CACHE INTERNAL "" FORCE)
set(USE_EXAMPLES           OFF CACHE INTERNAL "" FORCE)
set(USE_HDF5               OFF CACHE INTERNAL "" FORCE)
set(USE_MAYA               OFF CACHE INTERNAL "" FORCE)
set(USE_PRMAN              OFF CACHE INTERNAL "" FORCE)
set(USE_PYALEMBIC          OFF CACHE INTERNAL "" FORCE)
set(USE_STATIC_BOOST       OFF CACHE INTERNAL "" FORCE)
set(USE_STATIC_HDF5        OFF CACHE INTERNAL "" FORCE)
set(USE_TESTS              OFF CACHE INTERNAL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/alembic)
target_include_directories(Alembic PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/alembic/lib>)
set_target_properties(Alembic PROPERTIES CXX_STANDARD 11)

set(TIMER_BUILD_TEST OFF CACHE INTERNAL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/timer)

# ------------------------------------------------------------------------------
# Build the core library
# ------------------------------------------------------------------------------

file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${CMAKE_CURRENT_SOURCE_DIR}/include/elasty/*.hpp)

add_library(elasty STATIC ${sources} ${headers})
target_include_directories(elasty PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(elasty PUBLIC Eigen3::Eigen)
target_link_libraries(elasty PRIVATE Alembic tinyobjloader)

# ------------------------------------------------------------------------------
# Build examples
# ------------------------------------------------------------------------------

option(ELASTY_EXAMPLES ON "")
if(ELASTY_EXAMPLES)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/bigger)

  add_executable(cloth-alembic ${CMAKE_CURRENT_SOURCE_DIR}/examples/cloth-alembic/main.cpp)
  target_link_libraries(cloth-alembic elasty timer)
  add_custom_command(TARGET cloth-alembic POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/models $<TARGET_FILE_DIR:cloth-alembic>/models)

  add_executable(interactive-app ${CMAKE_CURRENT_SOURCE_DIR}/examples/interactive-app/main.cpp)
  target_link_libraries(interactive-app bigger elasty)
  add_custom_command(TARGET interactive-app POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${BIGGER_SHADER_OUTPUT_DIR} $<TARGET_FILE_DIR:interactive-app>/shaders)
  add_custom_command(TARGET interactive-app POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/models $<TARGET_FILE_DIR:interactive-app>/models)
else()
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/bigger/external/tinyobjloader)
endif()

# ------------------------------------------------------------------------------
# Build tests
# ------------------------------------------------------------------------------

option(ELASTY_TESTS ON "")
if(ELASTY_TESTS)
  enable_testing()

  add_executable(test-bending-constraint ${CMAKE_CURRENT_SOURCE_DIR}/tests/test-bending-constraint.cpp)
  target_link_libraries(test-bending-constraint elasty)

  add_test(NAME test-bending-constraint COMMAND $<TARGET_FILE:test-bending-constraint>)
endif()
