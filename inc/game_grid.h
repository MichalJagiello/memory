#ifndef __GAME_GRID_H__
#define __GAME_GRID_H__

#include <Elementary.h>
#define GAME_GRID_LAYOUT "/usr/apps/org.tizen.memory/res/memory_layouts.edj"
#define IMAGES_PATH "/usr/apps/org.tizen.memory/res/images/"

typedef struct
{
	void *value;
	Eina_Bool discovered;
	Eina_Bool visible;
	Evas_Object *icon;
} ButtonData;

Evas_Object *game_grid_window, *game_grid_restart_btn, *game_grid_pause_btn, *game_grid_layout;
Evas_Object *game_grid_clock, *game_grid_scores;

Eina_List *important_list;

Ecore_Timer *arcade_game_timer;
uint arcade_game_result;

void _show_classic_small_gg(void *data, Evas_Object *obj, void *event_info);
void _show_classic_medium_gg(void *data, Evas_Object *obj, void *event_info);
void _show_classic_big_gg(void *data, Evas_Object *obj, void *event_info);
void _show_arcade_gg(void *data, Evas_Object *obj, void *event_info);
void _on_game_finish_arcade(void);

#endif
