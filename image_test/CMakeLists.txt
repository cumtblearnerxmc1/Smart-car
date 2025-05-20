# demo2
add_executable(image_test image_test.cpp)

# 链接 OpenCV 库
target_link_libraries(image_test 
    PRIVATE
        Threads::Threads
        common_lib
        ${OpenCV_LIBS}
)