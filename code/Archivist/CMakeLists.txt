target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/DefaultArchivist.cpp)

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_LIST_DIR})
FOREACH(subdir ${SUBDIRS})
  include(${CMAKE_CURRENT_LIST_DIR}/${subdir}/CMakeLists.txt)
ENDFOREACH()
