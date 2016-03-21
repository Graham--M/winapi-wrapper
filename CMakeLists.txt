#-----------------------------------------------------------------------------
# setup

cmake_minimum_required (VERSION 3.4)

# Don't need backwards compatable lib path inheritance
cmake_policy (SET CMP0003 NEW)


#-----------------------------------------------------------------------------
# target: winapi lib

add_library (winapi STATIC
  application.cpp
  comboboxex_getitemtext.cpp
  combobox_getlbtext.cpp
  default_mdichild_class.cpp
  default_window_class.cpp
  dialog.cpp
  dialog_enddialog.cpp
  dialog_go1.cpp
  dialog_go2.cpp
  edit_getline.cpp
  listbox_gettext.cpp
  listview_getitemtext.cpp
  mdichild.cpp
  mdichild_defaultproc.cpp
  mdiwindow.cpp
  menu_getstring.cpp
  modelessdialog_create1.cpp
  modelessdialog_create2.cpp
  modelessdialog_create3.cpp
  regkey_deletekeytree.cpp
  regkey_enum.cpp
  regkey_enumkeys.cpp
  regkey_enumkeys2.cpp
  regkey_queryvalue.cpp
  socket.cpp
  statusbar_gettext.cpp
  statusbar_gettiptext.cpp
  tabcontrol_getitemtext.cpp
  thread.cpp
  toolbar_getbuttontext.cpp
  treeitem_getitemtext.cpp
  window.cpp
  windowclass_constructor.cpp
  window_class.cpp
  window_defaultproc.cpp
  window_gettext.cpp
  winmain.cpp
)


#-----------------------------------------------------------------------------
# target: sdi - single window example
add_executable (sdi WIN32 Examples/sdi.cpp)
target_include_directories (sdi PRIVATE . )
target_link_libraries (sdi winapi)


#-----------------------------------------------------------------------------
# target: mdi - multiple document interface example
add_executable (mdi WIN32 Examples/mdi.cpp)
target_include_directories (mdi PRIVATE .)
target_link_libraries (mdi winapi)


#-----------------------------------------------------------------------------
# target: dlg_modal - modal dialog example
add_executable (dlg_modal WIN32
 Examples/Dialog/dlg_modal.cpp
 Examples/Dialog/dialog.rc
)
target_include_directories (dlg_modal PRIVATE . Examples/Dialog/)
target_link_libraries (dlg_modal winapi)


#-----------------------------------------------------------------------------
# target: dlg_modeless - modeless dialog example
add_executable (dlg_modeless WIN32
 Examples/Dialog/dlg_modeless.cpp
 Examples/Dialog/dialog.rc
)
target_include_directories (dlg_modeless PRIVATE . Examples/Dialog/)
target_link_libraries (dlg_modeless winapi)


#-----------------------------------------------------------------------------
# target: multithreading example
add_executable (multithreading WIN32
 Examples/Multithreading/Multithreading.cpp
 Examples/Multithreading/Multithreading_.rc
)
target_include_directories (multithreading PRIVATE . Examples/Multithreading)
target_link_libraries (multithreading winapi comctl32)


#-----------------------------------------------------------------------------
# Set output dir for examples
set_target_properties (
  sdi mdi dlg_modal dlg_modeless multithreading
 PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Examples
)
