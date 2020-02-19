option(ENABLE_Optimizer_LexicaseOptimizer "Enable Module LexicaseOptimizer" OFF)
if (ENABLE_Optimizer_LexicaseOptimizer)
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/LexicaseOptimizer.cpp)
endif()
