#include "view_dispatcher.h"
//#include "view_config.h"
#include "view_main.h"

/*******************************************************************
 *                   VIEW DISPATCHER FUNCTIONS 
 *******************************************************************/
void app_change_view(EncApp* app, ViewIndex index) {
    if (index < ViewIndexMAX) {
        app->current_view = index;
        enc_reader_update_vbus_state(index == ViewIndexMain ? app->Vbus_state : VbusOFF);
        view_dispatcher_switch_to_view(app->view_dispatcher, index);
    }
}

static void enc_reader_app_tick_event_callback(void* context) {
    furi_assert(context);
    EncApp* app = context;
    app_view_main_redraw_callback(app);
}

static bool enc_reader_app_navigation_callback(void* context) { // This function is called when the user has pressed the Back key.
    furi_assert(context);
    EncApp* app = context;

    if (app->current_view == ViewIndexMain) {
        app_change_view(app, ViewIndexSettings);
    } else {
        view_dispatcher_stop(app->view_dispatcher);
    }
    return true;
}

void app_view_dispatcher_alloc(EncApp* app) {
    app->view_dispatcher = view_dispatcher_alloc();
    
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    view_dispatcher_add_view(app->view_dispatcher,  ViewIndexMain,      widget_get_view(app->widget));
    view_dispatcher_add_view(app->view_dispatcher,  ViewIndexSettings,  variable_item_list_get_view(app->var_item_list));

    view_dispatcher_set_tick_event_callback(app->view_dispatcher,       enc_reader_app_tick_event_callback, 40);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, enc_reader_app_navigation_callback);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
}

void app_view_dispatcher_free(EncApp* app) {
    furi_assert(app);
    
    view_dispatcher_remove_view(app->view_dispatcher, ViewIndexSettings);
    view_dispatcher_remove_view(app->view_dispatcher, ViewIndexMain);

    view_dispatcher_free(app->view_dispatcher);
}
