#include "enc_reader.h"

#include "views/view_main.h"

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

static void view_main_alloc(EncApp* app) {
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

static void view_main_free(EncApp* app) {
    furi_assert(app);
    widget_free(app->widget);
}
