#include "enc_reader.h"

#include "views/view_config.h"
#include "views/view_main.h"
#include "views/view_dispatcher.h"

/*******************************************************************
 *                   ALLOC FUNCTIONS 
 *******************************************************************/
EncApp* enc_reader_app_alloc() {
    EncApp* app = malloc(sizeof(EncApp));

    app->gui = furi_record_open(RECORD_GUI);

    app_setup_gpio_inputs(app);

    app_view_settings_alloc(app);
    app_view_main_alloc(app);
    app_view_dispatcher_alloc(app);

    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    return app;
}

void enc_reader_app_free(EncApp* app) {
    furi_assert(app);

    app_view_dispatcher_free(app);
    app_view_settings_free(app);
    app_view_main_free(app);

    furi_hal_gpio_disable_int_callback(app->input_pin.a);
    furi_hal_gpio_remove_int_callback(app->input_pin.a);

    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);
}

static void enc_reader_app_run(EncApp* app) {

    app->resolution = ResolutionRaw;
    app->Vbus_state = VbusOFF;

    app_change_view(app, ViewIndexSettings);
    
    variable_item_set_current_value_index(app->var_item[ItemIndexResolution],   app->resolution);
    variable_item_set_current_value_index(app->var_item[ItemIndexVbusState],    app->Vbus_state);
    
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