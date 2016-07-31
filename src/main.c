#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layerH, *s_time_layerM, *s_date_layerT, *s_date_layerB;
static GFont s_time_font, s_date_font;
static BitmapLayer *s_battery_layer;
static GBitmap *s_battery_bitmap;

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Callbacks ///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void battery_callback(BatteryChargeState state) {
	char *select_battery_percent = "";
 	persist_read_string(MESSAGE_KEY_SELECT_BATTERY_PERCENT,select_battery_percent,5);
	int select_battery_percent_int = atoi(select_battery_percent);
	
	int battery = persist_read_int(MESSAGE_KEY_TOGGLE_BATTERY);
	if(battery == 1) { //on
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
	int background = persist_read_int(MESSAGE_KEY_COLOUR_BACKGROUND);
	int hour = persist_read_int(MESSAGE_KEY_COLOUR_HOUR);
	int bluetooth = persist_read_int(MESSAGE_KEY_COLOUR_BLUETOOTH);
	
	if(!connected) {		// Disconected
		if(bluetooth) {
			#if defined(PBL_COLOR)
				GColor bt_colour = GColorFromHEX(bluetooth);
				text_layer_set_text_color(s_time_layerH, bt_colour);
				text_layer_set_text_color(s_time_layerM, bt_colour);
			#elif defined(PBL_BW)
				GColor bg_colour = GColorFromHEX(background);
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
		if(background || hour) {
			#if defined(PBL_COLOR)
				GColor hr_colour = GColorFromHEX(hour);
				int minute = persist_read_int(MESSAGE_KEY_COLOUR_MINUTE);
				GColor mn_colour = GColorFromHEX(minute);
				text_layer_set_text_color(s_time_layerH, hr_colour);
				text_layer_set_text_color(s_time_layerM, mn_colour);	
			#elif defined(PBL_BW)
				GColor bg_colour = GColorFromHEX(background);
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
	Tuple *colour_background_t = dict_find(iter, MESSAGE_KEY_COLOUR_BACKGROUND);
	Tuple *colour_hour_t = dict_find(iter, MESSAGE_KEY_COLOUR_HOUR);
	Tuple *colour_minute_t = dict_find(iter, MESSAGE_KEY_COLOUR_MINUTE);
	Tuple *colour_date_t = dict_find(iter, MESSAGE_KEY_COLOUR_DATE);
	Tuple *colour_bluetooth_t = dict_find(iter, MESSAGE_KEY_COLOUR_BLUETOOTH);
	Tuple *toggle_battery_t = dict_find(iter, MESSAGE_KEY_TOGGLE_BATTERY);
	Tuple *toggle_suffix_t = dict_find(iter, MESSAGE_KEY_TOGGLE_SUFFIX);
	Tuple *select_battery_percent_t = dict_find(iter, MESSAGE_KEY_SELECT_BATTERY_PERCENT);
	
    int background = colour_background_t->value->int32;
	int hour = colour_hour_t->value->int32;
	int minute = colour_minute_t->value->int32;
	int date = colour_date_t->value->int32;
	int bluetooth = colour_bluetooth_t->value->int32;
	int battery = toggle_battery_t->value->int32;
	int suffix = toggle_suffix_t->value->int32;
	char *select_battery_percent = select_battery_percent_t->value->cstring;

	persist_write_int(MESSAGE_KEY_COLOUR_BACKGROUND, background);
	persist_write_int(MESSAGE_KEY_COLOUR_HOUR, hour);
	persist_write_int(MESSAGE_KEY_COLOUR_MINUTE, minute);
	persist_write_int(MESSAGE_KEY_COLOUR_DATE, date);
	persist_write_int(MESSAGE_KEY_COLOUR_BLUETOOTH, bluetooth);
	persist_write_int(MESSAGE_KEY_TOGGLE_BATTERY, battery);
	persist_write_int(MESSAGE_KEY_TOGGLE_SUFFIX, suffix);
	persist_write_string(MESSAGE_KEY_SELECT_BATTERY_PERCENT, select_battery_percent);

	GColor bg_colour = GColorFromHEX(background);
	window_set_background_color(s_main_window, bg_colour);
	
	#if defined(PBL_COLOR)
		GColor dt_colour = GColorFromHEX(date);
		text_layer_set_text_color(s_date_layerT, dt_colour);
		text_layer_set_text_color(s_date_layerB, dt_colour);
		GColor hr_colour = GColorFromHEX(hour);
		text_layer_set_text_color(s_time_layerH, hr_colour);
		GColor mn_colour = GColorFromHEX(minute);
		text_layer_set_text_color(s_time_layerM, mn_colour);
	#elif defined(PBL_BW)	
		text_layer_set_text_color(s_date_layerT, gcolor_legible_over(bg_colour));
		text_layer_set_text_color(s_date_layerB, gcolor_legible_over(bg_colour));
		text_layer_set_text_color(s_time_layerH, gcolor_legible_over(bg_colour));
		text_layer_set_text_color(s_time_layerM, gcolor_legible_over(bg_colour));
	#endif	
	battery_callback(battery_state_service_peek());
		
	if(strcmp("10", select_battery_percent) == 0) {
		text_layer_set_text(s_date_layerT, "10");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// TIME ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

char *getSuffix() {
	time_t temp = time(NULL); 
	struct tm *tick_time = localtime(&temp);
		
	static char date_current[5];
	strftime(date_current, 80, "%d%m", tick_time);

	if (strncmp(date_current, "01", 2) || strncmp(date_current, "21", 2) || strncmp(date_current,"31",2)) { 
		return "st";
	} else if (strncmp(date_current, "02", 2) || strncmp(date_current, "22", 2)) {
		return "nd";
	} else if (strncmp(date_current, "03", 2) || strncmp(date_current, "23", 2)) { 
		return "st";
	} else {
		return "th";
	}
}

char *getMonth() {
	time_t temp = time(NULL); 
	struct tm *tick_time = localtime(&temp);
		
	static char month_current[16];
	strftime(month_current, sizeof(month_current), "  %B", tick_time);
 	
	#if defined(PBL_RECT)
		if(strlen(month_current) > 9) {
			strftime(month_current, sizeof(month_current), "  %b", tick_time);
		}
	#elif defined(PBL_ROUND)
		if(strlen(month_current) > 7) {
 			strftime(month_current, sizeof(month_current), "  %b", tick_time);
	 	}
	#endif
	
	return month_current;
}

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

	if(strcmp("0104", date_current) == 0) {		// If date is 1st April, Display "April Fools" on bottom
		strcpy(date_bufferB, "April  Fools");
	} else {
		strcpy(date_bufferT, "\0");
		strcpy(date_bufferB, "\0");
	}
	
	if(strcmp(date_bufferT, "\0") == 0) {		// If Top is empty, write current weekday
		strftime(date_bufferT, sizeof(date_bufferT), "%A", tick_time);		// %A
	} if(strcmp(date_bufferB, "\0") == 0) {		// If Bottom is empty, write current date
		int suffix = persist_read_int(MESSAGE_KEY_TOGGLE_SUFFIX);
		char char_suffix[16] = "%e";
		if(suffix == 1) {		 // On
			strcat(char_suffix,getSuffix());
			strcat(char_suffix,getMonth());
		} else { 		//	if(suffix == 0) { // Off
			strcat(char_suffix,getMonth());
		}
		strftime(date_bufferB, sizeof(char_suffix), char_suffix, tick_time);	// ᵗʰ
	}
	text_layer_set_text(s_date_layerT, date_bufferT);
	text_layer_set_text(s_date_layerB, date_bufferB);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Load ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void main_window_load(Window *window) {
// Fonts
	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_BOLD_72));
	s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_REGULAR_28));
	
// Locations
	#if defined(PBL_RECT)
		s_time_layerH = text_layer_create(GRect(0, 37, 72, 100)); //x, y, h, w
		s_time_layerM = text_layer_create(GRect(73, 37, 72, 100));
		s_date_layerT = text_layer_create(GRect(0, 11, 144, 30));
		s_date_layerB = text_layer_create(GRect(0, 121, 144, 30));
		s_battery_layer = bitmap_layer_create(GRect(4, 2, 12, 12)); // battery
	#elif defined(PBL_ROUND)
		s_time_layerH = text_layer_create(GRect(19, 37+7, 72, 100)); //x, y, h, w
		s_time_layerM = text_layer_create(GRect(73+19, 37+7, 72, 100));
		s_date_layerT = text_layer_create(GRect(19, 11+7, 144, 30));
		s_date_layerB = text_layer_create(GRect(19, 121+7, 144, 30));
		s_battery_layer = bitmap_layer_create(GRect(84, 2, 12, 12)); // battery
	#endif

// Battery Image
	s_battery_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_WHITE);
	bitmap_layer_set_bitmap(s_battery_layer, s_battery_bitmap);
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
	int background = persist_read_int(MESSAGE_KEY_COLOUR_BACKGROUND);
	int date = persist_read_int(MESSAGE_KEY_COLOUR_DATE);
	
	if(background && date) {
		GColor bg_colour = GColorFromHEX(background);
		window_set_background_color(s_main_window, bg_colour);
		#if defined(PBL_COLOR)
			GColor dt_colour = GColorFromHEX(date);
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

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Other ///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void main_window_unload(Window *window) {
	text_layer_destroy(s_time_layerH);
	text_layer_destroy(s_time_layerM);
	text_layer_destroy(s_date_layerT);
	text_layer_destroy(s_date_layerB);
	fonts_unload_custom_font(s_time_font);
	fonts_unload_custom_font(s_date_font);
}

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

