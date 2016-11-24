#include <pebble.h>

#define SETTINGS_KEY 1				// Persistent storage key

static Window *s_window;
static TextLayer *s_text_hour, *s_text_minute, *s_text_top, *s_text_bottom;
static GFont s_font_time, s_font_date;
static BitmapLayer *s_layer_battery;
static GBitmap *s_bitmap_battery;
char date_current[5], month_current[16];
static bool appStarted = false;

typedef struct ClaySettings {
	GColor ColourBackground;
	GColor ColourHour;
	GColor ColourMinute;
	GColor ColourDate;
	GColor ColourBluetooth;
	bool ToggleBluetoothQuietTime;
	int SelectBluetooth;
	int SelectBatteryPercent;
	bool ToggleSuffix;
	bool ToggleCalendarWeek;
} ClaySettings;						// Define our settings struct

static ClaySettings settings;		// An instance of the struct

static void config_default() {
	settings.ColourBackground	= GColorBlack;
	settings.ColourHour			= PBL_IF_BW_ELSE(GColorWhite,GColorChromeYellow);
	settings.ColourMinute		= GColorWhite;
	settings.ColourDate			= GColorWhite;
	settings.ColourBluetooth	= GColorRed;
	settings.ToggleBluetoothQuietTime = false;
	settings.SelectBluetooth	  = 2;
	settings.SelectBatteryPercent = 0;
	settings.ToggleSuffix		= false;
	settings.ToggleCalendarWeek	= false;
}

static void config_save() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));		// Write settings to persistent storage
}

static void config_load() {
	config_default();													// Load the default settings
	persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));		// Read settings from persistent storage, if they exist
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Methods /////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

void getBatteryIcon(int image_number) {
	int BatteryBlack[] = {RESOURCE_ID_IMAGE_BATTERY_BLACK_1, RESOURCE_ID_IMAGE_BATTERY_BLACK_2, RESOURCE_ID_IMAGE_BATTERY_BLACK_3, RESOURCE_ID_IMAGE_BATTERY_BLACK_4};
	int BatteryWhite[] = {RESOURCE_ID_IMAGE_BATTERY_WHITE_1, RESOURCE_ID_IMAGE_BATTERY_WHITE_2, RESOURCE_ID_IMAGE_BATTERY_WHITE_3, RESOURCE_ID_IMAGE_BATTERY_WHITE_4};
	
	gbitmap_destroy(s_bitmap_battery);
	if (gcolor_equal(gcolor_legible_over(settings.ColourBackground), GColorBlack)) {
		s_bitmap_battery = gbitmap_create_with_resource(BatteryBlack[image_number]);
	} else {
		s_bitmap_battery = gbitmap_create_with_resource(BatteryWhite[image_number]);
	}
	bitmap_layer_set_bitmap(s_layer_battery, s_bitmap_battery);
}

