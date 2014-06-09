#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdlib.h>
#include <appcore-efl.h>
#include <Evas.h>
#include <Elementary.h>
#include <Ecore_X.h>
#include <Eina.h>
#include <Ecore.h>
#include "main_menu.h"
#include "highscores.h"
#include "game_grid.h"
#include "settings.h"

#define RESULTS_FILE "~/results.eet"

#define THEME_BUTTON "/usr/apps/org.tizen.memory/res/button.edj"

#define MAIN_MENU_LAYOUT "/usr/apps/org.tizen.memory/res/memory_layouts.edj"

#define MEMORY_ERROR 1
#define MEMORY_SUCCESS 0

enum {
	MEMORY_MAIN_WIN = 0,
	MEMORY_SETTINGS_WIN,
	MEMORY_HIGHSCORES_WIN,
	MEMORY_GAME_GRID_WIN,
	MEMORY_WINDOWS
};

static int log_domain;

typedef struct WinData
{
	Evas_Object *win;
	Evas_Object *layout;
} WinData;

typedef struct MemoryData {
	Evas_Object **memory_windows[MEMORY_WINDOWS];
	Evas_Object **memory_layouts[MEMORY_WINDOWS];
} MemoryData;

#endif
