#pragma once

#include <furi.h>
#include <notification/notification_messages.h>

const NotificationSequence button_led_sequence = {
    &message_blue_255,
    &message_vibro_on,
    &message_delay_10,
    &message_vibro_off,
    &message_delay_250,
    &message_blue_0,
    NULL,
};