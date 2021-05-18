## Tells the build system how to build your module
## you usually don't need to worry about this file
## but if you need to compile extra cpp files
## or link special libraries for your module
## then read on!

## the option command makes your module
## togglable in build settings
option(enable_Brain_RNN "Enable Module RNNBrain" OFF)
if (enable_Brain_RNN)

  ## register_module() is a custom MABE function
  ## that registers your module and type with the
  ## rest of the build system
  register_module(Brain RNN)

  ## target_sources is a standard cmake command
  ## to tell the compiler to treat a cpp files
  ## as a unit of compilation.
  ## {EXE} is the executable output (mabe)
  ## CMAKE_CURRENT_LIST_DIR is the locatoin of
  ## your module, so you should always use this
  ## to reference files in your module.
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/RNNBrain.cpp)
  target_sources(${EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/RNNBrain.h)
  ## it will still work if you don't list the .h file, but the file won't show up in some IDE projects

  ## exapmle of a direct header include
  ## PPRINT
  #include_directories(${CMAKE_CURRENT_LIST_DIR}/pprint/include)

  ## example of finding the LAPACK or MKL libraries
  ## on the current computer, showing an error
  ## if not found (required), and linking them
  ## to the final executable.
  ## LAPACK / MKL
  #find_package(LAPACK REQUIRED)
  #include_directories(${LAPACK_INCLUDE_DIRS})
  #target_link_libraries(${EXE} ${LAPACK_LIBRARIES})

  ## example of finding the os-specific threading
  ## library to facilitate multithreading
  ## X-PLATFORM MULTITHREADING
  #find_package(Threads)
  #target_link_libraries(${EXE} ${CMAKE_THREAD_LIBS_INIT})

  ## each library has specific variables that are
  ## set when cmake finds it, so look up
  ## what variables to use for the library you're
  ## looking for.
endif()
