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

Evas_Object *game_grid_window, *game_grid_back_btn, *game_grid_pause_btn, *game_grid_layout;
Evas_Object *game_grid_clock;

Eina_List *important_list;

void _show_classic_gg(void *data, Evas_Object *obj, void *event_info);
