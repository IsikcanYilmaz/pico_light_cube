cmake_minimum_required(VERSION 3.13)
set(PICO_BOARD pico_w)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(test_proj)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# subdirectories
add_subdirectory(./colorspace-conversions)

# rest of your project
add_executable(test_proj
	test.c test_functionality.c
	usr_input.c usr_commands.c mic.c
	addr_led_driver.c animation_manager.c
	button.c color.c visual_util.c editable_value.c
	
	animation_scroller.c animation_canvas.c animation_sparkles.c
	)

target_include_directories(test_proj PRIVATE
													 ${CMAKE_CURRENT_LIST_DIR}
													 # ./misc/
													 ./colorspace-conversions
													)

# Add pico_stdlib library which aggregates commonly used features
add_library(kiss_fftr kiss_fftr.c)
add_library(kiss_fft kiss_fft.c)
target_link_libraries(kiss_fftr kiss_fft)
target_link_libraries(test_proj 
											pico_stdlib
											# pico_cyw43_arch_none
											pico_cyw43_arch_lwip_poll
											hardware_irq
											hardware_pwm
											hardware_spi
											hardware_adc
											hardware_dma
											# misc
											colorspace 
											kiss_fftr
											)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_TIME_DEFAULT_ALARM_POOL_DISABLED 1)
set(PICO_STDIO_USB_SUPPORT_CHARS_AVAILABLE_CALLBACK 1)

# enable usb output, disable uart output
pico_enable_stdio_usb(test_proj 1)
pico_enable_stdio_uart(test_proj 0)

# add url via pico_set_program_url
# example_auto_set_url(test_proj)

# add_compile_definitions(PICO_TIME_DEFAULT_ALARM_POOL_DISABLED=1)
# add_compile_definitions(PICO_STDIO_USB_SUPPORT_CHARS_AVAILABLE_CALLBACK=1)
# add_compile_definitions(LWIP_DEBUG=0)
# add_compile_definitions(NDEBUG) # Set to disable lwip debug
# add_compile_definitions(WIFI_BACKGROUND=1)
# add_compile_definitions(WIFI_POLL=0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(test_proj)
