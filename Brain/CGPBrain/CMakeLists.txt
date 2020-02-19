option(ENABLE_Brain_CGPBrain "Enable Module CGPBrain" OFF)
if (ENABLE_Brain_CGPBrain)
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/CGPBrain.cpp)
endif()
