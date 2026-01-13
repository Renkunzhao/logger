cmake_minimum_required(VERSION 3.10)
project(logger VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

##############################
# ROS1 / Catkin (optional)
##############################
find_package(catkin QUIET)
if(catkin_FOUND)
  message(STATUS "Found catkin")
  catkin_package(
    INCLUDE_DIRS include
    LIBRARIES CsvLogger
  )
endif()

##############################
# Dependencies
##############################
find_package(Eigen3 REQUIRED)

##############################
# Build library
##############################
add_library(CsvLogger STATIC
  src/CsvLogger.cpp
)

# 让下游使用 logger::CsvLogger（标准命名空间）
add_library(logger::CsvLogger ALIAS CsvLogger)

# 传播 Eigen（推荐用 target）
if(TARGET Eigen3::Eigen)
  target_link_libraries(CsvLogger PUBLIC Eigen3::Eigen)
else()
  # 兼容极少数只提供变量的 Eigen3
  target_include_directories(CsvLogger PUBLIC ${EIGEN3_INCLUDE_DIRS})
endif()

target_include_directories(CsvLogger PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

##############################
# Tests (local)
##############################
add_executable(CsvLogger_test test/CsvLogger_test.cpp)
target_link_libraries(CsvLogger_test PRIVATE logger::CsvLogger)

##############################
# Install (pure CMake package)
##############################
install(TARGETS CsvLogger
  EXPORT loggerTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# 导出 targets：loggerTargets.cmake
install(EXPORT loggerTargets
  FILE loggerTargets.cmake
  NAMESPACE logger::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/logger
)

# 生成 loggerConfig.cmake
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/loggerConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/loggerConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/logger
)

# 生成版本文件 loggerConfigVersion.cmake
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/loggerConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/loggerConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/loggerConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/logger
)

##############################
# ROS1 install (optional)
##############################
if(catkin_FOUND)
  install(TARGETS CsvLogger
    ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
  )
  install(DIRECTORY include/
    DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
  )
endif()

##############################
# ROS2 / ament (optional)
##############################
find_package(ament_cmake QUIET)
if(ament_cmake_FOUND)
  message(STATUS "Found ament_cmake")

  # 安装测试可执行文件（可选）
  install(TARGETS CsvLogger_test
    DESTINATION lib/${PROJECT_NAME}
  )

  # ament 导出：targets + dependencies（关键）
  ament_export_targets(loggerTargets HAS_LIBRARY_TARGET)
  ament_export_dependencies(Eigen3)

  ament_package()
endif()
