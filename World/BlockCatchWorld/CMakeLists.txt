option(ENABLE_World_BlockCatchWorld "Enable Module BlockCatchWorld" OFF)
if (ENABLE_World_BlockCatchWorld)
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/BlockCatchWorld.cpp)
endif()
