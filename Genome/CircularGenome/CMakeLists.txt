option(ENABLE_Genome_CircularGenome "Enable Module CircularGenome" OFF)
if (ENABLE_Genome_CircularGenome)
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/CircularGenome.cpp)
endif()
