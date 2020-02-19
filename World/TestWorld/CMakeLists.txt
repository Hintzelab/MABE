option(ENABLE_World_TestWorld "Enable Module TestWorld" OFF)
if (ENABLE_World_TestWorld)
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/TestWorld.cpp)
endif()
