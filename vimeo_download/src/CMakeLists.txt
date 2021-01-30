add_library(vimeocore STATIC Vimeo.cpp JSON.cpp Requests.cpp)
target_include_directories(vimeocore PUBLIC ${PROJECT_SOURCE_DIR}/include)