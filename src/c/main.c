#include <pebble.h>

uint32_t received = 0;	// Used to determine whether AppMessages have been received

static Window *s_window;
static TextLayer *s_text_hour, *s_text_minute, *s_text_top, *s_text_bottom;
static GFont s_font_time, s_font_date;
static BitmapLayer *s_layer_battery;
static GBitmap *s_bitmap_battery;
char date_current[5], month_current[16];
static bool appStarted = false;

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Methods /////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

void getBatteryIcon(int image_number) {
	int BatteryBlack[] = {RESOURCE_ID_IMAGE_BATTERY_BLACK_1, RESOURCE_ID_IMAGE_BATTERY_BLACK_2, RESOURCE_ID_IMAGE_BATTERY_BLACK_3, RESOURCE_ID_IMAGE_BATTERY_BLACK_4};
	int BatteryWhite[] = {RESOURCE_ID_IMAGE_BATTERY_WHITE_1, RESOURCE_ID_IMAGE_BATTERY_WHITE_2, RESOURCE_ID_IMAGE_BATTERY_WHITE_3, RESOURCE_ID_IMAGE_BATTERY_WHITE_4};
	
	gbitmap_destroy(s_bitmap_battery);
	if(persist_read_bool(received)) {
		if (gcolor_equal(gcolor_legible_over(GColorFromHEX(persist_read_int(MESSAGE_KEY_COLOUR_BACKGROUND))), GColorBlack)) {
			s_bitmap_battery = gbitmap_create_with_resource(BatteryBlack[image_number]);
		} else {
			s_bitmap_battery = gbitmap_create_with_resource(BatteryWhite[image_number]);
		}
	} else {
		s_bitmap_battery = gbitmap_create_with_resource(BatteryWhite[image_number]);
	}
	bitmap_layer_set_bitmap(s_layer_battery, s_bitmap_battery);
}

