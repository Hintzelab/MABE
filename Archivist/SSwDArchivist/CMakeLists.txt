option(ENABLE_Archivist_SSwDArchivist "Enable Module SSwDArchivist" OFF)
if (ENABLE_Archivist_SSwDArchivist)
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/SSwDArchivist.cpp)
endif()
