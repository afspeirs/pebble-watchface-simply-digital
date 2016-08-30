#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layerH, *s_time_layerM, *s_date_layerT, *s_date_layerB;
static GFont s_time_font, s_date_font;
static BitmapLayer *s_battery_layer;
static GBitmap *s_battery_bitmap;

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Methods /////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

void getBatteryIcon(int colour_background) {
 	if(colour_background) {
 		GColor bg_colour = GColorFromHEX(colour_background);
		if (gcolor_equal(gcolor_legible_over(bg_colour), GColorBlack)) { s_battery_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_BLACK); }
		else { s_battery_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_WHITE); }
	} else {
		s_battery_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_WHITE);
	}
	bitmap_layer_set_bitmap(s_battery_layer, s_battery_bitmap);
}

void getMonth(char *input, int location, struct tm *tick_time) {		// 0 is before, 1 is after
	char month_current[16];
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

void getSuffix(char *input, char date_current[5], struct tm *tick_time) {
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

void getDateBottom(char *input, char date_current[5], struct tm *tick_time) {
	char *select_date_bottom = "";
	persist_read_string(MESSAGE_KEY_SELECT_DATE_BOTTOM,select_date_bottom,5);
	int select_date_bottom_int = atoi(select_date_bottom);
	int toggle_suffix = persist_read_int(MESSAGE_KEY_TOGGLE_SUFFIX);	

	if(select_date_bottom_int == 1) { 		// Month Day
		getMonth(input, 1, tick_time);
		strcat(input," %e");
		if(toggle_suffix == 1) {
			getSuffix(input,date_current,tick_time);
		}
	} else {								// Default (Day Month)
		strcat(input,"%e");
		if(toggle_suffix == 1) {
			getSuffix(input,date_current,tick_time);
		}	
		getMonth(input, 0, tick_time);
	}
}

void customText(char date_bufferT[16], char date_bufferB[16], char date_current[5]) {
	int check_date_0 = persist_read_int(MESSAGE_KEY_CHECK_DATE);
	int check_date_1 = persist_read_int(MESSAGE_KEY_CHECK_DATE+1);
	int check_date_2 = persist_read_int(MESSAGE_KEY_CHECK_DATE+2);

	if(strcmp("0104", date_current) == 0) {		// If date is 1st April, Display "April Fools" on bottom
		strcpy(date_bufferT, "\0");
		strcpy(date_bufferB, "April  Fools");
	} else if(check_date_0 == 1 && strcmp("0101", date_current) == 0) {  // New Years
		strcpy(date_bufferT, "Happy");
		strcpy(date_bufferB, "New  Year");
	} else if(check_date_1 == 1 && strcmp("3110", date_current) == 0) {  // Christmas
		strcpy(date_bufferT, "\0");
		strcpy(date_bufferB, "Halloween");
	} else if(check_date_2 == 1 && strcmp("2512", date_current) == 0) {  // Halloween
		strcpy(date_bufferT, "Merry");
		strcpy(date_bufferB, "Christmas");
	}
	// rememberence sunday could be from the 8th to the 14th november
	else {
		strcpy(date_bufferT, "\0");
		strcpy(date_bufferB, "\0");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// TIME ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void update_time() {
	time_t temp = time(NULL); 
	struct tm *tick_time = localtime(&temp);
		
	static char date_current[16];
	strftime(date_current, 80, "%d%m", tick_time);

// Time
	static char bufferH[] = "00";
	static char bufferM[] = "00";
	if(clock_is_24h_style()) {
		strftime(bufferH, sizeof(bufferH), "%H", tick_time);	//%H
	} else {
		strftime(bufferH, sizeof(bufferH), "%I", tick_time);	//%I
	}
	strftime(bufferM, sizeof(bufferM), "%M", tick_time);		//%M
	text_layer_set_text(s_time_layerH, bufferH);
	text_layer_set_text(s_time_layerM, bufferM);

// Date
	static char date_bufferT[16];
	static char date_bufferB[16];

	customText(date_bufferT, date_bufferB, date_current);	
	if(strcmp(date_bufferT, "\0") == 0) {		// If Top is empty, write current weekday
		strftime(date_bufferT, sizeof(date_bufferT), "%A", tick_time);		// %A
	} if(strcmp(date_bufferB, "\0") == 0) {		// If Bottom is empty, write current date		
 		char char_suffix[32] = "";
		getDateBottom(char_suffix, date_current, tick_time);
		strftime(date_bufferB, sizeof(char_suffix), char_suffix, tick_time);	// ᵗʰ
	}
	text_layer_set_text(s_date_layerT, date_bufferT);
	text_layer_set_text(s_date_layerB, date_bufferB);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Callbacks ///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

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
	int colour_hour = persist_read_int(MESSAGE_KEY_COLOUR_HOUR);
	int colour_bluetooth = persist_read_int(MESSAGE_KEY_COLOUR_BLUETOOTH);
	
	if(!connected) {		// Disconected
		if(colour_bluetooth) {
			#if defined(PBL_COLOR)
				GColor bt_colour = GColorFromHEX(colour_bluetooth);
				text_layer_set_text_color(s_time_layerH, bt_colour);
				text_layer_set_text_color(s_time_layerM, bt_colour);
			#elif defined(PBL_BW)
				GColor bg_colour = GColorFromHEX(colour_bluetooth);
				text_layer_set_text_color(s_time_layerH, gcolor_legible_over(bg_colour));
				text_layer_set_text_color(s_time_layerM, gcolor_legible_over(bg_colour));
				text_layer_set_text_color(s_date_layerT, bg_colour);
				text_layer_set_text_color(s_date_layerB, bg_colour);
			#endif
		} else {
			#if defined(PBL_COLOR)
				text_layer_set_text_color(s_time_layerH, GColorRed);
				text_layer_set_text_color(s_time_layerM, GColorRed);
			#elif defined(PBL_BW)
				text_layer_set_text_color(s_time_layerH, GColorWhite);
				text_layer_set_text_color(s_time_layerM, GColorWhite);
				text_layer_set_text_color(s_date_layerT, GColorBlack);
				text_layer_set_text_color(s_date_layerB, GColorBlack);
			#endif
		}
		vibes_long_pulse();
	} else {				// Connected
		if(colour_background || colour_hour) {
			#if defined(PBL_COLOR)
				GColor hr_colour = GColorFromHEX(colour_hour);
				int colour_minute = persist_read_int(MESSAGE_KEY_COLOUR_MINUTE);
				GColor mn_colour = GColorFromHEX(colour_minute);
				text_layer_set_text_color(s_time_layerH, hr_colour);
				text_layer_set_text_color(s_time_layerM, mn_colour);	
			#elif defined(PBL_BW)
				GColor bg_colour = GColorFromHEX(colour_background);
				text_layer_set_text_color(s_time_layerH, gcolor_legible_over(bg_colour));
				text_layer_set_text_color(s_time_layerM, gcolor_legible_over(bg_colour));	
			#endif
		} else {	
			text_layer_set_text_color(s_time_layerH, GColorWhite);
			text_layer_set_text_color(s_time_layerM, GColorWhite);
		}
	}
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
// Colours
	Tuple *colour_background_t = dict_find(iter, MESSAGE_KEY_COLOUR_BACKGROUND);
	Tuple *colour_hour_t = dict_find(iter, MESSAGE_KEY_COLOUR_HOUR);
	Tuple *colour_minute_t = dict_find(iter, MESSAGE_KEY_COLOUR_MINUTE);
	Tuple *colour_date_t = dict_find(iter, MESSAGE_KEY_COLOUR_DATE);
	Tuple *colour_bluetooth_t = dict_find(iter, MESSAGE_KEY_COLOUR_BLUETOOTH);
    int colour_background = colour_background_t->value->int32;
	int colour_hour = colour_hour_t->value->int32;
	int colour_minute = colour_minute_t->value->int32;
	int colour_date = colour_date_t->value->int32;
	int colour_bluetooth = colour_bluetooth_t->value->int32;
	persist_write_int(MESSAGE_KEY_COLOUR_BACKGROUND, colour_background);
	persist_write_int(MESSAGE_KEY_COLOUR_HOUR, colour_hour);
	persist_write_int(MESSAGE_KEY_COLOUR_MINUTE, colour_minute);
	persist_write_int(MESSAGE_KEY_COLOUR_DATE, colour_date);
	persist_write_int(MESSAGE_KEY_COLOUR_BLUETOOTH, colour_bluetooth);	
// Battery
	Tuple *toggle_battery_t = dict_find(iter, MESSAGE_KEY_TOGGLE_BATTERY);
	Tuple *select_battery_percent_t = dict_find(iter, MESSAGE_KEY_SELECT_BATTERY_PERCENT);
	int toggle_battery = toggle_battery_t->value->int32;
	char *select_battery_percent = select_battery_percent_t->value->cstring;
	persist_write_int(MESSAGE_KEY_TOGGLE_BATTERY, toggle_battery);
	persist_write_string(MESSAGE_KEY_SELECT_BATTERY_PERCENT, select_battery_percent);
// Date	
	Tuple *select_date_bottom_t = dict_find(iter, MESSAGE_KEY_SELECT_DATE_BOTTOM);
	Tuple *toggle_suffix_t = dict_find(iter, MESSAGE_KEY_TOGGLE_SUFFIX);
	char *select_date_bottom = select_date_bottom_t->value->cstring;
	int toggle_suffix = toggle_suffix_t->value->int32;
	persist_write_string(MESSAGE_KEY_SELECT_DATE_BOTTOM, select_date_bottom);
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
	window_set_background_color(s_main_window, bg_colour);
	
	#if defined(PBL_COLOR)
		GColor dt_colour = GColorFromHEX(colour_date);
		text_layer_set_text_color(s_date_layerT, dt_colour);
		text_layer_set_text_color(s_date_layerB, dt_colour);
		GColor hr_colour = GColorFromHEX(colour_hour);
		text_layer_set_text_color(s_time_layerH, hr_colour);
		GColor mn_colour = GColorFromHEX(colour_minute);
		text_layer_set_text_color(s_time_layerM, mn_colour);
	#elif defined(PBL_BW)	
		text_layer_set_text_color(s_date_layerT, gcolor_legible_over(bg_colour));
		text_layer_set_text_color(s_date_layerB, gcolor_legible_over(bg_colour));
		text_layer_set_text_color(s_time_layerH, gcolor_legible_over(bg_colour));
		text_layer_set_text_color(s_time_layerM, gcolor_legible_over(bg_colour));
	#endif
	
	if(toggle_battery == 1) { //on
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

static void main_window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
// 	GRect bounds = layer_get_unobstructed_bounds(window_layer);
	
// Fonts
	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_BOLD_72));
	s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_REGULAR_28));
	
// Locations
	#if defined(PBL_RECT)
		s_time_layerH = text_layer_create(GRect( 0,  37, bounds.size.w / 2, 100));
		s_time_layerM = text_layer_create(GRect(72,  37, bounds.size.w / 2, 100));
		s_date_layerT = text_layer_create(GRect( 0,  11, bounds.size.w,      30));
		s_date_layerB = text_layer_create(GRect( 0, 121, bounds.size.w,      30));
// 		s_time_layerH = text_layer_create(GRect( 0, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
// 		s_time_layerM = text_layer_create(GRect(73, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
// 		s_date_layerT = text_layer_create(GRect( 0, bounds.size.h / 13 - 1, bounds.size.w,   30));
// 		s_date_layerB = text_layer_create(GRect( 0,	bounds.size.h * 3/4 -5, bounds.size.w,   30));
		s_battery_layer = bitmap_layer_create(GRect(4, 2, 12, 12)); // battery
	#elif defined(PBL_ROUND)
		s_time_layerH = text_layer_create(GRect(   10,  37+7, bounds.size.w / 2, 100));
		s_time_layerM = text_layer_create(GRect(72+10,  37+7, bounds.size.w / 2, 100));
		s_date_layerT = text_layer_create(GRect(    0,  11+7, bounds.size.w,      30));
		s_date_layerB = text_layer_create(GRect(    0, 121+7, bounds.size.w,      30));	
// 		s_time_layerH = text_layer_create(GRect(   10, bounds.size.h / 2 - 47 + 1, bounds.size.w/2, 75));
// 		s_time_layerM = text_layer_create(GRect(73+10, bounds.size.h / 2 - 47 + 1, bounds.size.w/2, 75));
// 		s_date_layerT = text_layer_create(GRect(    0, bounds.size.h / 12 + 3, bounds.size.w,   30));
// 		s_date_layerB = text_layer_create(GRect(    0, bounds.size.h * 3/4 -5-2, bounds.size.w,   30));
 		s_battery_layer = bitmap_layer_create(GRect(84, 2, 12, 12)); // battery
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
	text_layer_set_font(s_time_layerH, s_time_font);
	text_layer_set_text_alignment(s_time_layerH, GTextAlignmentCenter);
	text_layer_set_background_color(s_time_layerH, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layerH));
	
