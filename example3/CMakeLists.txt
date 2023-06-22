cmake_minimum_required (VERSION 3.0)
project (imweb LANGUAGES CXX)

set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -std=c11")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++17")

find_package (PkgConfig REQUIRED)

pkg_check_modules (IMWEB REQUIRED imweb)

add_executable (example3 main.cpp)
install (TARGETS example3 RUNTIME DESTINATION share/imweb)
target_include_directories (example3 PRIVATE "${IMWEB_INCLUDE_DIRS}")
target_link_libraries(example3 "${pkgcfg_lib_IMWEB_imgui}" "${pkgcfg_lib_IMWEB_implot}" "${pkgcfg_lib_IMWEB_imweb}")
