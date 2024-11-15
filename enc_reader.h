#pragma once

#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>

#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>

#include <notification/notification.h>

#include "configs/configs.h"

typedef struct EncApp{
    Gui* gui;
    NotificationApp* notifications;
    
    VariableItemList* var_item_list;
    VariableItem* var_item[ItemIndexMAX];

    Widget* widget;
    WidgetElement* widget_element[IndexWidgetMAX];

    ViewDispatcher* view_dispatcher;

    struct {
        const GpioPin*	a;
        const GpioPin*	b;
    } input_pin;

    struct {
        int32_t abs;
        int32_t org;
        int32_t rel;
    } coordinates;

    VbusState       Vbus_state;
    ResolutionIndex resolution;
    ViewIndex       current_view;

} EncApp;
