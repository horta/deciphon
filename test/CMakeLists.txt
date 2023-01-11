include(helper.cmake)

set(testdir "${CMAKE_CURRENT_BINARY_DIR}")
set(tmpdir "${CMAKE_CURRENT_BINARY_DIR}/tmp")
file(MAKE_DIRECTORY "${tmpdir}")

add_compile_definitions(TESTDIR="${testdir}")
add_compile_definitions(TMPDIR="${tmpdir}")

deciphon_add_test(standard_profile DECIPHON::deciphon)
# deciphon_add_test(protein_model DECIPHON::deciphon)
# deciphon_add_test(protein_profile DECIPHON::deciphon)
# deciphon_add_test(protein_db DECIPHON::deciphon)
# deciphon_add_test(prodfile_reader DECIPHON::deciphon)
