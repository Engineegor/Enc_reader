#include <furi_hal_gpio.h>
#include <furi_hal_power.h>

#include <input/input.h>

#include "gpio_input.h"

#include "../configs/configs.h"

/*******************************************************************
 *                   INPUT / OUTPUT FUNCTIONS 
 *******************************************************************/
void enc_reader_app_interrupt_callback(void* context) {
    furi_assert(context);
    EncApp* app = context;

    if (furi_hal_gpio_read(app->input_pin.b))	app->coordinates.abs++;
    else										app->coordinates.abs--;
}

void app_setup_gpio_inputs(EncApp* app) {
    app->input_pin.a	= &gpio_ext_pa4;
    app->input_pin.b	= &gpio_ext_pa7;

    furi_hal_gpio_init(app->input_pin.a, GpioModeInterruptFall,	GpioPullUp, GpioSpeedVeryHigh);
    furi_hal_gpio_init(app->input_pin.b, GpioModeInput,			GpioPullUp, GpioSpeedVeryHigh);

    furi_hal_gpio_add_int_callback(app->input_pin.a, enc_reader_app_interrupt_callback, app);
    furi_hal_gpio_enable_int_callback(app->input_pin.a);
}

void enc_reader_update_vbus_state(VbusState state) {
    if (state == VbusON)	{
        furi_hal_power_enable_otg();
    } else if (state == VbusOFF) {
        furi_hal_power_disable_otg();
    }
}
