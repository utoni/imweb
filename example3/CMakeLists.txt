cmake_minimum_required (VERSION 3.0)
project (imweb LANGUAGES CXX)
find_package (PkgConfig REQUIRED)

pkg_check_modules (IMWEB REQUIRED imweb)

add_executable (example3 main.cpp)
target_include_directories (example3 PRIVATE "${IMWEB_INCLUDE_DIRS}")
target_link_libraries(example3 "${pkgcfg_lib_IMWEB_imgui}" "${pkgcfg_lib_IMWEB_implot}" "${pkgcfg_lib_IMWEB_imweb}")
