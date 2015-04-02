#---------------------------------------------------------------
# General Description: PC-Lint/Flexelint tool auto configuration
#---------------------------------------------------------------

#------------------------------------------
# Gimpel Flexelint/PC-Lint detection  
#------------------------------------------

set(LINT_FOUND FALSE)
find_program(LINT_EXECUTABLE_PATH NAMES flint lint-nt flexelint PATHS ${LINT_INSTALL_PATH} ENV PATH)
get_filename_component(LINT_EXECUTABLE ${LINT_EXECUTABLE_PATH} NAME)

if(LINT_EXECUTABLE)
    set(LINT_FOUND TRUE)
    set(LINT_DATA_DIR ${CMAKE_SOURCE_DIR}/lint)

    # a phony target which causes all available *_LINT targets to be executed
    add_custom_target(ALL_LINT)

    macro(lint_check_sources _target_name _sources)

        #    create_folder_if_not_exist("${LINT_DATA_DIR}/${_target_name}")
        get_filename_component(ABS_LINT_DATA_DIR "${LINT_DATA_DIR}" REALPATH)
        file(MAKE_DIRECTORY "${ABS_LINT_DATA_DIR}/${_target_name}")

        set(LINT_CFG_FILES "lint_cmac.h lint_cppmac.h size-options.lnt gcc-include-path.lnt")

        #    message("GCC_BIN=${CMAKE_C_COMPILER}")
        #    message("GXX_BIN=${CMAKE_CXX_COMPILER}")
        #    message("CFLAGS=${CMAKE_C_FLAGS_RELEASE}")
        #    message("CXXFLAGS=${CMAKE_CXX_FLAGS_RELEASE}")

        add_custom_command( OUTPUT ${LINT_CFG_FILES}
            COMMAND "make" GCC_BIN=${CMAKE_C_COMPILER} GXX_BIN=${CMAKE_CXX_COMPILER} CFLAGS=${CMAKE_C_FLAGS_RELEASE} CXXFLAGS=${CMAKE_CXX_FLAGS_RELEASE} -f co-gcc.mak
            WORKING_DIRECTORY ${ABS_LINT_DATA_DIR})


        set(LINT_RULES_DIR "-i\"${ABS_LINT_DATA_DIR}\"")

        GET_DIRECTORY_PROPERTY(_inc_dirs INCLUDE_DIRECTORIES)

        foreach(_one_inc_dir ${_inc_dirs})
            list(APPEND PROJECT_INCLUDE_DIRS "-i\"${_one_inc_dir}\"")
        endforeach(_one_inc_dir)
        
        list(APPEND PROJECT_INCLUDE_DIRS "-i\"${CMAKE_CURRENT_BINARY_DIR}\"")

        set(_all_files_reports)

        foreach(_current_file ${_sources} ${ARGN})

            get_filename_component(_processed_file_name ${_current_file} NAME)

            set(_report_file_name "${_processed_file_name}.rpt")
            set(_report_file_dst ${LINT_DATA_DIR}/${_target_name}/${_report_file_name})

            ADD_CUSTOM_COMMAND( 
                OUTPUT  ${_report_file_dst}
                COMMAND ${LINT_EXECUTABLE} ${LINT_RULES_DIR} co-gcc.lnt project.lnt ${PROJECT_INCLUDE_DIRS} -zero ${_current_file} > ${_report_file_dst}
                DEPENDS ${LINT_CFG_FILES}
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                VERBATIM)

            set(_all_files_reports ${_all_files_reports} ${_report_file_dst})

        endforeach(_current_file)

        ADD_CUSTOM_TARGET(${_target_name}_LINT DEPENDS ${LINT_C_INCLUDE} ${LINT_CXX_INCLUDE} ${LINT_C_INCLUDE_PATH_FILE} ${LINT_CXX_INCLUDE_PATH_FILE} ${LINT_RULES_FILE} ${_all_files_reports})

        # make the ALL_LINT target depend on each and every *_LINT target
        add_dependencies(ALL_LINT ${_target_name}_LINT)

    endmacro(lint_check_sources)

else(LINT_EXECUTABLE)
    #empty macro - lint not found
    macro(lint_check_sources _target_name _sources)
    endmacro(lint_check_sources)
endif(LINT_EXECUTABLE)


