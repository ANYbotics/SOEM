set(SOURCES eepromtool.c ../ecatcomm_data.c)
add_executable(eepromtool ${SOURCES})
target_link_libraries(eepromtool soem)

if(WIN32)
   target_link_libraries(eepromtool wpcap.lib Packet.lib Ws2_32.lib Winmm.lib)
elseif(UNIX)
   target_link_libraries(eepromtool pthread rt)
endif()

# Prevent the linker from removing transitive dependencies (see Readme of anymal_low_level_controller package).
set_target_properties(eepromtool PROPERTIES LINK_FLAGS "-Wl,--no-as-needed")

install(TARGETS eepromtool
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
