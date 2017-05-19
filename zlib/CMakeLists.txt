project(zlib)
aux_source_directory(. src)
add_library(zlib STATIC ${src})
set_target_properties(zlib PROPERTIES output_name "zlib")
