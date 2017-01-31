#include <pebble.h>

#define SETTINGS_KEY 1				// Persistent storage key

static Window *s_window;
static TextLayer *s_text_hour, *s_text_minute, *s_text_top, *s_text_bottom;
static GFont s_font_time, s_font_date;
static BitmapLayer *s_layer_battery, *s_layer_bluetooth, *s_layer_quiet;
static GBitmap *s_bitmap_battery, *s_bitmap_bluetooth, *s_bitmap_quiet;
static bool appStarted = false;
char b_buffer[16], b_bufferCustom[16];

typedef struct ClaySettings {
	GColor ColourBackground;
	GColor ColourHour;
	GColor ColourMinute;
	GColor ColourDate;
	bool ToggleBluetooth;
	bool ToggleBluetoothQuietTime;
	int SelectBluetooth;
	int SelectBatteryPercent;
	bool TogglePowerSave;
	bool ToggleSuffix;
	bool ToggleCalendarWeek;
	bool CheckDate0;
	bool CheckDate1;
	bool CheckDate2;
	bool CheckDate3;
	bool CheckDate4;
	bool CheckDate5;
} ClaySettings;						// Define our settings struct

static ClaySettings settings;		// An instance of the struct

static void config_default() {
	settings.ColourBackground	= GColorBlack;
	settings.ColourHour			= PBL_IF_BW_ELSE(GColorWhite,GColorChromeYellow);
	settings.ColourMinute		= GColorWhite;
	settings.ColourDate			= GColorWhite;
	settings.ToggleBluetooth	= true;
	settings.ToggleBluetoothQuietTime = false;
	settings.SelectBluetooth	  = 2;
	settings.SelectBatteryPercent = 0;
	settings.TogglePowerSave	= false;
	settings.ToggleSuffix		= false;
	settings.ToggleCalendarWeek	= false;
	settings.CheckDate0			= true;
	settings.CheckDate1			= false;
	settings.CheckDate2			= false;
	settings.CheckDate3			= false;
	settings.CheckDate4			= false;
	settings.CheckDate4			= false;
}

static void config_load() {
	config_default();													// Load the default settings
	persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));		// Read settings from persistent storage, if they exist
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
////  Methods  ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

void getIcon(GBitmap *bitmap, BitmapLayer *bitmapLayer, int imageBlack, int imageWhite) {
	gbitmap_destroy(bitmap);
	if (gcolor_equal(gcolor_legible_over(settings.ColourBackground), GColorBlack)) {
		bitmap = gbitmap_create_with_resource(imageBlack);
	} else {
		bitmap = gbitmap_create_with_resource(imageWhite);
	}
	bitmap_layer_set_bitmap(bitmapLayer, bitmap);	
}

void setColours() {
	window_set_background_color(s_window, settings.ColourBackground);																	// Set Background Colour
	text_layer_set_text_color(s_text_hour, PBL_IF_BW_ELSE(gcolor_legible_over(settings.ColourBackground), settings.ColourHour));		// Set Hour Colour
	text_layer_set_text_color(s_text_minute, PBL_IF_BW_ELSE(gcolor_legible_over(settings.ColourBackground), settings.ColourMinute));	// Set Minute Colour
	text_layer_set_text_color(s_text_top, PBL_IF_BW_ELSE(gcolor_legible_over(settings.ColourBackground), settings.ColourDate));			// Set Top Colour
	text_layer_set_text_color(s_text_bottom, PBL_IF_BW_ELSE(gcolor_legible_over(settings.ColourBackground), settings.ColourDate));		// Set Bottom Colour
}

void textToggle(bool hide) {
	layer_set_hidden(text_layer_get_layer(s_text_hour), !hide);
	layer_set_hidden(text_layer_get_layer(s_text_minute), !hide);
	layer_set_hidden(text_layer_get_layer(s_text_top), !hide);
	layer_set_hidden(text_layer_get_layer(s_text_bottom), !hide);
}

static void dateResetToCustom() {
	text_layer_set_text(s_text_bottom, b_bufferCustom);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
////  Time & Date  ///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void update_time() {
	time_t temp = time(NULL); 
	struct tm *tick_time = localtime(&temp);
	static char h_buffer[3], m_buffer[3];
	
	strftime(h_buffer, sizeof(h_buffer), clock_is_24h_style() ? "%H" : "%I", tick_time);	//%H %I
	strftime(m_buffer, sizeof(m_buffer), "%M", tick_time);		//%M
	text_layer_set_text(s_text_hour, h_buffer);
	text_layer_set_text(s_text_minute, m_buffer);
}

