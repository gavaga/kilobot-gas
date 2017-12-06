if(ARGOS_BUILD_FOR_SIMULATOR)
  include_directories(${CMAKE_SOURCE_DIR}/plugins/robots/kilobot/control_interface)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})

  #
  # Common library to all kilobot code
  #
  add_library(lib err.c bitarray.c fifo.c list.c matf.c kb_math.c)
endif(ARGOS_BUILD_FOR_SIMULATOR)
