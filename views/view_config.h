//#pragma onse

#include "enc_reader.h"

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

const int32_t pos_divider[] = {
	1,
	50,
	100,
	200
};

const char* const resolution_text[ResolutionMAX] = {
    "Raw",
    "50",
	"100",
	"200",
};

typedef enum {
    VbusON,
    VbusOFF,
    VbusStatesNum,
} VbusState;

const char* const gpio_vbus_text[VbusStatesNum] = {
    "OFF",
    "ON",
};

static void view_settings_alloc(EncApp* app);
static void view_settings_free(EncApp* app);