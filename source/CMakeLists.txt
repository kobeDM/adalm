cmake_minimum_required(VERSION 3.1.3)
set(CMAKE_CXX_STANDARD 11)

project(analog LANGUAGES CXX)

# pthread (for non-Windows)
if (NOT WIN32)
    find_library(PTHREAD_LIBRARIES pthread)
endif()

# libm2k
if (LIBM2K_VERSION)  # in-source build
    set(PROJECT_VERSION ${LIBM2K_VERSION})
else()               # standalone build
    set(PROJECT_VERSION "1.0.0")
    find_package(libm2k REQUIRED)
endif()

# executables
add_executable(ad_out          ad_out.cpp)
add_executable(ad_aout         ad_aout.cpp)
add_executable(ad_dout         ad_dout.cpp)
add_executable(set_voltage     set_voltage.cpp)
add_executable(send_bitstream  send_bitstream.cpp)
add_executable(shutter_ctrl    shutter_ctrl.cpp)
add_executable(daq             daq.cpp)

# link libraries
set(TARGET_LIST
    ad_out
    ad_aout
    ad_dout
    set_voltage
    send_bitstream
    shutter_ctrl
    daq
)

foreach(target ${TARGET_LIST})
    target_link_libraries(${target} PRIVATE libm2k::libm2k)
endforeach()

# install (../bin に変更)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/..)

install(TARGETS ${TARGET_LIST}
        DESTINATION bin)