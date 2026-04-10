if(NOT DEFINED ARTIFACT)
  message(FATAL_ERROR "ARTIFACT is required")
endif()

if(NOT DEFINED OUTPUT_DIR)
  message(FATAL_ERROR "OUTPUT_DIR is required")
endif()

if(NOT DEFINED PYTHON_EXECUTABLE)
  message(FATAL_ERROR "PYTHON_EXECUTABLE is required")
endif()

if(NOT DEFINED CHECK_SCRIPT)
  message(FATAL_ERROR "CHECK_SCRIPT is required")
endif()

if(NOT DEFINED NEEDLES_FILE)
  message(FATAL_ERROR "NEEDLES_FILE is required")
endif()

file(MAKE_DIRECTORY "${OUTPUT_DIR}")

set(RAW_REPORT "${OUTPUT_DIR}/runtime_obf_basic_example.raw.txt")
set(DISASM_REPORT "${OUTPUT_DIR}/runtime_obf_basic_example.disasm.txt")

execute_process(
  COMMAND dumpbin /rawdata "${ARTIFACT}"
  OUTPUT_FILE "${RAW_REPORT}"
  RESULT_VARIABLE dumpbin_raw_result)

if(NOT dumpbin_raw_result EQUAL 0)
  message(FATAL_ERROR "dumpbin /rawdata failed with exit code ${dumpbin_raw_result}")
endif()

execute_process(
  COMMAND dumpbin /disasm "${ARTIFACT}"
  OUTPUT_FILE "${DISASM_REPORT}"
  RESULT_VARIABLE dumpbin_disasm_result)

if(NOT dumpbin_disasm_result EQUAL 0)
  message(FATAL_ERROR "dumpbin /disasm failed with exit code ${dumpbin_disasm_result}")
endif()

file(STRINGS "${NEEDLES_FILE}" NEEDLES_LIST ENCODING UTF-8)

execute_process(
  COMMAND
    "${PYTHON_EXECUTABLE}"
    "${CHECK_SCRIPT}"
    "${ARTIFACT}"
    --text-report "${RAW_REPORT}"
    --text-report "${DISASM_REPORT}"
    ${NEEDLES_LIST}
  RESULT_VARIABLE scan_result
  COMMAND_ECHO STDOUT)

if(NOT scan_result EQUAL 0)
  message(FATAL_ERROR "Plaintext verification failed with exit code ${scan_result}")
endif()
