#include <pebble.h>

static Window *s_window;
static TextLayer *s_hour_layer, *s_minute_layer, *s_top_layer, *s_bottom_layer;
static GFont s_time_font, s_date_font;
static BitmapLayer *s_battery_layer;
static GBitmap *s_battery_bitmap;
char date_current[5], month_current[16];

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Methods /////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

void getBatteryIcon(int colour_background) {
 	if(colour_background) {
 		GColor bg_colour = GColorFromHEX(colour_background);
		if (gcolor_equal(gcolor_legible_over(bg_colour), GColorBlack)) {
			s_battery_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_BLACK);
		} else {
			s_battery_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_WHITE);
		}
	} else {
		s_battery_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_WHITE);
	}
	bitmap_layer_set_bitmap(s_battery_layer, s_battery_bitmap);
}

void getMonth(char *input, int location, struct tm *tick_time) {		// 0 is before, 1 is after
	strftime(month_current, sizeof(month_current), "%B", tick_time);
 	
	#if defined(PBL_RECT)
		if(strlen(month_current) > 7) {
			if(location == 0) { strcat(input,"  %b"); }
			if(location == 1) { strcat(input,"%b"); }
		} else {
			if(location == 0) { strcat(input,"  %B"); }
			if(location == 1) { strcat(input,"%B"); }
		}
	#elif defined(PBL_ROUND)
		if(strlen(month_current) > 5) {
			if(location == 0) { strcat(input,"  %b"); }
			if(location == 1) { strcat(input,"%b"); }
	 	} else {
			if(location == 0) { strcat(input,"  %B"); }
			if(location == 1) { strcat(input,"%B"); }
		}
	#endif
}

