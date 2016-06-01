#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layerH, *s_time_layerM, *s_date_layerT, *s_date_layerB;
static GFont s_time_font, s_date_font;

enum {
	KEY_COLOUR_BACKGROUND = 0,	// Key: 0
	KEY_COLOUR_DATE,        	// Key: 1
	KEY_COLOUR_HOUR,			// Key: 2
	KEY_COLOUR_MINUTE			// Key: 3
};

char *dates[]	= {"0104","","April  Fools"};

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
// Colours
	Tuple *colour_background_t = dict_find(iter, KEY_COLOUR_BACKGROUND);
	Tuple *colour_hour_t = dict_find(iter, KEY_COLOUR_HOUR);
	Tuple *colour_minute_t = dict_find(iter, KEY_COLOUR_MINUTE);
	Tuple *colour_date_t = dict_find(iter, KEY_COLOUR_DATE);

    int background = colour_background_t->value->int32;
	int hour = colour_hour_t->value->int32;
	int minute = colour_minute_t->value->int32;
	int date = colour_date_t->value->int32;

	persist_write_int(KEY_COLOUR_BACKGROUND, background);
	persist_write_int(KEY_COLOUR_HOUR, hour);
	persist_write_int(KEY_COLOUR_MINUTE, minute);
	persist_write_int(KEY_COLOUR_DATE, date);

	GColor bg_colour = GColorFromHEX(background);
	window_set_background_color(s_main_window, bg_colour);
	GColor dt_colour = GColorFromHEX(date);
	text_layer_set_text_color(s_date_layerT, dt_colour);
	text_layer_set_text_color(s_date_layerB, dt_colour);
	GColor hr_colour = GColorFromHEX(hour);
	text_layer_set_text_color(s_time_layerH, hr_colour);
	GColor mn_colour = GColorFromHEX(minute);
	text_layer_set_text_color(s_time_layerM, mn_colour);
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
	int len = sizeof(dates)/sizeof(dates[0]);

// Top
	for(int i = 0; i < len; ++i) {
		if(!strcmp(dates[i], date_current)) {
			if(strcmp(dates[i+1], "\0") == 0) {
				strftime(date_bufferT, sizeof(date_bufferT), "%A", tick_time);		// %A
			} else {
				strftime(date_bufferT, sizeof(date_bufferT), dates[i+1], tick_time);
			}
			if(strcmp(dates[i+2], "\0") == 0) {
				strftime(date_bufferB, sizeof(date_bufferB), "%e  %B", tick_time);	// %e  %B
			} else {
				strftime(date_bufferB, sizeof(date_bufferB), dates[i+2], tick_time);
			}	
			break;
		} else {
			strftime(date_bufferT, sizeof(date_bufferT), "%A", tick_time);		// %A
			strftime(date_bufferB, sizeof(date_bufferB), "%e  %B", tick_time);	// %e  %B
		}
	}
	text_layer_set_text(s_date_layerT, date_bufferT);
	text_layer_set_text(s_date_layerB, date_bufferB);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// BLUETOOTH ///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void bluetooth_callback(bool connected) {
	int hour = persist_read_int(KEY_COLOUR_HOUR);
	GColor hr_colour = GColorFromHEX(hour);
	int minute = persist_read_int(KEY_COLOUR_MINUTE);
	GColor mn_colour = GColorFromHEX(minute);
	
	if(!connected) {
		text_layer_set_text_color(s_time_layerH, GColorRed);
		text_layer_set_text_color(s_time_layerM, GColorRed);
		vibes_long_pulse();
	} else {
		if(hour && minute) {
			text_layer_set_text_color(s_time_layerH, hr_colour);
			text_layer_set_text_color(s_time_layerM, mn_colour);
		} else {
			text_layer_set_text_color(s_time_layerH, GColorWhite);
			text_layer_set_text_color(s_time_layerM, GColorWhite);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// DISPLAY /////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void main_window_load(Window *window) {
// Fonts
	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_72));
	s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_28));
  	
// Hour
	s_time_layerH = text_layer_create(GRect(0, 37, 72, 100)); //0, 37, 72, 100
	text_layer_set_font(s_time_layerH, s_time_font);
	text_layer_set_text_alignment(s_time_layerH, GTextAlignmentCenter);
	text_layer_set_background_color(s_time_layerH, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layerH));
	
// Minutes
	s_time_layerM = text_layer_create(GRect(73, 37, 72, 100)); //73, 37, 72, 100
	text_layer_set_font(s_time_layerM, s_time_font);
	text_layer_set_text_alignment(s_time_layerM, GTextAlignmentCenter);
	text_layer_set_background_color(s_time_layerM, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layerM));

// Top
	s_date_layerT = text_layer_create(GRect(0, 11, 144, 30)); //0, 11, 144, 30
	text_layer_set_font(s_date_layerT, s_date_font);
	text_layer_set_text_alignment(s_date_layerT, GTextAlignmentCenter);
	text_layer_set_background_color(s_date_layerT, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layerT));

// Bottom
	s_date_layerB = text_layer_create(GRect(0, 121, 144, 30)); //0, 121, 144, 30
	text_layer_set_font(s_date_layerB, s_date_font);
	text_layer_set_text_alignment(s_date_layerB, GTextAlignmentCenter);
	text_layer_set_background_color(s_date_layerB, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layerB));

// Colours
	int background = persist_read_int(KEY_COLOUR_BACKGROUND);
	int date = persist_read_int(KEY_COLOUR_DATE);
	
	if(background && date) {
		GColor bg_color = GColorFromHEX(background);
		window_set_background_color(s_main_window, bg_color);
		GColor dt_colour = GColorFromHEX(date);
		text_layer_set_text_color(s_date_layerT, dt_colour);
		text_layer_set_text_color(s_date_layerB, dt_colour);
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
	
	update_time();
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
	window_destroy(s_main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}

