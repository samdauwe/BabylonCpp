# ============================================================================ #
#                             Cmake helper functions                           #
# ============================================================================ #

# Set policy if policy is available
function(set_policy POL VAL)
    if(POLICY ${POL})
        cmake_policy(SET ${POL} ${VAL})
    endif()
endfunction(set_policy)


# Define function "source_group_by_path with three mandatory arguments
# (PARENT_PATH, REGEX, GROUP, ...)
# to group source files in folders (e.g. for MSVC solutions).
#
# Example:
# source_group_by_path("${CMAKE_CURRENT_SOURCE_DIR}/src"
# "\\\\.h$|\\\\.hpp$|\\\\.cpp$|\\\\.c$|\\\\.ui$|\\\\.qrc$" "Source Files" ${sources})
function(source_group_by_path PARENT_PATH REGEX GROUP)
    foreach (FILENAME ${ARGN})
        get_filename_component(FILEPATH "${FILENAME}" REALPATH)
        file(RELATIVE_PATH FILEPATH ${PARENT_PATH} ${FILEPATH})
        get_filename_component(FILEPATH "${FILEPATH}" DIRECTORY)
        string(REPLACE "/" "\\" FILEPATH "${FILEPATH}")
	      source_group("${GROUP}\\${FILEPATH}" REGULAR_EXPRESSION "${REGEX}" FILES ${FILENAME})
    endforeach()
endfunction(source_group_by_path)

function(source_group_by_path_all PARENT_PATH)
    foreach (FILENAME ${ARGN})
        get_filename_component(FILEPATH "${FILENAME}" REALPATH)
        file(RELATIVE_PATH FILEPATH ${PARENT_PATH} ${FILEPATH})
        get_filename_component(FILEPATH "${FILEPATH}" DIRECTORY)
        string(REPLACE "/" "\\" FILEPATH "${FILEPATH}")
    source_group("${FILEPATH}" FILES ${FILENAME})
endforeach()

endfunction(source_group_by_path_all)


# Function that extract entries matching a given regex from a list.
# ${OUTPUT} will store the list of matching filenames.
function(list_extract OUTPUT REGEX)
    foreach(FILENAME ${ARGN})
        if(${FILENAME} MATCHES "${REGEX}")
            list(APPEND ${OUTPUT} ${FILENAME})
        endif()
    endforeach()
    set(${OUTPUT} ${${OUTPUT}} PARENT_SCOPE)
endfunction(list_extract)

# Display macro
macro(display test desc summary)
  if (${test})
    set(${summary} ${desc})
  else ()
    set(${summary} no)
  endif()
endmacro()

# Appends `str` to the variable named `var` with a whitespace as separator.
# Suppresses a leading whitespace if the variable is empty and does nothing if
# `str` is empty.
function(build_string var str)
  if(NOT str STREQUAL "")
    if("${${var}}" STREQUAL "")
      set("${var}" "${str}" PARENT_SCOPE)
    else()
      set("${var}" "${${var}} ${str}" PARENT_SCOPE)
    endif()
  endif()
endfunction(build_string)

# Copy single files
macro(copy_resource_files files)
    foreach(file ${files})
        message(STATUS "Copying resource ${file}")
        file(COPY ${file} DESTINATION ${ASSETS_OUTPUT_PATH})
    endforeach()
endmacro()

# Copy full directories
macro(copy_resource_dirs dirs)
    foreach(dir ${dirs})
        # Replace / at the end of the path (copy dir content VS copy dir)
        string(REGEX REPLACE "/+$" "" dirclean "${dir}")
        message(STATUS "Copying resource ${dirclean} to ${ASSETS_OUTPUT_PATH}")
        file(COPY ${dirclean} DESTINATION ${ASSETS_OUTPUT_PATH})
    endforeach()
endmacro()