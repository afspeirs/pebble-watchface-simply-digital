#include <pebble.h>

#define	KEY_COLOUR_BACKGROUND 0
#define	KEY_COLOUR_HOUR 1
#define	KEY_COLOUR_MINUTE 2
#define KEY_COLOUR_DATE 3
#define KEY_CUSTOM1_DATE 4
#define	KEY_CUSTOM1_TOP	5
#define	KEY_CUSTOM1_BOTTOM 6

static Window *s_main_window;
static TextLayer *s_time_layerH, *s_time_layerM, *s_date_layerT, *s_date_layerB;
static GFont s_time_font, s_date_font;

char *custom1[] = {"","",""};
char *custom2[] = {"0101","Happy","New Year"};
char *custom3[] = {"3110","Happy","Halloween"};
char *custom4[] = {"2512","Merry","Christmas"};
char *custom5[] = {"1611","Happy","Birthday"};
char *custom6[] = {"","",""};
char *custom7[] = {"","",""};
char *custom8[] = {"","",""};
char *custom9[] = {"","",""};


//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// TIME ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void update_time() {
time_t temp = time(NULL); 
struct tm *tick_time = localtime(&temp);
		
static char date_current[16];
strftime(date_current, 80, "%d%m", tick_time);
	
// Hour
	static char bufferH[] = "00:00";
	if(clock_is_24h_style()) { strftime(bufferH, sizeof("00:00"), "%H", tick_time); } 
	else { strftime(bufferH, sizeof("00:00"), "%I", tick_time); }
	text_layer_set_text(s_time_layerH, bufferH);

// Minutes	
	static char bufferM[] = "00:00";
	strftime(bufferM, sizeof("00:00"), "%M", tick_time);
	text_layer_set_text(s_time_layerM, bufferM);

	
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// DATE ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

	static char date_bufferT[16];
	static char date_bufferB[16];
	if(strcmp(date_current,custom1[0]) == 0 && strcmp(custom1[1],"") != 0) {		// Custom 1
		strcpy(date_bufferT, custom1[1]);
	} if(strcmp(date_current,custom1[0]) == 0 && strcmp(custom1[2],"") != 0) {
		strcpy(date_bufferB, custom1[2]);
	} if(strcmp(date_current,custom2[0]) == 0 && strcmp(custom2[1],"") != 0) {		// Custom 2
		strcpy(date_bufferT, custom2[1]);
	} if(strcmp(date_current,custom2[0]) == 0 && strcmp(custom2[2],"") != 0) {
		strcpy(date_bufferB, custom2[2]);
	} if(strcmp(date_current,custom3[0]) == 0 && strcmp(custom3[1],"") != 0) {		// Custom 3
		strcpy(date_bufferT, custom3[1]);
	} if(strcmp(date_current,custom3[0]) == 0 && strcmp(custom3[2],"") != 0) {
		strcpy(date_bufferB, custom3[2]);
	} if(strcmp(date_current,custom4[0]) == 0 && strcmp(custom4[1],"") != 0) {		// Custom 4
		strcpy(date_bufferT, custom4[1]);
	} if(strcmp(date_current,custom4[0]) == 0 && strcmp(custom4[2],"") != 0) {
		strcpy(date_bufferB, custom4[2]);
	} if(strcmp(date_current,custom5[0]) == 0 && strcmp(custom5[1],"") != 0) {		// Custom 5
		strcpy(date_bufferT, custom5[1]);
	} if(strcmp(date_current,custom5[0]) == 0 && strcmp(custom5[2],"") != 0) {
		strcpy(date_bufferB, custom5[2]);
	} if(strcmp(date_current,custom6[0]) == 0 && strcmp(custom6[1],"") != 0) {		// Custom 6
		strcpy(date_bufferT, custom6[1]);
	} if(strcmp(date_current,custom6[0]) == 0 && strcmp(custom6[2],"") != 0) {
		strcpy(date_bufferB, custom6[2]);
	} if(strcmp(date_current,custom7[0]) == 0 && strcmp(custom7[1],"") != 0) {		// Custom 7
		strcpy(date_bufferT, custom7[1]);
	} if(strcmp(date_current,custom7[0]) == 0 && strcmp(custom7[2],"") != 0) {
		strcpy(date_bufferB, custom7[2]);
	} if(strcmp(date_current,custom8[0]) == 0 && strcmp(custom8[1],"") != 0) {		// Custom 8
		strcpy(date_bufferT, custom8[1]);
	} if(strcmp(date_current,custom8[0]) == 0 && strcmp(custom8[2],"") != 0) {
		strcpy(date_bufferB, custom8[2]);
	} if(strcmp(date_current,custom9[0]) == 0 && strcmp(custom9[1],"") != 0) {		// Custom 9
		strcpy(date_bufferT, custom9[1]);
	} if(strcmp(date_current,custom9[0]) == 0 && strcmp(custom9[2],"") != 0) {
		strcpy(date_bufferB, custom9[2]);
	} if(strlen(date_bufferT) == 0) {
		strftime(date_bufferT, sizeof(date_bufferT), "%A", tick_time);		// %A
	} if(strlen(date_bufferB) == 0) {
		strftime(date_bufferB, sizeof(date_bufferB), "%d  %b", tick_time);	// %d  %b
	}
	text_layer_set_text(s_date_layerT, date_bufferT);
	text_layer_set_text(s_date_layerB, date_bufferB);
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
	Tuple *colour_background_t = dict_find(iter, KEY_COLOUR_BACKGROUND);
	Tuple *colour_hour_t = dict_find(iter, KEY_COLOUR_HOUR);
	Tuple *color_minute_t = dict_find(iter, KEY_COLOUR_MINUTE);
	Tuple *color_date_t = dict_find(iter, KEY_COLOUR_DATE);
	Tuple *custom1_date_t = dict_find(iter, KEY_CUSTOM1_DATE);
	Tuple *custom1_top_t = dict_find(iter, KEY_CUSTOM1_TOP);
	Tuple *custom1_bottom_t = dict_find(iter, KEY_CUSTOM1_BOTTOM);
	
	if (colour_background_t) {
		int colour_back = colour_background_t->value->int32;
		persist_write_int(KEY_COLOUR_BACKGROUND, colour_back);
		GColor bg_colour = GColorFromHEX(KEY_COLOUR_BACKGROUND);
		window_set_background_color(s_main_window, bg_colour);
	}
	if (colour_hour_t) {
		int colour_hour = colour_hour_t->value->int32;
		persist_write_int(KEY_COLOUR_HOUR, colour_hour);
	    GColor hr_colour = GColorFromHEX(KEY_COLOUR_HOUR);
		text_layer_set_text_color(s_time_layerH, hr_colour);
	}
	if (color_minute_t) {
		int colour_minute = color_minute_t->value->int32;
		persist_write_int(KEY_COLOUR_MINUTE, colour_minute);
		GColor mn_colour = GColorFromHEX(KEY_COLOUR_MINUTE);
		text_layer_set_text_color(s_time_layerM, mn_colour);
	}
	if (color_date_t) {
		int colour_date = color_date_t->value->int32;
		persist_write_int(KEY_COLOUR_DATE, colour_date);
		GColor dt_colour = GColorFromHEX(KEY_COLOUR_DATE);
		text_layer_set_text_color(s_date_layerT, dt_colour);
		text_layer_set_text_color(s_date_layerB, dt_colour);
	}
	
	
	if (custom1_date_t) {
		char *custom1_date = custom1_date_t->value->cstring;
		persist_write_string(KEY_CUSTOM1_DATE, custom1_date);
		text_layer_set_text(s_date_layerT, custom1_date);
	}
	if (custom1_top_t) {
		char *custom1_top = custom1_top_t->value->cstring;
		persist_write_string(KEY_CUSTOM1_TOP, custom1_top);
	}
	if (custom1_bottom_t) {
		char *custom1_bottom = custom1_bottom_t->value->cstring;
		persist_write_string(KEY_CUSTOM1_BOTTOM, custom1_bottom);
	}
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// DISPLAY /////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void main_window_load(Window *window) {
	int back = persist_read_int(KEY_COLOUR_BACKGROUND);
    int hour = persist_read_int(KEY_COLOUR_HOUR);
    int minute = persist_read_int(KEY_COLOUR_MINUTE);
	int date = persist_read_int(KEY_COLOUR_DATE);
	
    GColor bg_colour = GColorFromHEX(back);
	GColor hr_colour = GColorFromHEX(hour);
	GColor mn_colour = GColorFromHEX(minute);
	GColor dt_colour = GColorFromHEX(date);


