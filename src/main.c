#define _GNU_SOURCE
#include <stdio.h>
#include "main.h"
#include "game_types.h"
#include <efl_assist.h>

int check_result_file()
{
	Eet_File *eef = eet_open("results.eet", EET_FILE_MODE_READ);
	if(eef)
	{
		eet_close(eef);
	}
	else
	{
		eef = eet_open("results.eet", EET_FILE_MODE_WRITE);
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
	elm_exit();
}

static int _create_menu(void *data)
{
	win = elm_win_add(NULL, "Shortcuts", ELM_WIN_BASIC);
    elm_win_title_set(win, "Add this shortcut?");
    elm_win_autodel_set(win, EINA_TRUE);
    elm_win_alpha_set(win, EINA_TRUE);
    elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

	main_menu_layout = elm_layout_add(win);
    evas_object_size_hint_weight_set(main_menu_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(win, main_menu_layout);
	elm_layout_file_set(main_menu_layout, MAIN_MENU_LAYOUT, "memory_main_layout");
    
    new_game_btn = elm_hoversel_add(win);
	elm_object_text_set(new_game_btn, "New game");
	elm_object_part_content_set(main_menu_layout, "new_game_btn", new_game_btn);
	elm_hoversel_item_add(new_game_btn, "Classic - small", NULL, ELM_ICON_NONE, _show_classic_small_gg, win);
	elm_hoversel_item_add(new_game_btn, "Classic - medium", NULL, ELM_ICON_NONE, _show_classic_medium_gg, win);
	elm_hoversel_item_add(new_game_btn, "Classic - big", NULL, ELM_ICON_NONE, _show_classic_big_gg, win);
	elm_hoversel_item_add(new_game_btn, "Arcade", NULL, ELM_ICON_NONE, _show_arcade_gg, win);

	highscore_btn = elm_button_add(win);
	elm_object_text_set(highscore_btn, "Highscores");
	elm_object_part_content_set(main_menu_layout, "highscore_btn", highscore_btn);
    evas_object_smart_callback_add(highscore_btn, "clicked", _show_highscores, NULL);

	settings_btn = elm_button_add(win);
	elm_object_text_set(settings_btn, "Settings");
	elm_object_part_content_set(main_menu_layout, "settings_btn", settings_btn);
    evas_object_smart_callback_add(settings_btn, "clicked", _show_settings, NULL);
    
    ea_object_event_callback_add(win, EA_CALLBACK_BACK, _close_cb, NULL);

	evas_object_show(main_menu_layout);
	evas_object_show(win);
	
	if(check_result_file() == MEMORY_ERROR)
		return EXIT_SUCCESS;
	
    return EXIT_FAILURE;
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
    int ret;
    struct appcore_ops ops =
    {
        .create = _create_menu,
        .terminate = _terminate_memory,
        .pause = NULL,
        .resume = NULL,
        .reset = NULL
    };

    eina_init();
    evas_init();
    ecore_init();
    ecore_evas_init();

    log_domain = eina_log_domain_register("eMemory", EINA_COLOR_CYAN);

    ops.data = NULL;
    ret = appcore_efl_main("eMemory", &argc, &argv, &ops);

    ecore_main_loop_begin();

    ecore_evas_shutdown();
    ecore_shutdown();
    evas_shutdown();
    eina_shutdown();

    return ret;
}
