#include "enc_reader.h"

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_gpio.h>
#include <furi_hal_power.h>

#include <gui/gui.h>
#include <gui/elements.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>

#include <input/input.h>

#include <notification/notification.h>
/*
static void enc_reader_app_input_callback(InputEvent* input_event, void* context) {
    furi_assert(context);

    FuriMessageQueue* event_queue = context;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

static void enc_reader_app_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    EncApp* app = context;

    app->coordinates.rel = app->coordinates.abs - app->coordinates.org;
    
    char abs_coord[] = "00000000 ";
    char org_coord[] = "00000000 ";
    char rel_coord[] = "00000000 ";

    static const uint8_t header_height		= 12;
    static const uint8_t vertical_gap		= 2;
    static const uint8_t vertical_offset	= 16;

    int32_t abs_pos = app->coordinates.abs;
    int32_t org_pos = app->coordinates.org;
    int32_t rel_pos = app->coordinates.rel;
    int32_t divider = 1;

    if (!app->Show_raw) {
        if (app->resolution < ResolutionMAX) divider = pos_divider[app->resolution];
        abs_pos /= divider;
        org_pos /= divider;
        rel_pos /= divider;
    }

    snprintf(abs_coord, strlen(abs_coord), "%8d", (int)abs_pos);
    snprintf(org_coord, strlen(org_coord), "%8d", (int)org_pos);
    snprintf(rel_coord, strlen(rel_coord), "%8d", (int)rel_pos);

    canvas_clear(canvas);
    canvas_set_font(canvas, FontSecondary);

    elements_multiline_text_aligned(canvas, 4, vertical_gap, AlignLeft, AlignTop, app->Vbus_state ? "Bus:    enable" : "Bus:   disable");

    elements_frame(canvas, 0, header_height, canvas_width(canvas), canvas_height(canvas) - header_height);

    elements_multiline_text_aligned(canvas, 4, header_height + vertical_gap,						AlignLeft, AlignTop, "Abs:");
    elements_multiline_text_aligned(canvas, 4, header_height + vertical_gap + vertical_offset,		AlignLeft, AlignTop, "Org:");
    elements_multiline_text_aligned(canvas, 4, header_height + vertical_gap + vertical_offset * 2,	AlignLeft, AlignTop, "Rel:");
    
    canvas_set_font(canvas, FontBigNumbers);
    elements_multiline_text_aligned(canvas, 30, header_height + vertical_gap,						AlignLeft, AlignTop, (const char*)abs_coord);
    elements_multiline_text_aligned(canvas, 30, header_height + vertical_gap + vertical_offset,		AlignLeft, AlignTop, (const char*)org_coord);
    elements_multiline_text_aligned(canvas, 30, header_height + vertical_gap + vertical_offset * 2,	AlignLeft, AlignTop, (const char*)rel_coord);
}
*/
/*******************************************************************
 *                  vvv INPUT FUNCTIONS vvv
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

/*******************************************************************
 *                  vvv SETTINGS VIEW vvv
 *******************************************************************/
static void submenu_start_callback(void* context, uint32_t index) {
    furi_assert(context);
    EncApp* app = context;

    if(index == SubmenuIndexStart) {
        view_dispatcher_send_custom_event(app->view_dispatcher, ViewIndexMain);
    }
}

static void submenu_change_callback(void* context, uint32_t index) {
    furi_assert(context);
    EncApp* app = context;

    if (index == SubmenuIndexVbusState) {
        app->Vbus_state = ! app->Vbus_state;
        if (app->Vbus_state)	{
            furi_hal_power_enable_otg();
            notification_message(app->notifications, &vOn_led_sequence);
        } else {
            furi_hal_power_disable_otg();
            notification_message(app->notifications, &vOff_led_sequence);
        }
    }

}

static void enc_reader_setup_view_settings(EncApp* app) {
    app->submenu = submenu_alloc();

    submenu_add_item(app->submenu, "Start",			SubmenuIndexStart,		submenu_start_callback, 	app);
    submenu_add_item(app->submenu, "Resolution",	SubmenuIndexResolution,	submenu_change_callback,	app);
    //submenu_add_item(app->submenu, "Show raw",  	SubmenuIndexShowRaw,	submenu_change_callback,	app);
    submenu_add_item(app->submenu, "5v supply",		SubmenuIndexVbusState,	submenu_change_callback,	app);
}

/*******************************************************************
 *                  vvv MAIN VIEW vvv
 *******************************************************************/
static void enc_reader_app_button_callback(GuiButtonType button_type, InputType input_type, void* context) {
    furi_assert(context);
    EncApp* app = context;
    // Only request the view switch if the user short-presses the Center button.
    if (input_type == InputTypeShort) {
        if (button_type == GuiButtonTypeLeft) {
            view_dispatcher_send_custom_event(app->view_dispatcher, ViewIndexSettings);
        } else if (button_type == GuiButtonTypeCenter) {
            app->coordinates.org = app->coordinates.abs;
            notification_message(app->notifications, &button_led_sequence);
        } else if (button_type == GuiButtonTypeRight) {
            app->coordinates.org = 0;
            app->coordinates.abs = 0;
            notification_message(app->notifications, &button_led_sequence);
        }
    }
}