// Minutes
	text_layer_set_font(s_time_layerM, s_time_font);
	text_layer_set_text_alignment(s_time_layerM, GTextAlignmentCenter);
	text_layer_set_background_color(s_time_layerM, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layerM));

// Top
	text_layer_set_font(s_date_layerT, s_date_font);
	text_layer_set_text_alignment(s_date_layerT, GTextAlignmentCenter);
	text_layer_set_background_color(s_date_layerT, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layerT));

// Bottom
	text_layer_set_font(s_date_layerB, s_date_font);
	text_layer_set_text_alignment(s_date_layerB, GTextAlignmentCenter);
	text_layer_set_background_color(s_date_layerB, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layerB));

// Colours
//	int colour_background = persist_read_int(MESSAGE_KEY_COLOUR_BACKGROUND);
	int colour_date = persist_read_int(MESSAGE_KEY_COLOUR_DATE);
	
	if(colour_background || colour_date) {
		GColor bg_colour = GColorFromHEX(colour_background);
		window_set_background_color(s_main_window, bg_colour);
		#if defined(PBL_COLOR)
			GColor dt_colour = GColorFromHEX(colour_date);
			text_layer_set_text_color(s_date_layerT, dt_colour);
			text_layer_set_text_color(s_date_layerB, dt_colour);
		#elif defined(PBL_BW)	
			text_layer_set_text_color(s_date_layerT, gcolor_legible_over(bg_colour));
			text_layer_set_text_color(s_date_layerB, gcolor_legible_over(bg_colour));
		#endif
	} else {
		window_set_background_color(s_main_window, GColorBlack);
		text_layer_set_text_color(s_date_layerT, GColorWhite);
		text_layer_set_text_color(s_date_layerB, GColorWhite);
	}
	bluetooth_callback(connection_service_peek_pebble_app_connection());
	update_time();
}

static void main_window_unload(Window *window) {
	text_layer_destroy(s_time_layerH);
	text_layer_destroy(s_time_layerM);
	text_layer_destroy(s_date_layerT);
	text_layer_destroy(s_date_layerB);
	fonts_unload_custom_font(s_time_font);
	fonts_unload_custom_font(s_date_font);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Other ///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void init() {
	s_main_window = window_create();
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});
	window_stack_push(s_main_window, true);
	
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
	window_destroy(s_main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}