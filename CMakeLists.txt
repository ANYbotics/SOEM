cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
project(soem)

# Set RPATH, such that the libraries can be found without the need to call `ldconfig`.
# See https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/RPATH-handling for details.
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

find_package(catkin REQUIRED)

if(DEFINED HOST_INSTALL)
   set(SOEM_INCLUDE_INSTALL_DIR include/soem/soem)
else()
   #set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install)
   set(SOEM_INCLUDE_INSTALL_DIR include)
endif()

#message("CMAKE_SYSTEM_NAME is ${CMAKE_SYSTEM_NAME}")

if(WIN32)
   set(OS "win32")
   include_directories(include_win32/soem/oshw/wpcap/Include)
   link_directories(${CMAKE_SOURCE_DIR}/include_win32/soem/oshw/wpcap/Lib)
elseif(UNIX)
   set(OS "linux")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "rt-kernel")
   set(OS "rtk")
#   message("ARCH is ${ARCH}")
#   message("BSP is ${BSP}")
   include_directories(include_${OS}/soem/oshw/${ARCH})
   add_definitions("-Wno-unused-but-set-variable")
   add_definitions("-Wno-unused-function")
   add_definitions("-Wno-format")
endif()

#message("OS is ${OS}")

include_directories(
  include
  include_${OS}
  ${catkin_INCLUDE_DIRS}
)

if (MSVC)
  set(CMAKE_C_FLAGS_RELEASE "/D _CRT_SECURE_NO_WARNINGS /EHsc")
  set(CMAKE_C_FLAGS_DEBUG "/D _CRT_SECURE_NO_WARNINGS /EHsc /ZI /Od")
endif()

file(GLOB SOEM_SOURCES include/soem/soem/*.c)
file(GLOB OSAL_SOURCES include_${OS}/soem/osal/*.c)
file(GLOB OSHW_SOURCES include_${OS}/soem/oshw/*.c)

if(WIN32)
  set(SOEM_SOURCES ${SOEM_SOURCES})
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "rt-kernel")
  file(GLOB OSHW_ARCHSOURCES include_${OS}/soem/oshw/${ARCH}/*.c)
endif()

catkin_package(
  INCLUDE_DIRS
    include
    include_${OS}
  LIBRARIES
    soem
)

# Generate postinst
set(EXECUTABLE_PATHS "${CMAKE_INSTALL_PREFIX}/${CATKIN_PACKAGE_BIN_DESTINATION}/slaveinfo ${CMAKE_INSTALL_PREFIX}/${CATKIN_PACKAGE_BIN_DESTINATION}/eepromtool")
configure_file(debian/postinst.in ${CMAKE_CURRENT_SOURCE_DIR}/debian/postinst @ONLY)

add_library(soem ${SOEM_SOURCES} ${OSAL_SOURCES} ${OSHW_SOURCES} ${OSHW_ARCHSOURCES})
if(WIN32)
  target_link_libraries(soem wpcap.lib Packet.lib Ws2_32.lib Winmm.lib ${HDF5_LIBRARIES})
elseif(UNIX)
  target_link_libraries(soem pthread rt ${catkin_LIBRARIES})
endif()

install(TARGETS soem
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY include/${PROJECT_NAME}/ include_${OS}/${PROJECT_NAME}/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
  FILES_MATCHING PATTERN "*.h"
)

add_subdirectory(test/linux/slaveinfo)
add_subdirectory(test/linux/eepromtool)
#add_subdirectory(test/linux/simple_test)
#add_subdirectory(test/linux/elmo_twitter)

##########
## Test ##
##########
if(CATKIN_ENABLE_TESTING)
  catkin_add_gtest(test_${PROJECT_NAME}
    test/EmptyTests.cpp
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test
  )

  if(TARGET test_${PROJECT_NAME})
    target_link_libraries( test_${PROJECT_NAME}
      ${PROJECT_NAME}
      ${catkin_LIBRARIES}
      gtest_main
    )
    find_package(cmake_code_coverage QUIET)
    if(cmake_code_coverage_FOUND)
      add_gtest_coverage()
    endif(cmake_code_coverage_FOUND)
  endif()
endif()

