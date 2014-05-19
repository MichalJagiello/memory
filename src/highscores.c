#include "highscores.h"

//static int dispX, dispY;
//static int app_x_size;
//static int app_y_size;

void _highscores_back_button_cb(void *data, Evas_Object *obj, void *event_info)
{
	evas_object_del(highscores_window);
}

static char *
_item_label_get(void *data, Evas_Object *obj, const char *part)
{
	int num = elm_genlist_items_count(obj);
	int *res = (int*)data;
	char buf[256];
	sprintf(buf, "%d h %d m %d s", res[0], res[1], res[2]);
	return strdup(buf);
}

void _show_hs(void *data, Evas_Object *obj, void *event_info)
{
	elm_genlist_clear(highscores_genlist);
	if(!_itc)
	{
		_itc = elm_genlist_item_class_new();
        _itc->item_style = "default";
        _itc->func.text_get = _item_label_get;
        _itc->func.content_get = NULL;
        _itc->func.state_get = NULL;
        _itc->func.del = NULL;
	}

	Eet_File *eef = eet_open("results.eet", EET_FILE_MODE_READ);
	int game_type = (int)(uintptr_t)(data);
	int j = 0;
	for(; j < 10; ++j)
	{
		char buf[50];
		int *res;
		sprintf(buf, "%d%d", game_type, j);
			
		res = eet_read(eef, buf, NULL);
		if(res && res[0] != -1)
		{
			elm_genlist_item_append(highscores_genlist, _itc, res, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
		}
	}
		//free(res); //MEMORY LEAK?
	
	eet_close(eef);
}

void _show_highscores(void *data, Evas_Object *obj, void *event_info)
{
    highscores_window = elm_win_add(NULL, "Highscores", ELM_WIN_BASIC);
    elm_win_autodel_set(highscores_window, EINA_TRUE);
    elm_win_alpha_set(highscores_window, EINA_TRUE);
    elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
    
    highscores_layout = elm_layout_add(highscores_window);
    evas_object_size_hint_weight_set(highscores_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(highscores_window, highscores_layout);
	elm_layout_file_set(highscores_layout, HIGHSCORES_LAYOUT, "highscore_layout");
    
    highscores_toolbar = elm_toolbar_add(highscores_window);
	elm_toolbar_shrink_mode_set(highscores_toolbar, ELM_TOOLBAR_SHRINK_EXPAND);
    Elm_Object_Item *it = elm_toolbar_item_append(highscores_toolbar, NULL, "Classic - small grid", _show_hs, (int*)CLASSIC_GAME_SMALL_GRID);
    elm_toolbar_item_append(highscores_toolbar, NULL, "Classic - medium grid", _show_hs, (int*)CLASSIC_GAME_MEDIUM_GRID);
    elm_toolbar_item_append(highscores_toolbar, NULL, "Classic - big grid", _show_hs, (int*)CLASSIC_GAME_BIG_GRID);
    elm_toolbar_item_append(highscores_toolbar, NULL, "Arcade", _show_hs, (int*)ARCADE_GAME);
    elm_toolbar_select_mode_set(highscores_toolbar, ELM_OBJECT_SELECT_MODE_ALWAYS);
    elm_object_part_content_set(highscores_layout, "hs_toolbar", highscores_toolbar);
    
    highscores_genlist = elm_genlist_add(highscores_window);
    elm_object_part_content_set(highscores_layout, "list", highscores_genlist);

	highscores_back_button = elm_button_add(highscores_window);
	elm_object_text_set(highscores_back_button, "Back");
	elm_object_part_content_set(highscores_layout, "back_button", highscores_back_button);
    evas_object_smart_callback_add(highscores_back_button, "clicked", _highscores_back_button_cb, NULL);
    
    evas_object_show(highscores_layout);
    evas_object_show(highscores_window);
    elm_toolbar_item_selected_set(it, EINA_TRUE);
   
	_show_hs((int*)CLASSIC_GAME_SMALL_GRID, highscores_toolbar, it);
	
}
