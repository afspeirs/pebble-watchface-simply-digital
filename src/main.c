#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layerH, *s_time_layerM, *s_date_layerT, *s_date_layerB;
static GFont s_time_font, s_date_font;

char *custom1[] = {"2703","Happy","Easter"};
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
	if(clock_is_24h_style()) { strftime(bufferH, sizeof("00:00"), "%H", tick_time); } 		//%H
	else { strftime(bufferH, sizeof("00:00"), "%I", tick_time); }							//%I
	text_layer_set_text(s_time_layerH, bufferH);

// Minutes	
	static char bufferM[] = "00:00";
	strftime(bufferM, sizeof("00:00"), "%M", tick_time);									//%M
	text_layer_set_text(s_time_layerM, bufferM);

	
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// DATE ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

	static char date_bufferT[16];
	static char date_bufferB[16];
	
	if(strcmp(date_current,custom1[0]) == 0 && strcmp(custom1[1],"") != 0) {		// Top
		strcpy(date_bufferT, custom1[1]);
	} else if(strcmp(date_current,custom2[0]) == 0 && strcmp(custom2[1],"") != 0) {
		strcpy(date_bufferT, custom2[1]);
	} else if(strcmp(date_current,custom3[0]) == 0 && strcmp(custom3[1],"") != 0) {
		strcpy(date_bufferT, custom3[1]);
	} else if(strcmp(date_current,custom4[0]) == 0 && strcmp(custom4[1],"") != 0) {
		strcpy(date_bufferT, custom4[1]);
	} else if(strcmp(date_current,custom5[0]) == 0 && strcmp(custom5[1],"") != 0) {
		strcpy(date_bufferT, custom5[1]);
	} else if(strcmp(date_current,custom6[0]) == 0 && strcmp(custom6[1],"") != 0) {
		strcpy(date_bufferT, custom6[1]);
	} else if(strcmp(date_current,custom7[0]) == 0 && strcmp(custom7[1],"") != 0) {
		strcpy(date_bufferT, custom7[1]);
	} else if(strcmp(date_current,custom8[0]) == 0 && strcmp(custom8[1],"") != 0) {
		strcpy(date_bufferT, custom8[1]);
	} else if(strcmp(date_current,custom9[0]) == 0 && strcmp(custom9[1],"") != 0) {
		strcpy(date_bufferT, custom9[1]);
	} else {
		strftime(date_bufferT, sizeof(date_bufferT), "%A", tick_time);		// %A
	}
	
	if(strcmp(date_current,custom1[0]) == 0 && strcmp(custom1[2],"") != 0) {		// Bottom
		strcpy(date_bufferB, custom1[2]);
	} else if(strcmp(date_current,custom2[0]) == 0 && strcmp(custom2[2],"") != 0) {
		strcpy(date_bufferB, custom2[2]);
	} else if(strcmp(date_current,custom3[0]) == 0 && strcmp(custom3[2],"") != 0) {
		strcpy(date_bufferB, custom3[2]);
	} else if(strcmp(date_current,custom4[0]) == 0 && strcmp(custom4[2],"") != 0) {
		strcpy(date_bufferB, custom4[2]);
	} else if(strcmp(date_current,custom5[0]) == 0 && strcmp(custom5[2],"") != 0) {
		strcpy(date_bufferB, custom5[2]);
	} else if(strcmp(date_current,custom6[0]) == 0 && strcmp(custom6[2],"") != 0) {
		strcpy(date_bufferB, custom6[2]);
	} else if(strcmp(date_current,custom7[0]) == 0 && strcmp(custom7[2],"") != 0) {
		strcpy(date_bufferB, custom7[2]);
	} else if(strcmp(date_current,custom8[0]) == 0 && strcmp(custom8[2],"") != 0) {
		strcpy(date_bufferB, custom8[2]);
	} else if(strcmp(date_current,custom9[0]) == 0 && strcmp(custom9[2],"") != 0) {
		strcpy(date_bufferB, custom9[2]);
	} else {
		strftime(date_bufferB, sizeof(date_bufferB), "%e  %b", tick_time);	// %d = 00 and %e = 0 and %b = MAR and %B = MARCH
	}
	text_layer_set_text(s_date_layerT, date_bufferT);
	text_layer_set_text(s_date_layerB, date_bufferB);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// DISPLAY /////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void main_window_load(Window *window) {
// Fonts
	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_72));
	s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_28));
  	
// Hour
	s_time_layerH = text_layer_create(GRect(-35, 37, 144, 100)); //5, 52, 139, 50
	text_layer_set_background_color(s_time_layerH, GColorClear);
	text_layer_set_text_color(s_time_layerH, GColorChromeYellow);
	text_layer_set_text(s_time_layerH, "00:00");
	text_layer_set_font(s_time_layerH, s_time_font);
	text_layer_set_text_alignment(s_time_layerH, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layerH));
	
// Minutes
	s_time_layerM = text_layer_create(GRect(36, 37, 144, 100)); //35, 40, 144, 100
	text_layer_set_background_color(s_time_layerM, GColorClear);
	text_layer_set_text_color(s_time_layerM, GColorWhite);
	text_layer_set_text(s_time_layerM, "00:00");
	text_layer_set_font(s_time_layerM, s_time_font);
	text_layer_set_text_alignment(s_time_layerM, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layerM));

// Top
	s_date_layerT = text_layer_create(GRect(1, 11, 144, 30)); //0, 125, 144, 30
	text_layer_set_text_color(s_date_layerT, GColorWhite);
	text_layer_set_background_color(s_date_layerT, GColorClear);
	text_layer_set_text_alignment(s_date_layerT, GTextAlignmentCenter);
	text_layer_set_text(s_date_layerT, "Sept 23");
	text_layer_set_font(s_date_layerT, s_date_font);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layerT));

// Bottom
	s_date_layerB = text_layer_create(GRect(1, 121, 144, 30)); //0, 125, 144, 30
	text_layer_set_text_color(s_date_layerB, GColorWhite);
	text_layer_set_background_color(s_date_layerB, GColorClear);
	text_layer_set_text_alignment(s_date_layerB, GTextAlignmentCenter);
	text_layer_set_text(s_date_layerB, "Sept 23");
	text_layer_set_font(s_date_layerB, s_date_font);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layerB));

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
	update_time();
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	window_set_background_color(s_main_window, GColorBlack);
}

static void deinit() {
	window_destroy(s_main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}