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

add_executable(ad_dout "ad_dout.cpp")
#add_executable(ad_out "ad_out.cpp")
#add_executable(daq_test "daq_test.cpp")
#add_executable(daq_sahigash "daq_sahigash.cpp")

target_link_libraries(ad_dout PRIVATE libm2k::libm2k)
#target_link_libraries(daq_test PRIVATE libm2k::libm2k)
#target_link_libraries(daq_sahigash PRIVATE libm2k::libm2k)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/..)
install(TARGETS ad_dout DESTINATION bin)
#install(TARGETS daq_test DESTINATION bin)
#install(TARGETS daq_sahigash DESTINATION bin)


