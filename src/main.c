#include <pebble.h>
#include <pebble_app_info.h>
#include <pebble_fonts.h>
#include <stdio.h>
#include "turkish_time.h"
  

Window *window;
TextLayer *text_layer,*batterytext_layer;
TextLayer *fuzzy_text_layer_1,*fuzzy_text_layer_2,*fuzzy_text_layer_3;
static TextLayer *calendar_day_text_layer,*calendar_day_no_text_layer;
static TextLayer *calendar_month_text_layer;

char buffer[] = "00:00";

#define LINE_BUFFER_SIZE 50
static char daybuffer[LINE_BUFFER_SIZE];
static char daynobuffer[LINE_BUFFER_SIZE];
static char monthbuffer[LINE_BUFFER_SIZE];


static GRect battery_rect;
static GRect bluetooth_rect;
static GRect calendar_rect;
static GBitmap *battery;
static GBitmap *bluetooth;
static GBitmap *disconnect;
static GBitmap *calendar;
static BitmapLayer *batteryLayer;
static BitmapLayer *bluetoothLayer;
static BitmapLayer *calendarLayer;


typedef struct {
  TextLayer *layer[2];
  } TextLine;

typedef struct {
  char line1[LINE_BUFFER_SIZE];
  char line2[LINE_BUFFER_SIZE];
  char line3[LINE_BUFFER_SIZE];
} TheTime;

TextLine line1;
TextLine line2;
TextLine line3;

static TheTime cur_time;
static TheTime new_time;

GFont text_font;
GFont text_font_light;
GFont bar_font;
GFont fuzzy_text_font;

struct tm *t;
time_t temp;

//WatchItSelf-Here
void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  //Here we will update the watchface display
  //Format the buffer string using tick_time as the time source
  strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  int dakika = tick_time->tm_min;
  int saat = tick_time->tm_hour;
  if (!(dakika %5) || (dakika == 58) || (dakika == 2)) {
    fuzzy_time(saat, dakika, new_time.line1, new_time.line2, new_time.line3);
    text_layer_set_text(fuzzy_text_layer_1, new_time.line1);
    text_layer_set_text(fuzzy_text_layer_2, new_time.line2);
    text_layer_set_text(fuzzy_text_layer_3, new_time.line3);
    }
  text_layer_set_text(text_layer, buffer);
}
void update_fuzzy_watch() {
  temp = time(NULL);
  t = localtime(&temp);
  tick_handler(t, MINUTE_UNIT);
  
  fuzzy_time(t->tm_hour, t->tm_min, cur_time.line1, cur_time.line2, cur_time.line3);
  text_layer_set_text(fuzzy_text_layer_1, cur_time.line1);
  text_layer_set_text(fuzzy_text_layer_2, cur_time.line2);
  text_layer_set_text(fuzzy_text_layer_3, cur_time.line3);
  if((!(t->tm_min %5) || t->tm_min == 58 || t->tm_min == 2)) {
    fuzzy_time(t->tm_hour, t->tm_min, new_time.line1, new_time.line2, new_time.line3);
	   //update hour only if changed
  	if(strcmp(new_time.line1,cur_time.line1) != 0){
  		text_layer_set_text(fuzzy_text_layer_1, new_time.line1);
  	}
  	  //update min1 only if changed
  	if(strcmp(new_time.line2,cur_time.line2) != 0){
  		text_layer_set_text(fuzzy_text_layer_2, new_time.line2);
  	}
  	  //update min2 only if changed happens on
  	if(strcmp(new_time.line3,cur_time.line3) != 0){
  		text_layer_set_text(fuzzy_text_layer_3, new_time.line3);
    }
}
  
  strftime(daybuffer, sizeof(daybuffer), "%A", t);
  text_layer_set_text(calendar_day_text_layer, daybuffer);
  
  strftime(daynobuffer, sizeof(daynobuffer), "%e", t);
  text_layer_set_text(calendar_day_no_text_layer, daynobuffer);
  
  strftime(monthbuffer, sizeof(monthbuffer), "%B", t);
  text_layer_set_text(calendar_month_text_layer, monthbuffer);
  if (strlen (monthbuffer) > 5) {
    text_layer_set_font(calendar_month_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  }
  else {
    text_layer_set_font(calendar_month_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  }

}

//Battery
//Taken from Maneki Neko Watch - thanks to pi-king
static void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "100 ";
  if (charge_state.is_charging) {
    snprintf(battery_text, sizeof(battery_text), "+%d", charge_state.charge_percent);
  } else {
	  snprintf(battery_text, sizeof(battery_text), "%% %d", charge_state.charge_percent);
	  if (charge_state.charge_percent==20){
	  	vibes_double_pulse();
	  }else if(charge_state.charge_percent==10){
	  	vibes_long_pulse();
	  }
  }
  text_layer_set_text(batterytext_layer, battery_text);
  if (charge_state.charge_percent>75){
    battery = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_FULL);
  } else if (charge_state.charge_percent>50){
    battery = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_75);
  } else if (charge_state.charge_percent>25) {
    battery = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_50);  
  } else if (charge_state.charge_percent>20) {
    battery = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_25);
  } else {
    battery = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_20);
  }
}
//Bluetooth
//Taken from Maneki Neko Watch - thanks to pi-king
static void handle_bluetooth(bool connected) {
	layer_remove_from_parent(bitmap_layer_get_layer(bluetoothLayer));
    bitmap_layer_destroy(bluetoothLayer);
	
	bluetoothLayer = bitmap_layer_create(bluetooth_rect);
	if (connected) {
    bitmap_layer_set_bitmap(bluetoothLayer, bluetooth);
    //vibes_long_pulse();
	} else {
	bitmap_layer_set_bitmap(bluetoothLayer, disconnect);
	vibes_long_pulse();
	}
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bluetoothLayer));	
}
static void handle_appfocus(bool in_focus){
	if (in_focus){
	handle_bluetooth(bluetooth_connection_service_peek());
    }
}

