cmake_minimum_required (VERSION 2.9)
project (imweb)
find_package(PkgConfig REQUIRED)

if (NOT UNIX)
    message (FATAL_ERROR "Only *nix supported at the moment.")
endif ()

macro (AddImguiBackend backend)
    file (GLOB ${backend}_SRCS "${IMGUI_SRCDIR}/backends/imgui_impl_${backend}.cpp")
    if (${backend}_SRCS)
        message (STATUS "Adding IMGUI backend: ${backend}")
    else ()
        message (FATAL_ERROR "Backend not found: ${backend}")
    endif ()
endmacro ()

pkg_check_modules(GLFW REQUIRED glfw3)

set (IMGUI_SRCDIR "${PROJECT_SOURCE_DIR}/imgui")
set (IMPLOT_SRCDIR "${PROJECT_SOURCE_DIR}/implot")
set (WEBGUI_SRCDIR "${PROJECT_SOURCE_DIR}/webgui")

if (NOT DEFINED EMSCRIPTEN)
AddImguiBackend (glut)
AddImguiBackend (vulkan)
endif ()
AddImguiBackend (glfw)
AddImguiBackend (opengl2)
AddImguiBackend (opengl3)
#AddImguiBackend (sdl2)
#AddImguiBackend (sdl3)
#AddImguiBackend (sdlrenderer)

file (GLOB IMGUI_MAIN_SRC_FILES "${IMGUI_SRCDIR}/*.cpp"
      "${glfw_SRCS}" "${glut_SRCS}"
      "${opengl2_SRCS}" "${opengl3_SRCS}"
      "${sdl2_SRCS}" "${sdl3_SRCS}" "${sdlrenderer_SRCS}"
      "${vulkan_SRCS}")
add_library (imgui STATIC "${IMGUI_MAIN_SRC_FILES}")
target_include_directories (imgui PUBLIC "${IMGUI_SRCDIR}" "${IMGUI_SRCDIR}/backends")
set_property(TARGET imgui PROPERTY POSITION_INDEPENDENT_CODE ON)

file (GLOB IMPLOT_MAIN_SRC_FILES "${IMPLOT_SRCDIR}/*.cpp")
add_library (implot STATIC "${IMPLOT_MAIN_SRC_FILES}")
target_include_directories (implot PRIVATE "${IMGUI_SRCDIR}")
target_include_directories (implot PUBLIC "${IMPLOT_SRCDIR}")
set_property(TARGET implot PROPERTY POSITION_INDEPENDENT_CODE ON)

if (DEFINED EMSCRIPTEN)
    add_executable (imweb "${WEBGUI_SRCDIR}/main.cpp")
    add_dependencies (imweb imgui implot)
    target_include_directories (imweb PRIVATE "${IMGUI_SRCDIR}" "${IMGUI_SRCDIR}/backends")

    set (CMAKE_EXECUTABLE_SUFFIX ".html")
    set (WASM_LDFLAGS "-s WASM=0")

    set_target_properties (imweb PROPERTIES COMPILE_FLAGS "-Os")
    set_target_properties (imweb PROPERTIES LINK_FLAGS    "-Os ${WASM_LDFLAGS} -s USE_WEBGL2=1 -s USE_GLFW=3 -s FULL_ES3=1 --preload-file ${WEBGUI_SRCDIR}/data/xkcd-script.ttf@/data/xkcd-script.ttf")
    set_target_properties (imweb PROPERTIES LINK_LIBRARIES "${CMAKE_BINARY_DIR}/libimgui.a" "${CMAKE_BINARY_DIR}/libimplot.a" "-lglfw3")
endif ()
