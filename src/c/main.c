#include <pebble.h>

static Window *s_window;
static Layer *s_layer_time_date;
static TextLayer *s_text_hour, *s_text_minute, *s_text_top, *s_text_bottom;
static GFont s_font_time, s_font_date;
static BitmapLayer *s_layer_battery;
static GBitmap *s_bitmap_battery;
char date_current[5], month_current[16];

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Methods /////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

void getBatteryIcon(int colour_background, int image_number) {
	int BatteryBlack[] = {RESOURCE_ID_IMAGE_BATTERY_BLACK_1, RESOURCE_ID_IMAGE_BATTERY_BLACK_2, RESOURCE_ID_IMAGE_BATTERY_BLACK_3, RESOURCE_ID_IMAGE_BATTERY_BLACK_4};
	int BatteryWhite[] = {RESOURCE_ID_IMAGE_BATTERY_WHITE_1, RESOURCE_ID_IMAGE_BATTERY_WHITE_2, RESOURCE_ID_IMAGE_BATTERY_WHITE_3, RESOURCE_ID_IMAGE_BATTERY_WHITE_4};
	
	gbitmap_destroy(s_bitmap_battery);
 	if(colour_background) {
		if (gcolor_equal(gcolor_legible_over(GColorFromHEX(colour_background)), GColorBlack)) {
			s_bitmap_battery = gbitmap_create_with_resource(BatteryBlack[image_number]);
		} else {
			s_bitmap_battery = gbitmap_create_with_resource(BatteryWhite[image_number]);
		}
	} else {
		s_bitmap_battery = gbitmap_create_with_resource(BatteryWhite[image_number]);
	}
	bitmap_layer_set_bitmap(s_layer_battery, s_bitmap_battery);
}

void customText(char t_buffer[16], char b_buffer[16]) {
	int check_date_0 = persist_read_int(MESSAGE_KEY_CHECK_DATE);
	int check_date_1 = persist_read_int(MESSAGE_KEY_CHECK_DATE+1);
	int check_date_2 = persist_read_int(MESSAGE_KEY_CHECK_DATE+2);

	if(strcmp("0104", date_current) == 0) {		// If date is 1st April, Display "April Fools" on bottom
		strcpy(t_buffer, "\0");
		strcpy(b_buffer, "April  Fools");
	} else if(check_date_0 == 1 && strcmp("0101", date_current) == 0) {  // New Years
		strcpy(t_buffer, "Happy");
		strcpy(b_buffer, "New  Year");
	} else if(check_date_1 == 1 && strcmp("3110", date_current) == 0) {  // Halloween
		strcpy(t_buffer, "\0");
		strcpy(b_buffer, "Halloween");
	} else if(check_date_2 == 1 && strcmp("2512", date_current) == 0) {  // Christmas
		strcpy(t_buffer, "Merry");
		strcpy(b_buffer, "Christmas");
	}
	
	// Valentines				14th February
	// Earth Day				
	// Mother's Day US			May ish
	// Mother's Day UK			March ish
	// Father's Day US			
	// Father's Day UK			June ish
	// Independence Day (US)	4th July
	// Independence Day (MEX)
	// Canada Day				
	// Victoria Day (Canada)	
	// Thanksgiving				3rd thursday in november?
	// black friday??
	// Eid
	// Diwali
	// Boxing Day				26th December
	// Hannukah					december
	// Passover					
	// Chinese New Year
	// Kwanzaa
	// MLK Day
	
	
	// burns night				25h January
	// rememberence sunday could be from the 8th to the 14th november
	else {
		strcpy(t_buffer, "\0");
		strcpy(b_buffer, "\0");
	}
}

