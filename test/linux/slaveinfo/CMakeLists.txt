set(SOURCES slaveinfo.c ../ecatcomm_data.c)
add_executable(slaveinfo ${SOURCES})
target_link_libraries(slaveinfo soem)

if(WIN32)
  target_link_libraries(slaveinfo wpcap.lib Packet.lib Ws2_32.lib Winmm.lib)
elseif(UNIX)
  target_link_libraries(slaveinfo pthread rt)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "rt-kernel")
  target_link_libraries(slaveinfo "-Wl,--start-group -l${BSP} -l${ARCH} -lkern -ldev -lsio -lblock -lfs -lusb -llwip -leth -li2c -lrtc -lcan -lnand -lspi -lnor -lpwm -ladc -ltrace -lc -lm -Wl,--end-group")
endif()

# Prevent the linker from removing transitive dependencies (see Readme of anymal_low_level_controller package).
set_target_properties(slaveinfo PROPERTIES LINK_FLAGS "-Wl,--no-as-needed")

install(TARGETS slaveinfo
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