void getSuffix(char *input, struct tm *tick_time) {
	if (strncmp(date_current, "01", 2) == 0 || strncmp(date_current, "21", 2) == 0 || strncmp(date_current,"31",2) == 0) { 
		strcat(input,"st");
	} else if (strncmp(date_current, "02", 2) == 0 || strncmp(date_current, "22", 2) == 0) {
		strcat(input,"nd");
	} else if (strncmp(date_current, "03", 2) == 0 || strncmp(date_current, "23", 2) == 0) { 
		strcat(input,"rd");
	} else {
		strcat(input,"th");
	}
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
	} else if(check_date_1 == 1 && strcmp("3110", date_current) == 0) {  // Christmas
		strcpy(t_buffer, "\0");
		strcpy(b_buffer, "Halloween");
	} else if(check_date_2 == 1 && strcmp("2512", date_current) == 0) {  // Halloween
		strcpy(t_buffer, "Merry");
		strcpy(b_buffer, "Christmas");
	}
	// burns night is on the 25th January
	// rememberence sunday could be from the 8th to the 14th november
	else {
		strcpy(t_buffer, "\0");
		strcpy(b_buffer, "\0");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// TIME ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void update_time() {
	time_t temp = time(NULL); 
	struct tm *tick_time = localtime(&temp);
	strftime(date_current, 8, "%d%m", tick_time);

// Time
	static char h_buffer[3];
	static char m_buffer[3];
	
	if(clock_is_24h_style()) {
		strftime(h_buffer, sizeof(h_buffer), "%H", tick_time);	//%H
	} else {
		strftime(h_buffer, sizeof(h_buffer), "%I", tick_time);	//%I
	}
	strftime(m_buffer, sizeof(m_buffer), "%M", tick_time);		//%M
	text_layer_set_text(s_hour_layer, h_buffer);
	text_layer_set_text(s_minute_layer, m_buffer);

// Date
	static char t_buffer[16];
	static char b_buffer[16];

	customText(t_buffer, b_buffer);	
	if(strcmp(t_buffer, "\0") == 0) {			// If Top is empty, write current weekday
		strftime(t_buffer, sizeof(t_buffer), "%A", tick_time);		// %A
	} if(strcmp(b_buffer, "\0") == 0) {			// If Bottom is empty, write current date		
 		char char_suffix[16] = "";
// 		char *select_date_bottom = "";
// 		persist_read_string(MESSAGE_KEY_SELECT_DATE_BOTTOM,select_date_bottom,5);
// 		int select_date_bottom_int = atoi(select_date_bottom);
		int toggle_suffix = persist_read_int(MESSAGE_KEY_TOGGLE_SUFFIX);	

// 		if(select_date_bottom_int == 1) { 		// Month Day
// 			getMonth(char_suffix, 1, tick_time);
// 			strcat(char_suffix,"%e");
// 			if(toggle_suffix == 1) {
// 				getSuffix(char_suffix,tick_time);
// 			}
// 		} else {								// Default (Day Month)
			strcat(char_suffix,"%e");
			if(toggle_suffix == 1) {
				getSuffix(char_suffix,tick_time);
			}	
			getMonth(char_suffix, 0, tick_time);
// 		}
		strftime(b_buffer, sizeof(char_suffix), char_suffix, tick_time);	// áµ—Ê°
	}
	text_layer_set_text(s_top_layer, t_buffer);
	text_layer_set_text(s_bottom_layer, b_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Callbacks ///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

void unobstructed_change(AnimationProgress progress, void* data) {
	GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(s_window));

	layer_set_frame(text_layer_get_layer(s_hour_layer),GRect(				 0, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
	layer_set_frame(text_layer_get_layer(s_minute_layer),GRect(bounds.size.w/2, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
	layer_set_frame(text_layer_get_layer(s_top_layer),GRect(				 0, bounds.size.h / 4 - 31, bounds.size.w,   30));
	layer_set_frame(text_layer_get_layer(s_bottom_layer),GRect(				 0, bounds.size.h * 3/4 -5, bounds.size.w,   30));
}

static void battery_callback(BatteryChargeState state) {
	char *select_battery_percent = "";
 	persist_read_string(MESSAGE_KEY_SELECT_BATTERY_PERCENT,select_battery_percent,5);
	int select_battery_percent_int = atoi(select_battery_percent);
	
	int toggle_battery = persist_read_int(MESSAGE_KEY_TOGGLE_BATTERY);
	if(toggle_battery == 1) { //on
		if(state.charge_percent < select_battery_percent_int) {
			layer_set_hidden(bitmap_layer_get_layer(s_battery_layer), false); // Visible
		} else {
			layer_set_hidden(bitmap_layer_get_layer(s_battery_layer), true);  // Hidden
		}
	} else {
		layer_set_hidden(bitmap_layer_get_layer(s_battery_layer), true);
	}
}

static void bluetooth_callback(bool connected) {
	int colour_background = persist_read_int(MESSAGE_KEY_COLOUR_BACKGROUND);
	int colour_date = persist_read_int(MESSAGE_KEY_COLOUR_DATE);
	int colour_bluetooth = persist_read_int(MESSAGE_KEY_COLOUR_BLUETOOTH);
	
	if(!connected) {		// Disconected
		if(colour_bluetooth) {
			#if defined(PBL_COLOR)
				GColor bt_colour = GColorFromHEX(colour_bluetooth);
				text_layer_set_text_color(s_top_layer, bt_colour);
				text_layer_set_text_color(s_bottom_layer, bt_colour);
			#elif defined(PBL_BW)
				GColor bg_colour = GColorFromHEX(colour_bluetooth);
				text_layer_set_text_color(s_top_layer, bg_colour);
				text_layer_set_text_color(s_bottom_layer, bg_colour);
			#endif
		} else {
			#if defined(PBL_COLOR)
				text_layer_set_text_color(s_top_layer, GColorRed);
				text_layer_set_text_color(s_bottom_layer, GColorRed);
			#elif defined(PBL_BW)
				text_layer_set_text_color(s_top_layer, GColorBlack);
				text_layer_set_text_color(s_bottom_layer, GColorBlack);
			#endif
		}
		vibes_long_pulse();
	} else {				// Connected
		if(colour_background || colour_date) {
			#if defined(PBL_COLOR)
				GColor dt_colour = GColorFromHEX(colour_date);
				text_layer_set_text_color(s_top_layer, dt_colour);
				text_layer_set_text_color(s_bottom_layer, dt_colour);	
			#elif defined(PBL_BW)
				GColor bg_colour = GColorFromHEX(colour_background);
				text_layer_set_text_color(s_top_layer, bg_colour);
				text_layer_set_text_color(s_bottom_layer, bg_colour);
			#endif
		} else {	
			text_layer_set_text_color(s_top_layer, GColorWhite);
			text_layer_set_text_color(s_bottom_layer, GColorWhite);
		}
	}
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
// Colours
	Tuple *colour_background_t = dict_find(iter, MESSAGE_KEY_COLOUR_BACKGROUND);
	int colour_background = colour_background_t->value->int32;
	persist_write_int(MESSAGE_KEY_COLOUR_BACKGROUND, colour_background);
	#if defined(PBL_COLOR)
		Tuple *colour_hour_t = dict_find(iter, MESSAGE_KEY_COLOUR_HOUR);
		Tuple *colour_minute_t = dict_find(iter, MESSAGE_KEY_COLOUR_MINUTE);
		Tuple *colour_date_t = dict_find(iter, MESSAGE_KEY_COLOUR_DATE);
		Tuple *colour_bluetooth_t = dict_find(iter, MESSAGE_KEY_COLOUR_BLUETOOTH);
		int colour_hour = colour_hour_t->value->int32;
		int colour_minute = colour_minute_t->value->int32;
		int colour_date = colour_date_t->value->int32;
		int colour_bluetooth = colour_bluetooth_t->value->int32;
		persist_write_int(MESSAGE_KEY_COLOUR_HOUR, colour_hour);
		persist_write_int(MESSAGE_KEY_COLOUR_MINUTE, colour_minute);
		persist_write_int(MESSAGE_KEY_COLOUR_DATE, colour_date);
		persist_write_int(MESSAGE_KEY_COLOUR_BLUETOOTH, colour_bluetooth);
	#endif
// Battery
	Tuple *toggle_battery_t = dict_find(iter, MESSAGE_KEY_TOGGLE_BATTERY);
	Tuple *select_battery_percent_t = dict_find(iter, MESSAGE_KEY_SELECT_BATTERY_PERCENT);
	int toggle_battery = toggle_battery_t->value->int32;
	char *select_battery_percent = select_battery_percent_t->value->cstring;
	persist_write_int(MESSAGE_KEY_TOGGLE_BATTERY, toggle_battery);
	persist_write_string(MESSAGE_KEY_SELECT_BATTERY_PERCENT, select_battery_percent);
// Date	
// 	Tuple *select_date_bottom_t = dict_find(iter, MESSAGE_KEY_SELECT_DATE_BOTTOM);
	Tuple *toggle_suffix_t = dict_find(iter, MESSAGE_KEY_TOGGLE_SUFFIX);
// 	char *select_date_bottom = select_date_bottom_t->value->cstring;
	int toggle_suffix = toggle_suffix_t->value->int32;
// 	persist_write_string(MESSAGE_KEY_SELECT_DATE_BOTTOM, select_date_bottom);
	persist_write_int(MESSAGE_KEY_TOGGLE_SUFFIX, toggle_suffix);
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
	GColor bg_colour = GColorFromHEX(colour_background);
	window_set_background_color(s_window, bg_colour);
	
	#if defined(PBL_COLOR)
		GColor hr_colour = GColorFromHEX(colour_hour);
		text_layer_set_text_color(s_hour_layer, hr_colour);
		GColor mn_colour = GColorFromHEX(colour_minute);
		text_layer_set_text_color(s_minute_layer, mn_colour);
		GColor dt_colour = GColorFromHEX(colour_date);
		text_layer_set_text_color(s_top_layer, dt_colour);
		text_layer_set_text_color(s_bottom_layer, dt_colour);
	#elif defined(PBL_BW)	
		text_layer_set_text_color(s_hour_layer, gcolor_legible_over(bg_colour));
		text_layer_set_text_color(s_minute_layer, gcolor_legible_over(bg_colour));
		text_layer_set_text_color(s_top_layer, gcolor_legible_over(bg_colour));
		text_layer_set_text_color(s_bottom_layer, gcolor_legible_over(bg_colour));
	#endif
	
	if(toggle_battery == 1) {		// On
		gbitmap_destroy(s_battery_bitmap);
		getBatteryIcon(colour_background);
		battery_callback(battery_state_service_peek());
	}
	bluetooth_callback(connection_service_peek_pebble_app_connection());
	update_time();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Window //////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void window_load(Window *window) {
	GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(window));
	
// Fonts
	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_BOLD_72));
	s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_REGULAR_28));
	
// Locations
	#if defined(PBL_RECT)
		s_hour_layer	= text_layer_create(GRect(				0, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
 		s_minute_layer	= text_layer_create(GRect(bounds.size.w/2, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
		s_top_layer		= text_layer_create(GRect(				0, bounds.size.h / 4 - 31, bounds.size.w,   30));
 		s_bottom_layer	= text_layer_create(GRect(				0, bounds.size.h * 3/4 -5, bounds.size.w,   30));
		s_battery_layer	= bitmap_layer_create(GRect(4, 2, 12, 12)); // battery
	#elif defined(PBL_ROUND)
		s_hour_layer	= text_layer_create(GRect(   10, bounds.size.h / 2 - 47 + 1, bounds.size.w/2, 75));
		s_minute_layer	= text_layer_create(GRect(73+10, bounds.size.h / 2 - 47 + 1, bounds.size.w/2, 75));
		s_top_layer		= text_layer_create(GRect(    0, bounds.size.h / 12 + 3, bounds.size.w,   30));
		s_bottom_layer	= text_layer_create(GRect(    0, bounds.size.h * 3/4 -5-2, bounds.size.w,   30));
 		s_battery_layer	= bitmap_layer_create(GRect(84, 2, 12, 12)); // battery
	#endif

// Battery Image
	int colour_background = persist_read_int(MESSAGE_KEY_COLOUR_BACKGROUND);
	getBatteryIcon(colour_background);
	layer_mark_dirty(bitmap_layer_get_layer(s_battery_layer));
	#if defined(PBL_COLOR)
		bitmap_layer_set_compositing_mode(s_battery_layer, GCompOpSet);	
	#endif
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_battery_layer));
		
// Hour
	text_layer_set_font(s_hour_layer, s_time_font);
	text_layer_set_text_alignment(s_hour_layer, GTextAlignmentCenter);
	text_layer_set_background_color(s_hour_layer, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hour_layer));
	
// Minutes
	text_layer_set_font(s_minute_layer, s_time_font);
	text_layer_set_text_alignment(s_minute_layer, GTextAlignmentCenter);
	text_layer_set_background_color(s_minute_layer, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_minute_layer));

// Top
	text_layer_set_font(s_top_layer, s_date_font);
	text_layer_set_text_alignment(s_top_layer, GTextAlignmentCenter);
	text_layer_set_background_color(s_top_layer, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_top_layer));

