#define _GNU_SOURCE
#include <stdio.h>
#include "main.h"
#include "game_types.h"
#include <efl_assist.h>
#include <app.h>

int check_result_file()
{
	Eet_File *eef = eet_open("/home/app/memory/results.eet", EET_FILE_MODE_READ);
	if(eef)
	{
		eet_close(eef);
	}
	else
	{
		eef = eet_open("/home/app/memory/results.eet", EET_FILE_MODE_WRITE);
		if(!eef)
		{
			return MEMORY_ERROR;
		}
		int i = 0;
		for(; i <= ARCADE_GAME; ++i)
		{
			char buf[5];
			int j = 0;
			for(; j < 10; ++j)
			{
				sprintf(buf, "%d%d", i, j);
				int results[3];
				results[0] = -1;
				results[1] = -1;
				results[2] = -1;
				if(eet_write(eef, buf, results, sizeof(results)+1, 0) == 0)
					return MEMORY_ERROR;
			}
		}
		eet_close(eef);
	}
	return MEMORY_SUCCESS;
}

static void
_response_cb(void *data, Evas_Object *obj,
             void *event_info)
{
   evas_object_hide(data);
}

void _close_cb(void *data, Evas_Object *obj, void *event_info)
{
	MemoryData *md = (MemoryData*)data;
	evas_object_del(md->memory_layouts[MEMORY_MAIN_WIN]);
	evas_object_del(md->memory_windows[MEMORY_MAIN_WIN]);
	elm_exit();
}

static int _create_menu(void *data)
{
	MemoryData *md = (MemoryData*)data;
    elm_theme_extension_add(NULL, THEME_BUTTON);
	
	win = elm_win_add(NULL, "Shortcuts", ELM_WIN_BASIC);
    elm_win_title_set(win, "Add this shortcut?");
    elm_win_autodel_set(win, EINA_TRUE);
    elm_win_alpha_set(win, EINA_TRUE);
    elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

	main_menu_layout = elm_layout_add(win);
    evas_object_size_hint_weight_set(main_menu_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(win, main_menu_layout);
	elm_layout_file_set(main_menu_layout, MAIN_MENU_LAYOUT, "memory_main_layout");

    Evas_Object *btn = elm_button_add(win);
    evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_smart_callback_add(btn, "clicked", _show_classic_small_gg, win);
    elm_object_text_set(btn, "New game - small game grid");
	elm_layout_table_pack(main_menu_layout, "buttons_table", btn, 0, 0, 1, 1);
	elm_object_style_set(btn, "main_menu");
	evas_object_show(btn);

	btn = elm_button_add(win);
    evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_smart_callback_add(btn, "clicked", _show_classic_medium_gg, win);
    elm_object_text_set(btn, "New game - medium game grid");
	elm_layout_table_pack(main_menu_layout, "buttons_table", btn, 0, 1, 1, 1);
	evas_object_show(btn);

	btn = elm_button_add(win);
    evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_smart_callback_add(btn, "clicked", _show_classic_big_gg, win);
    elm_object_text_set(btn, "New game - big game grid");
	elm_layout_table_pack(main_menu_layout, "buttons_table", btn, 0, 2, 1, 1);
	evas_object_show(btn);

	btn = elm_button_add(win);
    evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_smart_callback_add(btn, "clicked", _show_arcade_gg, win);
    elm_object_text_set(btn, "New game - arcade");
	elm_layout_table_pack(main_menu_layout, "buttons_table", btn, 0, 3, 1, 1);
	evas_object_show(btn);

	btn = elm_button_add(win);
    evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_text_set(btn, "Highscores");
    evas_object_smart_callback_add(btn, "clicked", _show_highscores, win);
	elm_layout_table_pack(main_menu_layout, "buttons_table", btn, 0, 4, 1, 1);
	evas_object_show(btn);

    ea_object_event_callback_add(win, EA_CALLBACK_BACK, _close_cb, md);

	evas_object_show(main_menu_layout);
	evas_object_show(win);

	md->memory_windows[MEMORY_MAIN_WIN] = win;
	md->memory_layouts[MEMORY_MAIN_WIN] = main_menu_layout;
	
	if(check_result_file() == MEMORY_ERROR)
		return EXIT_FAILURE;
	
    return EXIT_SUCCESS;
}

static int _resume_memory(void *data)
{
	elm_win_raise(win);
	
	return EXIT_SUCCESS;
}

static int _terminate_memory(void *data)
{
    return EXIT_SUCCESS;
}


int main(int argc, char **argv)
{
	
	MemoryData *md;
    int ret;
    app_event_callback_s ops;
	ops.create = _create_menu;
	ops.terminate = _terminate_memory;
	ops.pause = NULL;
	ops.resume = NULL;
	ops.service = NULL;
	ops.low_memory = NULL;
	ops.low_battery = NULL;
	ops.device_orientation = NULL;
	ops.language_changed = NULL;
	ops.region_format_changed = NULL;

    eina_init();
    evas_init();
    ecore_init();
    ecore_evas_init();

    log_domain = eina_log_domain_register("eMemory", EINA_COLOR_CYAN);

	memset(&md, 0x0, sizeof(md));

    ret = app_efl_main(&argc, &argv, &ops, &md);

    ecore_main_loop_begin();

    ecore_evas_shutdown();
    ecore_shutdown();
    evas_shutdown();
    eina_shutdown();

    return ret;
}
