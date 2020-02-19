option(ENABLE_Brain_BiLogBrain "Enable Module BiLogBrain" OFF)
if (ENABLE_Brain_BiLogBrain)
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/BiLogBrain.cpp)
endif()