void window_load(Window *window)
{
  Layer *rootLayer = window_get_root_layer(window);
  //We will create the creation of windows elements here!
  //WatchLayer
  text_layer = text_layer_create(GRect(0, 0, 144, 168));
  text_layer_set_background_color(text_layer, GColorBlack);
  text_layer_set_text_color(text_layer, GColorClear);
  text_layer_set_text_alignment(text_layer, GTextAlignmentLeft);
  text_layer_set_font(text_layer,fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  layer_add_child(rootLayer, (Layer*) text_layer);
  
  //Fuzzy Watch
  fuzzy_text_layer_1 = text_layer_create(GRect(40, 0, 104, 60));
  text_layer_set_text_color(fuzzy_text_layer_1, GColorClear);
  text_layer_set_background_color(fuzzy_text_layer_1, GColorBlack);
  text_layer_set_font(fuzzy_text_layer_1, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(fuzzy_text_layer_1, GTextAlignmentRight);
  layer_add_child(rootLayer,(Layer*) fuzzy_text_layer_1); //text_layer_get_layer(fuzzy_text_layer_1));
  
  
  fuzzy_text_layer_2 = text_layer_create(GRect(20, 40, 124, 90));
  text_layer_set_text_color(fuzzy_text_layer_2, GColorClear);
  text_layer_set_background_color(fuzzy_text_layer_2, GColorBlack);
  text_layer_set_font(fuzzy_text_layer_2, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(fuzzy_text_layer_2, GTextAlignmentRight);
  layer_add_child(rootLayer,(Layer*) fuzzy_text_layer_2); //text_layer_get_layer(fuzzy_text_layer_2));
  
  fuzzy_text_layer_3 = text_layer_create(GRect(20, 80, 124, 90));
  text_layer_set_text_color(fuzzy_text_layer_3, GColorClear);
  text_layer_set_background_color(fuzzy_text_layer_3, GColorBlack);
  text_layer_set_font(fuzzy_text_layer_3, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(fuzzy_text_layer_3, GTextAlignmentRight);
  layer_add_child(rootLayer,(Layer*) fuzzy_text_layer_3); //text_layer_get_layer(fuzzy_text_layer_3));
  
  
  //Battery Layer - thanks to pi-king
  //Text
  batterytext_layer = text_layer_create(GRect(10,35,30,65));
  text_layer_set_text_color(batterytext_layer, GColorWhite);
  text_layer_set_background_color(batterytext_layer, GColorBlack);
  text_layer_set_font(batterytext_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(batterytext_layer, GTextAlignmentLeft);
  layer_add_child(rootLayer,(Layer*) text_layer_get_layer(batterytext_layer));
  
  handle_battery(battery_state_service_peek());
  //Icon    
  battery_rect = GRect(0,10,33,43);
  batteryLayer = bitmap_layer_create(battery_rect);
  bitmap_layer_set_bitmap(batteryLayer, battery);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(batteryLayer));
  
  //Bluetooth Layer
  bluetooth = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_CONNECTED);
	disconnect = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_NOT_CONNECTED);
  bluetooth_rect = GRect(0,50,20,43);
  bluetoothLayer = bitmap_layer_create(bluetooth_rect);
  bitmap_layer_set_bitmap(bluetoothLayer, bluetooth);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bluetoothLayer));
  handle_bluetooth(bluetooth_connection_service_peek());
  
  //Calendar
  calendar = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CAL);
  calendar_rect = GRect(1,50,68,168);
  calendarLayer = bitmap_layer_create(calendar_rect);
  bitmap_layer_set_bitmap(calendarLayer, calendar);
  layer_add_child(rootLayer, bitmap_layer_get_layer(calendarLayer));
  //Day_Text
  calendar_day_text_layer = text_layer_create(GRect(1,100,68,14));
  text_layer_set_text_color(calendar_day_text_layer, GColorWhite);
  text_layer_set_background_color(calendar_day_text_layer, GColorClear);
  text_layer_set_font(calendar_day_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(calendar_day_text_layer, GTextAlignmentCenter);
  layer_add_child(rootLayer,(Layer*) text_layer_get_layer(calendar_day_text_layer));
  //Day_Number
  calendar_day_no_text_layer = text_layer_create(GRect(5,115,55,50));
  text_layer_set_text_color(calendar_day_no_text_layer, GColorBlack);
  text_layer_set_background_color(calendar_day_no_text_layer, GColorClear);
  text_layer_set_font(calendar_day_no_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(calendar_day_no_text_layer, GTextAlignmentCenter);
  layer_add_child(rootLayer,(Layer*) text_layer_get_layer(calendar_day_no_text_layer));
  //Month
  calendar_month_text_layer = text_layer_create(GRect(5,140,55,50));
  text_layer_set_text_color(calendar_month_text_layer, GColorBlack);
  text_layer_set_background_color(calendar_month_text_layer, GColorClear);
  //text_layer_set_font(calendar_month_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(calendar_month_text_layer, GTextAlignmentCenter);
  layer_add_child(rootLayer,(Layer*) text_layer_get_layer(calendar_month_text_layer));

  //Get a time structure so that the watchface does not start blank
  //Manualy call the tick_handler when the window is loading
  temp = time(NULL);
  t = localtime(&temp);
  tick_handler(t, MINUTE_UNIT);
  
  
}
void window_unload(Window *window)
{
  //We will safely destroy window's elements here!
  text_layer_destroy(text_layer);
  text_layer_destroy(fuzzy_text_layer_1);
  text_layer_destroy(fuzzy_text_layer_2);
  text_layer_destroy(fuzzy_text_layer_3);
  bitmap_layer_destroy(batteryLayer);
  text_layer_destroy(batterytext_layer);
  bitmap_layer_destroy(bluetoothLayer);
  gbitmap_destroy(bluetooth);
	gbitmap_destroy(disconnect);
  gbitmap_destroy(calendar);
}

void init()
{
  //Initialize the app event here!
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler) tick_handler);
  update_fuzzy_watch();
  battery_state_service_subscribe(&handle_battery);
  bluetooth_connection_service_subscribe(&handle_bluetooth);
  app_focus_service_subscribe(&handle_appfocus);
}
void deinit()
{
  //De-initiallize elements here to save memory
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
	app_focus_service_unsubscribe();
  window_stack_remove(window, false);
  window_destroy(window);  
}

int main(void)
{
  init();
  app_event_loop();
  deinit();
}