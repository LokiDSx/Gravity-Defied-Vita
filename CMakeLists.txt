cmake_minimum_required(VERSION 3.24)
project(gravity_defied_cpp C CXX)

unset(ENV{PKG_CONFIG_PATH})

if(VITA)
    include("${VITASDK}/share/vita.cmake" REQUIRED)
endif()

if(WIN32 AND NOT VITA)
    include(cmake/mingw-sdl2.cmake)
    download_mingw_sdl2(
        "2.26.1"    # SDL2
        "2.20.1"    # SDL2_ttf
        "2.6.2"     # SDL2_image
    )
endif()

if(VITA)
    find_package(SDL2 REQUIRED CONFIG)
    find_package(SDL2_ttf REQUIRED CONFIG)
    find_package(SDL2_image REQUIRED CONFIG)
else()
    find_package(PkgConfig REQUIRED)

    pkg_search_module(SDL2 REQUIRED IMPORTED_TARGET sdl2)
    pkg_search_module(SDL2_TTF REQUIRED IMPORTED_TARGET SDL2_ttf)
    pkg_search_module(SDL2_IMAGE REQUIRED IMPORTED_TARGET SDL2_image)
endif()

# Create a sources variable with a link to all cpp files to compile
set(SOURCES
    src/utils/Time.cpp
    src/utils/EmbedFileStream.cpp
    src/main.cpp
    src/MathF16.cpp
    src/GameCanvas.cpp
    src/GamePhysics.cpp
    src/TimerOrMotoPartOrMenuElem.cpp
    src/class_10.cpp
    src/GameLevel.cpp
    src/LevelLoader.cpp
    src/Micro.cpp
    src/TextRender.cpp
    src/GameMenu.cpp
    src/SettingsStringRender.cpp
    src/MenuManager.cpp
    src/ModsManager.cpp
    src/Theme.cpp
    src/RecordManager.cpp
    src/Timer.cpp
    src/lcdui/CanvasImpl.cpp
    src/lcdui/Canvas.cpp
    src/lcdui/Graphics.cpp
    src/lcdui/Image.cpp
    src/lcdui/Font.cpp
    src/lcdui/Command.cpp
    src/lcdui/FontStorage.cpp
    src/rms/RecordEnumerationImpl.cpp
    src/rms/RecordStore.cpp
)

include(cmake/CMakeRC.cmake)

cmrc_add_resource_library(Assets
    NAMESPACE assets
    ALIAS Assets::Assets
    WHENCE assets
    assets/bluearm.png
    assets/helmet.png
    assets/sprites.png
    assets/blueleg.png
    assets/bluebody.png
    assets/logo.png
    assets/fender.png
    assets/levels.mrg
    assets/engine.png
    assets/splash.png
    assets/raster.png
    assets/FontSansSerif.ttf
)

# Compile resources when building for Windows.
if(WIN32 AND NOT VITA)
    enable_language(RC)
    list(APPEND SOURCES winres/res.rc)
endif()

add_executable(GravityDefied ${SOURCES})

target_compile_features(GravityDefied PRIVATE cxx_std_17)
target_compile_options(GravityDefied PRIVATE -Wall -Wextra)

if(VITA)
    target_compile_definitions(GravityDefied PRIVATE VITA_PORT)
endif()

target_include_directories(GravityDefied PRIVATE
                           ${SDL2_INCLUDE_DIRS}
                           ${SDL2_TTF_INCLUDE_DIRS}
                           ${SDL2_IMAGE_INCLUDE_DIRS})

target_link_directories(GravityDefied PRIVATE
                        ${SDL2_LIBRARY_DIRS}
                        ${SDL2_TTF_LIBRARY_DIRS}
                        ${SDL2_IMAGE_LIBRARY_DIRS})

if(NOT VITA)
    set_target_properties(GravityDefied PROPERTIES LINK_FLAGS_RELEASE -s)
endif()

if(VITA)
    # Static libraries on Vita need to be wrapped in a group: SDL2_image needs
    # libpng/zlib, SDL2_ttf needs freetype/bzip2/zlib in no particular order.
    target_link_libraries(GravityDefied
                          -Wl,--start-group
                          SDL2_image::SDL2_image-static
                          PNG::PNG
                          SDL2_ttf::SDL2_ttf-static
                          Freetype::Freetype
                          SDL2::SDL2
                          Assets::Assets
                          z
                          bz2
                          -Wl,--end-group)
elseif(WIN32)
    target_link_options(GravityDefied PRIVATE -static -mwindows)
    target_link_libraries(GravityDefied 
                         ${SDL2_STATIC_LIBRARIES} 
                         ${SDL2_TTF_STATIC_LIBRARIES}
                         ${SDL2_IMAGE_STATIC_LIBRARIES} 
                         Assets::Assets)
else()
    target_link_libraries(GravityDefied 
                          ${SDL2_LIBRARIES}
                          ${SDL2_TTF_LIBRARIES}
                          ${SDL2_IMAGE_LIBRARIES}
                          Assets::Assets)
endif()

if(VITA)
    vita_create_self(eboot.bin GravityDefied)
    vita_create_vpk(GravityDefied.vpk GDIF00001 eboot.bin
        VERSION 01.05
        NAME "Gravity Defied"
        FILE sce_sys/icon0.png sce_sys/icon0.png
        FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
        FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
        FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
    )
endif()

# Copy assets to the build directory
file(COPY assets/levels.mrg DESTINATION ${CMAKE_BINARY_DIR})
