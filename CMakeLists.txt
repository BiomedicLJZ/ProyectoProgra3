# =============================================================================
#  CMakeLists.txt — NO MODIFICAR
#
#  Tres perfiles (CLion los importa solos desde CMakePresets.json):
#    debug  -> -O0 con símbolos. Para trabajar y depurar.
#    asan   -> debug + AddressSanitizer (+ UBSan en macOS/Linux).
#    exp    -> -O2 (Release). Para medir tiempos y correr la carga grande.
#
#  Dos ejecutables:
#    minidb        src/*.cpp
#    experimentos  experimentos/*.cpp + src/*.cpp excepto main.cpp
#                  (sólo si experimentos/ tiene archivos .cpp; ahí va tu propio main)
#
#  Compiladores soportados: Apple clang (macOS), MSVC (Visual Studio Build
#  Tools, Windows), GCC/clang (Linux).
# =============================================================================
cmake_minimum_required(VERSION 3.21)
project(MiniDB LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(MINIDB_ASAN "Compilar con AddressSanitizer" OFF)

file(GLOB FUENTES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB FUENTES_EXP CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/experimentos/*.cpp)
set(FUENTES_SIN_MAIN ${FUENTES})
list(REMOVE_ITEM FUENTES_SIN_MAIN ${CMAKE_SOURCE_DIR}/src/main.cpp)

set(OBJETIVOS minidb)
add_executable(minidb ${FUENTES})
if(FUENTES_EXP)
    add_executable(experimentos ${FUENTES_EXP} ${FUENTES_SIN_MAIN})
    list(APPEND OBJETIVOS experimentos)
endif()

find_package(Threads REQUIRED)

foreach(t ${OBJETIVOS})
target_include_directories(${t} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${t} PRIVATE Threads::Threads)

# --- Advertencias ------------------------------------------------------------
if(MSVC)
    # /utf-8: los fuentes están en UTF-8 (acentos en comentarios).
    target_compile_options(${t} PRIVATE /W4 /permissive- /utf-8)
    target_compile_definitions(${t} PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(${t} PRIVATE -Wall -Wextra -pedantic)
endif()

# --- AddressSanitizer --------------------------------------------------------
if(MINIDB_ASAN)
    if(MSVC)
        # MSVC: /RTC (runtime checks, activo por defecto en Debug) y el
        # enlazado incremental son incompatibles con ASan.
        foreach(v CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS)
            string(REGEX REPLACE "/RTC[1csu]*" "" ${v} "${${v}}")
        endforeach()
        target_compile_options(${t} PRIVATE /fsanitize=address /Zi)
        target_link_options(${t} PRIVATE /INCREMENTAL:NO /DEBUG)

        # Fuera de Visual Studio, la DLL de ASan debe estar junto al .exe o en
        # PATH. Vive en la misma carpeta que cl.exe; se copia al compilar.
        get_filename_component(DIR_CL "${CMAKE_CXX_COMPILER}" DIRECTORY)
        file(GLOB DLLS_ASAN "${DIR_CL}/clang_rt.asan*_dynamic-*.dll")
        if(DLLS_ASAN)
            foreach(dll ${DLLS_ASAN})
                add_custom_command(TARGET ${t} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${dll}" "$<TARGET_FILE_DIR:${t}>")
            endforeach()
        else()
            message(WARNING
                "No encontre la DLL de ASan junto a cl.exe (${DIR_CL}). Instala el componente "
                "'C++ AddressSanitizer' con el Visual Studio Installer.")
        endif()
    else()
        set(SAN -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_compile_options(${t} PRIVATE ${SAN})
        target_link_options(${t} PRIVATE ${SAN})
    endif()
endif()

endforeach()
