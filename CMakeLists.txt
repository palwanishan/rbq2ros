cmake_minimum_required(VERSION 3.14)

project(rbq2ros LANGUAGES CXX)

find_package(catkin REQUIRED COMPONENTS cmake_modules roscpp sensor_msgs std_msgs)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network REQUIRED)
find_package(Eigen3 REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

catkin_package(
  INCLUDE_DIRS include
  CATKIN_DEPENDS cmake_modules roscpp sensor_msgs std_msgs 
)

include_directories(include ${catkin_INCLUDE_DIRS} ${QT_INCLUDES} ${EIGEN3_INCLUDE_DIR})

add_executable(rbq2ros
  src/rbq2ros.cpp
  src/client.cpp
  include/client.h
)
add_dependencies(rbq2ros ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(rbq2ros Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network ${catkin_LIBRARIES} ${EIGEN3_LIBRARIES})


install(TARGETS rbq2ros
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)