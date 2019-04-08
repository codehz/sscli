cmake_minimum_required(VERSION 3.13)

SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
SET(BUILD_SHARED_LIBS OFF)
SET(CMAKE_EXE_LINKER_FLAGS "-static -Os -Wl,--gc-sections,-flto -s")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -Wall -Wno-switch -flto -ffunction-sections -fdata-sections")
SET(CMAKE_AR "gcc-ar")
SET(CMAKE_NM "gcc-nm")
SET(CMAKE_RANLIB "gcc-ranlib")

add_subdirectory(APID EXCLUDE_FROM_ALL)

include(ExternalProject)

ExternalProject_Add(
  editline
  PREFIX editline
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/editline
  CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/configure-editline.sh CC=${CMAKE_C_COMPILER} CFLAGS=${CMAKE_C_FLAGS} SRC=${CMAKE_CURRENT_SOURCE_DIR} DST=${CMAKE_CURRENT_BINARY_DIR}
  BUILD_COMMAND make
  INSTALL_COMMAND make install
)
ExternalProject_Get_property(editline INSTALL_DIR)
set(EDITLINE_DIR ${INSTALL_DIR})
unset(INSTALL_DIR)

add_executable(sscli src/cli.c)
target_include_directories(sscli PRIVATE ${EDITLINE_DIR}/include)
target_link_directories(sscli PRIVATE ${EDITLINE_DIR}/lib)
target_link_libraries(sscli apid)
target_link_libraries(sscli -leditline)
add_dependencies(sscli editline)