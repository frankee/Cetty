SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
cxx_executable_current_path(EchoServer cetty)
ADD_DEPENDENCIES(EchoServer cetty)