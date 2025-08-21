#include <pebble.h>

#define SETTINGS_KEY 1 // Persistent storage key

// Named constants
#define APP_MESSAGE_INBOX_SIZE 256
#define APP_MESSAGE_OUTBOX_SIZE 256
#define ACCEL_TAP_HIDE_DELAY_MS 3500
#define ACCEL_TAP_RESET_DATE_DELAY_MS 3500

// Forward declarations
static void getIcon(GBitmap **bitmap_ptr, BitmapLayer *bitmapLayer, int imageBlack, int imageWhite);
static void update_time(struct tm *tick_time);
static void update_date(struct tm *tick_time);
static void update_icon_positions();
static void update_quiet_time_icon();

// Global static variables
static Window *s_window;
static TextLayer *s_text_hour, *s_text_minute, *s_text_top, *s_text_bottom;
static GFont s_font_time, s_font_date;
static BitmapLayer *s_layer_battery, *s_layer_bluetooth, *s_layer_quiet;
static GBitmap *s_bitmap_battery, *s_bitmap_bluetooth, *s_bitmap_quiet;
static bool s_app_started = false;
char s_b_buffer[32], s_b_buffer_custom[32];

// Clay settings struct
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
} ClaySettings;

static ClaySettings settings;

// Struct for handling special dates
typedef struct {
  const char* date_str; // "DDMM"
  const char* display_str;
  const bool* setting_flag;
} SpecialDate;

// --- CONFIGURATION ---

static void config_default() {
  settings.ColourBackground         = GColorBlack;
  settings.ColourHour               = PBL_IF_BW_ELSE(GColorWhite, GColorChromeYellow);
  settings.ColourMinute             = GColorWhite;
  settings.ColourDate               = GColorWhite;
  settings.ToggleBluetooth          = true;
  settings.ToggleBluetoothQuietTime = false;
  settings.SelectBluetooth          = 2;
  settings.SelectBatteryPercent     = 0;
  settings.TogglePowerSave          = false;
  settings.ToggleSuffix             = false;
  settings.ToggleCalendarWeek       = false;
  settings.CheckDate0               = true;
  settings.CheckDate1               = false;
  settings.CheckDate2               = false;
  settings.CheckDate3               = false;
  settings.CheckDate4               = false;
  settings.CheckDate5               = false;
}

static void config_load() {
  config_default();

  // Read settings from persistent storage. Check return value.
  if (persist_read_data(SETTINGS_KEY, &settings, sizeof(settings)) == S_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Settings loaded from persistent storage.");
  } else {
    APP_LOG(APP_LOG_LEVEL_INFO, "No settings found in persistent storage, using defaults.");
  }
}

// --- UI & HELPERS ---

void getIcon(GBitmap **bitmap_ptr, BitmapLayer *bitmapLayer, int imageBlack, int imageWhite) {
  if (*bitmap_ptr) {
    gbitmap_destroy(*bitmap_ptr);
    *bitmap_ptr = NULL;
  }
  if (gcolor_equal(gcolor_legible_over(settings.ColourBackground), GColorBlack)) {
    *bitmap_ptr = gbitmap_create_with_resource(imageBlack);
  } else {
    *bitmap_ptr = gbitmap_create_with_resource(imageWhite);
  }
  bitmap_layer_set_bitmap(bitmapLayer, *bitmap_ptr);
}

static void update_quiet_time_icon() {
  if (quiet_time_is_active()) {
    getIcon(&s_bitmap_quiet, s_layer_quiet, RESOURCE_ID_IMAGE_QUIET_TIME_BLACK, RESOURCE_ID_IMAGE_QUIET_TIME_WHITE);
    layer_set_hidden(bitmap_layer_get_layer(s_layer_quiet), false);
  } else {
    layer_set_hidden(bitmap_layer_get_layer(s_layer_quiet), true);
  }
}