static void update_date() {
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	static char date_current[5], month_current[16], t_buffer[16];
	strftime(date_current, sizeof(date_current), "%d%m", tick_time);
	strftime(month_current, sizeof(month_current), "%B", tick_time);

// Top
	strftime(t_buffer, sizeof(t_buffer), "%A", tick_time);		// %A
// Bottom
	if(strcmp("0104", date_current) == 0) {	// April Fools
		strcpy(b_bufferCustom, "April  Fools");
	} else if(strcmp("0101", date_current) == 0 && settings.CheckDate0) { // New Year's Day
		strftime(b_bufferCustom, sizeof(b_bufferCustom), "Happy  %Y", tick_time);
	} else if(strcmp("2501", date_current) == 0 && settings.CheckDate1) { // Burns Night
		strcpy(b_bufferCustom, "Burns  Night");
	} else if(strcmp("3110", date_current) == 0 && settings.CheckDate2) { // Halloween
		strcpy(b_bufferCustom, "Halloween");
	} else if(strcmp("2412", date_current) == 0 && settings.CheckDate3) { // Christmas Eve
		#if PBL_DISPLAY_HEIGHT == 180			// Chalk
			strcpy(b_bufferCustom, "xmas  Eve");
		#else
			strcpy(b_bufferCustom, "Christmas  Eve");
		#endif
	} else if(strcmp("2512", date_current) == 0 && settings.CheckDate4) { // Christmas
		strcpy(b_bufferCustom, "Christmas");
	} else if(strcmp("2612", date_current) == 0 && settings.CheckDate5) { // Boxing Day
		strcpy(b_bufferCustom, "Boxing  Day");
	} else {
		strcpy(b_bufferCustom, "");
	}
			
// Mother's Day US				May ish
// Mother's Day UK				March ish
// Father's Day US
// Father's Day UK				June ish
// Independence Day (US)		4th July
// Thanksgiving					A Thursday between the 22nd and the 28th
// Black friday					Day after Thanksgiving
// Rememberence Sunday (UK)		A sunday from the 8th to the 14th november

// Day
	char char_buffer[16] = "%e";
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
	#if PBL_DISPLAY_HEIGHT == 180			// Chalk
		if(strlen(month_current) > 5) {
			strcat(char_buffer,"  %b"); 	// Short
		} else {
			strcat(char_buffer,"  %B");
		}
	#else									// Aplite, Basalt, Diorite
		if(strlen(month_current) > 7 || settings.ToggleCalendarWeek) {
			strcat(char_buffer,"  %b"); 	// Short
			if(settings.ToggleCalendarWeek) {
				strcat(char_buffer,"  W%V");
			}
		} else {
			strcat(char_buffer,"  %B");
		}
	#endif
	strftime(b_buffer, sizeof(char_buffer), char_buffer, tick_time);
	
	text_layer_set_text(s_text_top, t_buffer);
	if(strcmp(b_bufferCustom, "\0") != 0) {
		text_layer_set_text(s_text_bottom, b_bufferCustom);
	} else {
		text_layer_set_text(s_text_bottom, b_buffer);
	}
		
// 	APP_LOG(APP_LOG_LEVEL_DEBUG, b_buffer);
// 	APP_LOG(APP_LOG_LEVEL_DEBUG, b_bufferCustom);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	if(MINUTE_UNIT && units_changed) {
		if(!settings.TogglePowerSave) {
			update_time();	
		}
		if(quiet_time_is_active()) {
			getIcon(s_bitmap_quiet, s_layer_quiet, RESOURCE_ID_IMAGE_QUIET_TIME_BLACK, RESOURCE_ID_IMAGE_QUIET_TIME_WHITE);
			layer_set_hidden(bitmap_layer_get_layer(s_layer_quiet), false);	// Visible
		} else {
			layer_set_hidden(bitmap_layer_get_layer(s_layer_quiet), true);	// Hidden
			#if PBL_DISPLAY_HEIGHT == 180			// Chalk
				layer_set_frame(bitmap_layer_get_layer(s_layer_battery),GRect(84, 10, 13,  6));	// battery
			#else									// Aplite, Basalt, Diorite
				layer_set_frame(bitmap_layer_get_layer(s_layer_battery),GRect(6, 4, 13,  6));	// battery
			#endif
		}
	}
	if(DAY_UNIT && units_changed && !settings.TogglePowerSave) {
		update_date();
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
////  Callbacks  /////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
	if(settings.TogglePowerSave) {
		update_time();
		update_date();
		textToggle(true);     //Show
		AppTimer *updateTimer = app_timer_register(3500, (AppTimerCallback) textToggle, false); // Hide after 3 seconds
	} else {
		if(strcmp(b_bufferCustom, "\0") != 0) {
			text_layer_set_text(s_text_bottom, b_buffer);
			AppTimer *updateTimer = app_timer_register(3500, (AppTimerCallback) dateResetToCustom, NULL);
		}
	}
}

