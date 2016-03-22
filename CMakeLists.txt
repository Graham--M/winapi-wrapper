#=============================================================================
# winapi library
#
# target names: winapi, winapi_examples (not built by default)
#
cmake_minimum_required (VERSION 3.4)

set (WINAPI_PROJECT_NAME winapi)



#-----------------------------------------------------------------------------
# Add the examples as targets to the winapi_examples pseudo-target
set (EXAMPLES_TARGET_NAMES )
add_subdirectory (examples)
add_custom_target("${WINAPI_PROJECT_NAME}_examples")
add_dependencies("${WINAPI_PROJECT_NAME}_examples" ${EXAMPLES_TARGET_NAMES})



#-----------------------------------------------------------------------------
# Create the library target
project (${WINAPI_PROJECT_NAME})
 
set (PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
 
set (PROJECT_SRCS
  ${PROJECT_SOURCE_DIR}/application.cpp
  ${PROJECT_SOURCE_DIR}/comboboxex_getitemtext.cpp
  ${PROJECT_SOURCE_DIR}/combobox_getlbtext.cpp
  ${PROJECT_SOURCE_DIR}/default_mdichild_class.cpp
  ${PROJECT_SOURCE_DIR}/default_window_class.cpp
  ${PROJECT_SOURCE_DIR}/dialog.cpp
  ${PROJECT_SOURCE_DIR}/dialog_enddialog.cpp
  ${PROJECT_SOURCE_DIR}/dialog_go1.cpp
  ${PROJECT_SOURCE_DIR}/dialog_go2.cpp
  ${PROJECT_SOURCE_DIR}/edit_getline.cpp
  ${PROJECT_SOURCE_DIR}/listbox_gettext.cpp
  ${PROJECT_SOURCE_DIR}/listview_getitemtext.cpp
  ${PROJECT_SOURCE_DIR}/mdichild.cpp
  ${PROJECT_SOURCE_DIR}/mdichild_defaultproc.cpp
  ${PROJECT_SOURCE_DIR}/mdiwindow.cpp
  ${PROJECT_SOURCE_DIR}/menu_getstring.cpp
  ${PROJECT_SOURCE_DIR}/modelessdialog_create1.cpp
  ${PROJECT_SOURCE_DIR}/modelessdialog_create2.cpp
  ${PROJECT_SOURCE_DIR}/modelessdialog_create3.cpp
  ${PROJECT_SOURCE_DIR}/regkey_deletekeytree.cpp
  ${PROJECT_SOURCE_DIR}/regkey_enum.cpp
  ${PROJECT_SOURCE_DIR}/regkey_enumkeys.cpp
  ${PROJECT_SOURCE_DIR}/regkey_enumkeys2.cpp
  ${PROJECT_SOURCE_DIR}/regkey_queryvalue.cpp
  ${PROJECT_SOURCE_DIR}/socket.cpp
  ${PROJECT_SOURCE_DIR}/statusbar_gettext.cpp
  ${PROJECT_SOURCE_DIR}/statusbar_gettiptext.cpp
  ${PROJECT_SOURCE_DIR}/tabcontrol_getitemtext.cpp
  ${PROJECT_SOURCE_DIR}/thread.cpp
  ${PROJECT_SOURCE_DIR}/toolbar_getbuttontext.cpp
  ${PROJECT_SOURCE_DIR}/treeitem_getitemtext.cpp
  ${PROJECT_SOURCE_DIR}/window.cpp
  ${PROJECT_SOURCE_DIR}/windowclass_constructor.cpp
  ${PROJECT_SOURCE_DIR}/window_class.cpp
  ${PROJECT_SOURCE_DIR}/window_defaultproc.cpp
  ${PROJECT_SOURCE_DIR}/window_gettext.cpp
  ${PROJECT_SOURCE_DIR}/winmain.cpp
)

add_library (${PROJECT_NAME} STATIC ${PROJECT_SRCS})

target_include_directories ( ${PROJECT_NAME}
  PUBLIC ${PROJECT_INCLUDE_DIR}
  PRIVATE ${PROJECT_SOURCE_DIR}
)

