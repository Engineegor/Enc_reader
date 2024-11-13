#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
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
    SubmenuIndexStart,
	SubmenuIndexResolution,
//	SubmenuIndexShowRaw,
    SubmenuIndexVbusState,
	SubmenuIndexMAX,
} SubmenuIndex;

typedef enum {
	ResolutionRaw,
	Resolution50,
	Resolution100,
	Resolution200,
	ResolutionMAX,
} ResolutionIndex;

const int32_t pos_divider[] = {
	1,
	50,
	100,
	200
};

typedef struct EncApp{
    Gui* gui;
    ViewPort* view_port;
	FuriMessageQueue* event_queue;
	NotificationApp* notifications;
	ViewDispatcher* view_dispatcher;
    Widget* widget;
    Submenu* submenu;

	struct {
		const GpioPin*	a;
		const GpioPin*	b;
	} input_pin;

	struct {
		int32_t abs;
		int32_t org;
		int32_t rel;
	} coordinates;

	bool Vbus_state;
	ResolutionIndex resolution;

} EncApp;