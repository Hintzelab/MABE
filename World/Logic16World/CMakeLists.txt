option(ENABLE_World_Logic16World "Enable Module Logic16World" OFF)
if (ENABLE_World_Logic16World)
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Logic16World.cpp)
endif()
