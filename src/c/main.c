#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_text_layer;
static Layer *s_bar_layer;

static AppTimer *s_class_timer;
static AppTimer *s_move_timer;

static int s_class_time;
static int s_move_time;

static char *s_timer_buf;

const int CLASS_DURATION = 45 * 60;
const int CLASS_TIMER_SEGMENT = CLASS_DURATION / 3;
const int CLASS_FINAL_SEGMENT = CLASS_TIMER_SEGMENT / 3;

static const uint32_t segments[] = { 400, 200, 400, 200, 400 };

static VibePattern s_vibe = {
  .durations = segments,
  .num_segments = ARRAY_LENGTH(segments),
};

static void class_timer_callback(void *data) {
  s_class_time--;
  layer_mark_dirty(s_bar_layer);
  
  if (s_class_time == CLASS_FINAL_SEGMENT) {
    vibes_double_pulse();
    s_class_timer = app_timer_register(1000, class_timer_callback, NULL);
  }
  else if (s_class_time == 0) {
    vibes_enqueue_custom_pattern(s_vibe);
  }
  else {
    s_class_timer = app_timer_register(1000, class_timer_callback, NULL);
  }
}

static void move_timer_callback(void *data) {
  s_move_time--;
  snprintf(s_timer_buf, 4, "%u", s_move_time);
  text_layer_set_text(s_text_layer, s_timer_buf);
  
  if (s_move_time == 0) {
    vibes_short_pulse();
  }
  else {
    s_move_timer = app_timer_register(1000, move_timer_callback, NULL);
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_move_time = 60;
  snprintf(s_timer_buf, 4, "%u", s_move_time);
  text_layer_set_text(s_text_layer, s_timer_buf);
  app_timer_cancel(s_move_timer);
  s_move_timer = app_timer_register(1000, move_timer_callback, NULL);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_move_time = 90;
  snprintf(s_timer_buf, 4, "%u", s_move_time);
  text_layer_set_text(s_text_layer, s_timer_buf);
  app_timer_cancel(s_move_timer);
  s_move_timer = app_timer_register(1000, move_timer_callback, NULL);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_move_time = 120;
  snprintf(s_timer_buf, 4, "%u", s_move_time);
  text_layer_set_text(s_text_layer, s_timer_buf);
  app_timer_cancel(s_move_timer);
  s_move_timer = app_timer_register(1000, move_timer_callback, NULL);
}

static void up_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_move_time = 150;
  snprintf(s_timer_buf, 4, "%u", s_move_time);
  text_layer_set_text(s_text_layer, s_timer_buf);
  app_timer_cancel(s_move_timer);
  s_move_timer = app_timer_register(1000, move_timer_callback, NULL);
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_move_time = 180;
  snprintf(s_timer_buf, 4, "%u", s_move_time);
  text_layer_set_text(s_text_layer, s_timer_buf);
  app_timer_cancel(s_move_timer);
  s_move_timer = app_timer_register(1000, move_timer_callback, NULL);
}

static void down_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_move_time = 210;
  snprintf(s_timer_buf, 4, "%u", s_move_time);
  text_layer_set_text(s_text_layer, s_timer_buf);
  app_timer_cancel(s_move_timer);
  s_move_timer = app_timer_register(1000, move_timer_callback, NULL);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_long_click_subscribe(BUTTON_ID_UP, 0, up_long_click_handler, NULL);
  window_long_click_subscribe(BUTTON_ID_SELECT, 0, select_long_click_handler, NULL);
  window_long_click_subscribe(BUTTON_ID_DOWN, 0, down_long_click_handler, NULL);
}

static void bar_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  int width = bounds.size.w;
  int height = bounds.size.w - 20;

  graphics_context_set_stroke_color(ctx, GColorCyan);
  graphics_context_set_stroke_width(ctx, 21);
  graphics_draw_line(ctx, GPoint(0, height), GPoint(0, 0));
  graphics_draw_line(ctx, GPoint(0, 0), GPoint(width, 0));
  graphics_draw_line(ctx, GPoint(width, 0), GPoint(width, height));

  graphics_context_set_stroke_color(ctx, GColorShockingPink);
  graphics_context_set_stroke_width(ctx, 41);

  if (s_class_time > (CLASS_TIMER_SEGMENT * 2)) {
    graphics_draw_line(ctx, GPoint(0, height), GPoint(0, 0));
    graphics_draw_line(ctx, GPoint(0, 0), GPoint(width, 0));
    graphics_draw_line(ctx, GPoint(width, 0), GPoint(width, (((s_class_time - CLASS_TIMER_SEGMENT * 2) * 100 / CLASS_TIMER_SEGMENT) * height / 100)));
  }
  else if (s_class_time > (CLASS_TIMER_SEGMENT)) {
    graphics_draw_line(ctx, GPoint(0, height), GPoint(0, 0));
    graphics_draw_line(ctx, GPoint(0, 0), GPoint((((s_class_time - CLASS_TIMER_SEGMENT) * 100 / CLASS_TIMER_SEGMENT) * width) / 100, 0));
  }
  else if (s_class_time > 0) {
    if (s_class_time <= CLASS_FINAL_SEGMENT) {
      graphics_context_set_stroke_color(ctx, GColorDarkCandyAppleRed);
    }

    graphics_draw_line(ctx, GPoint(0, height), GPoint(0, height - ((s_class_time * 100 / CLASS_TIMER_SEGMENT) * height) / 100));
  }
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_text_layer = text_layer_create(GRect(0, 80, bounds.size.w, 80));
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  s_bar_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
  layer_set_update_proc(s_bar_layer, bar_update_proc);
  layer_add_child(window_layer, s_bar_layer);
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
}

static void init(void) {
  s_timer_buf = malloc(6);
  s_class_time = CLASS_DURATION;
  s_class_timer = app_timer_register(1000, class_timer_callback, NULL);
  s_main_window = window_create();
  window_set_click_config_provider(s_main_window, click_config_provider);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
  light_enable(true);
}

static void deinit(void) {
  window_destroy(s_main_window);
  light_enable(false);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
