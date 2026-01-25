# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "src\\app\\CMakeFiles\\FEM-program_autogen.dir\\AutogenUsed.txt"
  "src\\app\\CMakeFiles\\FEM-program_autogen.dir\\ParseCache.txt"
  "src\\app\\FEM-program_autogen"
  )
endif()
