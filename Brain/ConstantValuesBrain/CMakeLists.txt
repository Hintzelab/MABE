option(ENABLE_Brain_ConstantValuesBrain "Enable Module ConstantValuesBrain" OFF)
if (ENABLE_Brain_ConstantValuesBrain)
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/ConstantValuesBrain.cpp)
endif()
