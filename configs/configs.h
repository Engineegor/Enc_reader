#pragma once

#include <furi.h>

typedef enum {
    ViewIndexSettings,
    ViewIndexMain,
    ViewIndexMAX,
} ViewIndex;

typedef enum {
    ItemIndexStart,
    ItemIndexResolution,
    ItemIndexVbusState,
    ItemIndexMAX,
} ItemIndex;

typedef enum {
    IndexWidgetAbs,
    IndexWidgetRel,
    IndexWidgetMAX,
} WidgetIndex;

typedef struct {
    const int32_t divider;
    const char* text;
} resolution_t;

extern const resolution_t resolution_list[];

typedef enum {
    ResolutionRaw,
    Resolution50,
    Resolution100,
    Resolution200,
    ResolutionMAX,
} ResolutionIndex;

typedef enum {
    VbusOFF,
    VbusON,
    VbusStatesNum,
} VbusState;

extern const char* const vbus_state_list[];
