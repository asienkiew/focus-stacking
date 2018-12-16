set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3")
add_executable(converter main.cpp FocusStacking.cpp Matrix8U.cpp)
target_link_libraries(converter ${OpenCV_LIBS})