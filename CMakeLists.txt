cmake_minimum_required(VERSION 3.30)
project(image-lab LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
find_program(GLSLC NAMES glslc HINTS Vulkan::glslc)
pkg_check_modules(VULKAN REQUIRED vulkan)
pkg_check_modules(GLFW REQUIRED glfw3)

set(SHADER_SOURCE_DIR ${PROJECT_SOURCE_DIR}/shaders)
set(SHADER_BINARY_DIR ${PROJECT_BINARY_DIR}/shaders)

file(GLOB SOURCES LIST_DIRECTORIES false "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB SHADERS ${SHADER_SOURCE_DIR}/*.comp)

make_directory(${SHADER_BINARY_DIR})
foreach(source IN LISTS SHADERS)
	get_filename_component(FILENAME ${source} NAME)
	add_custom_command(
		COMMAND
		${GLSLC} -o ${SHADER_BINARY_DIR}/${FILENAME}.spv ${source}
		OUTPUT ${SHADER_BINARY_DIR}/${FILENAME}.spv
		DEPENDS ${source} ${SHADER_BINARY_DIR}
		COMMENT "Compiling Shader ${FILENAME}"
	)
	list(APPEND SPV_SHADERS ${SHADER_BINARY_DIR}/${FILENAME}.spv)
endforeach()

add_custom_target(shaders ALL DEPENDS ${SPV_SHADERS})

add_subdirectory(vendor/stb)

add_executable(${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/src/main.cpp")
target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW_LIBRARIES})
target_compile_options(${PROJECT_NAME} PRIVATE ${GLFW_CLAGS_OTHER})
target_include_directories(${PROJECT_NAME} PRIVATE ${VULKAN_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${VULKAN_LIBRARIES})
target_compile_options(${PROJECT_NAME} PRIVATE ${VULKAN_CLAGS_OTHER})
target_include_directories(${PROJECT_NAME} PRIVATE
	"${PROJECT_SOURCE_DIR}/include"
)
target_include_directories(${PROJECT_NAME} PRIVATE
	"${PROJECT_SOURCE_DIR}/vendor"
)
target_link_libraries(${PROJECT_NAME} PRIVATE stb)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