static void enc_reader_setup_view_main(EncApp* app) {
    app->widget = widget_alloc();

    widget_add_string_multiline_element(app->widget, 64, 32, AlignCenter, AlignCenter, FontSecondary, "Press the Button below");

    widget_add_button_element(app->widget, GuiButtonTypeLeft,   "Config",   enc_reader_app_button_callback, app);
    widget_add_button_element(app->widget, GuiButtonTypeCenter, "Org",      enc_reader_app_button_callback, app);
    widget_add_button_element(app->widget, GuiButtonTypeRight,  "Set 0",    enc_reader_app_button_callback, app);
}

/*******************************************************************
 *                  vvv VIEW DISPATCHER FUNCTIONS vvv
 *******************************************************************/
static bool example_view_dispatcher_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    EncApp* app = context;
    // The event numerical value can mean different things (the application is responsible to uphold its chosen convention)
    // In this example, the only possible meaning is the view index to switch to.
    furi_assert(event < ViewIndexMAX);
    // Switch to the requested view.
    view_dispatcher_switch_to_view(app->view_dispatcher, event);

    return true;
}

static bool example_view_dispatcher_app_navigation_callback(void* context) { // This function is called when the user has pressed the Back key.
    furi_assert(context);
    EncApp* app = context;
    // Back means exit the application, which can be done by stopping the ViewDispatcher.
    view_dispatcher_stop(app->view_dispatcher);
    return true;
}

static void enc_reader_setup_view_dispatcher(EncApp* app) {
    app->view_dispatcher = view_dispatcher_alloc();
    
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    view_dispatcher_add_view(app->view_dispatcher,  ViewIndexMain,      widget_get_view(app->widget));
    view_dispatcher_add_view(app->view_dispatcher,  ViewIndexSettings,  submenu_get_view(app->submenu));

    view_dispatcher_set_custom_event_callback(app->view_dispatcher,     example_view_dispatcher_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, example_view_dispatcher_app_navigation_callback);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
}

/*******************************************************************
 *                  vvv ALLOC FUNCTIONS vvv
 *******************************************************************/
EncApp* enc_reader_app_alloc() {
    EncApp* app = malloc(sizeof(EncApp));

    app->gui			= furi_record_open(RECORD_GUI);
    //app->view_port		= view_port_alloc();
    //app->event_queue	= furi_message_queue_alloc(8, sizeof(InputEvent));

    enc_reader_setup_gpio_inputs(app);
    enc_reader_setup_view_settings(app);
    enc_reader_setup_view_main(app);
    enc_reader_setup_view_dispatcher(app);
    

    //gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    
    //view_port_input_callback_set(app->view_port, enc_reader_app_input_callback, app->event_queue);
    //view_port_draw_callback_set(app->view_port, enc_reader_app_draw_callback, app);

    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    return app;
}

void enc_reader_app_free(EncApp* app) {
    furi_assert(app);

    view_dispatcher_remove_view(app->view_dispatcher, ViewIndexSettings);
    view_dispatcher_remove_view(app->view_dispatcher, ViewIndexMain);

    view_dispatcher_free(app->view_dispatcher);

    widget_free(app->widget);
    submenu_free(app->submenu);

    furi_record_close(RECORD_NOTIFICATION);

    furi_hal_gpio_disable_int_callback(app->input_pin.a);
    furi_hal_gpio_remove_int_callback(app->input_pin.a);

    //furi_message_queue_free(app->event_queue);
    //view_port_enabled_set(app->view_port, false);
    //gui_remove_view_port(app->gui, app->view_port);
    //view_port_free(app->view_port);

    furi_record_close(RECORD_GUI);
}

static void enc_reader_app_run(EncApp* app) {
    // Display the Settings view on the screen.
    view_dispatcher_switch_to_view(app->view_dispatcher, ViewIndexSettings);
    // This function will block until enc_app_stop() is called.
    // Internally, it uses a FuriEventLoop (see FuriEventLoop examples for more info on this).
    view_dispatcher_run(app->view_dispatcher);
}

/*******************************************************************
 *                  vvv START HERE vvv
 *******************************************************************/
int32_t enc_reader_app(void *p) {
    UNUSED(p);
    EncApp* app = enc_reader_app_alloc();
    enc_reader_app_run(app);

    /*InputEvent event;

    bool running = true;

    while (running) {

        if (furi_message_queue_get(app->event_queue, &event, 200) == FuriStatusOk) {
            if (event.key == InputKeyBack) {
                if (event.type == InputTypeLong) running = false;
            } else if (event.key == InputKeyOk) {
                if (event.type == InputTypeShort) {
                    app->coordinates.org = app->coordinates.abs;
                    notification_message(app->notifications, &button_led_sequence);
                }
            } else if (event.key == InputKeyDown) {
                if (event.type == InputTypeShort) {
                    app->coordinates.org = 0;
                    app->coordinates.abs = 0;
                    notification_message(app->notifications, &button_led_sequence);
                }
            } else if (event.key == InputKeyUp) {
                if (event.type == InputTypeShort) {
                    app->Vbus_state = !app->Vbus_state;

                    if (app->Vbus_state)	{
                        furi_hal_power_enable_otg();
                        notification_message(app->notifications, &vOn_led_sequence);
                    } else {
                        furi_hal_power_disable_otg();
                        notification_message(app->notifications, &vOff_led_sequence);
                    }
                }
            }
        }
    }*/


    furi_hal_power_disable_otg();
    enc_reader_app_free(app);
    return 0;
}