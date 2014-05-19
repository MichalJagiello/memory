#include "settings.h"
#include "game_types.h"

void _settings_reset_highscores_cb(void *data, Evas_Object *obj, void *event_info)
{
	Eet_File *eef = eet_open("results.eet", EET_FILE_MODE_WRITE);
	if(!eef)
	{
		return;
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
				return;
		}
	}
	eet_close(eef);
}

void _settings_back_button_cb(void *data, Evas_Object *obj, void *event_info)
{
	evas_object_del(settings_window);
}

void _show_settings(void *data, Evas_Object *obj, void *event_info)
{
	settings_window = elm_win_add(NULL, "Settings", ELM_WIN_BASIC);
    elm_win_autodel_set(settings_window, EINA_TRUE);
    elm_win_alpha_set(settings_window, EINA_TRUE);
    
    settings_layout = elm_layout_add(settings_window);
    evas_object_size_hint_weight_set(settings_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(settings_window, settings_layout);
	elm_layout_file_set(settings_layout, SETTINGS_LAYOUT, "settings");
	
	settings_change_layout_btn = elm_button_add(settings_window);
	elm_object_text_set(settings_change_layout_btn, "Change layout");
	elm_object_part_content_set(settings_layout, "change_layout_btn", settings_change_layout_btn);

	settings_reset_highscores_btn = elm_button_add(settings_window);
	elm_object_text_set(settings_reset_highscores_btn, "Reset highscores");
	elm_object_part_content_set(settings_layout, "reset_highscores_btn", settings_reset_highscores_btn);
	evas_object_smart_callback_add(settings_reset_highscores_btn, "clicked", _settings_reset_highscores_cb, NULL);
	
	settings_back_btn = elm_button_add(settings_window);
	elm_object_text_set(settings_back_btn, "Back");
	elm_object_part_content_set(settings_layout, "back_btn", settings_back_btn);
	evas_object_smart_callback_add(settings_back_btn, "clicked", _settings_back_button_cb, NULL);
	
	evas_object_show(settings_layout);
	evas_object_show(settings_window);
}