// Bottom
	text_layer_set_font(s_bottom_layer, s_date_font);
	text_layer_set_text_alignment(s_bottom_layer, GTextAlignmentCenter);
	text_layer_set_background_color(s_bottom_layer, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_bottom_layer));

// Colours
//	int colour_background = persist_read_int(MESSAGE_KEY_COLOUR_BACKGROUND);
	int colour_hour = persist_read_int(MESSAGE_KEY_COLOUR_HOUR);
	if(colour_background || colour_hour) {
		GColor bg_colour = GColorFromHEX(colour_background);
		window_set_background_color(s_window, bg_colour);
		#if defined(PBL_COLOR)
// 			int colour_hour = persist_read_int(MESSAGE_KEY_COLOUR_HOUR);
			GColor hr_colour = GColorFromHEX(colour_hour);
			text_layer_set_text_color(s_hour_layer, hr_colour);
			int colour_minute = persist_read_int(MESSAGE_KEY_COLOUR_MINUTE);
			GColor mn_colour = GColorFromHEX(colour_minute);
			text_layer_set_text_color(s_minute_layer, mn_colour);
		#elif defined(PBL_BW)	
			text_layer_set_text_color(s_hour_layer, gcolor_legible_over(bg_colour));
			text_layer_set_text_color(s_minute_layer, gcolor_legible_over(bg_colour));
		#endif
	} else {
		window_set_background_color(s_window, GColorBlack);
		text_layer_set_text_color(s_hour_layer, GColorWhite);
		text_layer_set_text_color(s_minute_layer, GColorWhite);
	}
	bluetooth_callback(connection_service_peek_pebble_app_connection());
	update_time();
}

static void window_unload(Window *window) {
	text_layer_destroy(s_hour_layer);
	text_layer_destroy(s_minute_layer);
	text_layer_destroy(s_top_layer);
	text_layer_destroy(s_bottom_layer);
	fonts_unload_custom_font(s_time_font);
	fonts_unload_custom_font(s_date_font);
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

	const int inbox_size = 128;
	const int outbox_size = 128;
	app_message_register_inbox_received(inbox_received_handler);
	app_message_open(inbox_size, outbox_size);

	battery_state_service_subscribe(battery_callback);
	battery_callback(battery_state_service_peek());

	update_time();
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
	gbitmap_destroy(s_battery_bitmap);
	bitmap_layer_destroy(s_battery_layer);
	window_destroy(s_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}