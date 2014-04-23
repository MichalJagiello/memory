#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <appcore-efl.h>
#include <Evas.h>
#include <Elementary.h>
#include <Ecore_X.h>
#include <Eina.h>
#include <Ecore.h>

#define MAIN_MENU_LAYOUT "/usr/apps/org.tizen.memory/res/main_menu.edj"

Evas_Object *win, *main_menu_layout, *table, *new_game_btn, *highscore_btn, *settings_btn;
static int log_domain;

void check_result_file()
{
	Eet_File *eef = eet_open("results.eet", EET_FILE_MODE_READ);
	if(eef)
	{
		eet_close(eef);
	}
	else
	{
		eef = eet_open("results.eet", EET_FILE_MODE_WRITE);
		int i = 1;
		for(; i <= 10; ++i)
		{
			char buf[5];
			sprintf(buf, "%d", i);
			eet_write(eef, buf, NULL, 0, 1);
		}
		eet_close(eef);
	}
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

	/*table = elm_table_add(win);
	elm_win_resize_object_add(win, table);
	evas_object_size_hint_weight_set(table, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_table_padding_set(table, 5, 5);*/
    
    new_game_btn = elm_hoversel_add(win);
	elm_object_text_set(new_game_btn, "New game");
	elm_object_part_content_set(main_menu_layout, "new_game_btn", new_game_btn);
	
    //evas_object_size_hint_weight_set(new_game_btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    //evas_object_size_hint_align_set(new_game_btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
	//elm_hoversel_item_add(new_game_btn, "Normal", NULL, ELM_ICON_NONE,
    //                      prepare_for_game, win);
    //elm_table_pack(table, new_game_btn, 0, 0, 1, 1);

	highscore_btn = elm_button_add(win);
	elm_object_text_set(highscore_btn, "Highscores");
	elm_object_part_content_set(main_menu_layout, "highscore_btn", highscore_btn);
    //evas_object_size_hint_weight_set(highscore_btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    //evas_object_size_hint_align_set(highscore_btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
    //elm_table_pack(table, highscore_btn, 0, 1, 1, 1);
    //evas_object_smart_callback_add(highscore_btn, "clicked", _show_highscores, win);

	settings_btn = elm_button_add(win);
	elm_object_text_set(settings_btn, "Settings");
	elm_object_part_content_set(main_menu_layout, "settings_btn", settings_btn);
    //evas_object_size_hint_weight_set(exit_btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    //evas_object_size_hint_align_set(exit_btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
    //elm_table_pack(table, exit_btn, 0, 2, 1, 1);
    //evas_object_smart_callback_add(exit_btn, "clicked", _exit_game, NULL);

	//evas_object_show(new_game_btn);
	//evas_object_show(highscore_btn);
	//evas_object_show(exit_btn);
	//evas_object_show(table);
	evas_object_show(main_menu_layout);
	evas_object_show(win);
	
	check_result_file();
	
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

    log_domain = eina_log_domain_register("Notifier", EINA_COLOR_CYAN);

    ops.data = NULL;
    ret = appcore_efl_main("Notifier", &argc, &argv, &ops);

    ecore_main_loop_begin();

    ecore_evas_shutdown();
    ecore_shutdown();
    evas_shutdown();
    eina_shutdown();

    return ret;
}
