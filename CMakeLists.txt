cmake_minimum_required (VERSION 3.0)
project (imweb LANGUAGES C CXX
         HOMEPAGE_URL https://github.com/utoni/imweb
         DESCRIPTION "Dear ImGui + ImPlot + WebGui"
         VERSION 0.0.1)

option (SINGLE_LIBRARY "Bundle all libraries into one." ON)
option (BUILD_EXAMPLES "Build all examples." ON)
option (ENABLE_LTO "Enable link-time optimizations." OFF)
option (ENABLE_SANITIZER "Enable ASAN/LSAN/UBSAN." OFF)

set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -std=c11")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++17")
include_directories (BEFORE SYSTEM "${PROJECT_SOURCE_DIR}")
find_package (PkgConfig REQUIRED)

if (ENABLE_SANITIZER)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og -g3 -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined -fno-sanitize=alignment -fsanitize=enum -fsanitize=leak")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og -g3 -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined -fno-sanitize=alignment -fsanitize=enum -fsanitize=leak")
    link_libraries (asan ubsan)
endif ()

if (NOT UNIX)
    message (FATAL_ERROR "Only *nix supported at the moment.")
endif ()

if (ENABLE_LTO)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -flto -ffunction-sections -fdata-sections -Wl,--gc-sections")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto -ffunction-sections -fdata-sections -Wl,--gc-sections")
endif ()

macro (AddImguiBackend backend)
    file (GLOB ${backend}_SRCS "${IMGUI_SRCDIR}/backends/imgui_impl_${backend}.cpp")
    if (${backend}_SRCS)
        message (STATUS "Adding IMGUI backend: ${backend}")
    else ()
        message (FATAL_ERROR "Backend not found: ${backend}")
    endif ()
endmacro ()