static void update_icon_positions() {
  bool quiet_time = quiet_time_is_active();
  GRect battery_frame;
  GRect quiet_frame;

  #if PBL_DISPLAY_HEIGHT == 180 // Chalk
    battery_frame = quiet_time ? GRect(84, 17, 13, 6) : GRect(84, 10, 13, 6);
    quiet_frame = GRect(86, 3, 10, 10);
  #else // Other platforms
    battery_frame = quiet_time ? GRect(22, 4, 13, 6) : GRect(6, 4, 13, 6);
    quiet_frame = GRect(6, 2, 10, 10);
  #endif

  layer_set_frame(bitmap_layer_get_layer(s_layer_battery), battery_frame);
  layer_set_frame(bitmap_layer_get_layer(s_layer_quiet), quiet_frame);
}

void setColours() {
  window_set_background_color(s_window, settings.ColourBackground);

  #ifdef PBL_COLOR
    text_layer_set_text_color(s_text_hour, settings.ColourHour);
    text_layer_set_text_color(s_text_minute, settings.ColourMinute);
    text_layer_set_text_color(s_text_top, settings.ColourDate);
    text_layer_set_text_color(s_text_bottom, settings.ColourDate);
  #else
    GColor legible_color = gcolor_legible_over(settings.ColourBackground);
    text_layer_set_text_color(s_text_hour, legible_color);
    text_layer_set_text_color(s_text_minute, legible_color);
    text_layer_set_text_color(s_text_top, legible_color);
    text_layer_set_text_color(s_text_bottom, legible_color);
  #endif
}

void textToggle(bool hide) {
  layer_set_hidden(text_layer_get_layer(s_text_hour), !hide);
  layer_set_hidden(text_layer_get_layer(s_text_minute), !hide);
  layer_set_hidden(text_layer_get_layer(s_text_top), !hide);
  layer_set_hidden(text_layer_get_layer(s_text_bottom), !hide);
}

static void dateResetToCustom() {
  text_layer_set_text(s_text_bottom, s_b_buffer_custom);
}

// --- TIME & DATE LOGIC ---

static void update_time(struct tm *tick_time) {
  static char h_buffer[3], m_buffer[3];

  strftime(h_buffer, sizeof(h_buffer), clock_is_24h_style() ? "%H" : "%I", tick_time);
  strftime(m_buffer, sizeof(m_buffer), "%M", tick_time);
  text_layer_set_text(s_text_hour, h_buffer);
  text_layer_set_text(s_text_minute, m_buffer);
}

