cmake_minimum_required (VERSION 2.9)
project (imweb)

option (BUILD_EXAMPLES "Build all examples." ON)

set (CMAKE_C_STANDARD 11)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
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

if (NOT DEFINED EMSCRIPTEN)
pkg_check_modules(GL REQUIRED gl)
endif ()
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

if (BUILD_EXAMPLES)
    add_executable (example "${PROJECT_SOURCE_DIR}/glad/src/glad.c"
                          "${PROJECT_SOURCE_DIR}/example.cpp")
    add_dependencies (example imgui implot)
    target_include_directories (example PRIVATE "${PROJECT_SOURCE_DIR}/glad/include"
                                              "${IMGUI_SRCDIR}"
                                              "${IMGUI_SRCDIR}/backends")
    target_link_libraries (example imgui implot GL glfw ${CMAKE_DL_LIBS})

    if (DEFINED EMSCRIPTEN)
        set (IMWEB_JS_FILE "example.js")
        configure_file ("${PROJECT_SOURCE_DIR}/imweb.html.in" "${CMAKE_BINARY_DIR}/example.html")

        set (CMAKE_EXECUTABLE_SUFFIX ".js")

        set_target_properties (example PROPERTIES COMPILE_FLAGS "-Os")
        set_target_properties (example PROPERTIES LINK_FLAGS    "-Os -s WASM=0 -s USE_WEBGL2=1 -s USE_GLFW=3 -s FULL_ES3=1 --preload-file ${WEBGUI_SRCDIR}/data/xkcd-script.ttf@/data/xkcd-script.ttf")

        add_custom_target (run)
        add_custom_command (
            TARGET run
            COMMAND BUILD_DIR=${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/run.sh
        )
    else ()
        add_custom_command (TARGET example PRE_BUILD
                            COMMAND ${CMAKE_COMMAND} -E create_symlink
                            ${WEBGUI_SRCDIR}/data $<TARGET_FILE_DIR:example>/data)
        add_custom_target (run)
        add_custom_command (
            TARGET run
            COMMAND cd "${CMAKE_BINARY_DIR}" && ./example
        )
    endif ()

    add_dependencies (run example)
endif ()

message (STATUS "Build all examples: ${BUILD_EXAMPLES}")
