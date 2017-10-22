cmake_minimum_required(VERSION 2.8)

add_definitions(
  -DENABLE_BMP
  -DREAD_WITH_KUIO
)

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/kuio/include
  ${CMAKE_SOURCE_DIR}/DSMotion
)

link_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/kuio/lib
  ${CMAKE_SOURCE_DIR}/DSMotion
)

add_executable(fakecamerakbmp
  ${CMAKE_SOURCE_DIR}/main.c
  #${CMAKE_SOURCE_DIR}/log.c
)

target_link_libraries(fakecamerakbmp
  taihen_stub
  SceLibKernel_stub
  k
  gcc
  SceKernelModuleMgr_stub
  SceKernelThreadMgr_stub
  SceAppMgr_stub
  SceSysmem_stub
  #SceIofilemgr_stub
  kuio_stub
  dsmotion_stub
)

set_target_properties(fakecamerakbmp
  PROPERTIES LINK_FLAGS "-nostdlib"
)

vita_create_self(fakecamerakbmp.suprx fakecamerakbmp
  UNSAFE
  CONFIG ${CMAKE_SOURCE_DIR}/export.yml
)
