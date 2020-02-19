option(ENABLE_Brain_ANNBrain "Enable Module ANNBrain" OFF)
if (ENABLE_Brain_ANNBrain)
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/ANNBrain.cpp)
endif()
