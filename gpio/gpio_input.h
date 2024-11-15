#pragma once

#include "../enc_reader.h"

void enc_reader_app_interrupt_callback(void* context);
void app_setup_gpio_inputs(EncApp* app);
void enc_reader_update_vbus_state(VbusState state);
