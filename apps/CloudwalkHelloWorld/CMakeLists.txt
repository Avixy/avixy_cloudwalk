cmake_minimum_required(VERSION 3.2.1)

set(TARGET cloudwalkHelloWorld)

# device architecture
if(NOT ARCH_TARGET)
	set(ARCH_TARGET CACHE STRING "Choose the architecture, options are: 3400, 3500 and 4000" )
endif(NOT ARCH_TARGET)

if(ARCH_TARGET EQUAL "3400")
	add_definitions(-DDDEVICE_AVX3400)
elseif(ARCH_TARGET EQUAL "3500")
	add_definitions(-DDEVICE_AVX3500)
elseif(ARCH_TARGET EQUAL "4000")
	add_definitions(-DDEVICE_AVX4000)
else()
	message(FATAL_ERROR "Unknown architecture")
endif()

set(LIBRARY_OUTPUT_PATH 	$ENV{SDK_WORKSPACE_PATH}/usr/lib/avx${ARCH_TARGET} CACHE PATH "Output directory for libraries" )
set(EXECUTABLE_OUTPUT_PATH 	$ENV{SDK_WORKSPACE_PATH}/usr/bin/avx${ARCH_TARGET} CACHE PATH "Output directory for applications" )
set(CMAKE_TOOLCHAIN_FILE 	$ENV{SDK_WORKSPACE_PATH}/scripts/toolchain_arm.cmake)

project(${TARGET})

set(WORKSPACE_PATH $ENV{SDK_WORKSPACE_PATH})
set(3RDPARTY_PATH $ENV{SDK_3RDPARTY_PATH})
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(AVX_LIBS ${WORKSPACE_PATH}/usr/lib/avx${ARCH_TARGET})

# status bar height
if(NOT STATUS_BAR_HEIGHT)
	set(STATUS_BAR_HEIGHT CACHE STRING "Define the status bar height value, options are: 0, 20 and 30" )
endif(NOT STATUS_BAR_HEIGHT)

if(STATUS_BAR_HEIGHT EQUAL "0")
	set(ALTURA_STATUS_BAR 0)
elseif(STATUS_BAR_HEIGHT EQUAL "20")
	set(ALTURA_STATUS_BAR 20)
elseif(STATUS_BAR_HEIGHT EQUAL "30")
	set(ALTURA_STATUS_BAR 30)
else()
	message(FATAL_ERROR "Unknown value to status bar height")
endif()

add_definitions(-DALTURA_STATUS_BAR=${ALTURA_STATUS_BAR})

if(ARCH_TARGET EQUAL "3400" OR ARCH_TARGET EQUAL "3500")
	message(FATAL_ERROR "This project should be only used in the AVX_4000 architecture")
endif(ARCH_TARGET EQUAL "3400" OR ARCH_TARGET EQUAL "3500")

set(SOURCES ${SRC_DIR}/helloWorld.c)
set(LIBS mruby emvhal crypto avixy magcard network ethernet wifi gprs core pthread)
set(LIBS_MX25 avxpinpad emvc2 emvc2_config emv_common avxcrypto avxgfx avxinput avxgui avxcore)

add_definitions(-DAVXPP_ENABLE_DEBUG_PRINT=1)	

# defined the .h (includes)
include_directories(${WORKSPACE_PATH}/libraries/avixy/inc)
include_directories(${WORKSPACE_PATH}/libraries/core/inc)
include_directories(${WORKSPACE_PATH}/avixy/avx${ARCH_TARGET}/include)
include_directories(${WORKSPACE_PATH}/cloudwalk/around_the_world/mruby/include)

# path of the .a (binaries)
link_directories(${AVX_LIBS})
link_directories(${WORKSPACE_PATH}/avixy/avx${ARCH_TARGET}/lib)
link_directories(${3RDPARTY_PATH}/lib)
link_directories(${WORKSPACE_PATH}/cloudwalk/around_the_world/mruby/build/avixy${ARCH_TARGET}/lib)

add_custom_target(around_the_world ALL
    COMMAND CONFIG=build.rb rake -f ${WORKSPACE_PATH}/cloudwalk/around_the_world/Rakefile
)

# creating the binary of the project
add_executable(${TARGET} ${SOURCES})

# configuring the libaries needed for linking
target_link_libraries(${TARGET} ${LIBS} ${LIBS_MX25} ${LIBS})

add_dependencies(${TARGET} around_the_world)

add_custom_target(${TARGET}_deploy ALL
    COMMAND ${WORKSPACE_PATH}/scripts/signAndDeployApp.sh ${PROJECT_SOURCE_DIR} ${TARGET} ${ALTURA_STATUS_BAR} ${WORKSPACE_PATH} ${ARCH_TARGET}
    DEPENDS ${TARGET}
)

