# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(tmceval)

# set(CMAKE_C_FLAGS "-Wall -Wextra -Wimplicit -Wcast-align -Wpointer-arith -Wredundant-decls -Wshadow -Wcast-qual -Wcast-align")

set(CMAKE_C_FLAGS "-g -O")

set(CMAKE_C_FLAGS "-Wno-incompatible-pointer-types -Wno-pointer-to-int-cast")

file(GLOB_RECURSE tmceval_SRC
	 "*.h"
	 "*.c"
)

file(GLOB_RECURSE landungsbruecke_SRC
	 "${CMAKE_CURRENT_SOURCE_DIR}/hal/Landungsbruecke/*.h"
	 "${CMAKE_CURRENT_SOURCE_DIR}/hal/Landungsbruecke/*.c"
)

file(GLOB_RECURSE landungsbruecke_V3_SRC
	 "${CMAKE_CURRENT_SOURCE_DIR}/hal/Landungsbruecke_V3/*.h"
	 "${CMAKE_CURRENT_SOURCE_DIR}/hal/Landungsbruecke_V3/*.c"
)

file(GLOB_RECURSE zephyr_SRC
	 "${CMAKE_CURRENT_SOURCE_DIR}/hal/zephyr/*.h"
	 "${CMAKE_CURRENT_SOURCE_DIR}/hal/zephyr/*.c"
)

file(GLOB_RECURSE eval_SRC
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/MAX22216/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC2130/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC2160/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC2208/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC2209/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC2224/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC2225/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC2226/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC2240/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC2300/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC2590/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC262/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC2660/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC389/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC4330/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC4331/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC4361A/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC43xx/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC5031/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC5041/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC5062/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC5072/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC5130/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC5160/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC5240/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC5271/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC5272/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC6100/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC6200/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC7300/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC8461/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC8462/*.*"
"${CMAKE_CURRENT_SOURCE_DIR}/TMC-API/tmc/ic/TMC846x/*.*"
)

file(GLOB_RECURSE REMOVE_CMAKE "${CMAKE_CURRENT_SOURCE_DIR}/build/*")
list(REMOVE_ITEM tmceval_SRC ${REMOVE_CMAKE})

if (DEVICE STREQUAL Zephyr)
	message(STATUS "Zephyr plataform selected!")
	message(STATUS ${tmceval_SRC})
	#message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})
	list(REMOVE_ITEM tmceval_SRC ${CMAKE_CURRENT_SOURCE_DIR}/tmc/BLDC_Landungsbruecke.c)
	list(REMOVE_ITEM tmceval_SRC ${CMAKE_CURRENT_SOURCE_DIR}/tmc/BLDC_LandungsbrueckeV3.c)
	list(REMOVE_ITEM tmceval_SRC ${CMAKE_CURRENT_SOURCE_DIR}/tmc/IdDetection_Landungsbruecke.c)
	list(REMOVE_ITEM tmceval_SRC ${CMAKE_CURRENT_SOURCE_DIR}/tmc/IdDetection_LandungsbrueckeV3.c)
	list(REMOVE_ITEM tmceval_SRC ${CMAKE_CURRENT_SOURCE_DIR}/boards/SelfTest_Landungsbruecke.c)
	list(REMOVE_ITEM tmceval_SRC ${CMAKE_CURRENT_SOURCE_DIR}/boards/SelfTest_LandungsbrueckeV3.c)
	list(REMOVE_ITEM tmceval_SRC ${CMAKE_CURRENT_SOURCE_DIR}/boards/TMC6140_eval.c)
	# list(REMOVE_ITEM tmceval_SRC ${eval_SRC})
	list(REMOVE_ITEM tmceval_SRC ${landungsbruecke_SRC})
	list(REMOVE_ITEM tmceval_SRC ${landungsbruecke_V3_SRC})
elseif (DEVICE STREQUAL Landungsbruecke)
	list(REMOVE_ITEM tmceval_SRC ${zephyr_SRC})
	list(REMOVE_ITEM tmceval_SRC ${landungsbruecke_V3_SRC})
elseif (DEVICE STREQUAL LandungsbrueckeV3)

endif()

add_definitions(-DBUILD_VERSION=3.08)
add_definitions(-DDEVICE=Zephyr)

# add_definitions(-DLandungsbruecke)

#add_executable(tmceval tmc/TMCL.c
#                       tmc/TMCL.h)


add_executable(tmceval ${tmceval_SRC})

target_include_directories(tmceval PUBLIC  
						   .
	                       TMC-API 
						   hal
)

# target_sources(tmceval PRIVATE ${tmceval_SRC})

