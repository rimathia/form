cmake_minimum_required(VERSION 2.8)

project(FORM C CXX)

include(CheckIncludeFiles)
include(CheckTypeSize)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

include_directories("." sources)

add_subdirectory(sources)
set (FORM_SOURCES
		sources/argument.c
		sources/bugtool.c
		sources/checkpoint.c
		sources/comexpr.c
		sources/compcomm.c
		sources/compiler.c
		sources/compress.c
		sources/comtool.c
		sources/dict.c
		sources/dollar.c
		sources/execute.c
		sources/extcmd.c
		sources/factor.c
		sources/findpat.c
		sources/function.c
		sources/if.c
		sources/index.c
		sources/lus.c
		sources/message.c
		sources/minos.c
		sources/module.c
		#sources/mpi.c
		sources/mytime.cc
		sources/names.c
		sources/normal.c
		sources/notation.c
		sources/opera.c
		sources/optimize.cc
		#sources/parallel.c
		sources/pattern.c
		sources/poly.cc
		sources/polyfact.cc
		sources/polygcd.cc
		sources/polywrap.cc
		sources/pre.c
		sources/proces.c
		sources/ratio.c
		sources/reken.c
		sources/reshuf.c
		sources/sch.c
		sources/setfile.c
		sources/smart.c
		sources/sort.c
		sources/spectator.c
		sources/startup.c
		sources/store.c
		sources/symmetr.c
		sources/tables.c
		sources/threads.c
		sources/token.c
		sources/tools.c
		sources/transform.c
		sources/unixfile.c
		sources/wildcard.c
)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		set(LINUX TRUE)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

check_include_files(fcntl.h HAVE_FCNTL_H)
check_include_files(inttypes.h HAVE_INTTYPES_H)
check_include_files(limits.h HAVE_LIMITS_H)
check_include_files(memory.h HAVE_MEMORY_H)
check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(stdlib.h HAVE_STDLIB_H)
check_include_files(strings.h HAVE_STRINGS_H)
check_include_files(string.h HAVE_STRING_H)
check_include_files(sys/file.h HAVE_SYS_FILE_H)
check_include_files(sys/stat.h HAVE_SYS_STAT_H)
check_include_files(sys/types.h HAVE_SYS_TYPES_H)
check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(windows.h HAVE_WINDOWS_H)

check_type_size(char SIZEOF_CHAR)
check_type_size(int SIZEOF_INT)
check_type_size(int SIZEOF_INT)
check_type_size(long SIZEOF_LONG)
check_type_size("long long" SIZEOF_LONG_LONG)
check_type_size(off_t SIZEOF_OFF_T)
check_type_size(short SIZEOF_SHORT)
check_type_size(void* SIZEOF_VOID_P)

# "" is not correct, needs to be replaced
if((${SIZEOF_CHAR}      EQUAL  1) AND 
   (${SIZEOF_SHORT}     EQUAL  2) AND 
   (${SIZEOF_INT}       EQUAL  2) AND 
   (${SIZEOF_LONG}      EQUAL  4) AND 
   (${SIZEOF_LONG_LONG} EQUAL "") AND 
   (${SIZEOF_VOID_P}    EQUAL  4) AND 
   (${SIZEOF_OFF_T}     EQUAL ""))
	set(ILP32 true)
endif()

# "" is not correct, needs to be replaced
if((${SIZEOF_CHAR}      EQUAL  1) AND 
   (${SIZEOF_SHORT}     EQUAL  2) AND 
   (${SIZEOF_INT}       EQUAL  4) AND 
   (${SIZEOF_LONG}      EQUAL  4) AND 
   (${SIZEOF_LONG_LONG} EQUAL  8) AND 
   (${SIZEOF_VOID_P}    EQUAL  8) AND 
   (${SIZEOF_OFF_T}     EQUAL ""))
	set(LLP64 true)
endif()

if((${SIZEOF_CHAR}      EQUAL  1) AND 
   (${SIZEOF_SHORT}     EQUAL  2) AND 
   (${SIZEOF_INT}       EQUAL  4) AND 
   (${SIZEOF_LONG}      EQUAL  8) AND 
   (${SIZEOF_LONG_LONG} EQUAL  8) AND 
   (${SIZEOF_VOID_P}    EQUAL  8) AND 
   (${SIZEOF_OFF_T}     EQUAL  8))
	set(LP64 true)
endif()

find_package(GMP REQUIRED)
include_directories(${GMP_INCLUDE_DIR})

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIR})

configure_file("configcmake.h.in" "config.h")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHAVE_CONFIG_H")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_CONFIG_H")

configure_file("configcmake.h.in" "config.h")

add_executable(form ${FORM_SOURCES})
target_link_libraries(form ${GMP_LIBRARY})
target_link_libraries(form ${ZLIB_LIBRARY})