void setColours() {
	window_set_background_color(s_window, settings.ColourBackground);																	// Set Background Colour
	text_layer_set_text_color(s_text_hour, PBL_IF_BW_ELSE(gcolor_legible_over(settings.ColourBackground), settings.ColourHour));		// Set Hour Colour
	text_layer_set_text_color(s_text_minute, PBL_IF_BW_ELSE(gcolor_legible_over(settings.ColourBackground), settings.ColourMinute));	// Set Minute Colour
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Time & Date /////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void update_time() {
	time_t temp = time(NULL); 
	struct tm *tick_time = localtime(&temp);
	static char h_buffer[3], m_buffer[3], t_buffer[16], b_buffer[16];
	strftime(date_current, sizeof(date_current), "%d%m", tick_time);
	strftime(month_current, sizeof(month_current), "%B", tick_time);
	
	strftime(h_buffer, sizeof(h_buffer), clock_is_24h_style() ? "%H" : "%I", tick_time);	//%M %I
	strftime(m_buffer, sizeof(m_buffer), "%M", tick_time);		//%M
	text_layer_set_text(s_text_hour, h_buffer);
	text_layer_set_text(s_text_minute, m_buffer);

// Top
	strftime(t_buffer, sizeof(t_buffer), "%A", tick_time);		// %A
// Bottom
	char char_buffer[16] = "";
	if(strcmp("0104", date_current) == 0) {	// April Fools		Should this be hardcoded?
		strcpy(char_buffer, "April  Fools");
	} else {
// Day
		strcpy(char_buffer,"%e");
		if(settings.ToggleSuffix) {
			if (strncmp(date_current, "01", 2) == 0 || strncmp(date_current, "21", 2) == 0 || strncmp(date_current,"31",2) == 0) { 
				strcat(char_buffer,"st");
			} else if (strncmp(date_current, "02", 2) == 0 || strncmp(date_current, "22", 2) == 0) {
				strcat(char_buffer,"nd");
			} else if (strncmp(date_current, "03", 2) == 0 || strncmp(date_current, "23", 2) == 0) { 
				strcat(char_buffer,"rd");
			} else {
				strcat(char_buffer,"th");
			}
		}
// Month
		#if PBL_DISPLAY_HEIGHT == 228			// EMERY
			if(strlen(month_current) > 9 || settings.ToggleCalendarWeek) {
				strcat(char_buffer,"  %b"); 	// Short
				if(settings.ToggleCalendarWeek) {
					strcat(char_buffer,"  W%V");
				}
			} else {
				strcat(char_buffer,"  %B");
			}
		#elif PBL_DISPLAY_HEIGHT == 180			// Round
			if(strlen(month_current) > 5) {
				strcat(char_buffer,"  %b"); 	// Short
			} else {
				strcat(char_buffer,"  %B");
			}
		#else									// TIME and OG
			if(strlen(month_current) > 7 || settings.ToggleCalendarWeek) {
				strcat(char_buffer,"  %b"); 	// Short
				if(settings.ToggleCalendarWeek) {
					strcat(char_buffer,"  W%V");
				}
			} else {
				strcat(char_buffer,"  %B");
			}
		#endif
	}
	strftime(b_buffer, sizeof(char_buffer), char_buffer, tick_time);
	text_layer_set_text(s_text_top, t_buffer);
	text_layer_set_text(s_text_bottom, b_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Callbacks ///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void battery_callback(BatteryChargeState state) {
	if(settings.SelectBatteryPercent == 100) {
		if(75 < state.charge_percent && state.charge_percent <= 100) {
			getBatteryIcon(3);
		} else if(50 < state.charge_percent && state.charge_percent <= 75) {
			getBatteryIcon(2);
		} else if(25 < state.charge_percent && state.charge_percent <= 50) {
			getBatteryIcon(1);
		} else if( 0 < state.charge_percent && state.charge_percent <= 25) {
			getBatteryIcon(0);
		}
		layer_set_hidden(bitmap_layer_get_layer(s_layer_battery), false);		// Visible
	} else {
		if(state.charge_percent <= settings.SelectBatteryPercent) {
			getBatteryIcon(0);
			layer_set_hidden(bitmap_layer_get_layer(s_layer_battery), false);	// Visible
		} else {
			layer_set_hidden(bitmap_layer_get_layer(s_layer_battery), true);	// Hidden
		}
	}
}

// bool vibrateBool() {
// 	if(quiet_time_is_active() && settings.ToggleBluetoothQuietTime) {					// True True
// 		return true;
// 	} else if(!quiet_time_is_active() && settings.ToggleBluetoothQuietTime) {			// False True
// 		return true;
// 	} else if(quiet_time_is_active() && !settings.ToggleBluetoothQuietTime) {			// True False
// 		return false;
// 	} else if(!quiet_time_is_active() && !settings.ToggleBluetoothQuietTime) {			// False False
// 		return true;
// 	} else {
// 		return false;
// 	}
// }

bool vibrateBool() {
	if(quiet_time_is_active() && !settings.ToggleBluetoothQuietTime) {			// True False
		return false;
	} else {
		return true;
	}
}

static void bluetooth_callback(bool connected) {													  	
	if(!connected) {
		text_layer_set_text_color(s_text_top, PBL_IF_BW_ELSE(settings.ColourBackground, settings.ColourBluetooth));		// Set Top Colour
		text_layer_set_text_color(s_text_bottom, PBL_IF_BW_ELSE(settings.ColourBackground, settings.ColourBluetooth));	// Set Bottom Colour
		if(appStarted && vibrateBool()) {	
			if(settings.SelectBluetooth == 0) { }								// No vibration 
			else if(settings.SelectBluetooth == 1) { vibes_short_pulse(); }		// Short vibration
			else if(settings.SelectBluetooth == 2) { vibes_long_pulse(); }		// Long vibration
			else if(settings.SelectBluetooth == 3) { vibes_double_pulse(); }	// Double vibration
			else { vibes_long_pulse(); }					 // Default // Long Vibration
		}
	} else {
		text_layer_set_text_color(s_text_top, PBL_IF_BW_ELSE(gcolor_legible_over(settings.ColourBackground), settings.ColourDate));		// Set Top Colour
		text_layer_set_text_color(s_text_bottom, PBL_IF_BW_ELSE(gcolor_legible_over(settings.ColourBackground), settings.ColourDate));	// Set Bottom Colour
	}
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
// Colours
	Tuple *bg_colour_t = dict_find(iter, MESSAGE_KEY_COLOUR_BACKGROUND);
	if(bg_colour_t) { settings.ColourBackground = GColorFromHEX(bg_colour_t->value->int32); }
	Tuple *hr_colour_t = dict_find(iter, MESSAGE_KEY_COLOUR_HOUR);
	if(hr_colour_t) { settings.ColourHour = GColorFromHEX(hr_colour_t->value->int32); }
	Tuple *mn_colour_t = dict_find(iter, MESSAGE_KEY_COLOUR_MINUTE);
	if(mn_colour_t) { settings.ColourMinute = GColorFromHEX(mn_colour_t->value->int32); }
	Tuple *dt_colour_t = dict_find(iter, MESSAGE_KEY_COLOUR_DATE);
	if(dt_colour_t) { settings.ColourDate = GColorFromHEX(dt_colour_t->value->int32); }
	Tuple *bt_colour_t = dict_find(iter, MESSAGE_KEY_COLOUR_BLUETOOTH);
	if(bt_colour_t) { settings.ColourBluetooth = GColorFromHEX(bt_colour_t->value->int32); }
// Bluetooth
	Tuple *bq_toggle_t = dict_find(iter, MESSAGE_KEY_TOGGLE_BLUETOOTH_QUIET_TIME);
	if(bq_toggle_t) { settings.ToggleBluetoothQuietTime = bq_toggle_t->value->int32 == 1; }
	Tuple *bt_select_t = dict_find(iter, MESSAGE_KEY_SELECT_BLUETOOTH);
	if(bt_select_t) { settings.SelectBluetooth = atoi(bt_select_t->value->cstring); }			// Pretty sure this shouldnt need to convert to a string
// Battery
	Tuple *bp_select_t = dict_find(iter, MESSAGE_KEY_SELECT_BATTERY_PERCENT);
	if(bp_select_t) { settings.SelectBatteryPercent = atoi(bp_select_t->value->cstring); }
// Bottom Text
	Tuple *su_toggle_t = dict_find(iter, MESSAGE_KEY_TOGGLE_SUFFIX);
	if(su_toggle_t) { settings.ToggleSuffix = su_toggle_t->value->int32 == 1; }
	Tuple *wk_toggle_t = dict_find(iter, MESSAGE_KEY_TOGGLE_WEEK);
	if(wk_toggle_t) { settings.ToggleCalendarWeek = wk_toggle_t->value->int32 == 1;	}
	
// 	APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", settings.SelectBatteryPercent);
	
	config_save();
	
	battery_callback(battery_state_service_peek());
	appStarted = false;
	bluetooth_callback(connection_service_peek_pebble_app_connection());		// Sets date colours (and detects if a phone is connected)
	appStarted = true;
	setColours();
	update_time();
}

void unobstructed_change(AnimationProgress progress, void* data) {
	GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(s_window));

	#if PBL_DISPLAY_HEIGHT == 228			// EMERY
		layer_set_frame(text_layer_get_layer(s_text_hour),	GRect(				0, bounds.size.h / 2 - 47 - 10, bounds.size.w/2, 95));
		layer_set_frame(text_layer_get_layer(s_text_minute),GRect(bounds.size.w/2, bounds.size.h / 2 - 47 - 10, bounds.size.w/2, 95));
		layer_set_frame(text_layer_get_layer(s_text_top),	GRect(				0, bounds.size.h / 4 - 31 -  7, bounds.size.w,   40));
		layer_set_frame(text_layer_get_layer(s_text_bottom),GRect(				0, bounds.size.h * 3/4 -5 -  1, bounds.size.w,   40));
	#else									// TIME and OG	
		layer_set_frame(text_layer_get_layer(s_text_hour),	GRect(				0, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
		layer_set_frame(text_layer_get_layer(s_text_minute),GRect(bounds.size.w/2, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
		layer_set_frame(text_layer_get_layer(s_text_top),	GRect(				0, bounds.size.h / 4 - 31, bounds.size.w,   30));
		layer_set_frame(text_layer_get_layer(s_text_bottom),GRect(				0, bounds.size.h * 3/4 -5, bounds.size.w,   30));
	#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Window //////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void window_load(Window *window) {
	GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(window));
	setlocale(LC_ALL, "");

// Locations
	#if PBL_DISPLAY_HEIGHT == 228			// EMERY
		s_font_time = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_BOLD_88));
		s_font_date = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_REGULAR_34));
	
		s_text_hour		= text_layer_create(GRect(				0, bounds.size.h / 2 - 47 - 10, bounds.size.w/2, 95));
		s_text_minute	= text_layer_create(GRect(bounds.size.w/2, bounds.size.h / 2 - 47 - 10, bounds.size.w/2, 95));
		s_text_top		= text_layer_create(GRect(				0, bounds.size.h / 4 - 31 -  7, bounds.size.w,   40));
		s_text_bottom	= text_layer_create(GRect(				0, bounds.size.h * 3/4 -5 -  1, bounds.size.w,   40));
		s_layer_battery	= bitmap_layer_create(GRect(4, 4, 13, 6)); // battery
	#elif PBL_DISPLAY_HEIGHT == 180			// Round
