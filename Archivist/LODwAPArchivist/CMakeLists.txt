option(ENABLE_Archivist_LODwAPArchivist "Enable Module LODwAPArchivist" OFF)
if (ENABLE_Archivist_LODwAPArchivist)
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/LODwAPArchivist.cpp)
endif()
