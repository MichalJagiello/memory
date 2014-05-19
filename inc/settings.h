#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <Elementary.h>
#define SETTINGS_LAYOUT "/usr/apps/org.tizen.memory/res/memory_layouts.edj"
void _show_settings(void *data, Evas_Object *obj, void *event_info);
Evas_Object *settings_window, *settings_layout;
Evas_Object *settings_change_layout_btn, *settings_reset_highscores_btn;
Evas_Object *settings_back_btn;

#endif
