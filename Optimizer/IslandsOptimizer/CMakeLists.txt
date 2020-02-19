option(ENABLE_Optimizer_IslandsOptimizer "Enable Module IslandsOptimizer" OFF)
if (ENABLE_Optimizer_IslandsOptimizer)
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/IslandsOptimizer.cpp)
endif()