void setColours(int colour_background, int colour_hour, int colour_minute) {
	if(colour_background || colour_hour) {
		GColor bg_colour = GColorFromHEX(colour_background);
		window_set_background_color(s_window, bg_colour);			// Set Background Colour
		#if defined(PBL_COLOR)
			text_layer_set_text_color(s_text_hour, GColorFromHEX(colour_hour));		// Set Hour Colour
			text_layer_set_text_color(s_text_minute, GColorFromHEX(colour_minute));	// Set Minute Colour
		#elif defined(PBL_BW)	
			text_layer_set_text_color(s_text_hour, gcolor_legible_over(bg_colour));	// Set Hour Colour
			text_layer_set_text_color(s_text_minute, gcolor_legible_over(bg_colour));	// Set Minute Colour
		#endif
	} else {
		window_set_background_color(s_window, GColorBlack);			// Set Background Colour
		text_layer_set_text_color(s_text_hour, GColorWhite);		// Set Hour Colour
		text_layer_set_text_color(s_text_minute, GColorWhite);		// Set Minute Colour
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Callbacks ///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

void unobstructed_change(AnimationProgress progress, void* data) {
	GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(s_window));

	layer_set_frame(text_layer_get_layer(s_text_hour),GRect(				 0, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
	layer_set_frame(text_layer_get_layer(s_text_minute),GRect(bounds.size.w/2, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
	layer_set_frame(text_layer_get_layer(s_text_top),GRect(				 0, bounds.size.h / 4 - 31, bounds.size.w,   30));
	layer_set_frame(text_layer_get_layer(s_text_bottom),GRect(				 0, bounds.size.h * 3/4 -5, bounds.size.w,   30));
}

static void battery_callback(BatteryChargeState state) {
	char *select_battery_percent = "";
 	persist_read_string(MESSAGE_KEY_SELECT_BATTERY_PERCENT,select_battery_percent,5);
	int select_battery_percent_int = atoi(select_battery_percent);
	int colour_background = persist_read_int(MESSAGE_KEY_COLOUR_BACKGROUND);
	
	if(!select_battery_percent_int == 0) { // Show
		if(select_battery_percent_int == 100) { // Always show battery
			if(75 < state.charge_percent && state.charge_percent <= 100) {
				getBatteryIcon(colour_background, 3);
			} else if(50 < state.charge_percent && state.charge_percent <= 75) {
				getBatteryIcon(colour_background, 2);
			} else if(25 < state.charge_percent && state.charge_percent <= 50) {
				getBatteryIcon(colour_background, 1);
			} else if( 0 < state.charge_percent && state.charge_percent <= 25) {
				getBatteryIcon(colour_background, 0);
			}
			layer_set_hidden(bitmap_layer_get_layer(s_layer_battery), false);	// Visible
		} else {
			if(state.charge_percent < select_battery_percent_int) {
				getBatteryIcon(colour_background, 0);
				layer_set_hidden(bitmap_layer_get_layer(s_layer_battery), false);	// Visible
			} else {
				layer_set_hidden(bitmap_layer_get_layer(s_layer_battery), true);	// Hidden
			}
		}
	} else {
		layer_set_hidden(bitmap_layer_get_layer(s_layer_battery), true);	// Hidden
	}
}

static void bluetooth_callback(bool connected) {
	char *select_bluetooth_disconnect = "";
 	persist_read_string(MESSAGE_KEY_SELECT_BLUETOOTH_DISCONNECT,select_bluetooth_disconnect,5);
	int select_bluetooth_disconnect_int = atoi(select_bluetooth_disconnect);
	int colour_background = persist_read_int(MESSAGE_KEY_COLOUR_BACKGROUND);
	int colour_date = persist_read_int(MESSAGE_KEY_COLOUR_DATE);
	int colour_bluetooth = persist_read_int(MESSAGE_KEY_COLOUR_BLUETOOTH);
	
	if(!connected) {												
		if(colour_background || colour_bluetooth) {	// Disconected with config
			text_layer_set_text_color(s_text_top, PBL_IF_BW_ELSE(gcolor_legible_over(GColorFromHEX(colour_background)), GColorFromHEX(colour_bluetooth)));		// Set Top Colour
			text_layer_set_text_color(s_text_bottom, PBL_IF_BW_ELSE(gcolor_legible_over(GColorFromHEX(colour_background)), GColorFromHEX(colour_bluetooth)));	// Set Bottom Colour
		} else { 									// Disconnected and no config
			text_layer_set_text_color(s_text_top, PBL_IF_BW_ELSE(GColorBlack, GColorRed));		// Set Top Colour
			text_layer_set_text_color(s_text_bottom, PBL_IF_BW_ELSE(GColorBlack, GColorRed));	// Set Bottom Colour
		}
		if(select_bluetooth_disconnect_int == 0) { }		// No vibration 
		else if(select_bluetooth_disconnect_int == 1) { vibes_short_pulse(); }		// Short vibration
		else if(select_bluetooth_disconnect_int == 2) { vibes_long_pulse(); }	// Long vibration
		else if(select_bluetooth_disconnect_int == 3) { vibes_double_pulse(); }	// Double vibration
		else { vibes_long_pulse(); }	// Default
	} else {														// Connected
		if(colour_background || colour_date) {
			text_layer_set_text_color(s_text_top, PBL_IF_BW_ELSE(gcolor_legible_over(GColorFromHEX(colour_background)), GColorFromHEX(colour_date)));		// Set Top Colour
			text_layer_set_text_color(s_text_bottom, PBL_IF_BW_ELSE(gcolor_legible_over(GColorFromHEX(colour_background)), GColorFromHEX(colour_date)));	// Set Bottom Colour
		} else {
			text_layer_set_text_color(s_text_top, GColorWhite);		// Set Top Colour
			text_layer_set_text_color(s_text_bottom, GColorWhite);		// Set Bottom Colour
		}
	}
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
// Colours
	Tuple *colour_background_t = dict_find(iter, MESSAGE_KEY_COLOUR_BACKGROUND);
	int colour_background = colour_background_t->value->int32;
	persist_write_int(MESSAGE_KEY_COLOUR_BACKGROUND, colour_background);
	int colour_hour = 0, colour_minute = 0;
	#if defined(PBL_COLOR)
		Tuple *colour_hour_t = dict_find(iter, MESSAGE_KEY_COLOUR_HOUR);
		Tuple *colour_minute_t = dict_find(iter, MESSAGE_KEY_COLOUR_MINUTE);
		Tuple *colour_date_t = dict_find(iter, MESSAGE_KEY_COLOUR_DATE);
			colour_hour = colour_hour_t->value->int32;
			colour_minute = colour_minute_t->value->int32;
		int colour_date = colour_date_t->value->int32;
		persist_write_int(MESSAGE_KEY_COLOUR_HOUR, colour_hour);
		persist_write_int(MESSAGE_KEY_COLOUR_MINUTE, colour_minute);
		persist_write_int(MESSAGE_KEY_COLOUR_DATE, colour_date);
// Bluetooth
		Tuple *colour_bluetooth_t = dict_find(iter, MESSAGE_KEY_COLOUR_BLUETOOTH);
		int colour_bluetooth = colour_bluetooth_t->value->int32;
		persist_write_int(MESSAGE_KEY_COLOUR_BLUETOOTH, colour_bluetooth);
	#endif
	Tuple *select_bluetooth_diconnect_t = dict_find(iter, MESSAGE_KEY_SELECT_BLUETOOTH_DISCONNECT);
	char *select_bluetooth_disconnect = select_bluetooth_diconnect_t->value->cstring;
	persist_write_string(MESSAGE_KEY_SELECT_BLUETOOTH_DISCONNECT, select_bluetooth_disconnect);	
// Battery
	Tuple *select_battery_percent_t = dict_find(iter, MESSAGE_KEY_SELECT_BATTERY_PERCENT);
	char *select_battery_percent = select_battery_percent_t->value->cstring;
	persist_write_string(MESSAGE_KEY_SELECT_BATTERY_PERCENT, select_battery_percent);
// Date	
	Tuple *toggle_suffix_t = dict_find(iter, MESSAGE_KEY_TOGGLE_SUFFIX);
	int toggle_suffix = toggle_suffix_t->value->int32;
	persist_write_int(MESSAGE_KEY_TOGGLE_SUFFIX, toggle_suffix);
	Tuple *toggle_week_t = dict_find(iter, MESSAGE_KEY_TOGGLE_WEEK);
	int toggle_week = toggle_week_t->value->int32;
	persist_write_int(MESSAGE_KEY_TOGGLE_WEEK, toggle_week);
// Custom Text
	Tuple *check_date_0_t = dict_find(iter, MESSAGE_KEY_CHECK_DATE);
	Tuple *check_date_1_t = dict_find(iter, MESSAGE_KEY_CHECK_DATE+1);
	Tuple *check_date_2_t = dict_find(iter, MESSAGE_KEY_CHECK_DATE+2);
 	int check_date_0 = check_date_0_t->value->int32;
	int check_date_1 = check_date_1_t->value->int32;
	int check_date_2 = check_date_2_t->value->int32;
	persist_write_int(MESSAGE_KEY_CHECK_DATE, check_date_0);
	persist_write_int(MESSAGE_KEY_CHECK_DATE+1, check_date_1);
	persist_write_int(MESSAGE_KEY_CHECK_DATE+2, check_date_2);
	
// Set Colours
	setColours(colour_background, colour_hour, colour_minute);
	
	battery_callback(battery_state_service_peek());
	bluetooth_callback(connection_service_peek_pebble_app_connection());
	layer_mark_dirty(window_get_root_layer(s_window));
	
//	APP_LOG(APP_LOG_LEVEL_DEBUG, select_battery_percent);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Time & Date /////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void time_date_update_proc(Layer *layer, GContext *ctx) {
	time_t temp = time(NULL); 
	struct tm *tick_time = localtime(&temp);
	static char h_buffer[3], m_buffer[3], t_buffer[16], b_buffer[16];

// Time
	if(clock_is_24h_style()) {
		strftime(h_buffer, sizeof(h_buffer), "%H", tick_time);	//%H
	} else {
		strftime(h_buffer, sizeof(h_buffer), "%I", tick_time);	//%I
	}
	strftime(m_buffer, sizeof(m_buffer), "%M", tick_time);		//%M
	text_layer_set_text(s_text_hour, h_buffer);
	text_layer_set_text(s_text_minute, m_buffer);

// Date
	customText(t_buffer, b_buffer);	
	if(strcmp(t_buffer, "\0") == 0) {			// If Top is empty, write current weekday
		strftime(t_buffer, sizeof(t_buffer), "%A", tick_time);		// %A
	} if(strcmp(b_buffer, "\0") == 0) {			// If Bottom is empty, write current date		
 		char char_buffer[16] = "";
		int toggle_suffix = persist_read_int(MESSAGE_KEY_TOGGLE_SUFFIX);
		strftime(date_current, sizeof(date_current), "%d%m", tick_time);
		strftime(month_current, sizeof(month_current), "%B", tick_time);
// Day
		strcat(char_buffer,"%e");
		if(toggle_suffix == 1) {
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
		int toggle_week = persist_read_int(MESSAGE_KEY_TOGGLE_WEEK);
		#if defined(PBL_RECT)
			if(strlen(month_current) > 7 || toggle_week) {
				strcat(char_buffer,"  %b"); // Short
				if(toggle_week) {
					strcat(char_buffer,"  W%V");
				}
			} else {
				strcat(char_buffer,"  %B");
			}
		#elif defined(PBL_ROUND)
			if(strlen(month_current) > 5) {
				strcat(char_buffer,"  %b"); // Short
			} else {
				strcat(char_buffer,"  %B");
			}
		#endif
		strftime(b_buffer, sizeof(char_buffer), char_buffer, tick_time);	// ᵗʰ
	}
	text_layer_set_text(s_text_top, t_buffer);
	text_layer_set_text(s_text_bottom, b_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	layer_mark_dirty(window_get_root_layer(s_window));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Window //////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_unobstructed_bounds(window_layer);
	setlocale(LC_ALL, "");
	
	s_layer_time_date = layer_create(bounds);
	layer_set_update_proc(s_layer_time_date, time_date_update_proc);
	layer_add_child(window_layer, s_layer_time_date);
		
// Fonts
	s_font_time = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_BOLD_72));
	s_font_date = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_REGULAR_28));
	
// Locations
	#if defined(PBL_RECT)
		s_text_hour		= text_layer_create(GRect(				0, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
 		s_text_minute	= text_layer_create(GRect(bounds.size.w/2, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
		s_text_top		= text_layer_create(GRect(				0, bounds.size.h / 4 - 31, bounds.size.w,   30));
 		s_text_bottom	= text_layer_create(GRect(				0, bounds.size.h * 3/4 -5, bounds.size.w,   30));
		s_layer_battery	= bitmap_layer_create(GRect(4, 2, 13, 6)); // battery
	#elif defined(PBL_ROUND)
		s_text_hour		= text_layer_create(GRect(   10, bounds.size.h / 2 - 47 + 1, bounds.size.w/2, 75));
		s_text_minute	= text_layer_create(GRect(73+10, bounds.size.h / 2 - 47 + 1, bounds.size.w/2, 75));
		s_text_top		= text_layer_create(GRect(    0, bounds.size.h / 12 + 3, bounds.size.w,   30));
		s_text_bottom	= text_layer_create(GRect(    0, bounds.size.h * 3/4 -5-2, bounds.size.w,   30));
 		s_layer_battery	= bitmap_layer_create(GRect(84, 4, 13, 6)); // battery
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

// Colours
	int colour_background = persist_read_int(MESSAGE_KEY_COLOUR_BACKGROUND);
	int colour_hour = persist_read_int(MESSAGE_KEY_COLOUR_HOUR);
	int colour_minute = persist_read_int(MESSAGE_KEY_COLOUR_MINUTE);
	setColours(colour_background, colour_hour, colour_minute);					// Sets background, hour and minute colours
	bluetooth_callback(connection_service_peek_pebble_app_connection());		// Sets date colours (and detects if a phone is connected)
}

static void window_unload(Window *window) {
	layer_destroy(s_layer_time_date);
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

	const int inbox_size = 256;
	const int outbox_size = 256;
	app_message_register_inbox_received(inbox_received_handler);
	app_message_open(inbox_size, outbox_size);

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