option(ENABLE_World_BerryWorld "Enable Module BerryWorld" OFF)
if (ENABLE_World_BerryWorld)
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/BerryWorld.cpp)
endif()
