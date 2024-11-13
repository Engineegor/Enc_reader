#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_gpio.h>
#include <furi_hal_power.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/widget.h>

#include <input/input.h>

#include <notification/notification.h>

#include "enc_reader.h"

/*******************************************************************
 *                   INPUT / OUTPUT FUNCTIONS 
 *******************************************************************/
static void enc_reader_app_interrupt_callback(void* context) {
    furi_assert(context);
    EncApp* app = context;

    if (furi_hal_gpio_read(app->input_pin.b))	app->coordinates.abs++;
    else										app->coordinates.abs--;
}

static void enc_reader_setup_gpio_inputs(EncApp* app) {
    app->input_pin.a	= &gpio_ext_pa4;
    app->input_pin.b	= &gpio_ext_pa7;

    furi_hal_gpio_init(app->input_pin.a, GpioModeInterruptFall,	GpioPullUp, GpioSpeedVeryHigh);
    furi_hal_gpio_init(app->input_pin.b, GpioModeInput,			GpioPullUp, GpioSpeedVeryHigh);

    furi_hal_gpio_add_int_callback(app->input_pin.a, enc_reader_app_interrupt_callback, app);
    furi_hal_gpio_enable_int_callback(app->input_pin.a);
}

static void enc_reader_update_vbus_state(VbusState state) {
    if (state == VbusON)	{
        furi_hal_power_enable_otg();
        //notification_message(app->notifications, &vOn_led_sequence);
    } else if (state == VbusOFF) {
        furi_hal_power_disable_otg();
        //notification_message(app->notifications, &vOff_led_sequence);
    }
}

/*******************************************************************
 *                   VIEW DISPATCHER FUNCTIONS 
 *******************************************************************/
static void enc_reader_app_tick_event_callback(void* context) {
    furi_assert(context);
    EncApp* app = context;

    if (app->current_view == ViewIndexMain) {

    }

}

static bool enc_reader_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    EncApp* app = context;
    // The event numerical value can mean different things (the application is responsible to uphold its chosen convention)
    // In this example, the only possible meaning is the view index to switch to.
    furi_assert(event < ViewIndexMAX);
    // Switch to the requested view.
    view_dispatcher_switch_to_view(app->view_dispatcher, event);

    return true;
}

static bool enc_reader_app_navigation_callback(void* context) { // This function is called when the user has pressed the Back key.
    furi_assert(context);
    EncApp* app = context;
    // Back means exit the application, which can be done by stopping the ViewDispatcher.
    view_dispatcher_stop(app->view_dispatcher);
    return true;
}

static void view_dispatcher_alloc(EncApp* app) {
    app->view_dispatcher = view_dispatcher_alloc();
    
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    view_dispatcher_add_view(app->view_dispatcher,  ViewIndexMain,      widget_get_view(app->widget));
    view_dispatcher_add_view(app->view_dispatcher,  ViewIndexSettings,  variable_item_list_get_view(app->var_item_list));

    view_dispatcher_set_tick_event_callback(app->view_dispatcher,       enc_reader_app_tick_event_callback, 40);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher,     enc_reader_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, enc_reader_app_navigation_callback);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
}

static void view_dispatcher_free(EncApp* app) {
    furi_assert(app);
    
    view_dispatcher_remove_view(app->view_dispatcher, ViewIndexSettings);
    view_dispatcher_remove_view(app->view_dispatcher, ViewIndexMain);

    view_dispatcher_free(app->view_dispatcher);
}

/*******************************************************************
 *                   ALLOC FUNCTIONS 
 *******************************************************************/
EncApp* enc_reader_app_alloc() {
    EncApp* app = malloc(sizeof(EncApp));

    app->gui			= furi_record_open(RECORD_GUI);
    //app->view_port	= view_port_alloc();
    //app->event_queue	= furi_message_queue_alloc(8, sizeof(InputEvent));

    enc_reader_setup_gpio_inputs(app);
    view_settings_alloc(app);
    view_main_alloc(app);
    view_dispatcher_alloc(app);
    
    //gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    
    //view_port_draw_callback_set(app->view_port, enc_reader_app_draw_callback, app);
    //view_port_input_callback_set(app->view_port, enc_reader_app_input_callback, app->event_queue);

    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    return app;
}

void enc_reader_app_free(EncApp* app) {
    furi_assert(app);

    view_dispatcher_free(app);
    view_settings_free(app);
    view_main_free(app);


    furi_hal_gpio_disable_int_callback(app->input_pin.a);
    furi_hal_gpio_remove_int_callback(app->input_pin.a);

    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);
}

static void enc_reader_app_run(EncApp* app) {

    app->resolution     = ResolutionRaw;
    app->Vbus_state     = VbusON;
    app->current_view   = ViewIndexSettings;

    view_dispatcher_switch_to_view(app->view_dispatcher, ViewIndexSettings);
    view_dispatcher_run(app->view_dispatcher);
}

/*******************************************************************
 *                  vvv START HERE vvv
 *******************************************************************/
int32_t enc_reader_app(void *p) {
    UNUSED(p);
    EncApp* app = enc_reader_app_alloc();
    enc_reader_app_run(app);
    furi_hal_power_disable_otg();
    enc_reader_app_free(app);
    return 0;
}