static void update_date(struct tm *tick_time) {
  static char date_current[5]; // Stores "DDMM"
  static char month_full_name[16]; // Stores full month name, e.g., "September"
  static char t_buffer[16]; // Stores day of week, e.g., "Friday"

  // Always get full month name to determine length for abbreviation logic
  strftime(date_current, sizeof(date_current), "%d%m", tick_time);
  strftime(month_full_name, sizeof(month_full_name), "%B", tick_time);

  // Top (Day of Week)
  strftime(t_buffer, sizeof(t_buffer), "%A", tick_time);

  // --- Handle Custom Dates (Data-Driven Approach) ---
  const SpecialDate special_dates[] = {
    {"0104", "April  Fools", NULL},
    {"0101", "Happy  %Y", &settings.CheckDate0},
    {"2501", "Burns  Night", &settings.CheckDate1},
    {"3110", "Halloween", &settings.CheckDate2},
    {"2412", PBL_IF_ROUND_ELSE("xmas  Eve", "Christmas  Eve"), &settings.CheckDate3},
    {"2512", "Christmas", &settings.CheckDate4},
    {"2612", "Boxing  Day", &settings.CheckDate5},
  };

  s_b_buffer_custom[0] = '\0';
  for (size_t i = 0; i < ARRAY_LENGTH(special_dates); i++) {
    if (
      strcmp(special_dates[i].date_str, date_current) == 0 &&
      (special_dates[i].setting_flag == NULL || *special_dates[i].setting_flag)
    ) {
      if (strstr(special_dates[i].display_str, "%Y")) {
        strftime(s_b_buffer_custom, sizeof(s_b_buffer_custom), special_dates[i].display_str, tick_time);
      } else {
        strcpy(s_b_buffer_custom, special_dates[i].display_str);
      }
      break;
    }
  }

  // --- Main Date Formatting (Day, Suffix, Month, Week) ---
  char temp_day_str[6]; // e.g., "1st", "23rd"
  char temp_month_str[8]; // e.g., "Sep", "September" (up to 7 chars + null for short)
  char temp_week_str[5]; // e.g., " W30"

  // 1. Format Day with Suffix
  int day_of_month = tick_time->tm_mday;
  const char *suffix = "";
  if (settings.ToggleSuffix) {
    if (day_of_month % 10 == 1 && day_of_month != 11) {
      suffix = "st";
    } else if (day_of_month % 10 == 2 && day_of_month != 12) {
      suffix = "nd";
    } else if (day_of_month % 10 == 3 && day_of_month != 13) {
      suffix = "rd";
    } else {
      suffix = "th";
    }
  }
  snprintf(temp_day_str, sizeof(temp_day_str), "%d%s", day_of_month, suffix);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Day string: %s", temp_day_str);

  // 2. Format Month (short or full)
  #if PBL_DISPLAY_HEIGHT == 180 // Chalk
    if (strlen(month_full_name) > 5) {
      strftime(temp_month_str, sizeof(temp_month_str), "%b", tick_time); // Abbreviated month
    } else {
      strftime(temp_month_str, sizeof(temp_month_str), "%B", tick_time); // Full month
    }
  #else // Aplite, Basalt, Diorite
    if (strlen(month_full_name) > 7 || settings.ToggleCalendarWeek) {
      strftime(temp_month_str, sizeof(temp_month_str), "%b", tick_time); // Abbreviated month
    } else {
      strftime(temp_month_str, sizeof(temp_month_str), "%B", tick_time); // Full month
    }
  #endif
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Month string: %s", temp_month_str);

  // 3. Format Calendar Week (if enabled)
  temp_week_str[0] = '\0';
  if (settings.ToggleCalendarWeek) {
    strftime(temp_week_str, sizeof(temp_week_str), " W%V", tick_time);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Week string: %s", temp_week_str);
  }

  // 4. Combine all parts into s_b_buffer
  int written = snprintf(s_b_buffer, sizeof(s_b_buffer), "%s  %s%s", temp_day_str, temp_month_str, temp_week_str);
  if (written < 0 || (size_t)written >= sizeof(s_b_buffer)) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Buffer overflow for main date string!");
    s_b_buffer[0] = '\0';
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Final main date buffer: %s", s_b_buffer);

  text_layer_set_text(s_text_top, t_buffer); // Day of Week
  if (s_b_buffer_custom[0] != '\0') {
    text_layer_set_text(s_text_bottom, s_b_buffer_custom); // Custom text (e.g., "Christmas")
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Displaying custom date: %s", s_b_buffer_custom);
  } else {
    text_layer_set_text(s_text_bottom, s_b_buffer); // Normal date (e.g., "25th December")
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Displaying normal date: %s", s_b_buffer);
  }
}

// --- EVENT HANDLERS ---

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if (MINUTE_UNIT & units_changed) {
    if (!settings.TogglePowerSave) {
      update_time(tick_time);
    }
    update_quiet_time_icon();
    update_icon_positions();
  }
  if ((DAY_UNIT & units_changed) && !settings.TogglePowerSave) {
    update_date(tick_time);
  }
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  if (settings.TogglePowerSave) {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    update_time(tick_time);
    update_date(tick_time);
    textToggle(true);
    AppTimer *updateTimer = app_timer_register(ACCEL_TAP_HIDE_DELAY_MS, (AppTimerCallback) textToggle, (void*)false);
    (void)updateTimer; // Suppress unused variable warning
  } else {
    if (s_b_buffer_custom[0] != '\0') {
      text_layer_set_text(s_text_bottom, s_b_buffer);
      AppTimer *updateTimer = app_timer_register(ACCEL_TAP_RESET_DATE_DELAY_MS, (AppTimerCallback) dateResetToCustom, NULL);
      (void)updateTimer; // Suppress unused variable warning
    }
  }
}

