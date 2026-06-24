add_library(ImageData ImageData.c)

target_include_directories(ImageData PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
