idf_component_register(SRCS "rain_compute.cpp"
                    "main.c" 
                    "actuation.c"
                    INCLUDE_DIRS ".")

target_compile_definitions(${COMPONENT_LIB} PRIVATE PLATFORM_ESP32)