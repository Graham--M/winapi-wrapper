#=============================================================================
# winapi examples
#
cmake_minimum_required (VERSION 3.4)
project (examples)

# build a list of target names so they can be returned to the parent makelist
set (EXAMPLES_TARGET_NAMES_LOCAL )



#-----------------------------------------------------------------------------
# macro: add_example_executable - avoid writing common commands over and over

macro (add_example_executable name srcs include_dirs libs)
  
  set (EXAMPLES_TARGET_NAMES_LOCAL ${EXAMPLES_TARGET_NAMES_LOCAL} ${name} )
  add_executable (${name} WIN32 EXCLUDE_FROM_ALL ${srcs})
  target_include_directories (${name} PRIVATE
    ${include_dirs}
    $<TARGET_PROPERTY:${WINAPI_PROJECT_NAME},INTERFACE_INCLUDE_DIRECTORIES>
  )
  target_link_libraries (${name} ${WINAPI_PROJECT_NAME} ${libs})
  
endmacro (add_example_executable)



#-----------------------------------------------------------------------------
# EXAMPLE1 sdi
set (EXAMPLE1_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set (EXAMPLE1_SRCS ${EXAMPLE1_SOURCE_DIR}/sdi.cpp)
add_example_executable (sdi ${EXAMPLE1_SRCS} ${EXAMPLE1_SOURCE_DIR} "")



#-----------------------------------------------------------------------------
# EXAMPLE2 mdi - multiple document interface
set (EXAMPLE2_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set (EXAMPLE2_SRCS ${EXAMPLE2_SOURCE_DIR}/mdi.cpp)
add_example_executable (mdi ${EXAMPLE2_SRCS} ${EXAMPLE2_SOURCE_DIR} "")



#-----------------------------------------------------------------------------
# EXAMPLE3 dlg_modal - modal dialog
set (EXAMPLE3_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dialogs)

set (EXAMPLE3_SRCS
  ${EXAMPLE3_SOURCE_DIR}/dlg_modal.cpp
  ${EXAMPLE3_SOURCE_DIR}/dialog.rc
)

add_example_executable (dlg_modal "${EXAMPLE3_SRCS}" ${EXAMPLE3_SOURCE_DIR} "")



#-----------------------------------------------------------------------------
# EXAMPLE4 dlg_modeless - modeless dialog
set (EXAMPLE4_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dialogs)

set (EXAMPLE4_SRCS
  ${EXAMPLE4_SOURCE_DIR}/dlg_modeless.cpp
  ${EXAMPLE4_SOURCE_DIR}/dialog.rc
)

add_example_executable (dlg_modeless "${EXAMPLE4_SRCS}" ${EXAMPLE4_SOURCE_DIR} "")



#-----------------------------------------------------------------------------
# EXAMPLE5 multithreading
set (EXAMPLE5_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/multithreading)

set (EXAMPLE5_SRCS
  ${EXAMPLE5_SOURCE_DIR}/multithreading.cpp
  ${EXAMPLE5_SOURCE_DIR}/mtdialog.rc
)

add_example_executable (
  multithreading "${EXAMPLE5_SRCS}" ${EXAMPLE5_SOURCE_DIR} comctl32
)



#-----------------------------------------------------------------------------
# Return target names to parent make list
set (EXAMPLES_TARGET_NAMES ${EXAMPLES_TARGET_NAMES_LOCAL} PARENT_SCOPE)