static void battery_callback(BatteryChargeState state) {
  if (state.charge_percent <= settings.SelectBatteryPercent) {
    getIcon(&s_bitmap_battery, s_layer_battery, RESOURCE_ID_IMAGE_BATTERY_BLACK, RESOURCE_ID_IMAGE_BATTERY_WHITE);
    layer_set_hidden(bitmap_layer_get_layer(s_layer_battery), false);
  } else {
    layer_set_hidden(bitmap_layer_get_layer(s_layer_battery), true);
  }
  update_icon_positions();
}

static void bluetooth_callback(bool connected) {
  if (!connected) {
    if (s_app_started && !(quiet_time_is_active() && !settings.ToggleBluetoothQuietTime)) {
      switch (settings.SelectBluetooth) {
        case 0: /* No vibration */ break;
        case 1: vibes_short_pulse(); break;
        case 2: vibes_long_pulse(); break;
        case 3: vibes_double_pulse(); break;
        default: vibes_long_pulse(); break;
      }
    }
    if (settings.ToggleBluetooth) {
      getIcon(&s_bitmap_bluetooth, s_layer_bluetooth, RESOURCE_ID_IMAGE_BLUETOOTH_BLACK, RESOURCE_ID_IMAGE_BLUETOOTH_WHITE);
      layer_set_hidden(bitmap_layer_get_layer(s_layer_bluetooth), false);
    }
  } else {
    layer_set_hidden(bitmap_layer_get_layer(s_layer_bluetooth), true);
  }
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  // APP_LOG(APP_LOG_LEVEL_INFO, "Processing received settings...");

  // Colours
  Tuple *bg_colour_t = dict_find(iter, MESSAGE_KEY_COLOUR_BACKGROUND);
  if (bg_colour_t) {
    settings.ColourBackground = GColorFromHEX(bg_colour_t->value->int32);
    // APP_LOG(APP_LOG_LEVEL_INFO, "  Background Color = %d", settings.ColourBackground.argb);
  }
  Tuple *hr_colour_t = dict_find(iter, MESSAGE_KEY_COLOUR_HOUR);
  if (hr_colour_t) {
    settings.ColourHour = GColorFromHEX(hr_colour_t->value->int32);
    // APP_LOG(APP_LOG_LEVEL_INFO, "  Hour Color = %d", settings.ColourHour.argb);
  }
  Tuple *mn_colour_t = dict_find(iter, MESSAGE_KEY_COLOUR_MINUTE);
  if (mn_colour_t) {
    settings.ColourMinute = GColorFromHEX(mn_colour_t->value->int32);
    // APP_LOG(APP_LOG_LEVEL_INFO, "  Minute Color = %d", settings.ColourMinute.argb);
  }
  Tuple *dt_colour_t = dict_find(iter, MESSAGE_KEY_COLOUR_DATE);
  if (dt_colour_t) {
    settings.ColourDate = GColorFromHEX(dt_colour_t->value->int32);
    // APP_LOG(APP_LOG_LEVEL_INFO, "  Date Color = %d", settings.ColourDate.argb);
  }

  // Bluetooth
  Tuple *bt_toggle_t = dict_find(iter, MESSAGE_KEY_TOGGLE_BLUETOOTH);
  if (bt_toggle_t) {
    settings.ToggleBluetooth = bt_toggle_t->value->int32 == 1;
    // APP_LOG(APP_LOG_LEVEL_INFO, "  Bluetooth Icon Toggle = %d", settings.ToggleBluetooth);
  }
  Tuple *bq_toggle_t = dict_find(iter, MESSAGE_KEY_TOGGLE_BLUETOOTH_QUIET_TIME);
  if (bq_toggle_t) {
    settings.ToggleBluetoothQuietTime = bq_toggle_t->value->int32 == 1;
    // APP_LOG(APP_LOG_LEVEL_INFO, "  Bluetooth Quiet Time Toggle = %d", settings.ToggleBluetoothQuietTime);
  }
  Tuple *bt_select_t = dict_find(iter, MESSAGE_KEY_SELECT_BLUETOOTH_VIBRATION);
  if (bt_select_t) {
    settings.SelectBluetooth = atoi(bt_select_t->value->cstring);
    // APP_LOG(APP_LOG_LEVEL_INFO, "  Bluetooth Vibration = %d", settings.SelectBluetooth);
  }

  // Battery
  Tuple *bp_select_t = dict_find(iter, MESSAGE_KEY_SELECT_BATTERY_PERCENT);
  if (bp_select_t) {
    settings.SelectBatteryPercent = atoi(bp_select_t->value->cstring);
    // APP_LOG(APP_LOG_LEVEL_INFO, "  Battery Percent Threshold = %d", settings.SelectBatteryPercent);
  }

  Tuple *bd_toggle_t = dict_find(iter, MESSAGE_KEY_TOGGLE_POWER_SAVE);
  if (bd_toggle_t) {
    settings.TogglePowerSave = bd_toggle_t->value->int32 == 1;
    // APP_LOG(APP_LOG_LEVEL_INFO, "  Power Save Toggle = %d", settings.TogglePowerSave);
  }

  // Bottom Text
  Tuple *su_toggle_t = dict_find(iter, MESSAGE_KEY_TOGGLE_SUFFIX);
  if (su_toggle_t) {
    settings.ToggleSuffix = su_toggle_t->value->int32 == 1;
    // APP_LOG(APP_LOG_LEVEL_INFO, "  Date Suffix Toggle = %d", settings.ToggleSuffix);
  }
  Tuple *wk_toggle_t = dict_find(iter, MESSAGE_KEY_TOGGLE_WEEK);
  if (wk_toggle_t) {
    settings.ToggleCalendarWeek = wk_toggle_t->value->int32 == 1;
    // APP_LOG(APP_LOG_LEVEL_INFO, "  Calendar Week Toggle = %d", settings.ToggleCalendarWeek);
  }

  // Custom Text
  for (uint16_t i = 0; i < 6; ++i) {
    Tuple *dt_check_t = dict_find(iter, MESSAGE_KEY_CHECK_DATE + i);
    if (dt_check_t) {
      bool value = dt_check_t->value->uint16;
      // APP_LOG(APP_LOG_LEVEL_INFO, "  CheckDate[%d] = %d", (int)i, value);
      switch (i) {
        case 0: settings.CheckDate0 = value; break;
        case 1: settings.CheckDate1 = value; break;
        case 2: settings.CheckDate2 = value; break;
        case 3: settings.CheckDate3 = value; break;
        case 4: settings.CheckDate4 = value; break;
        case 5: settings.CheckDate5 = value; break;
      }
    }
  }

  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));

  battery_callback(battery_state_service_peek());
  s_app_started = false;
  bluetooth_callback(connection_service_peek_pebble_app_connection());
  s_app_started = true;
  setColours();

  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  update_date(tick_time);

  if (settings.TogglePowerSave) {
    textToggle(false);
  } else {
    textToggle(true);
  }

  update_quiet_time_icon();
}

