cmake_minimum_required(VERSION 3.10)

if (POLICY CMP0020)
	cmake_policy(SET CMP0020 NEW)
endif (POLICY CMP0020)

project(Versioner)

set(ROOT_PROJECT_PATH "${CMAKE_SOURCE_DIR}/../..")
set(CMAKE_MODULE_PATH "${ROOT_PROJECT_PATH}/source/versioner/source/build/cmake")

include(OmegaEnviroment)

set(TIGER_ROOT_DIRECTORY "${ROOT_PROJECT_PATH}/Build/Omega")

set(BINARY_OUTPUT_DIRECTORY "bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${TIGER_ROOT_DIRECTORY}/${BINARY_OUTPUT_DIRECTORY}")

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(LIBRARY_OUTPUT_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

message(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
message(${EXECUTABLE_OUTPUT_PATH})
message(${LIBRARY_OUTPUT_PATH})

include(Qt6)

if (OMEGA_WIN32)
	include(ThirdPartyWindows)
elseif (OMEGA_MACOSX)
	include(ThirdPartyMacOSX)
elseif (OMEGA_LINUX)
	include(ThirdPartyLinux)
endif (OMEGA_WIN32)

add_subdirectory(source)
