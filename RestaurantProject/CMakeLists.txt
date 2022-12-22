cmake_minimum_required(VERSION 3.5)
project(RestaurantProject)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(include)
add_executable(RestaurantProject Kitchen.cpp)

# THIS EXECUTABLE IS ONLY TO TEST THE MINIMAL SUBSCRIBER!
target_link_libraries(RestaurantProject PRIVATE Threads::Threads)





# install(TARGETS
#     turtlebot_navigator
#     kitchen
#     talker # THIS IS JUST FOR TESTING
#     DESTINATION lib/${PROJECT_NAME}
# )