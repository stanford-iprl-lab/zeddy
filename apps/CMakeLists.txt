macro(add_app exec_name)
  add_executable("${exec_name}" "${exec_name}.cc")
  target_link_libraries("${exec_name}" util)
endmacro(add_app)

add_app(zeddyfun)
