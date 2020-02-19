option(ENABLE_Brain_WireBrain "Enable Module WireBrain" OFF)
if (ENABLE_Brain_WireBrain)
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/WireBrain.cpp)
endif()