void setColours() {
	if(persist_read_bool(received)) {
		GColor bg_colour = GColorFromHEX(persist_read_int(MESSAGE_KEY_COLOUR_BACKGROUND));
		window_set_background_color(s_window, bg_colour);			// Set Background Colour
		#if defined(PBL_BW)
			text_layer_set_text_color(s_text_hour, gcolor_legible_over(bg_colour));		// Set Hour Colour
			text_layer_set_text_color(s_text_minute, gcolor_legible_over(bg_colour));	// Set Minute Colour
		#elif defined(PBL_COLOR)
			text_layer_set_text_color(s_text_hour, GColorFromHEX(persist_read_int(MESSAGE_KEY_COLOUR_HOUR)));			// Set Hour Colour
			text_layer_set_text_color(s_text_minute, GColorFromHEX(persist_read_int(MESSAGE_KEY_COLOUR_MINUTE)));		// Set Minute Colour
		#endif
	} else {
		window_set_background_color(s_window, GColorBlack);			// Set Background Colour
		text_layer_set_text_color(s_text_hour, GColorWhite);		// Set Hour Colour
		text_layer_set_text_color(s_text_minute, GColorWhite);		// Set Minute Colour
	}
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

// Time
	if(clock_is_24h_style()) {
		strftime(h_buffer, sizeof(h_buffer), "%H", tick_time);	//%H
	} else {
		strftime(h_buffer, sizeof(h_buffer), "%I", tick_time);	//%I
	}
	strftime(m_buffer, sizeof(m_buffer), "%M", tick_time);		//%M
	text_layer_set_text(s_text_hour, h_buffer);
	text_layer_set_text(s_text_minute, m_buffer);

// Top
	strftime(t_buffer, sizeof(t_buffer), "%A", tick_time);		// %A
// Bottom
	char char_buffer[16] = "";
	if(strcmp("0104", date_current) == 0) {	// April Fools			Should this be hardcoded?
		strcpy(char_buffer, "April  Fools");
	} else if(strcmp("0101", date_current) == 0 && persist_read_int(MESSAGE_KEY_CHECK_DATE	)) { // New Year's Day
		strcpy(char_buffer, "Happy  %Y");		//  1st January
	} else if(strcmp("2501", date_current) == 0 && persist_read_int(MESSAGE_KEY_CHECK_DATE+1)) { // Burns Night
		strcpy(char_buffer, "Burns  Night");
	} else if(strcmp("3110", date_current) == 0 && persist_read_int(MESSAGE_KEY_CHECK_DATE+2)) { // Halloween
		strcpy(char_buffer, "Halloween");
	} else if(strcmp("2512", date_current) == 0 && persist_read_int(MESSAGE_KEY_CHECK_DATE+3)) { // Christmas
		strcpy(char_buffer, "Christmas");
	} else if(strcmp("2612", date_current) == 0 && persist_read_int(MESSAGE_KEY_CHECK_DATE+4)) { // Boxing Day
		strcpy(char_buffer, "Boxing  Day");
	}

	// Mother's Day US			May ish
	// Mother's Day UK			March ish
	// Father's Day US			
	// Father's Day UK			June ish
	// Independence Day (US)	4th July
	// Thanksgiving				3rd thursday in november?
	// black friday??	
	// Rememberence Sunday (UK) A sunday from the 8th to the 14th november
	
	else {
// Day
		strcpy(char_buffer,"%e");
		if(persist_read_int(MESSAGE_KEY_TOGGLE_SUFFIX) == 1) {
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
		#if defined(PBL_RECT)
			if(strlen(month_current) > 7 || persist_read_int(MESSAGE_KEY_TOGGLE_WEEK)) {
				strcat(char_buffer,"  %b"); // Short
				if(persist_read_int(MESSAGE_KEY_TOGGLE_WEEK)) {
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
	if(persist_read_bool(received)) {
		char *select_battery_percent = "";
		persist_read_string(MESSAGE_KEY_SELECT_BATTERY_PERCENT,select_battery_percent,5);
		int select_battery_percent_int = atoi(select_battery_percent);

		if(select_battery_percent_int == 100) { // Always show battery
			if(75 < state.charge_percent && state.charge_percent <= 100) {
				getBatteryIcon(3);
			} else if(50 < state.charge_percent && state.charge_percent <= 75) {
				getBatteryIcon(2);
			} else if(25 < state.charge_percent && state.charge_percent <= 50) {
				getBatteryIcon(1);
			} else if( 0 < state.charge_percent && state.charge_percent <= 25) {
				getBatteryIcon(0);
			}
			layer_set_hidden(bitmap_layer_get_layer(s_layer_battery), false);	// Visible
		} else {
			if(state.charge_percent < select_battery_percent_int) {
				getBatteryIcon(0);
				layer_set_hidden(bitmap_layer_get_layer(s_layer_battery), false);	// Visible
			} else {
				layer_set_hidden(bitmap_layer_get_layer(s_layer_battery), true);	// Hidden
			}
		}
	} else {	// Hidden
		layer_set_hidden(bitmap_layer_get_layer(s_layer_battery), true);
	}
}

static void bluetooth_callback(bool connected) {													  	
	if(!connected) {
		if(persist_read_bool(received)) {	// Disconected with config
			#if defined(PBL_BW)
				GColor bg_colour = GColorFromHEX(persist_read_int(MESSAGE_KEY_COLOUR_BACKGROUND));
				text_layer_set_text_color(s_text_top, gcolor_legible_over(bg_colour));		// Set Top Colour
				text_layer_set_text_color(s_text_bottom, gcolor_legible_over(bg_colour));	// Set Bottom Colour
			#elif defined(PBL_COLOR)
				GColor bt_colour = GColorFromHEX(persist_read_int(MESSAGE_KEY_COLOUR_BLUETOOTH));
				text_layer_set_text_color(s_text_top, bt_colour);		// Set Top Colour
				text_layer_set_text_color(s_text_bottom, bt_colour);	// Set Bottom Colour
			#endif
		} else { 							// Disconnected without config
			#if defined(PBL_BW)
				text_layer_set_text_color(s_text_top, GColorBlack);		// Set Top Colour
				text_layer_set_text_color(s_text_bottom, GColorBlack);	// Set Bottom Colour
			#elif defined(PBL_COLOR)
				text_layer_set_text_color(s_text_top, GColorRed);		// Set Top Colour
				text_layer_set_text_color(s_text_bottom, GColorRed);	// Set Bottom Colour
			#endif
		}
		if(appStarted) {
			char *select_bluetooth_disconnect = "";
 			persist_read_string(MESSAGE_KEY_SELECT_BLUETOOTH_DISCONNECT,select_bluetooth_disconnect,5);
			int select_bluetooth_disconnect_int = atoi(select_bluetooth_disconnect);
			
			if(select_bluetooth_disconnect_int == 0) { }							// No vibration 
			else if(select_bluetooth_disconnect_int == 1) { vibes_short_pulse(); }	// Short vibration
			else if(select_bluetooth_disconnect_int == 2) { vibes_long_pulse(); }	// Long vibration
			else if(select_bluetooth_disconnect_int == 3) { vibes_double_pulse(); }	// Double vibration
			else { vibes_long_pulse(); }	// Default
		}
	} else {
		if(persist_read_bool(received)) {	// Connected with config
			#if defined(PBL_BW)
				GColor bg_colour = GColorFromHEX(persist_read_int(MESSAGE_KEY_COLOUR_BACKGROUND));
				text_layer_set_text_color(s_text_top, gcolor_legible_over(bg_colour));		// Set Top Colour
				text_layer_set_text_color(s_text_bottom, gcolor_legible_over(bg_colour));	// Set Bottom Colour
			#elif defined(PBL_COLOR)
				GColor dt_colour = GColorFromHEX(persist_read_int(MESSAGE_KEY_COLOUR_DATE));
				text_layer_set_text_color(s_text_top, dt_colour);							// Set Top Colour
				text_layer_set_text_color(s_text_bottom, dt_colour);						// Set Bottom Colour
			#endif
		} else {							// Connected without config
			text_layer_set_text_color(s_text_top, GColorWhite);		// Set Top Colour
			text_layer_set_text_color(s_text_bottom, GColorWhite);	// Set Bottom Colour
		}
	}
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
	persist_write_bool(received, true);
// Colours
	int colour_background = dict_find(iter, MESSAGE_KEY_COLOUR_BACKGROUND)->value->int32;
	persist_write_int(MESSAGE_KEY_COLOUR_BACKGROUND, colour_background);	
	#if defined(PBL_COLOR)
		persist_write_int(MESSAGE_KEY_COLOUR_HOUR, dict_find(iter, MESSAGE_KEY_COLOUR_HOUR)->value->int32);
		persist_write_int(MESSAGE_KEY_COLOUR_MINUTE, dict_find(iter, MESSAGE_KEY_COLOUR_MINUTE)->value->int32);	
		persist_write_int(MESSAGE_KEY_COLOUR_DATE, dict_find(iter, MESSAGE_KEY_COLOUR_DATE)->value->int32);
// Bluetooth
		persist_write_int(MESSAGE_KEY_COLOUR_BLUETOOTH, dict_find(iter, MESSAGE_KEY_COLOUR_BLUETOOTH)->value->int32);
	#endif
	persist_write_string(MESSAGE_KEY_SELECT_BLUETOOTH_DISCONNECT, dict_find(iter, MESSAGE_KEY_SELECT_BLUETOOTH_DISCONNECT)->value->cstring);	
// Battery
	persist_write_string(MESSAGE_KEY_SELECT_BATTERY_PERCENT, dict_find(iter, MESSAGE_KEY_SELECT_BATTERY_PERCENT)->value->cstring);
// Date	
	persist_write_int(MESSAGE_KEY_TOGGLE_SUFFIX, dict_find(iter, MESSAGE_KEY_TOGGLE_SUFFIX)->value->int32);
	persist_write_int(MESSAGE_KEY_TOGGLE_WEEK, dict_find(iter, MESSAGE_KEY_TOGGLE_WEEK)->value->int32);
// Custom Text
	persist_write_int(MESSAGE_KEY_CHECK_DATE, 	dict_find(iter, MESSAGE_KEY_CHECK_DATE	)->value->int32);		
	persist_write_int(MESSAGE_KEY_CHECK_DATE+1, dict_find(iter, MESSAGE_KEY_CHECK_DATE+1)->value->int32);		
	persist_write_int(MESSAGE_KEY_CHECK_DATE+2, dict_find(iter, MESSAGE_KEY_CHECK_DATE+2)->value->int32);
	persist_write_int(MESSAGE_KEY_CHECK_DATE+3, dict_find(iter, MESSAGE_KEY_CHECK_DATE+3)->value->int32);
	persist_write_int(MESSAGE_KEY_CHECK_DATE+4, dict_find(iter, MESSAGE_KEY_CHECK_DATE+4)->value->int32);

	battery_callback(battery_state_service_peek());
	appStarted = false;
	bluetooth_callback(connection_service_peek_pebble_app_connection());		// Sets date colours (and detects if a phone is connected)
	appStarted = true;
	setColours();
	update_time();
//	APP_LOG(APP_LOG_LEVEL_DEBUG, select_battery_percent);
}

void unobstructed_change(AnimationProgress progress, void* data) {
	GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(s_window));

	layer_set_frame(text_layer_get_layer(s_text_hour),	GRect(				0, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
	layer_set_frame(text_layer_get_layer(s_text_minute),GRect(bounds.size.w/2, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
	layer_set_frame(text_layer_get_layer(s_text_top),	GRect(				0, bounds.size.h / 4 - 31, bounds.size.w,   30));
	layer_set_frame(text_layer_get_layer(s_text_bottom),GRect(				0, bounds.size.h * 3/4 -5, bounds.size.w,   30));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Window //////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_unobstructed_bounds(window_layer);
	setlocale(LC_ALL, "");
		
// Fonts
	s_font_time = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_BOLD_72));
	s_font_date = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_REGULAR_28));
	
// Locations
	#if defined(PBL_RECT)
		s_text_hour		= text_layer_create(GRect(				0, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
 		s_text_minute	= text_layer_create(GRect(bounds.size.w/2, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
		s_text_top		= text_layer_create(GRect(				0, bounds.size.h / 4 - 31, bounds.size.w,   30));
 		s_text_bottom	= text_layer_create(GRect(				0, bounds.size.h * 3/4 -5, bounds.size.w,   30));
		s_layer_battery	= bitmap_layer_create(GRect(4, 4, 13, 6)); // battery
	#elif defined(PBL_ROUND)
		s_text_hour		= text_layer_create(GRect(			   10, bounds.size.h / 2 - 47, bounds.size.w/2-10, 75));
 		s_text_minute	= text_layer_create(GRect(bounds.size.w/2, bounds.size.h / 2 - 47, bounds.size.w/2-10, 75));
		s_text_top		= text_layer_create(GRect(				0, bounds.size.h / 4 - 31+5, bounds.size.w,    30));
 		s_text_bottom	= text_layer_create(GRect(				0, bounds.size.h * 3/4 -5-5, bounds.size.w,    30));
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
	appStarted = false;
	bluetooth_callback(connection_service_peek_pebble_app_connection());		// Sets date colours (and detects if a phone is connected)
	appStarted = true;
	setColours();					// Sets background, hour and minute colours
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

	update_time();
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