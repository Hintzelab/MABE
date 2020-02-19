option(ENABLE_Brain_HumanBrain "Enable Module HumanBrain" OFF)
if (ENABLE_Brain_HumanBrain)
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/HumanBrain.cpp)
endif()