static void battery_callback(BatteryChargeState state) {
	if(state.charge_percent <= settings.SelectBatteryPercent) {
		getIcon(s_bitmap_battery, s_layer_battery, RESOURCE_ID_IMAGE_BATTERY_BLACK, RESOURCE_ID_IMAGE_BATTERY_WHITE);
		layer_set_hidden(bitmap_layer_get_layer(s_layer_battery), false);	// Visible
	} else {
		layer_set_hidden(bitmap_layer_get_layer(s_layer_battery), true);	// Hidden
	}
	
	// Move battery when in quiet time, and move to original location if not
	if(quiet_time_is_active()) {
		#if PBL_DISPLAY_HEIGHT == 180			// Chalk
			layer_set_frame(bitmap_layer_get_layer(s_layer_battery),GRect(84, 17, 13,  6));	// battery
		#else									// Aplite, Basalt, Diorite
			layer_set_frame(bitmap_layer_get_layer(s_layer_battery),GRect(22, 4, 13,  6));	// battery
		#endif
	} else {
		#if PBL_DISPLAY_HEIGHT == 180			// Chalk
			layer_set_frame(bitmap_layer_get_layer(s_layer_battery),GRect(84, 10, 13,  6));	// battery
		#else									// Aplite, Basalt, Diorite
			layer_set_frame(bitmap_layer_get_layer(s_layer_battery),GRect(6, 4, 13,  6));	// battery
		#endif
	}
}

static void bluetooth_callback(bool connected) {													  	
	if(!connected) {
		if(appStarted && !(quiet_time_is_active() && !settings.ToggleBluetoothQuietTime)) {
			if(settings.SelectBluetooth == 0) { }								// No vibration 
			else if(settings.SelectBluetooth == 1) { vibes_short_pulse(); }		// Short vibration
			else if(settings.SelectBluetooth == 2) { vibes_long_pulse(); }		// Long vibration
			else if(settings.SelectBluetooth == 3) { vibes_double_pulse(); }	// Double vibration
			else { vibes_long_pulse(); }					 // Default			// Long Vibration
		}
		if(settings.ToggleBluetooth) {
			getIcon(s_bitmap_bluetooth, s_layer_bluetooth, RESOURCE_ID_IMAGE_BLUETOOTH_BLACK, RESOURCE_ID_IMAGE_BLUETOOTH_WHITE);
			layer_set_hidden(bitmap_layer_get_layer(s_layer_bluetooth), false);	// Visible	
		}
	} else {
		layer_set_hidden(bitmap_layer_get_layer(s_layer_bluetooth), true);	// Hidden
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
// Bluetooth
	Tuple *bt_toggle_t = dict_find(iter, MESSAGE_KEY_TOGGLE_BLUETOOTH);
	if(bt_toggle_t) { settings.ToggleBluetooth = bt_toggle_t->value->int32 == 1; }
	Tuple *bq_toggle_t = dict_find(iter, MESSAGE_KEY_TOGGLE_BLUETOOTH_QUIET_TIME);
	if(bq_toggle_t) { settings.ToggleBluetoothQuietTime = bq_toggle_t->value->int32 == 1; }
	Tuple *bt_select_t = dict_find(iter, MESSAGE_KEY_SELECT_BLUETOOTH);
	if(bt_select_t) { settings.SelectBluetooth = atoi(bt_select_t->value->cstring); }
// Battery
	Tuple *bp_select_t = dict_find(iter, MESSAGE_KEY_SELECT_BATTERY_PERCENT);
	if(bp_select_t) { settings.SelectBatteryPercent = atoi(bp_select_t->value->cstring); }
	Tuple *bd_toggle_t = dict_find(iter, MESSAGE_KEY_TOGGLE_POWER_SAVE);
	if(bd_toggle_t) { settings.TogglePowerSave = bd_toggle_t->value->int32 == 1; }
// Bottom Text
	Tuple *su_toggle_t = dict_find(iter, MESSAGE_KEY_TOGGLE_SUFFIX);
	if(su_toggle_t) { settings.ToggleSuffix = su_toggle_t->value->int32 == 1; }
	Tuple *wk_toggle_t = dict_find(iter, MESSAGE_KEY_TOGGLE_WEEK);
	if(wk_toggle_t) { settings.ToggleCalendarWeek = wk_toggle_t->value->int32 == 1;	}
// Custom Text
	Tuple *dt0_check_t = dict_find(iter, MESSAGE_KEY_CHECK_DATE);
	if(dt0_check_t) { settings.CheckDate0 = dt0_check_t->value->uint16; }
	Tuple *dt1_check_t = dict_find(iter, MESSAGE_KEY_CHECK_DATE+1);
	if(dt1_check_t) { settings.CheckDate1 = dt1_check_t->value->uint16; }
	Tuple *dt2_check_t = dict_find(iter, MESSAGE_KEY_CHECK_DATE+2);
	if(dt2_check_t) { settings.CheckDate2 = dt2_check_t->value->uint16; }
	Tuple *dt3_check_t = dict_find(iter, MESSAGE_KEY_CHECK_DATE+3);
	if(dt3_check_t) { settings.CheckDate3 = dt3_check_t->value->uint16; }
	Tuple *dt4_check_t = dict_find(iter, MESSAGE_KEY_CHECK_DATE+4);
	if(dt4_check_t) { settings.CheckDate4 = dt4_check_t->value->uint16; }
	Tuple *dt5_check_t = dict_find(iter, MESSAGE_KEY_CHECK_DATE+5);
	if(dt5_check_t) { settings.CheckDate5 = dt5_check_t->value->uint16; }
	
  	persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));		// Write settings to persistent storage
	
