#option(ENABLE_SimpleOptimizer "Enable Module SimpleOptimizer" OFF)
#if (ENABLE_SimpleOptimizer)

## SimpleOptimizer is required, so don't give the user an option
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/SimpleOptimizer.cpp)

#endif()
