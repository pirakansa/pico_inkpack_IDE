add_library(ImageData ImageData.cpp)

target_include_directories(ImageData PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