// Update watchface with new settings
	battery_callback(battery_state_service_peek());
	appStarted = false;
	bluetooth_callback(connection_service_peek_pebble_app_connection());
	appStarted = true;
	setColours();
	
	if(settings.TogglePowerSave) {
		textToggle(false);    // Hide
	} else {
// 		update_time();
		update_date();
		textToggle(true);     // Show
	}

	if(quiet_time_is_active()) {
		getIcon(s_bitmap_quiet, s_layer_quiet, RESOURCE_ID_IMAGE_QUIET_TIME_BLACK, RESOURCE_ID_IMAGE_QUIET_TIME_WHITE);
		layer_set_hidden(bitmap_layer_get_layer(s_layer_quiet), false);	// Visible
	} else {
		layer_set_hidden(bitmap_layer_get_layer(s_layer_quiet), true);	// Hidden
	}
		
// 	APP_LOG(APP_LOG_LEVEL_DEBUG, "CheckDate0: %u", dt0_check_t->value->uint16);
// 	APP_LOG(APP_LOG_LEVEL_DEBUG, "checkdate0: %d", settings.CheckDate0);
}

void unobstructed_change(AnimationProgress progress, void* data) {
	GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(s_window));

	layer_set_frame(text_layer_get_layer(s_text_hour),	GRect(				0, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
	layer_set_frame(text_layer_get_layer(s_text_minute),GRect(bounds.size.w/2, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
	layer_set_frame(text_layer_get_layer(s_text_top),	GRect(				0, bounds.size.h / 4 - 31, bounds.size.w,   30));
	layer_set_frame(text_layer_get_layer(s_text_bottom),GRect(				0, bounds.size.h * 3/4 -5, bounds.size.w,   30));
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
////  Window  ////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void window_load(Window *window) {
	GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(window));
	setlocale(LC_ALL, "");

//Fonts
	s_font_time = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_BOLD_72));
	s_font_date = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_REGULAR_28));
	
