#include "view_config.h"
#include "view_dispatcher.h"
#include "../configs/configs.h"

/*******************************************************************
 *                   SETTINGS VIEW 
 *******************************************************************/
static void variable_item_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    EncApp* app = context;
    if (index == ItemIndexStart) {
        app_change_view(app, ViewIndexMain);
    }
}

static void variable_item_change_callback(VariableItem* item) {
    EncApp* app     = variable_item_get_context(item);
    uint8_t index   = variable_item_get_current_value_index(item);

    if (item == app->var_item[ItemIndexResolution]) {
        variable_item_set_current_value_text(item, resolution_list[index].text);
        app->resolution = index;
    } else if (item == app->var_item[ItemIndexVbusState]) {
        variable_item_set_current_value_text(item, vbus_state_list[index]);
        app->Vbus_state = index;
    }

    
}

void app_view_settings_alloc(EncApp* app) {

    app->var_item_list = variable_item_list_alloc();

    variable_item_list_set_enter_callback(app->var_item_list, variable_item_enter_callback, app);

    app->var_item[ItemIndexStart]       = variable_item_list_add(app->var_item_list, "Start",         0,             NULL,                            NULL);
    app->var_item[ItemIndexResolution]  = variable_item_list_add(app->var_item_list, "Resolution",    ResolutionMAX, variable_item_change_callback,   app);
    app->var_item[ItemIndexVbusState]   = variable_item_list_add(app->var_item_list, "5V supply",     VbusStatesNum, variable_item_change_callback,   app);

    variable_item_set_current_value_text(app->var_item[ItemIndexResolution],    resolution_list[app->resolution].text);
    variable_item_set_current_value_text(app->var_item[ItemIndexVbusState],     vbus_state_list[app->Vbus_state]);
}

void app_view_settings_free(EncApp* app) {
    furi_assert(app);
    variable_item_list_free(app->var_item_list);
}
