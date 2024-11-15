#include "view_main.h"
#include "view_dispatcher.h"
#include "notifications.h"

/*******************************************************************
 *                   MAIN VIEW 
 *******************************************************************/
static void app_view_main_button_callback(GuiButtonType button_type, InputType input_type, void* context) {
    furi_assert(context);
    EncApp* app = context;
	
    if (input_type == InputTypeShort) {
        if (button_type == GuiButtonTypeLeft) {
            app_change_view(app, ViewIndexSettings);
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

void app_view_main_redraw_callback(EncApp* app) {
    if (app->current_view == ViewIndexMain) {

        static const uint8_t screen_width       = 128;
        static const uint8_t offset_vertical    = 2;
        static const uint8_t offset_horizontal  = 2;
        static const uint8_t offset_bottom      = 14;
        static const uint8_t gap  		        = 16;

        app->coordinates.rel = app->coordinates.abs - app->coordinates.org;

        char string_abs[] = "00000000 ";
        char string_rel[] = "00000000 ";

        int32_t asb_pos = app->coordinates.abs / resolution_list[app->resolution].divider;
        int32_t rel_pos = app->coordinates.rel / resolution_list[app->resolution].divider;

        snprintf(string_abs, strlen(string_abs), "%8d", (int)asb_pos);
        snprintf(string_rel, strlen(string_rel), "%8d", (int)rel_pos);

        widget_reset(app->widget);

        widget_add_frame_element(app->widget, 0, 0, screen_width, 64 - offset_bottom, 2);

        widget_add_string_element(app->widget, offset_horizontal,                   offset_vertical,            AlignLeft,  AlignTop, FontSecondary, app->Vbus_state == VbusON ? "5V ON " : "5V OFF");
        widget_add_string_element(app->widget, screen_width - offset_horizontal,    offset_vertical,            AlignRight, AlignTop, FontSecondary, resolution_list[app->resolution].text);

        widget_add_string_element(app->widget, offset_horizontal,                   offset_vertical + gap,      AlignLeft,  AlignTop, FontPrimary,   "Abs:");
        widget_add_string_element(app->widget, offset_horizontal,                   offset_vertical + gap * 2,  AlignLeft,  AlignTop, FontPrimary,   "Rel:");

        widget_add_string_element(app->widget, screen_width - offset_horizontal,    offset_vertical + gap,      AlignRight, AlignTop, FontBigNumbers, string_abs);
        widget_add_string_element(app->widget, screen_width - offset_horizontal,    offset_vertical + gap * 2,  AlignRight, AlignTop, FontBigNumbers, string_rel);

        widget_add_button_element(app->widget, GuiButtonTypeLeft,   "Config",   app_view_main_button_callback, app);
        widget_add_button_element(app->widget, GuiButtonTypeCenter, "Org",      app_view_main_button_callback, app);
        widget_add_button_element(app->widget, GuiButtonTypeRight,  "Reset",    app_view_main_button_callback, app);

    }
}

void app_view_main_alloc(EncApp* app) {
    app->widget     = widget_alloc();
}

void app_view_main_free(EncApp* app) {
    furi_assert(app);
    widget_free(app->widget);
}
