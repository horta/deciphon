cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(
  deciphon
  VERSION 0.5.0
  LANGUAGES C)

include(cmake/warnings.cmake)
include(cmake/sanitizers.cmake)
include(cmake/CPM.cmake)
include(cmake/code-coverage.cmake)

cpmaddpackage("gh:EBI-Metagenomics/hmmer-reader@0.2.1")
cpmaddpackage("gh:EBI-Metagenomics/imm@2.0.14")
cpmaddpackage("gh:EBI-Metagenomics/lite-pack@0.4.1")

# find_package(OpenMP REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_EXTENSIONS OFF)

# if(OpenMP_FOUND)
#   add_compile_definitions(DECIPHON_OPENM=1)
# endif()

add_library(
  deciphon
  db_prof_reader.c
  db_prot_reader.c
  db_prot_writer.c
  db_reader.c
  db_writer.c
  expect.c
  fs.c
  model_nuclt_dist.c
  model_prof.c
  model_prof_types.c
  model_prot_codec.c
  model_prot_h3reader.c
  model_prot_model.c
  model_prot_prof.c
  model_prot_state.c
  model_std_prof.c
  model_std_state.c
  partition_size.c
  strlcpy.c
  u16toa.c
)
add_library(DECIPHON::deciphon ALIAS deciphon)
target_code_coverage(deciphon)

# Linked libraries
# target_link_libraries(deciphon PRIVATE OpenMP::OpenMP_C)
target_link_libraries(deciphon PUBLIC HMR::hmr)
target_link_libraries(deciphon PUBLIC IMM::imm)
target_link_libraries(deciphon PUBLIC LITE_PACK::lite_pack)

# Include
target_include_directories(
  deciphon
  PUBLIC ${CMAKE_SOURCE_DIR}
  PRIVATE ${CMAKE_SOURCE_DIR})

target_compile_options(deciphon PRIVATE ${WARNING_FLAGS})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  include(CheckIPOSupported)
  check_ipo_supported()
  set_target_properties(deciphon PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

add_subdirectory(press)

add_code_coverage_all_targets()
enable_testing()
add_subdirectory(test)
