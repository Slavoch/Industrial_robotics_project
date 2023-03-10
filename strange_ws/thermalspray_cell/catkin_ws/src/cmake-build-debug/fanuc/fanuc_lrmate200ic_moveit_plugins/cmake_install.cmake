# Install script for directory: /home/artemyakimchuk/Documents/thermalspray_cell/catkin_ws/src/fanuc/fanuc_lrmate200ic_moveit_plugins

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/artemyakimchuk/Documents/thermalspray_cell/catkin_ws/src/cmake-build-debug/fanuc/fanuc_lrmate200ic_moveit_plugins/catkin_generated/installspace/fanuc_lrmate200ic_moveit_plugins.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/fanuc_lrmate200ic_moveit_plugins/cmake" TYPE FILE FILES
    "/home/artemyakimchuk/Documents/thermalspray_cell/catkin_ws/src/cmake-build-debug/fanuc/fanuc_lrmate200ic_moveit_plugins/catkin_generated/installspace/fanuc_lrmate200ic_moveit_pluginsConfig.cmake"
    "/home/artemyakimchuk/Documents/thermalspray_cell/catkin_ws/src/cmake-build-debug/fanuc/fanuc_lrmate200ic_moveit_plugins/catkin_generated/installspace/fanuc_lrmate200ic_moveit_pluginsConfig-version.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/fanuc_lrmate200ic_moveit_plugins" TYPE FILE FILES "/home/artemyakimchuk/Documents/thermalspray_cell/catkin_ws/src/fanuc/fanuc_lrmate200ic_moveit_plugins/package.xml")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/fanuc_lrmate200ic_moveit_plugins" TYPE FILE FILES "/home/artemyakimchuk/Documents/thermalspray_cell/catkin_ws/src/fanuc/fanuc_lrmate200ic_moveit_plugins/readme.md")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/artemyakimchuk/Documents/thermalspray_cell/catkin_ws/src/cmake-build-debug/fanuc/fanuc_lrmate200ic_moveit_plugins/lrmate200ic_kinematics/cmake_install.cmake")
  include("/home/artemyakimchuk/Documents/thermalspray_cell/catkin_ws/src/cmake-build-debug/fanuc/fanuc_lrmate200ic_moveit_plugins/lrmate200ic5f_kinematics/cmake_install.cmake")
  include("/home/artemyakimchuk/Documents/thermalspray_cell/catkin_ws/src/cmake-build-debug/fanuc/fanuc_lrmate200ic_moveit_plugins/lrmate200ic5h_kinematics/cmake_install.cmake")
  include("/home/artemyakimchuk/Documents/thermalspray_cell/catkin_ws/src/cmake-build-debug/fanuc/fanuc_lrmate200ic_moveit_plugins/lrmate200ic5l_kinematics/cmake_install.cmake")

endif()

