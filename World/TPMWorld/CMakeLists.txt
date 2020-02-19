option(ENABLE_World_TPMWorld "Enable Module TPMWorld" OFF)
if (ENABLE_World_TPMWorld)
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/TPMWorld.cpp)
endif()
