# demo2
add_executable(udp_receive udp_receive.cpp)

# 链接 OpenCV 库
target_link_libraries(udp_receive 
    PRIVATE
        Threads::Threads
        common_lib
        ${OpenCV_LIBS}
)