void unobstructed_change(AnimationProgress progress, void* data) {
  GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(s_window));
  layer_set_frame(text_layer_get_layer(s_text_hour),   GRect(              0, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
  layer_set_frame(text_layer_get_layer(s_text_minute), GRect(bounds.size.w/2, bounds.size.h / 2 - 47, bounds.size.w/2, 75));
  layer_set_frame(text_layer_get_layer(s_text_top),    GRect(              0, bounds.size.h / 4 - 31, bounds.size.w,   30));
  layer_set_frame(text_layer_get_layer(s_text_bottom), GRect(              0, bounds.size.h * 3/4 -5, bounds.size.w,   30));
}

// --- WINDOW LOAD & UNLOAD ---

static void window_load(Window *window) {
  setlocale(LC_ALL, "");
  Layer *window_layer = window_get_root_layer(window);

  // Fonts
  s_font_time = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_BOLD_72));
  s_font_date = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BEBAS_NEUE_REGULAR_28));

  // Create layers with dummy GRects; positions will be set later
  s_text_hour     = text_layer_create(GRectZero);
  s_text_minute   = text_layer_create(GRectZero);
  s_text_top      = text_layer_create(GRectZero);
  s_text_bottom   = text_layer_create(GRectZero);
  s_layer_battery = bitmap_layer_create(GRectZero);
  s_layer_quiet   = bitmap_layer_create(GRectZero);
  #if PBL_DISPLAY_HEIGHT == 180 // Chalk
    s_layer_bluetooth = bitmap_layer_create(GRect(84, 150, 7, 11));
  #else // Aplite, Basalt, Diorite
    s_layer_bluetooth = bitmap_layer_create(GRect(130, 3, 7, 11));
  #endif

  // Configure text layers
  TextLayer *text_layers[] = {s_text_hour, s_text_minute, s_text_top, s_text_bottom};
  for (size_t i = 0; i < ARRAY_LENGTH(text_layers); i++) {
    text_layer_set_text_alignment(text_layers[i], GTextAlignmentCenter);
    text_layer_set_background_color(text_layers[i], GColorClear);
    layer_add_child(window_layer, text_layer_get_layer(text_layers[i]));
  }
  text_layer_set_font(s_text_hour, s_font_time);
  text_layer_set_font(s_text_minute, s_font_time);
  text_layer_set_font(s_text_top, s_font_date);
  text_layer_set_font(s_text_bottom, s_font_date);

  // Configure icon layers
  BitmapLayer *icon_layers[] = {s_layer_battery, s_layer_bluetooth, s_layer_quiet};
  for (size_t i = 0; i < ARRAY_LENGTH(icon_layers); i++) {
    #if defined(PBL_COLOR)
      bitmap_layer_set_compositing_mode(icon_layers[i], GCompOpSet);
    #endif
    layer_add_child(window_layer, bitmap_layer_get_layer(icon_layers[i]));
  }

  // Initial UI state setup
  setColours();
  battery_callback(battery_state_service_peek());
  s_app_started = false;
  bluetooth_callback(connection_service_peek_pebble_app_connection());
  s_app_started = true;
  update_quiet_time_icon();
  update_icon_positions();

  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  update_time(tick_time);
  update_date(tick_time);

  if (settings.TogglePowerSave) {
    textToggle(false);
  } else {
    textToggle(true);
  }

  unobstructed_change(ANIMATION_NORMALIZED_MAX, NULL);
}