macro (BuildExample name)
    add_executable (${name} "${PROJECT_SOURCE_DIR}/${name}.cpp")
    target_include_directories (${name} PRIVATE "${IMPLOT_SRCDIR}"
                                                "${IMGUI_SRCDIR}"
                                                "${IMGUI_SRCDIR}/backends")
    if (SINGLE_LIBRARY)
        target_link_libraries (${name} imweb GL glfw X11 ${CMAKE_DL_LIBS})
        add_dependencies (${name} imweb)
    else (NOT SINGLE_LIBRARY)
        target_link_libraries (${name} imweb imgui implot GL glfw X11 ${CMAKE_DL_LIBS})
        add_dependencies (${name} imweb imgui implot)
    endif ()

    if (DEFINED EMSCRIPTEN)
        set (IMWEB_JS_FILE "${name}.js")
        configure_file ("${PROJECT_SOURCE_DIR}/imweb.html.in" "${CMAKE_BINARY_DIR}/${name}.html")

        set (CMAKE_EXECUTABLE_SUFFIX ".js")

        set_target_properties (${name} PROPERTIES COMPILE_FLAGS "-Os")
        set_target_properties (${name} PROPERTIES LINK_FLAGS    "-Os -s WASM=0 -s USE_WEBGL2=1 -s USE_GLFW=3 -s FULL_ES3=1 --preload-file ${WEBGUI_SRCDIR}/data/xkcd-script.ttf@/data/xkcd-script.ttf")

        add_custom_target (run-${name})
        add_custom_command (
            TARGET run-${name}
            COMMAND BUILD_DIR=${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/run.sh ${name}
        )
    else ()
        add_custom_command (TARGET ${name} PRE_BUILD
                            COMMAND ${CMAKE_COMMAND} -E create_symlink
                            ${WEBGUI_SRCDIR}/data $<TARGET_FILE_DIR:example>/data)
        add_custom_target (run-${name})
        add_custom_command (
            TARGET run-${name}
            COMMAND cd "${CMAKE_BINARY_DIR}" && ./${name}
        )
    endif ()

    set_target_properties(${name} PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)

    add_dependencies (run-${name} ${name})
endmacro ()

set (pc_libs)
set (pc_libs_private)
set (pc_req_private)
set (pc_req_public)

if (NOT DEFINED EMSCRIPTEN)
    pkg_check_modules (GL REQUIRED gl)
endif ()
pkg_check_modules (GLFW REQUIRED glfw3)

set (IMGUI_SRCDIR "${PROJECT_SOURCE_DIR}/imgui")
set (IMPLOT_SRCDIR "${PROJECT_SOURCE_DIR}/implot")
set (WEBGUI_SRCDIR "${PROJECT_SOURCE_DIR}/webgui")

#if (NOT DEFINED EMSCRIPTEN)
    #AddImguiBackend (glut)
    #AddImguiBackend (vulkan)
#endif ()
AddImguiBackend (glfw)
#AddImguiBackend (opengl2)
AddImguiBackend (opengl3)
#AddImguiBackend (sdl2)
#AddImguiBackend (sdl3)
#AddImguiBackend (sdlrenderer)

file (GLOB IMGUI_MAIN_SRC_FILES "${IMGUI_SRCDIR}/*.cpp"
      "${glfw_SRCS}" "${glut_SRCS}"
      "${opengl2_SRCS}" "${opengl3_SRCS}"
      "${sdl2_SRCS}" "${sdl3_SRCS}" "${sdlrenderer_SRCS}"
      "${vulkan_SRCS}")
list (FILTER IMGUI_MAIN_SRC_FILES EXCLUDE REGEX "imgui_demo\\.cpp")
file (GLOB IMPLOT_MAIN_SRC_FILES "${IMPLOT_SRCDIR}/*.cpp")
list (FILTER IMPLOT_MAIN_SRC_FILES EXCLUDE REGEX "implot_demo\\.cpp")
file (GLOB IMWEB_MAIN_SRC_FILES "${PROJECT_SOURCE_DIR}/*.cpp")
list (FILTER IMWEB_MAIN_SRC_FILES EXCLUDE REGEX "example.*cpp$")
if (DEFINED EMSCRIPTEN)
    list (FILTER IMWEB_MAIN_SRC_FILES EXCLUDE REGEX "imweb_native_.*cpp$")
else (NOT DEFINED EMSCRIPTEN)
    list (FILTER IMWEB_MAIN_SRC_FILES EXCLUDE REGEX "imweb_emscripten_.*cpp$")
    set (IMWEB_MAIN_SRC_FILES "${IMWEB_MAIN_SRC_FILES}" "${PROJECT_SOURCE_DIR}/glad/src/glad.c")
    set (IMWEB_GLAD_INCLUDE "${PROJECT_SOURCE_DIR}/glad/include")
endif ()

if (NOT SINGLE_LIBRARY)

    add_library (imgui STATIC "${IMGUI_MAIN_SRC_FILES}")
    target_include_directories (imgui PRIVATE "${IMGUI_SRCDIR}" "${IMGUI_SRCDIR}/backends")
    set_property(TARGET imgui PROPERTY POSITION_INDEPENDENT_CODE ON)

    add_library (implot STATIC "${IMPLOT_MAIN_SRC_FILES}")
    target_include_directories (implot PRIVATE "${IMGUI_SRCDIR}" "${IMPLOT_SRCDIR}")
    set_property(TARGET implot PROPERTY POSITION_INDEPENDENT_CODE ON)

    add_library (imweb STATIC "${IMWEB_MAIN_SRC_FILES}")
    target_include_directories (imweb PRIVATE "${IMWEB_GLAD_INCLUDE}" "${IMGUI_SRCDIR}" "${IMGUI_SRCDIR}/backends" "${IMPLOT_SRCDIR}")
    set_property (TARGET imweb PROPERTY POSITION_INDEPENDENT_CODE ON)

    set_target_properties(imgui PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)
    set_target_properties(implot PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)

    set_target_properties (imgui PROPERTIES PUBLIC_HEADER "${IMGUI_SRCDIR}/imgui.h;${IMGUI_SRCDIR}/imconfig.h")
    set_target_properties (implot PROPERTIES PUBLIC_HEADER "${IMPLOT_SRCDIR}/implot.h")
    set_target_properties (imweb PROPERTIES PUBLIC_HEADER "${PROJECT_SOURCE_DIR}/imweb.hpp;${PROJECT_SOURCE_DIR}/imweb_emscripten_glfw.hpp;${PROJECT_SOURCE_DIR}/imweb_native_glfw.hpp")

    set (pc_libs "-limgui -limplot -limweb")

    install (TARGETS imgui implot imweb
             ARCHIVE DESTINATION lib/imweb
             LIBRARY DESTINATION lib/imweb
             PUBLIC_HEADER DESTINATION include/imweb)

else (SINGLE_LIBRARY)

    add_library (imweb STATIC "${IMWEB_MAIN_SRC_FILES}" "${IMGUI_MAIN_SRC_FILES}" "${IMPLOT_MAIN_SRC_FILES}")
    target_include_directories (imweb PRIVATE "${IMWEB_GLAD_INCLUDE}" "${IMGUI_SRCDIR}" "${IMGUI_SRCDIR}/backends" "${IMPLOT_SRCDIR}")
    set_property (TARGET imweb PROPERTY POSITION_INDEPENDENT_CODE ON)
    set_target_properties (imweb PROPERTIES PUBLIC_HEADER "some.h;another.h")

    set_target_properties (imweb PROPERTIES PUBLIC_HEADER "${IMGUI_SRCDIR}/imgui.h;${IMGUI_SRCDIR}/imconfig.h;${IMPLOT_SRCDIR}/implot.h;${PROJECT_SOURCE_DIR}/imweb.hpp;${PROJECT_SOURCE_DIR}/imweb_emscripten_glfw.hpp;${PROJECT_SOURCE_DIR}/imweb_native_glfw.hpp")

    set (pc_libs "-limweb")

    install (TARGETS imweb
             ARCHIVE DESTINATION lib/imweb
             LIBRARY DESTINATION lib/imweb
             PUBLIC_HEADER DESTINATION include/imweb)

endif ()

set_target_properties(imweb PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
         DESTINATION lib/pkgconfig)

if (BUILD_EXAMPLES)
    BuildExample ("example")
    BuildExample ("example2")
endif ()

configure_file (imweb.pc.in imweb.pc @ONLY) 

if (DEFINED EMSCRIPTEN)
set (EMSCRIPTEN_DEFINED "yes")
else (NOT DEFINED EMSCRIPTEN)
set (EMSCRIPTEN_DEFINED "no")
endif ()
message (STATUS "CMAKE_C_FLAGS.............: ${CMAKE_C_FLAGS}")
message (STATUS "CMAKE_CXX_FLAGS...........: ${CMAKE_CXX_FLAGS}")
message (STATUS "Emscripten................: ${EMSCRIPTEN_DEFINED}")
message (STATUS "Bundle into single library: ${SINGLE_LIBRARY}")
message (STATUS "Build all examples........: ${BUILD_EXAMPLES}")
message (STATUS "Link-time optimization....: ${ENABLE_LTO}")
message (STATUS "Enable ASAN/LSAN/UBSAN....: ${ENABLE_SANITIZER}")
