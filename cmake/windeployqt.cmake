set(_exe "${CMAKE_INSTALL_PREFIX}/gluefiles.exe")

if(NOT EXISTS "${_exe}")
    file(GLOB_RECURSE FOUND_EXE "${CMAKE_INSTALL_PREFIX}/gluefiles.exe")
    if(FOUND_EXE)
        list(GET FOUND_EXE 0 _exe)
    endif()
endif()

if(EXISTS "${_exe}")
    find_program(WINDEPLOYQT_EXE NAMES windeployqt windeployqt.exe HINTS "C:/Qt/6.11.0/mingw_64/bin")
    if(WINDEPLOYQT_EXE)
        execute_process(
            COMMAND "${WINDEPLOYQT_EXE}" 
                    --release 
                    --compiler-runtime 
                    --no-translations 
                    "${_exe}"
        )
    endif()
endif()