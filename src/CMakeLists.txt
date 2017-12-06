if(ARGOS_BUILD_FOR_SIMULATOR)
  include_directories(${CMAKE_SOURCE_DIR}/plugins/robots/kilobot/control_interface)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/kb)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/state)

  #
  # Subdirectory libraries
  #
  add_subdirectory(lib)
  add_subdirectory(kb)
  add_subdirectory(state)

  #
  # boundary detection
  #
  add_executable(topology topology.c)
  # link against main plugin and subdirectory libraries
  target_link_libraries(topology argos3plugin_simulator_kilolib kb state lib)
endif(ARGOS_BUILD_FOR_SIMULATOR)
