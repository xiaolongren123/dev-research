file(GLOB_RECURSE SRC_HTTP ${CMAKE_SOURCE_DIR} "components/device_http*.c")
file(GLOB_RECURSE SRC_wifi ${CMAKE_SOURCE_DIR} "components/device_wifi*.c")


idf_component_register(
    SRCS
        "main.c"
        ${SRC_HTTP}
        ${SRC_wifi}
    INCLUDE_DIRS
        "."
        "components/device_http"
        "components/device_wifi"
)
