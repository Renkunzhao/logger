cmake_minimum_required(VERSION 3.10)
project(logger VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(catkin QUIET)
if(catkin_FOUND)
    message("Found catkin")
    catkin_package(
        INCLUDE_DIRS include
        LIBRARIES CsvLogger
    )
endif()

##############################
# Dependency
##############################

find_package(Eigen3 REQUIRED)

##############################
# Build 
##############################

add_library(CsvLogger STATIC src/CsvLogger.cpp)

target_include_directories(CsvLogger
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${EIGEN3_INCLUDE_DIRS}
)

##############################
# Install 
##############################

# 安装库
install(TARGETS CsvLogger EXPORT CsvLogger_Targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)

# 安装头文件
install(DIRECTORY include/ DESTINATION include)

# 安装导出文件
install(EXPORT CsvLogger_Targets
        FILE CsvLoggerConfig.cmake
        NAMESPACE CsvLogger::
        DESTINATION lib/cmake/CsvLogger)

##############################
# Test 
##############################

add_executable(CsvLogger_test test/CsvLogger_test.cpp)

target_link_libraries(CsvLogger_test PRIVATE CsvLogger)

##############################
# ROS1 
##############################

if(catkin_FOUND)
    # 安装库和头文件
    install(TARGETS CsvLogger
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
    )
    install(DIRECTORY include/CsvLogger/
        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
    )
endif()

##############################
# ROS2
##############################

find_package(ament_cmake QUIET)
if(ament_cmake_FOUND)
    message("Found ament_cmake")

    # 安装可执行文件
    install(TARGETS CsvLogger_test 
        DESTINATION lib/${PROJECT_NAME}
    )

    ament_package()
endif()
