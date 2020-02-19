option(ENABLE_World_XorWorld "Enable Module XorWorld" OFF)
if (ENABLE_World_XorWorld)
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/XorWorld.cpp)
endif()
