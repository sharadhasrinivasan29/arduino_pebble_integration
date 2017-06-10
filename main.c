#include "pebble.h"

#define NUM_MENU_SECTIONS 1
#define NUM_FIRST_MENU_ITEMS 24

static Window *s_main_window;
static SimpleMenuLayer *s_simple_menu_layer;
static SimpleMenuSection s_menu_sections[NUM_MENU_SECTIONS];
static SimpleMenuItem s_first_menu_items[NUM_FIRST_MENU_ITEMS];
// static void main_window_unload(Window *window);
// static void main_window_load(Window *window);


int index = 0;
int key;
char msg[100];
char msg0[100];
char msg1[100];
char msg2[100];
char msg3[100];
char msg4[100];
char msg5[100];
char msg6[100];
char msg7[100];
char msg8[100];
char msg9[100];
char msg10[100];
char msg11[100];
char msg12[100];
char msg13[100];
char msg14[100];
char msg15[100];
char msg16[100];
char msg17[100];
char msg18[100];
char msg19[100];
char msg20[100];
char msg21[100];
char msg22[100];
char msg23[100];



void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
 // outgoing message failed
//  text_layer_set_text(hello_layer, "Error out!");
  s_first_menu_items[index].subtitle = "Error out!";
}



void in_received_handler(DictionaryIterator *received, void *context) {
  // incoming message received
  // looks for key # in the incoming message
  Tuple *text_tuple = dict_find(received, key);
  if (text_tuple) {
    if (text_tuple->value) {
      if (key == 0) {
        strcpy(msg2, text_tuple->value->cstring);
        s_first_menu_items[2].subtitle = msg2;
      }
      if (key == 2) {
//         strcpy(msg1, text_tuple->value->cstring);
//         s_first_menu_items[1].subtitle = "Celsius Set!";
        s_first_menu_items[1].subtitle = msg0;
      }
      if (key == 1) {
//         strcpy(msg0, text_tuple->value->cstring);
//         SimpleMenuItem *menu_item = &s_first_menu_items[0];
//         menu_item->subtitle = "Fahrenheit Set!";
//         s_first_menu_items[0].subtitle = "Fahrenheit Set!";
//         s_first_menu_items[0].subtitle = msg0;
      }
      else if (key == 3) {
        strcpy(msg3, text_tuple->value->cstring);
        s_first_menu_items[4].subtitle = msg3;
      }      
      else if (key == 4) {
        strcpy(msg4, text_tuple->value->cstring);
        s_first_menu_items[5].subtitle = msg4;
      }
      else if (key == 5) {
        strcpy(msg5, text_tuple->value->cstring);
        s_first_menu_items[3].subtitle = msg5;
      }
      else if (key == 6) {
        strcpy(msg6, text_tuple->value->cstring);
        s_first_menu_items[6].subtitle = msg6;
      }
      else if (key == 7) {
        strcpy(msg7, text_tuple->value->cstring);
        s_first_menu_items[8].subtitle = msg7;
      }
      else if (key == 8) {
        strcpy(msg8, text_tuple->value->cstring);
        s_first_menu_items[9].subtitle = msg8;
      }
      else if (key == 9) {
        strcpy(msg9, text_tuple->value->cstring);
        s_first_menu_items[12].subtitle = msg9;
      }
      else if (key == 10) {
        strcpy(msg10, text_tuple->value->cstring);
        s_first_menu_items[10].subtitle = msg10;
      }
      else if (key == 11) {
        strcpy(msg11, text_tuple->value->cstring);
        s_first_menu_items[11].subtitle = msg11;
      }
      else if (key == 12) {
        strcpy(msg12, text_tuple->value->cstring);
        s_first_menu_items[23].subtitle = msg12;
      }
      else if (key == 13) {
        strcpy(msg13, text_tuple->value->cstring);
        s_first_menu_items[7].subtitle = msg13;
      }
      else if (key == 14) {
        strcpy(msg14, text_tuple->value->cstring);
        s_first_menu_items[10].subtitle = msg14;
      }
      else if (key == 15) {
        strcpy(msg15, text_tuple->value->cstring);
        s_first_menu_items[11].subtitle = msg15;
      }
      else if (key == 16) {
        strcpy(msg16, text_tuple->value->cstring);
        s_first_menu_items[15].subtitle = msg16;
      }
      else if (key == 17) {
        strcpy(msg17, text_tuple->value->cstring);
        s_first_menu_items[16].subtitle = msg17;
      }
      else if (key == 18) {
        strcpy(msg18, text_tuple->value->cstring);
        s_first_menu_items[17].subtitle = msg18;
      }
      else if (key == 19) {
        strcpy(msg19, text_tuple->value->cstring);
        s_first_menu_items[18].subtitle = msg19;
      }
      else if (key == 20) {
        strcpy(msg20, text_tuple->value->cstring);
        s_first_menu_items[19].subtitle = msg20;
      }
      else if (key == 21) {
        strcpy(msg21, text_tuple->value->cstring);
        s_first_menu_items[20].subtitle = msg21;
      }
      else if (key == 22) {
        strcpy(msg22, text_tuple->value->cstring);
        s_first_menu_items[21].subtitle = msg22;
      }
      else if (key == 23) {
        strcpy(msg23, text_tuple->value->cstring);
        s_first_menu_items[22].subtitle = msg23;
      }
    }
    else strcpy(msg, "no value.");
        s_first_menu_items[index].subtitle = msg;
    }
  else {
  // error message will show at the 0th index
//   s_first_menu_items[0].subtitle = "No value.";
  }
}