// 	#if PBL_DISPLAY_HEIGHT == 180			// Round	
		s_font_time = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_BOLD_72));
		s_font_date = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_REGULAR_28));
	
		s_text_hour		= text_layer_create(GRect(			   10, bounds.size.h / 2 - 47, bounds.size.w/2-10, 75));
		s_text_minute	= text_layer_create(GRect(bounds.size.w/2, bounds.size.h / 2 - 47, bounds.size.w/2-10, 75));
		s_text_top		= text_layer_create(GRect(				0, bounds.size.h / 4 - 31+5, bounds.size.w,    30));
		s_text_bottom	= text_layer_create(GRect(				0, bounds.size.h * 3/4 -5-5, bounds.size.w,    30));
		s_layer_battery	= bitmap_layer_create(GRect(84, 4, 13, 6)); // battery
	#else									// TIME and OG
		s_font_time = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_BOLD_72));
		s_font_date = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_REGULAR_28));
	
		s_text_hour		= text_layer_create(GRect(				0, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
		s_text_minute	= text_layer_create(GRect(bounds.size.w/2, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
		s_text_top		= text_layer_create(GRect(				0, bounds.size.h / 4 - 31, bounds.size.w,   30));
		s_text_bottom	= text_layer_create(GRect(				0, bounds.size.h * 3/4 -5, bounds.size.w,   30));
		s_layer_battery	= bitmap_layer_create(GRect(4, 4, 13, 6)); // battery
	#endif

// Battery Image
	layer_mark_dirty(bitmap_layer_get_layer(s_layer_battery));
	#if defined(PBL_COLOR)
		bitmap_layer_set_compositing_mode(s_layer_battery, GCompOpSet);	
	#endif
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_layer_battery));
		
// Hour
	text_layer_set_font(s_text_hour, s_font_time);
	text_layer_set_text_alignment(s_text_hour, GTextAlignmentCenter);
	text_layer_set_background_color(s_text_hour, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_text_hour));
	
// Minutes
	text_layer_set_font(s_text_minute, s_font_time);
	text_layer_set_text_alignment(s_text_minute, GTextAlignmentCenter);
	text_layer_set_background_color(s_text_minute, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_text_minute));

