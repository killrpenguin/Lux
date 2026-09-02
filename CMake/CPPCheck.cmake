find_program(CPPCHECK_PATH cppcheck)

if(CPPCHECK_PATH)
    set(COMPILE_COMMANDS_PATH "${CMAKE_BINARY_DIR}/compile_commands.json")
    set(VENDOR_PATH "${CMAKE_SOURCE_DIR}/vendor")

    set(CMAKE_CXX_CPPCHECK 
        ${CPPCHECK_PATH}
        " --enable=warning,performance,portability,information,unusedFunction"
	" -i ${VENDOR_PATH}"
	" --project=${COMPILE_COMMANDS_PATH}"
        " --error-exitcode=1"
	" -j8"
    )
    message(STATUS "Cppcheck found and enabled: ${CPPCHECK_PATH}")
else()
    message(WARNING "Cppcheck not found! Static analysis will be skipped.")
endif()
