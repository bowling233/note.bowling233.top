cmake_minimum_required(VERSION 3.10)
project(RDMA_example)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2")

add_executable(RDMA_RC_example RDMA_RC_example.c)
target_link_libraries(RDMA_RC_example PRIVATE ibverbs)

add_executable(mckey mckey.c)
target_link_libraries(mckey PRIVATE ibverbs rdmacm)
target_compile_definitions(mckey PRIVATE _GNU_SOURCE)

add_executable(srq srq.c)
target_link_libraries(srq PRIVATE ibverbs rdmacm)