// Top
	text_layer_set_font(s_text_top, s_font_date);
	text_layer_set_text_alignment(s_text_top, GTextAlignmentCenter);
	text_layer_set_background_color(s_text_top, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_text_top));

// Bottom
	text_layer_set_font(s_text_bottom, s_font_date);
	text_layer_set_text_alignment(s_text_bottom, GTextAlignmentCenter);
	text_layer_set_background_color(s_text_bottom, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_text_bottom));

// Standard
// 	APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", settings.SelectBatteryPercent);
	battery_callback(battery_state_service_peek());
	appStarted = false;
	bluetooth_callback(connection_service_peek_pebble_app_connection());		// Sets date colours (and detects if a phone is connected)
	appStarted = true;
	setColours();
	update_time();
}

static void window_unload(Window *window) {
	text_layer_destroy(s_text_hour);
	text_layer_destroy(s_text_minute);
	text_layer_destroy(s_text_top);
	text_layer_destroy(s_text_bottom);
	fonts_unload_custom_font(s_font_time);
	fonts_unload_custom_font(s_font_date);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Other ///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void init() {
	config_load();

	s_window = window_create();
	window_set_window_handlers(s_window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload
	});
	window_stack_push(s_window, true);

	UnobstructedAreaHandlers handlers = {
		.change = unobstructed_change,
	};
	unobstructed_area_service_subscribe(handlers, NULL);

	connection_service_subscribe((ConnectionHandlers) {
  		.pebble_app_connection_handler = bluetooth_callback
	});

	app_message_register_inbox_received(inbox_received_handler);
	app_message_open(256, 256);

	battery_state_service_subscribe(battery_callback);
	battery_callback(battery_state_service_peek());

	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
	gbitmap_destroy(s_bitmap_battery);
	bitmap_layer_destroy(s_layer_battery);
	
	tick_timer_service_unsubscribe();
	window_destroy(s_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}

//	APP_LOG(APP_LOG_LEVEL_DEBUG, select_battery_percent);