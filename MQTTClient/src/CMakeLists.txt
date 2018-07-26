project(tos_mqtt)

add_library(paho_mqtt_private OBJECT stub.cpp MQTTClient.h MQTTLogging.h FP.h)

add_library(paho_mqtt stub.cpp $<TARGET_OBJECTS:paho_mqtt_private>)
target_link_libraries(paho_mqtt PUBLIC MQTTPacketClient tos_core)
target_include_directories(paho_mqtt PUBLIC .)

add_custom_command(
        TARGET paho_mqtt PRE_LINK
        COMMAND ${CMAKE_OBJCOPY} --rename-section .text=.irom0.text --rename-section .literal=.irom0.literal $<TARGET_OBJECTS:paho_mqtt_private>
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Rename .text to .irom0.text"
)