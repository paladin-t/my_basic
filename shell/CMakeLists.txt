get_filename_component(EXEC_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
set(EXEC_NAME "${PROJECT_NAME}_${EXEC_NAME}")

set(Source_Files
        "main.c"
)
source_group("Source Files" FILES "${Source_Files}")

add_executable("${EXEC_NAME}" "${Source_Files}")
target_link_libraries("${EXEC_NAME}" PRIVATE "core")

set_target_properties(
        "${EXEC_NAME}"
        PROPERTIES
        LINKER_LANGUAGE
        C
)

# install rules

# setup the version numbering
set_property(TARGET "${EXEC_NAME}" PROPERTY VERSION "1.0.0")
set_property(TARGET "${EXEC_NAME}" PROPERTY SOVERSION "1")

set(installable_libs "${EXEC_NAME}" "${PROJECT_NAME}_compiler_flags")

if (TARGET "${DEPENDANT_LIBRARY}")
    list(APPEND installable_libs "${DEPENDANT_LIBRARY}")
endif ()
install(TARGETS ${installable_libs}
        DESTINATION "bin"
        EXPORT "${EXEC_NAME}Targets")
