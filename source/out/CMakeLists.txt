cmake_minimum_required(VERSION 3.1.3)
set (CMAKE_CXX_STANDARD 11)
project(analog LANGUAGES CXX)


if (NOT WIN32)
	find_library(PTHREAD_LIBRARIES pthread)
endif()

if (LIBM2K_VERSION)			# in-source build
	set(PROJECT_VERSION ${LIBM2K_VERSION})
else()					# standalone build
	set(PROJECT_VERSION "1.0.0")
	find_package(libm2k REQUIRED)
endif()

add_executable(ad_out "ad_out.cpp")
add_executable(ad_aout "ad_aout.cpp")
add_executable(ad_dout "ad_dout.cpp")
add_executable(set_voltage "set_voltage.cpp")
add_executable(send_bitstream "send_bitstream.cpp")
add_executable(shutter_ctrl "shutter_ctrl.cpp")

target_link_libraries(ad_out PRIVATE libm2k::libm2k)
target_link_libraries(ad_dout PRIVATE libm2k::libm2k)
target_link_libraries(ad_aout PRIVATE libm2k::libm2k)
target_link_libraries(set_voltage PRIVATE libm2k::libm2k)
target_link_libraries(send_bitstream PRIVATE libm2k::libm2k)
target_link_libraries(shutter_ctrl PRIVATE libm2k::libm2k)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/../..)
install(TARGETS ad_out DESTINATION bin)
install(TARGETS ad_dout DESTINATION bin)
install(TARGETS ad_aout DESTINATION bin)
install(TARGETS set_voltage DESTINATION bin)
install(TARGETS send_bitstream DESTINATION bin)
install(TARGETS shutter_ctrl DESTINATION bin)