static void window_unload(Window *window) {
  if (s_text_hour) text_layer_destroy(s_text_hour);
  if (s_text_minute) text_layer_destroy(s_text_minute);
  if (s_text_top) text_layer_destroy(s_text_top);
  if (s_text_bottom) text_layer_destroy(s_text_bottom);

  fonts_unload_custom_font(s_font_time);
  fonts_unload_custom_font(s_font_date);
}

// --- APP INIT & DEINIT ---

static void init() {
  config_load();

  s_window = window_create();
  if (!s_window) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to create window!");
    return;
  }

  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_stack_push(s_window, true);

  UnobstructedAreaHandlers handlers = { .change = unobstructed_change };
  unobstructed_area_service_subscribe(handlers, NULL);
  connection_service_subscribe((ConnectionHandlers) { .pebble_app_connection_handler = bluetooth_callback });
  accel_tap_service_subscribe(accel_tap_handler);
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(APP_MESSAGE_INBOX_SIZE, APP_MESSAGE_OUTBOX_SIZE);
  battery_state_service_subscribe(battery_callback);
  battery_callback(battery_state_service_peek());

  tick_timer_service_subscribe(MINUTE_UNIT | DAY_UNIT, tick_handler);
}

static void deinit() {
  // Unsubscribe from services to be tidy
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  accel_tap_service_unsubscribe();
  connection_service_unsubscribe();

  // Destroy Bitmaps
  if (s_bitmap_battery) gbitmap_destroy(s_bitmap_battery);
  if (s_bitmap_bluetooth) gbitmap_destroy(s_bitmap_bluetooth);
  if (s_bitmap_quiet) gbitmap_destroy(s_bitmap_quiet);

  // Destroy BitmapLayers
  if (s_layer_battery) bitmap_layer_destroy(s_layer_battery);
  if (s_layer_bluetooth) bitmap_layer_destroy(s_layer_bluetooth);
  if (s_layer_quiet) bitmap_layer_destroy(s_layer_quiet);

  // Destroy the Window
  if (s_window) window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
