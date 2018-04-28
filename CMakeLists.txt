cmake_minimum_required(VERSION 3.5.1)
project(pjsua2-demo)

set(CMAKE_CXX_STANDARD 11)

include (FindPkgConfig)
if (PKG_CONFIG_FOUND) # true if pkg-config works on the system
    pkg_check_modules(PJSIP REQUIRED libpjproject)
endif()

find_library(COREAUDIO_LIBRARY CoreAudio REQUIRED)
find_library(AUDIOUNIT_LIBRARY AudioUnit REQUIRED)
find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox REQUIRED)

include_directories(${PJSIP_INCLUDE_DIRS})
link_directories(${PJSIP_LIBRARY_DIRS})

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} ${PJSIP_LIBRARIES} ${COREAUDIO_LIBRARY} ${AUDIOUNIT_LIBRARY} ${AUDIOTOOLBOX_LIBRARY})
