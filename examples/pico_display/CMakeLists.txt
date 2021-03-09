add_executable(
  display
  demo.cpp
  image_data.cpp
)

# Pull in pico libraries that we need
target_link_libraries(display pico_stdlib hardware_spi hardware_pwm hardware_dma pico_display)

# create map/bin/hex file etc.
pico_add_extra_outputs(display)

add_executable(
  font_demo_display
  font_demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(font_demo_display pico_stdlib pico_display)

# create map/bin/hex file etc.
pico_add_extra_outputs(font_demo_display)