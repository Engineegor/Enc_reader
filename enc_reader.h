#pragma once

#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>

#include <notification/notification.h>
#include <notification/notification_messages.h>

const NotificationSequence button_led_sequence = {
    &message_blue_255,
    &message_vibro_on,
    &message_delay_25,
    &message_vibro_off,
    &message_delay_250,
    &message_blue_0,
    NULL,
};
const NotificationSequence vOn_led_sequence = {
    &message_green_255,
    &message_vibro_on,
    &message_delay_25,
    &message_vibro_off,
    &message_delay_100,
    &message_vibro_on,
    &message_delay_25,
    &message_vibro_off,
    NULL,
};
const NotificationSequence vOff_led_sequence = {
    &message_red_255,
    &message_vibro_on,
    &message_delay_100,
    &message_delay_50,
    &message_vibro_off,
    NULL,
};

// Enumeration of the view indexes.
typedef enum {
    ViewIndexSettings,
    ViewIndexMain,
    ViewIndexMAX,
} ViewIndex;

// Enumeration of submenu items.
typedef enum {
    ItemIndexStart,
    ItemIndexResolution,
    ItemIndexVbusState,
    ItemIndexMAX,
} ItemIndex;

typedef enum {
    ResolutionRaw,
    Resolution50,
    Resolution100,
    Resolution200,
    ResolutionMAX,
} ResolutionIndex;

typedef struct {
    int32_t			divider;
    char*			text;
} resolution_t;

const resolution_t resolution_list[] = {
    {1,		"Raw"},
    {50,	"50"},
    {100,	"100"},
    {200,	"200"},
};

typedef enum {
    VbusON,
    VbusOFF,
    VbusStatesNum,
} VbusState;

const char* const vbus_state_list[] = {
    {"ON"},
    {"OFF"},
};

typedef struct EncApp{
    Gui* gui;
    NotificationApp* notifications;
    ViewDispatcher* view_dispatcher;

    View* main_view;
    Widget* widget;
    
    VariableItemList* var_item_list;

    struct {
        const GpioPin*	a;
        const GpioPin*	b;
    } input_pin;

    struct {
        int32_t abs;
        int32_t org;
        int32_t rel;
    } coordinates;

    VbusState 		Vbus_state;
    ResolutionIndex resolution;
    ViewIndex		current_view;

} EncApp;