void in_dropped_handler(AppMessageResult reason, void *context) {
  // incoming message dropped
//   text_layer_set_text(hello_layer, "Error in!");
  s_first_menu_items[index].subtitle = "Error in!";
}




/* This is called when the select button is clicked */
// void select_click_handler(ClickRecognizerRef recognizer, void *context) {
//   // text_layer_set_text(hello_layer, "Selected!");
  
//   DictionaryIterator *iter;
//   app_message_outbox_begin(&iter);
// //   int key = 0;
//   // send the message "hello?" to the phone, using key #0
//   Tuplet value = TupletCString(key, "hello?");
//   dict_write_tuplet(iter, &value);
//   app_message_outbox_send();
// }

/* this registers the appropriate function to the appropriate button */
// void config_provider(void *context) {
//   window_single_click_subscribe(BUTTON_ID_SELECT,
//   select_click_handler);
// }



static void fahrenheit_select_callback(int index, void *ctx) {
//   index = 0;
  key = 1;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  // send the message to the phone
  Tuplet value = TupletCString(key, "Fahrenheit Set");
  s_first_menu_items[0].subtitle = "Fahrenheit set.";
  dict_write_tuplet(iter, &value);
//   layer_set_hidden((Layer*) s_simple_menu_layer, true);
//   layer_set_hidden((Layer*) text_layer, false);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}



static void celsius_select_callback(int index, void *ctx) {
//   index = 1;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 2;
  // send the message to the phone
  Tuplet value = TupletCString(key, "Celsius Set");
  s_first_menu_items[1].subtitle = "Celsius set.";
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}



static void recent_select_callback(int index, void *ctx) {
//   index = 2;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 0;
  // send the message to the phone
  Tuplet value = TupletCString(key, "Recent Temp");
//   s_first_menu_items[index].subtitle = "Recent Temp" + value;
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}



static void low_select_callback(int index, void *ctx) {
//   index = 3;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 5;
  // send the message to the phone
  Tuplet value = TupletCString(key, "Low Temp");
//   s_first_menu_items[index].subtitle = "Low Temp" + value;
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}



static void average_select_callback(int index, void *ctx) {
//   index = 4;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 3;
  // send the message to the phone
  Tuplet value = TupletCString(key, "Average Temp");
//   s_first_menu_items[index].subtitle = "Average Temp" + value;
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}
 


static void high_select_callback(int index, void *ctx) {
//   index = 5;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 4;
  // send the message to the phone
  Tuplet value = TupletCString(key, "High Temp");
//   s_first_menu_items[index].subtitle = "High Temp" + value;
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}



static void standby_on_select_callback(int index, void *ctx) {
//   index = 6;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 6;
  // send the message to the phone
  Tuplet value = TupletCString(key, "Entering Stand-by");
  s_first_menu_items[6].subtitle = "Entering Stand-by";
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}



static void standby_off_select_callback(int index, void *ctx) {
//   index = 7;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 13;
  // send the message to the phone
  Tuplet value = TupletCString(key, "Exiting Stand-by");
  s_first_menu_items[7].subtitle = "Exiting Stand-by";
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}



static void rising_falling_select_callback(int index, void *ctx) {
//   index = 8;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 7;
  // send the message to the phone
  Tuplet value = TupletCString(key, "Rising/Falling");
//   s_first_menu_items[index].subtitle = "Rising/Falling";
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}



static void print_select_callback(int index, void *ctx) {
//   index = 9;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 8;
  // send the message to the phone
  Tuplet value = TupletCString(key, "Printing Word 1");
  s_first_menu_items[9].subtitle = "Printing";
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}



static void cold_select_callback(int index, void *ctx) {
//   index = 12;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 10;
  // send the message to the phone
  Tuplet value = TupletCString(key, "Too cold");
  s_first_menu_items[12].subtitle = "Selected";
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}



static void hot_select_callback(int index, void *ctx) {
//   index = 13;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 11;
  // send the message to the phone
  Tuplet value = TupletCString(key, "Too hot");
  s_first_menu_items[13].subtitle = "Selected";
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}



static void comfy_select_callback(int index, void *ctx) {
//   index = 14;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 9;
  // send the message to the phone
  Tuplet value = TupletCString(key, "Just right");
  s_first_menu_items[14].subtitle = "Selected";
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}



static void drink_select_callback(int index, void *ctx) {
//   index = 23;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 12;
  // send the message to the phone
  Tuplet value = TupletCString(key, "Recommended Drink");
//   s_first_menu_items[index].subtitle = "Recommended Drink";
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}



static void print_2_select_callback(int index, void *ctx) {
//   index = 10;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 14;
  // send the message to the phone
  Tuplet value = TupletCString(key, "Printing Word 2");
  s_first_menu_items[10].subtitle = "Printing";
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}



static void print_3_select_callback(int index, void *ctx) {
//   index = 11;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 15;
   // send the message to the phone
  Tuplet value = TupletCString(key, "Printing Word 3");
  s_first_menu_items[11].subtitle = "Printing";
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}



static void select_hot_1_callback(int index, void *ctx) {
//   index = 16;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 17;
  // send the message to the phone
  Tuplet value = TupletCString(key, "35 hot");
  s_first_menu_items[16].subtitle = "Selected";
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}



static void select_hot_2_callback(int index, void *ctx) {
//   index = 17;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 18;
  // send the message to the phone
  Tuplet value = TupletCString(key, "30 hot");
  s_first_menu_items[17].subtitle = "Selected";
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}



static void select_hot_3_callback(int index, void *ctx) {
//   index = 18;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 19;
  // send the message to the phone
  Tuplet value = TupletCString(key, "25 hot");
  s_first_menu_items[18].subtitle = "Selected";
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}



static void select_cold_1_callback(int index, void *ctx) {
//   index = 20;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 21;
  // send the message to the phone
  Tuplet value = TupletCString(key, "0 Cold");
  s_first_menu_items[20].subtitle = "Selected";
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}



static void select_cold_2_callback(int index, void *ctx) {
//   index = 21;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 22;
  // send the message to the phone
  Tuplet value = TupletCString(key, "5 Cold");
  s_first_menu_items[21].subtitle = "Selected";
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}



static void select_cold_3_callback(int index, void *ctx) {
//   index = 22;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  key = 23;
  // send the message to the phone
  Tuplet value = TupletCString(key, "10 Cold");
  s_first_menu_items[22].subtitle = "Selected";
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}




static void main_window_load(Window *window) {

  // Although we already defined NUM_FIRST_MENU_ITEMS, you can define
  // an int as such to easily change the order of menu items later

  s_first_menu_items[0] = (SimpleMenuItem) {
    .title = "Set Fahrenheit",
//     .subtitle = msg0,
    .callback = fahrenheit_select_callback,
  };
  
  s_first_menu_items[1] = (SimpleMenuItem) {
    .title = "Set Celsius",
//     .subtitle = "B",
    .callback = celsius_select_callback,
  };
  
  s_first_menu_items[2] = (SimpleMenuItem) {
    .title = "Most Recent Temperature:",
//     .subtitle = "C",
    .callback = recent_select_callback,
  };
  
  s_first_menu_items[3] = (SimpleMenuItem) {
    .title = "Low Temperature:",
    .subtitle = "Low: ",
    .callback = low_select_callback,
  };
  
  s_first_menu_items[4] = (SimpleMenuItem) {
    .title = "Average Temperature:",
    .subtitle = "Average: ",
    .callback = average_select_callback,
  };
  
  s_first_menu_items[5] = (SimpleMenuItem) {
    .title = "High Temperature:",
    .subtitle = "High: ",
    .callback = high_select_callback,
  };
  
  s_first_menu_items[6] = (SimpleMenuItem) {
    .title = "Enter Standby Mode:",
//     .subtitle = "D ",
    .callback = standby_on_select_callback,
  };
  
  s_first_menu_items[7] = (SimpleMenuItem) {
    .title = "Leave Standby Mode:",
//     .subtitle = "E",
    .callback = standby_off_select_callback,
  };
  
  s_first_menu_items[8] = (SimpleMenuItem) {
    .title = "Is temp rising or falling?:",
//     .subtitle = "E",
    .callback = rising_falling_select_callback,
  };
  
  s_first_menu_items[9] = (SimpleMenuItem) {
    .title = "Print word 1 to 7SEG:",
//     .subtitle = "F",
    .callback = print_select_callback,
  };
  
  s_first_menu_items[10] = (SimpleMenuItem) {
    .title = "Print word 2 to 7SEG:",
//     .subtitle = "K",
    .callback = print_2_select_callback,
  };
  
  s_first_menu_items[11] = (SimpleMenuItem) {
    .title = "Print word 3 to 7SEG:",
//     .subtitle = "L",
    .callback = print_3_select_callback,
  };
  
  s_first_menu_items[12] = (SimpleMenuItem) {
    .title = "Click if too cold.",
//     .subtitle = "G",
    .callback = cold_select_callback,
  };
  
  s_first_menu_items[13] = (SimpleMenuItem) {
    .title = "Click if too hot.",
//     .subtitle = "H",
    .callback = hot_select_callback,
  };
  
  s_first_menu_items[14] = (SimpleMenuItem) {
    .title = "Click if you are just right.",
//     .subtitle = "I",
    .callback = comfy_select_callback,
  };
  
    s_first_menu_items[15] = (SimpleMenuItem) {
    .title = "Select Hot Temp",
//     .callback = print_select_callback,
  };
  
  s_first_menu_items[16] = (SimpleMenuItem) {
    .title = "35 degrees C",
//     .subtitle = "N",
    .callback = select_hot_1_callback,
  };
  
  s_first_menu_items[17] = (SimpleMenuItem) {
    .title = "30 degrees C",
//     .subtitle = "O",
    .callback = select_hot_2_callback,
  };
  
  s_first_menu_items[18] = (SimpleMenuItem) {
    .title = "25 degrees C",
//     .subtitle = "P",
    .callback = select_hot_3_callback,
  };
  
  s_first_menu_items[19] = (SimpleMenuItem) {
    .title = "Select Cold Temp",
//     .callback = print_select_callback,
  };
  
  s_first_menu_items[20] = (SimpleMenuItem) {
    .title = "0 degrees C",
//     .subtitle = "R",
    .callback = select_cold_1_callback,
  };
  
  s_first_menu_items[21] = (SimpleMenuItem) {
    .title = "5 degrees C",
//     .subtitle = "S",
    .callback = select_cold_2_callback,
  };
  
  s_first_menu_items[22] = (SimpleMenuItem) {
    .title = "10 degrees C",
//     .subtitle = "T",
    .callback = select_cold_3_callback,
  };
  
  s_first_menu_items[23] = (SimpleMenuItem) {
    .title = "Recommend a drink.",
//     .subtitle = "U",
    .callback = drink_select_callback,
  };
  
  
  s_menu_sections[0] = (SimpleMenuSection) {
    .num_items = NUM_FIRST_MENU_ITEMS,
    .items = s_first_menu_items,
  };
  
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  s_simple_menu_layer = simple_menu_layer_create(bounds, window, s_menu_sections, NUM_MENU_SECTIONS, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(s_simple_menu_layer));
  
  
//   text_layer = text_layer_create((GRect) { .origin = {0, 20}, .size = {bounds.size.h, 50}});
//   text_layer_set_text(text_layer, "Hello");
//   text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  
//   layer_set_hidden((Layer*) text_layer, true);
//   layer_add_child(window_layer, text_layer_get_layer(text_layer));

}



void main_window_unload(Window *window) {
  simple_menu_layer_destroy(s_simple_menu_layer);
}



static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  
  // need this for adding the listener
  // window_set_click_config_provider(s_main_window, config_provider);

  // for registering AppMessage handlers
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
//   app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);
  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
  
  const bool animated = true;
  window_stack_push(s_main_window, animated);
}



static void deinit() {
  window_destroy(s_main_window);
}



int main(void) {
  init();
  app_event_loop();
  deinit();
}