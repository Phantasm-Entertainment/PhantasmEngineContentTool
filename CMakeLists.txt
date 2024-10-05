cmake_minimum_required (VERSION 3.25)
project(PhantasmEngineContentTool VERSION 1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(ZLIB 1.3.1 REQUIRED)
find_package(PNG 1.6.44 REQUIRED)
find_package(wxWidgets 3.2.6 REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})
find_package(Freetype 2.13.3 REQUIRED)

set(PECT_SRC_FILES
    src/App.cpp
    src/AtlasPage.cpp
    src/ContentFile.cpp
    src/ContentLoader.cpp
    src/MainUI.cpp
    src/PageTexture.cpp
)

if(NOT DEFINED BROTLI_INCLUDE_DIRS OR NOT DEFINED BROTLI_LIBRARIES)
    message(FATAL_ERROR "BROTLI_INCLUDE_DIRS and BROTLI_LIBRARIES need to be defined")
endif()

if(WIN32)
    add_executable(PECT WIN32 ${PECT_SRC_FILES})
else()
    add_executable(PECT ${PECT_SRC_FILES})
endif()
target_include_directories(PECT PRIVATE include ${BROTLI_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS} ${PNG_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})
target_link_libraries(PECT ${BROTLI_LIBRARIES} ${FREETYPE_LIBRARIES} ${PNG_LIBRARIES} ${ZLIB_LIBRARIES} ${wxWidgets_LIBRARIES})
target_compile_definitions(PECT PRIVATE ${PNG_DEFINITIONS} UNICODE _UNICODE)
