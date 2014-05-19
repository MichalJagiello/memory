#ifndef __HIGHSCORES_H__
#define __HIGHSCORES_H__

#include <Elementary.h>
#include "game_types.h"

Evas_Object *highscores_window, *highscores_layout, *highscores_toolbar, *highscores_genlist, *highscores_back_button;

#define HIGHSCORES_LAYOUT "/usr/apps/org.tizen.memory/res/memory_layouts.edj"

void _show_highscores(void *data, Evas_Object *obj, void *event_info);
static Elm_Genlist_Item_Class *_itc = NULL;

#endif