// Fonts
	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_72));
	s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_28));
  	
// Hour
	s_time_layerH = text_layer_create(GRect(-35, 37, 144, 100));
	text_layer_set_background_color(s_time_layerH, GColorClear);
	text_layer_set_text_color(s_time_layerH, hr_colour);
	text_layer_set_text(s_time_layerH, "00:00");
	text_layer_set_font(s_time_layerH, s_time_font);
	text_layer_set_text_alignment(s_time_layerH, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layerH));
	
// Minutes
	s_time_layerM = text_layer_create(GRect(36, 37, 144, 100));
	text_layer_set_background_color(s_time_layerM, GColorClear);
	text_layer_set_text_color(s_time_layerM, mn_colour);
	text_layer_set_text(s_time_layerM, "00:00");
	text_layer_set_font(s_time_layerM, s_time_font);
	text_layer_set_text_alignment(s_time_layerM, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layerM));

// Top
	s_date_layerT = text_layer_create(GRect(1, 11, 144, 30));
	text_layer_set_background_color(s_date_layerT, GColorClear);
	text_layer_set_text_color(s_date_layerT, dt_colour);
	text_layer_set_text_alignment(s_date_layerT, GTextAlignmentCenter);
	text_layer_set_text(s_date_layerT, "Sept 23");
	text_layer_set_font(s_date_layerT, s_date_font);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layerT));

// Bottom
	s_date_layerB = text_layer_create(GRect(1, 121, 144, 30));
	text_layer_set_background_color(s_date_layerB, GColorClear);
	text_layer_set_text_color(s_date_layerB, dt_colour);
	text_layer_set_text_alignment(s_date_layerB, GTextAlignmentCenter);
	text_layer_set_text(s_date_layerB, "Sept 23");
	text_layer_set_font(s_date_layerB, s_date_font);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layerB));

    window_set_background_color(s_main_window, bg_colour);
	
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
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

	app_message_register_inbox_received(inbox_received_handler);
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
//	update_time();
}

static void deinit() {
	window_destroy(s_main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}