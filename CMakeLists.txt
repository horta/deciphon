cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(
  deciphon
  VERSION 1.0.0
  LANGUAGES C)

include(cmake/compiler-options.cmake)
include(cmake/sanitizers.cmake)
include(cmake/CPM.cmake)

cpmaddpackage("gh:EBI-Metagenomics/h3client@0.8.0")
cpmaddpackage("gh:EBI-Metagenomics/hmmer-reader@0.2.1")
cpmaddpackage("gh:EBI-Metagenomics/imm@2.0.14")
cpmaddpackage("gh:EBI-Metagenomics/lite-pack@0.4.1")

find_package(OpenMP REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_EXTENSIONS OFF)

if(OpenMP_FOUND)
  add_compile_definitions(DECIPHON_OPENM=1)
endif()

add_library(
  deciphon
  lazylog.c
  db/press.c
  db/profile_reader.c
  db/prot_reader.c
  db/prot_writer.c
  db/reader.c
  db/writer.c
  die.c
  dirname.c
  errmsg.c
  is.c
  itoa.c
  rc.c
  zc.c
  fs.c
  xmath.c
  to.c
  expect.c
  model/nuclt_dist.c
  model/profile.c
  model/profile_types.c
  model/prot_codec.c
  model/prot_h3reader.c
  model/prot_model.c
  model/prot_profile.c
  model/prot_state.c
  model/standard_profile.c
  model/standard_state.c
)
add_library(DECIPHON::deciphon ALIAS deciphon)

# Linked libraries
target_link_libraries(deciphon PRIVATE OpenMP::OpenMP_C)
target_link_libraries(deciphon PUBLIC HMR::hmr)
target_link_libraries(deciphon PUBLIC IMM::imm)
target_link_libraries(deciphon PUBLIC LITE_PACK::lite_pack)

# Include
target_include_directories(
  deciphon
  PUBLIC $<INSTALL_INTERFACE:${CMAKE_SOURCE_DIR}>
         $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
         $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_options(deciphon PRIVATE ${WARNING_FLAGS})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  include(CheckIPOSupported)
  check_ipo_supported()
  set_target_properties(deciphon PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

enable_testing()
add_subdirectory(test)
