#include "enc_reader.h"

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_gpio.h>
#include <furi_hal_power.h>

#include <gui/gui.h>
//#include <gui/elements.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/widget.h>

#include <input/input.h>

#include <notification/notification.h>


/*static void enc_reader_app_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    EncApp* app = context;

    char abs_coord[] = "00000000 ";
    char rel_coord[] = "00000000 ";

    app->coordinates.rel = app->coordinates.abs - app->coordinates.org;
    int32_t abs_pos = app->coordinates.abs;
    int32_t rel_pos = app->coordinates.rel;

    if (app->resolution > ResolutionRaw) {
        abs_pos /= pos_divider[app->resolution];
        rel_pos /= pos_divider[app->resolution];
    }

    snprintf(abs_coord, strlen(abs_coord), "%8d", (int)abs_pos);
    snprintf(rel_coord, strlen(rel_coord), "%8d", (int)rel_pos);


    static const uint8_t offset_vertical	= 2;
    static const uint8_t offset_horizontal	= 2;
    static const uint8_t offset_bottom      = 14;
    static const uint8_t gap  		        = 16;
    static const uint8_t gap_horizontal     = 26;

    if (app->current_view == ViewIndexMain) {
        canvas_clear(canvas);
        canvas_set_font(canvas, FontSecondary);

        elements_frame(canvas, 0, 0, 128, 64 - offset_bottom);

        elements_multiline_text_aligned(canvas, offset_horizontal, offset_vertical + gap,       AlignLeft, AlignTop, "Abs:");
        elements_multiline_text_aligned(canvas, offset_horizontal, offset_vertical + gap * 2,   AlignLeft, AlignTop, "Rel:");
        
        canvas_set_font(canvas, FontBigNumbers);
        elements_multiline_text_aligned(canvas, offset_horizontal + gap_horizontal, offset_vertical + gap,      AlignLeft, AlignTop, (const char*)abs_coord);
        elements_multiline_text_aligned(canvas, offset_horizontal + gap_horizontal, offset_vertical + gap * 2,  AlignLeft, AlignTop, (const char*)rel_coord);
    }
}*/

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
    } else if (state == VbusOff) {
        furi_hal_power_disable_otg();
        //notification_message(app->notifications, &vOff_led_sequence);
    }
}

/*******************************************************************
 *                   SETTINGS VIEW 
 *******************************************************************/
static void variable_item_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    EncApp* app = context;
    if (index == ItemIndexStart) {
        app->current_view = ViewIndexMain;
        view_dispatcher_send_custom_event(app->view_dispatcher, ViewIndexMain);
    }
}

static void variable_item_change_callback(VariableItem* item) {
    EncApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    if (index == ItemIndexResolution) {
        variable_item_set_current_value_text(item, resolution_text[index]);
        app->resolution = index;
    } else if (index == ItemIndexVbusState) {
        variable_item_set_current_value_text(item, gpio_vbus_text[index]);
        enc_reader_update_vbus_state(index);
        if(index == VbusOff) {
            notification_message(app->notifications, &vOn_led_sequence);
        } else if(index == VbusON) {
            notification_message(app->notifications, &vOff_led_sequence);
        }
    }

    
}

static void enc_reader_setup_view_settings(EncApp* app) {

    app->var_item_list = variable_item_list_alloc();

    variable_item_list_set_enter_callback(app->var_item_list, variable_item_enter_callback, app);

    variable_item_list_add(app->var_item_list, "Start",         0,                  NULL, NULL);
    VariableItem* item_res = variable_item_list_add(app->var_item_list, "Resolution",    ResolutionMAX, variable_item_change_callback, app);
    VariableItem* item_sup = variable_item_list_add(app->var_item_list, "5V supply",     VbusStatesNum, variable_item_change_callback, app);

    variable_item_set_current_value_index(item_res, app->resolution);
    variable_item_set_current_value_index(item_sup, app->Vbus_state);

    variable_item_set_current_value_text(item_res, resolution_text[variable_item_get_current_value_index(item_res)]);
    variable_item_set_current_value_text(item_sup, gpio_vbus_text[variable_item_get_current_value_index(item_sup)]);
}

/*******************************************************************
 *                   MAIN VIEW 
 *******************************************************************/
static void enc_reader_app_button_callback(GuiButtonType button_type, InputType input_type, void* context) {
    furi_assert(context);
    EncApp* app = context;
    // Only request the view switch if the user short-presses the Center button.
    if (input_type == InputTypeShort) {
        if (button_type == GuiButtonTypeLeft) {
            enc_reader_update_vbus_state(false);
            app->current_view = ViewIndexSettings;
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

    static const uint8_t offset_vertical	= 2;
    static const uint8_t offset_horizontal	= 2;
    static const uint8_t offset_bottom      = 14;
    static const uint8_t gap  		        = 16;
    static const uint8_t gap_horizontal     = 26;

    widget_add_frame_element(app->widget, 0, 0, 128, 64 - offset_bottom, 2);

    widget_add_string_element(app->widget, offset_horizontal, offset_vertical + gap,        AlignLeft, AlignTop, FontSecondary, "Abs:");
    widget_add_string_element(app->widget, offset_horizontal, offset_vertical + gap * 2,    AlignLeft, AlignTop, FontSecondary, "Rel:");

    widget_add_string_element(app->widget, offset_horizontal + gap_horizontal, offset_vertical + gap,        AlignLeft, AlignTop, FontBigNumbers, "00000000");
    widget_add_string_element(app->widget, offset_horizontal + gap_horizontal, offset_vertical + gap * 2,    AlignLeft, AlignTop, FontBigNumbers, "00000000");

    widget_add_button_element(app->widget, GuiButtonTypeLeft,   "Config",   enc_reader_app_button_callback, app);
    widget_add_button_element(app->widget, GuiButtonTypeCenter, "Org",      enc_reader_app_button_callback, app);
    widget_add_button_element(app->widget, GuiButtonTypeRight,  "Set 0",    enc_reader_app_button_callback, app);
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

static void enc_reader_setup_view_dispatcher(EncApp* app) {
    app->view_dispatcher = view_dispatcher_alloc();
    
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    view_dispatcher_add_view(app->view_dispatcher,  ViewIndexMain,      widget_get_view(app->widget));
    view_dispatcher_add_view(app->view_dispatcher,  ViewIndexSettings,  variable_item_list_get_view(app->var_item_list));

    view_dispatcher_set_tick_event_callback(app->view_dispatcher,       enc_reader_app_tick_event_callback, 40);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher,     enc_reader_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, enc_reader_app_navigation_callback);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
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
    enc_reader_setup_view_settings(app);
    enc_reader_setup_view_main(app);
    enc_reader_setup_view_dispatcher(app);
    
    //gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    
    //view_port_draw_callback_set(app->view_port, enc_reader_app_draw_callback, app);
    //view_port_input_callback_set(app->view_port, enc_reader_app_input_callback, app->event_queue);

    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    return app;
}

void enc_reader_app_free(EncApp* app) {
    furi_assert(app);

    view_dispatcher_remove_view(app->view_dispatcher, ViewIndexSettings);
    view_dispatcher_remove_view(app->view_dispatcher, ViewIndexMain);

    view_dispatcher_free(app->view_dispatcher);

    widget_free(app->widget);
    variable_item_list_free(app->var_item_list);

    furi_record_close(RECORD_NOTIFICATION);

    furi_hal_gpio_disable_int_callback(app->input_pin.a);
    furi_hal_gpio_remove_int_callback(app->input_pin.a);

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