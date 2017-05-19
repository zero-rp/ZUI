project(duktape)
aux_source_directory(. src)
add_library(duktape STATIC ${src})
set_target_properties(duktape PROPERTIES output_name "duktape")
