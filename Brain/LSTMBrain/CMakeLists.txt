option(ENABLE_Brain_LSTMBrain "Enable Module LSTMBrain" OFF)
if (ENABLE_Brain_LSTMBrain)
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/LSTMBrain.cpp)
endif()