// Locations
	#if PBL_DISPLAY_HEIGHT == 180			// Chalk
		s_text_hour		= text_layer_create(GRect(			   10, bounds.size.h / 2 - 47, bounds.size.w/2-10, 75));
		s_text_minute	= text_layer_create(GRect(bounds.size.w/2, bounds.size.h / 2 - 47, bounds.size.w/2-10, 75));
		s_text_top		= text_layer_create(GRect(				0, bounds.size.h / 4 - 26, bounds.size.w,	   30));
		s_text_bottom	= text_layer_create(GRect(				0, bounds.size.h * 3/4-10, bounds.size.w,	   30));
		s_layer_bluetooth = bitmap_layer_create(GRect(bounds.size.w/2-3, bounds.size.h-15, 7,  11));			// bluetooth										// TODO: fix this value
	#else									// Aplite, Basalt, Diorite
		s_text_hour		= text_layer_create(GRect(				0, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
		s_text_minute	= text_layer_create(GRect(bounds.size.w/2, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
		s_text_top		= text_layer_create(GRect(				0, bounds.size.h / 4 - 31, bounds.size.w,   30));
		s_text_bottom	= text_layer_create(GRect(				0, bounds.size.h * 3/4 -5, bounds.size.w,   30));
		s_layer_bluetooth = bitmap_layer_create(GRect(bounds.size.w-13, 3, 7,  11));							// bluetooth
	#endif
	
	// Show Quiet Time icon when active, and move battery 
	if(quiet_time_is_active()) {
		#if PBL_DISPLAY_HEIGHT == 180			// Chalk
			s_layer_battery	= bitmap_layer_create(GRect(84, 17, 13,  6));	// battery
			s_layer_quiet	= bitmap_layer_create(GRect(86,  3, 10, 10));	// quiet
		#else									// Aplite, Basalt, Diorite
			s_layer_battery		= bitmap_layer_create(GRect(22, 4, 13,  6));	// battery
			s_layer_quiet		= bitmap_layer_create(GRect( 6, 2, 10, 10));	// quiet
		#endif
		getIcon(s_bitmap_quiet, s_layer_quiet, RESOURCE_ID_IMAGE_QUIET_TIME_BLACK, RESOURCE_ID_IMAGE_QUIET_TIME_WHITE);
		layer_set_hidden(bitmap_layer_get_layer(s_layer_quiet), false);	// Visible
	} else {
		#if PBL_DISPLAY_HEIGHT == 180			// Chalk
			s_layer_battery	= bitmap_layer_create(GRect(84, 10, 13,  6));	// battery
			s_layer_quiet	= bitmap_layer_create(GRect(86,  2, 10, 10));	// quiet
		#else									// Aplite, Basalt, Diorite
			s_layer_battery	= bitmap_layer_create(GRect(6, 4, 13,  6));		// battery
			s_layer_quiet	= bitmap_layer_create(GRect(6, 2, 10, 10));		// quiet
		#endif
		layer_set_hidden(bitmap_layer_get_layer(s_layer_quiet), true);	// Hidden
	}

// Battery Icon
	layer_mark_dirty(bitmap_layer_get_layer(s_layer_battery));
	#if defined(PBL_COLOR)
		bitmap_layer_set_compositing_mode(s_layer_battery, GCompOpSet);	
	#endif
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_layer_battery));

// Battery Icon
	layer_mark_dirty(bitmap_layer_get_layer(s_layer_bluetooth));
	#if defined(PBL_COLOR)
		bitmap_layer_set_compositing_mode(s_layer_bluetooth, GCompOpSet);	
	#endif
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_layer_bluetooth));
	
// Quiet Time Icon
	layer_mark_dirty(bitmap_layer_get_layer(s_layer_quiet));
	#if defined(PBL_COLOR)
		bitmap_layer_set_compositing_mode(s_layer_quiet, GCompOpSet);	
	#endif
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_layer_quiet));
	
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
	battery_callback(battery_state_service_peek());
	appStarted = false;
	bluetooth_callback(connection_service_peek_pebble_app_connection());		// Sets date colours (and detects if a phone is connected)
	appStarted = true;
	setColours();
	update_time();
	update_date();
	
	if(settings.TogglePowerSave) {
		textToggle(false);    // Hide
	} else {
		textToggle(true);     // Show
	}
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
////  Other  /////////////////////////////////////////////////////////////////////////////////////////
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

	accel_tap_service_subscribe(accel_tap_handler);
	
	app_message_register_inbox_received(inbox_received_handler);
	app_message_open(256, 256);

	battery_state_service_subscribe(battery_callback);
	battery_callback(battery_state_service_peek());

	tick_timer_service_subscribe(MINUTE_UNIT | DAY_UNIT, tick_handler); 
}

static void deinit() {
	gbitmap_destroy(s_bitmap_battery);
	gbitmap_destroy(s_bitmap_bluetooth);
	gbitmap_destroy(s_bitmap_quiet);
	bitmap_layer_destroy(s_layer_battery);
	bitmap_layer_destroy(s_layer_bluetooth);
	bitmap_layer_destroy(s_layer_quiet);
	
	tick_timer_service_unsubscribe();
	window_destroy(s_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}

//	APP_LOG(APP_LOG_LEVEL_DEBUG, char_bufferCustom);


///////////////////////////////////////////////////////////////////////////
////  End  ////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////