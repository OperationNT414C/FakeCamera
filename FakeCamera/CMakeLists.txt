cmake_minimum_required(VERSION 2.8)

include_directories(
)

link_directories(
  ${CMAKE_SOURCE_DIR}
)

add_executable(fakecamera
  ${CMAKE_SOURCE_DIR}/main.c
)

target_link_libraries(fakecamera
  taihen_stub
  SceLibKernel_stub
  k
  gcc
  SceKernelModuleMgr_stub
  SceKernelThreadMgr_stub
)

set_target_properties(fakecamera
  PROPERTIES LINK_FLAGS "-nostdlib"
)

vita_create_self(fakecamera.suprx fakecamera
  UNSAFE
  CONFIG ${CMAKE_SOURCE_DIR}/export.yml
)
