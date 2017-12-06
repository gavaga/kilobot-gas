if(ARGOS_BUILD_FOR_SIMULATOR)
  include_directories(${CMAKE_SOURCE_DIR}/plugins/robots/kilobot/control_interface)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../kb)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib)

  #
  # All states code
  #
  add_library(state state.c s_err.c s_idle.c s_wait.c s_g_ohn.c s_lcv.c s_elect.c s_twiddle.c s_coalesce.c s_finalize.c s_search.c s_avoid.c s_done.c)
  target_link_libraries(state argos3plugin_simulator_kilolib lib kb)
endif(ARGOS_BUILD_FOR_SIMULATOR)
