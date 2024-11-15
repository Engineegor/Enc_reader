#pragma once

#include "../enc_reader.h"
#include "../gpio/gpio_input.h"



void app_change_view(EncApp* app, ViewIndex index);
void app_view_dispatcher_alloc(EncApp* app);
void app_view_dispatcher_free(EncApp* app);