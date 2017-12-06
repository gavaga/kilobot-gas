if(ARGOS_BUILD_FOR_SIMULATOR)
  include_directories(${CMAKE_SOURCE_DIR}/plugins/robots/kilobot/control_interface)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib)

  #
  # All kilobot code without the main kilobot framework
  #
  add_library(kb lcv.c nbi.c nbr.c netcomp.c localize.c msg.c)
  target_link_libraries(kb argos3plugin_simulator_kilolib lib)
endif(ARGOS_BUILD_FOR_SIMULATOR)
