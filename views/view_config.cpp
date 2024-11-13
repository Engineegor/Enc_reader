#include "enc_reader.h"

#include "views/view_config.h"

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
        if(index == VbusON)         {notification_message(app->notifications, &vOn_led_sequence);}
        else if(index == VbusOFF)   {notification_message(app->notifications, &vOff_led_sequence);}
    }

    
}

static void view_settings_alloc(EncApp* app) {

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

static void view_settings_free(EncApp* app) {
    furi_assert(app);
    variable_item_list_free(app->var_item_list);
}