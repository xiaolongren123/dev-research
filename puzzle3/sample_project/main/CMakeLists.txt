file(GLOB_RECURSE SRC_HTTP ${CMAKE_SOURCE_DIR} "components/device_http*.c")
file(GLOB_RECURSE SRC_WIFI ${CMAKE_SOURCE_DIR} "components/device_wifi*.c")
file(GLOB_RECURSE SRC_FLASH ${CMAKE_SOURCE_DIR} "components/device_flash*.c")
file(GLOB_RECURSE SRC_USART ${CMAKE_SOURCE_DIR} "components/device_usart*.c")

idf_component_register(
    SRCS
        "main.c"
        ${SRC_HTTP}
        ${SRC_WIFI}
        ${SRC_FLASH}
        ${SRC_USART}
    INCLUDE_DIRS
        "."
        "components/device_http"
        "components/device_wifi"
        "components/device_flash"
        "components/device_usart"
)

