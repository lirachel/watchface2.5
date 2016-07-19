#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;  
static GFont s_time_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static GBitmap *bitmap_image;
static BitmapLayer *bitmap_layer;

static int s_battery_level;
static Layer *s_battery_layer;

static TextLayer *s_battery_percentage;

static void battery_callback(BatteryChargeState state) {
  // Record the new battery level
  s_battery_level = state.charge_percent;
	// Update meter
  layer_mark_dirty(s_battery_layer);

}

static void battery_update_proc(Layer *layer, GContext *ctx) {
	
  GRect bounds = layer_get_bounds(layer);

  // Find the width of the bar
  int width = (int)(float)(((float)s_battery_level / 175.0F) * 25.0F);

  // Draw the background
  /*graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, bounds.size.h), 0, GCornerNone);*/
	
 //APP_LOG(APP_LOG_LEVEL_DEBUG, "Width: %d", width);
	
	static char buf[] = "00000000000";
	snprintf(buf, sizeof(buf), "%d%%", s_battery_level);
	text_layer_set_text(s_battery_percentage, buf);

  // Draw the bar
  if(s_battery_level>20){
	  graphics_context_set_fill_color(ctx, GColorGreen);
  } else if(s_battery_level<=20){
	  graphics_context_set_fill_color(ctx, GColorRed);
  }
	

  graphics_fill_rect(ctx, GRect(0, 0, width, bounds.size.h - 2), 0, GCornerNone);
 
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // format the way that the time text is going to be displayed
  strftime(buffer, sizeof("00:00"),"%I:%M", tick_time);


  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}


static void main_window_load(Window *window) {
  
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 180));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  

  //time layer
  s_time_layer = text_layer_create(GRect(0, 50, 144, 180));
  bitmap_layer_set_background_color(s_background_layer, GColorBlack);
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  
  // Use a system font in a TextLayer
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text(s_time_layer, "00:00");

  // Improve the layout to be more like a watchface

  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
	
  // Create battery meter Layer
  s_battery_layer = layer_create(GRect(5, 10, 100, 10));
  //background_layer_set_background_color(s_background_layer, GColorRed)
  layer_set_update_proc(s_battery_layer, battery_update_proc);

  // Add to Window

	
	
  bitmap_image = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_ICON);
    // This resource is set up to be different for each platform, so it will look good on both Color and Black & White displayer
  
// Set the size of the bitmap layer to the size of the screen, or the "bounds"
  bitmap_layer = bitmap_layer_create(GRect(5, 0, 105, 30));
  
// Put the bitmap image in the bitmap layer
  bitmap_layer_set_bitmap(bitmap_layer, bitmap_image);
  
// Put the image in the center of the screen
  bitmap_layer_set_alignment(bitmap_layer, GAlignLeft);
  bitmap_layer_set_background_color(bitmap_layer, GColorClear);
  
// Set the compositing mode so that transparent images are transparent
  bitmap_layer_set_compositing_mode(bitmap_layer, GCompOpSet);
	
  s_battery_percentage = text_layer_create(GRect(23, 5, 144, 168));
  text_layer_set_background_color(s_battery_percentage, GColorClear);
  text_layer_set_text_color(s_battery_percentage, GColorWhite);
	
  text_layer_set_font(s_battery_percentage, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	
	
  //text_layer_set_text(s_battery_percentage, "");
    
// Add the bitmap layer to the window
 layer_add_child(window_get_root_layer(window), s_battery_layer);
 layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_battery_percentage));
 layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bitmap_layer));


}

static void main_window_unload(Window *window) {
    
    // Unload GFont
    fonts_unload_custom_font(s_time_font);
    // Destroy time layer
    text_layer_destroy(s_time_layer);
	layer_destroy(s_battery_layer);
    
}

//register all events
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
 update_time();
 
}

  


static void init() {
 
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  // Register for battery level updates
//  battery_state_service_subscribe(battery_callback);
  
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Make sure the time is displayed from the start
  update_time();
	
	// Ensure battery level is displayed from the start
  battery_callback(battery_state_service_peek());

}

static void deinit() {
    // Destroy Window
    window_destroy(s_main_window);
}


//this is the main road map for the app  first it will initialize by running init(), then it will run the app until
//the user exits the app and then will run the deinit() code, which will free up all of the memory that we 
//set aside at the for the app.
int main(void) {
  init();
  app_event_loop();
  deinit();
	
	
}
