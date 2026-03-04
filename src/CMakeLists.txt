cmake_minimum_required(VERSION 3.8)
project(detect)


if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(OpenCV REQUIRED)
find_package(cv_bridge REQUIRED)

# 添加可执行文件
add_executable(detect_node src/detect_node.cpp)

ament_target_dependencies(detect_node 
  rclcpp 
  std_msgs 
  sensor_msgs 
  OpenCV 
  cv_bridge
)

# 安装可执行文件
install(TARGETS
  detect_node
  DESTINATION lib/${PROJECT_NAME}
)
# 安装模板、配置和launch文件
install(DIRECTORY templates config launch
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
