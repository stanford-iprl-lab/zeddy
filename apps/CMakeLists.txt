macro(add_app exec_name)
  add_executable("${exec_name}" "${exec_name}.cc")
  target_link_libraries("${exec_name}" video)
  target_link_libraries("${exec_name}" util)
  target_link_libraries("${exec_name}" ${X264_LDFLAGS})
  target_link_libraries("${exec_name}" ${X264_LDFLAGS_OTHER})
endmacro(add_app)

add_app(zeddyfun)
