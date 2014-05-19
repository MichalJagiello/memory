#include "game_grid.h"
#include "game_types.h"
#include <efl_assist.h>

static Eina_List *game_buttons = NULL;
static int visible_elements;
Eina_Bool game_grid_is_visible = EINA_FALSE;
int game_type = 0;
int arcade_game_level = 0;

Eina_List* create_list(int elements)
{
	Eina_List *list = NULL;
	int i = 0;
	for(; i < (elements/2); ++i)
	{
		int *a = (int*)malloc(sizeof(int));
		int *b = (int*)malloc(sizeof(int));
		*a = i;
		*b = i;
		list = eina_list_append(list, a);
		list = eina_list_append(list, b);
	}
	return list;
}

void _pause_button_cb(void *data, Evas_Object *obj, void *event_info)
{
	Eina_Bool paused;
	if(game_type != ARCADE_GAME)
	{
		paused = elm_clock_pause_get(game_grid_clock);
		if(paused)
		{
			elm_object_text_set(game_grid_pause_btn, "Pause");
		}
		else
		{
			elm_object_text_set(game_grid_pause_btn, "Resume");
		}
		elm_clock_pause_set(game_grid_clock, !paused);
	}
	else
	{
		paused = !!strcmp(elm_object_text_get(game_grid_pause_btn), "Pause");
		if(paused)
		{
			elm_object_text_set(game_grid_pause_btn, "Pause");
			ecore_timer_thaw(arcade_game_timer);
		}
		else
		{
			elm_object_text_set(game_grid_pause_btn, "Resume");
			ecore_timer_freeze(arcade_game_timer);
		}
	}
	Eina_List *l;
	void *ld;
	EINA_LIST_FOREACH(game_buttons, l, ld)
	{
		Evas_Object *btn = (Evas_Object*)ld;
		if(btn)
		{
			elm_object_disabled_set(btn, !paused);
		}
	}
}

Eina_Bool _hide_cards(void *function_data)
{
	if(!game_grid_is_visible)
		return EINA_FALSE;
	Eina_List *l;
	void *data;
	Eina_List *visible_list = NULL;
	EINA_LIST_FOREACH(game_buttons, l, data)
	{
		Evas_Object *btn = (Evas_Object*)data;
		ButtonData *bd = evas_object_data_get(btn, "game_data");
		if(bd->discovered == EINA_FALSE && bd->visible == EINA_TRUE)
		{
			visible_list = eina_list_append(visible_list, btn);
		}
	}
	EINA_LIST_FOREACH(visible_list, l, data)
	{
		Evas_Object *btn = (Evas_Object*)data;
		ButtonData *bd = evas_object_data_get(btn, "game_data");
		bd->visible = EINA_FALSE;
		Evas_Object *ic = elm_object_part_content_get(btn, "icon");
		evas_object_color_set(ic, 255, 255, 255, 0);
	}
	visible_elements = 0;
	return EINA_FALSE;
}

void
_block_clicked(void *data, Evas_Object *obj,
               void *event_info)
{
   evas_object_hide(obj);
}

void _show_arcade_game_finish(Eina_Bool win)
{
	Evas_Object *popup = elm_popup_add(game_grid_window);
	Evas_Object *content = elm_entry_add(game_grid_window);
	int h, m, s;
	elm_clock_time_get(game_grid_clock, &h, &m, &s);
	printf("%d\n", arcade_game_result);
	//elm_clock_pause_set(game_grid_clock, EINA_TRUE);
	ecore_timer_freeze(arcade_game_timer);
	arcade_game_result += s + m*60;
	char buf[100];
	int result_pos = _check_arcade_result(arcade_game_result);
	if(result_pos)
	{
		sprintf(buf, "Winner!!! Your result: %d</br>Your result is on %d position in highscores\n", arcade_game_result, result_pos);
		_save_arcade_result(result_pos, arcade_game_result);
	}
	else
	{
		sprintf(buf, "Winner!!! Your result: %d</br>Your result is to weak to save in highscores\n", arcade_game_result);
	}
	elm_entry_entry_set(content, buf);
	evas_object_show(popup);
	elm_object_content_set(popup, content);
    evas_object_smart_callback_add(popup, "block,clicked", _block_clicked, NULL);
    //1. pobrac wyniki
    //2. sprawdzic czy sa do dodania do listy
    //3. dodac do listy jesli jest taka mozliwosc
    //4. wyswietlic odpowiedni popup
	if(win == EINA_TRUE)
	{
		printf("I won\n");
		return;
	}
	printf("I lost\n");
}

Eina_Bool _arcade_timer_cb(void *data)
{
	int min, sec;
	elm_clock_time_get(game_grid_clock, NULL, &min, &sec);
	elm_clock_time_set(game_grid_clock, 0, min, sec-1);
	if(min == 0 && sec-1 == 0)
	{
		_show_arcade_game_finish(EINA_FALSE);
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

Eina_Bool _compare_visible_buttons()
{
	Eina_List *l;
	void *data;
	Eina_List *visible_list = NULL;
	ButtonData *btn1, *btn2;
	EINA_LIST_FOREACH(game_buttons, l, data)
	{
		Evas_Object *btn = (Evas_Object*)data;
		ButtonData *bd = evas_object_data_get(btn, "game_data");
		if(bd->discovered == EINA_FALSE && bd->visible == EINA_TRUE)
		{
			visible_list = eina_list_append(visible_list, bd);
		}
	}
	btn1 = eina_list_nth(visible_list, 0);
	btn2 = eina_list_nth(visible_list, 1);
	eina_list_free(visible_list);
	int *value1 = btn1->value;
	int *value2 = btn2->value;
	if(*value1 == *value2)
	{
		btn1->discovered = EINA_TRUE;
		btn2->discovered = EINA_TRUE;
		visible_elements = 0;
		return EINA_TRUE;
	}
	else
	{
		ecore_timer_add(1.0, _hide_cards, NULL);
		return EINA_FALSE;
	}
}

Eina_Bool _check_if_game_is_finished()
{
	Eina_List *l;
	void *data;
	EINA_LIST_FOREACH(game_buttons, l, data)
	{
		Evas_Object *btn = (Evas_Object*)data;
		ButtonData *bd = evas_object_data_get(btn, "game_data");
		if(bd->discovered == EINA_FALSE)
		{
			return EINA_FALSE;
		}
	}
	return EINA_TRUE;
}

Eina_Bool _compare_results(int *arr, int h, int m, int s)
{
	if(arr[0] > h || arr[1] > m || arr[2] > s)
		return EINA_TRUE;
	return EINA_FALSE;
}

int _check_result(h, m, s)
{
	Eet_File *results = eet_open("results.eet", EET_FILE_MODE_READ);
	int *result = NULL;
	int i = 1;
	for(; i <= 10; ++i)
	{
		char buf[10];
		sprintf(buf, "%d%d", game_type, i);
		result = eet_read(results, buf, NULL);
		if(result[0] != -1)
		{
			if(_compare_results(result, h, m, s))
			{
				free(result);
				eet_close(results);
				return i;
			}
			free(result);
		}
		else
		{
			eet_close(results);
			return i;
		}
	}
	eet_close(results);
	return 0;
}

int _check_arcade_result(int scores)
{
	Eet_File *results = eet_open("results.eet", EET_FILE_MODE_READ);
	int *result = NULL;
	int i = 1;
	for(; i <= 10; ++i)
	{
		char buf[10];
		sprintf(buf, "%d%d", game_type, i);
		result = eet_read(results, buf, NULL);
		if(result[0] != -1)
		{
			if(result[0] < scores)
			{
				free(result);
				eet_close(results);
				return i;
			}
			free(result);
		}
		else
		{
			eet_close(results);
			return i;
		}
	}
	eet_close(results);
	return 0;
}

void _save_result(int pos, int h, int m, int s)
{
	Eet_File *eef = eet_open("results.eet", EET_FILE_MODE_READ_WRITE);
	int i;
	int result[3] = {h, m, s};
	int pom[3];
	char temp[50];
	for(i = 9; i >= pos; --i)
	{
		sprintf(temp, "%d%d", game_type, i);
		int *temp_res = eet_read(eef, temp, NULL);
		if(temp_res)
		{
			char temp2[50];
			sprintf(temp2, "%d", i+1);
			eet_delete(eef, temp2);
			pom[0] = temp_res[0];
			pom[1] = temp_res[1];
			pom[2] = temp_res[2];
			eet_write(eef, temp2, pom, sizeof(pom)+1, 1);
			free(temp_res);
		}
	}
	sprintf(temp, "%d%d", game_type, pos);
	eet_write(eef, temp, result, sizeof(pom)+1, 1);
	eet_close(eef);
}

void _save_arcade_result(int pos, int scores)
{
	Eet_File *eef = eet_open("results.eet", EET_FILE_MODE_READ_WRITE);
	int i;
	int result[3] = {scores, -1, -1};
	int pom[3];
	char temp[50];
	for(i = 9; i >= pos; --i)
	{
		sprintf(temp, "%d%d", game_type, i);
		int *temp_res = eet_read(eef, temp, NULL);
		if(temp_res)
		{
			char temp2[50];
			sprintf(temp2, "%d", i+1);
			eet_delete(eef, temp2);
			pom[0] = temp_res[0];
			pom[1] = temp_res[1];
			pom[2] = temp_res[2];
			eet_write(eef, temp2, pom, sizeof(pom)+1, 1);
			free(temp_res);
		}
	}
	sprintf(temp, "%d%d", game_type, pos);
	eet_write(eef, temp, result, sizeof(pom)+1, 1);
	eet_close(eef);
}

void _on_game_finish()
{
	Evas_Object *popup = elm_popup_add(game_grid_window);
	Evas_Object *content = elm_entry_add(game_grid_window);
	int h, m, s;
	elm_clock_time_get(game_grid_clock, &h, &m, &s);
	elm_clock_pause_set(game_grid_clock, EINA_TRUE);
	char buf[100];
	int result_pos = _check_result(h, m, s);
	if(result_pos)
	{
		sprintf(buf, "Winner!!! Your time: %d h %d m %d s</br>Your result is on %d position in highscores\n", h, m, s, result_pos);
		_save_result(result_pos, h, m, s);
	}
	else
	{
		sprintf(buf, "Winner!!! Your time: %d h %d m %d s</br>Your result is to weak to save in highscores\n", h, m, s);
	}
	elm_entry_entry_set(content, buf);
	evas_object_show(popup);
	elm_object_content_set(popup, content);
    evas_object_smart_callback_add(popup, "block,clicked", _block_clicked, NULL);
}

void _free_buttons()
{
	Eina_List *l;
	void *ld;
	EINA_LIST_FOREACH(game_buttons, l, ld)
	{
		Evas_Object *btn = (Evas_Object*)ld;
		if(btn)
		{
			ButtonData *bd = evas_object_data_get(btn, "game_data");
			if(bd->value) free(bd->value);
			if(bd) free(bd);
		}
		elm_layout_table_unpack(game_grid_layout, "game_board", btn);
		evas_object_hide(btn);
		evas_object_del(btn);
	}
	game_buttons = eina_list_free(game_buttons);
	game_buttons = NULL;
}

void _close_gg_cb(void *data, Evas_Object *obj, void *event_info)
{
	game_grid_is_visible = EINA_FALSE;
	_free_buttons();
	if(game_type == ARCADE_GAME)
		ecore_timer_del(arcade_game_timer);
	evas_object_del(game_grid_window);
	game_grid_window = NULL;
}

Eina_List* create_game_button(Evas_Object *btn, Eina_List *list)
{
	int count = eina_list_count(list) - 1;
	int index;
	if(count != 0)
		index = rand()%count;
	else
		index = 0;
	int *value = eina_list_nth(list, index);
	list = eina_list_remove(list, value);
	ButtonData *bd = (ButtonData*)malloc(sizeof(ButtonData));
	bd->value = value;
	bd->discovered = EINA_FALSE;
	bd->visible = EINA_FALSE;
	char buf[50];
	sprintf(buf, "%s0%d.png", IMAGES_PATH, *value);
	Evas_Object *ic = elm_icon_add(game_grid_window);
	elm_image_file_set(ic, buf, NULL);
    elm_image_resizable_set(ic, EINA_FALSE, EINA_FALSE);
    evas_object_color_set(ic, 255, 255, 255, 0);
	bd->icon = ic;
	elm_object_part_content_set(btn, "icon", ic);
	evas_object_data_set(btn, "game_data", bd);
	game_buttons = eina_list_append(game_buttons, btn);
	return list;
}

void _game_btn_click(void *data, Evas_Object *obj, void *event_info)
{
	if(visible_elements == 0) //po prostu odkrycie karty i podbicie licznika
	{
		ButtonData *bd = evas_object_data_get(obj, "game_data");
		char buf[50];
		if(bd->discovered == EINA_TRUE)
			return;
		Evas_Object *ic = elm_object_part_content_get(obj, "icon");
		evas_object_color_set(ic, 255, 255, 255, 255);
		bd->visible = EINA_TRUE;
		visible_elements++;
	}
	else if(visible_elements == 1)
	//odkrycie karty i porownanie z juz odkrytymi. Jesli 
    //znajdzie się para to gramy dalej, w innym przypadku
    //ustawiamy licznik i zakrywamy te dwa elementy 
	{	
		ButtonData *bd = evas_object_data_get(obj, "game_data");
		if(bd->visible == EINA_TRUE || bd->discovered == EINA_TRUE)
			return;
		char buf[50];
		int *value = bd->value;
		sprintf(buf, "%s0%d.png", IMAGES_PATH, *value);
		Evas_Object *ic = elm_object_part_content_get(obj, "icon");
		evas_object_color_set(ic, 255, 255, 255, 255);
		bd->visible = EINA_TRUE;
		visible_elements++;
		if(_compare_visible_buttons())
		{
			if(game_type == ARCADE_GAME)
			{
				arcade_game_result += 2;
				_update_scores();
			}
			if(_check_if_game_is_finished())
			{
				if(game_type == ARCADE_GAME)
					_on_game_finish_arcade();
				else
					_on_game_finish();
			}
		}
	}
}

void _on_game_finish_arcade()
{
	arcade_game_level++;
	int mins, secs;
	elm_clock_time_get(game_grid_clock, NULL, &mins, &secs);
	if(arcade_game_level == 2)
	{
		_free_buttons();
		important_list = create_list(6);

		int i, j;
		
		for(i = 0; i < 3; ++i)
		{
			for(j = 0; j < 2; ++j)
			{
				Evas_Object *btn = elm_button_add(game_grid_window);
				elm_object_style_set(btn, "game_grid");
				evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
				evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
				important_list = create_game_button(btn, important_list);
				evas_object_smart_callback_add(btn, "clicked", _game_btn_click, game_grid_window);
				elm_layout_table_pack(game_grid_layout, "game_board", btn, i, j, 1, 1);
				evas_object_show(btn);
			}
		}
		elm_clock_time_set(game_grid_clock, 0, mins, secs+10);
		arcade_game_result += 5;
		_update_scores();
	}
	else if(arcade_game_level < 4)
	{
		//gra 4x2
		_free_buttons();
		important_list = create_list(8);

		int i, j;
		
		for(i = 0; i < 4; ++i)
		{
			for(j = 0; j < 2; ++j)
			{
				Evas_Object *btn = elm_button_add(game_grid_window);
				elm_object_style_set(btn, "game_grid");
				evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
				evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
				important_list = create_game_button(btn, important_list);
				evas_object_smart_callback_add(btn, "clicked", _game_btn_click, game_grid_window);
				elm_layout_table_pack(game_grid_layout, "game_board", btn, i, j, 1, 1);
				evas_object_show(btn);
			}
		}
		elm_clock_time_set(game_grid_clock, 0, mins, secs+10);
		arcade_game_result += 7;
		_update_scores();
	}
	else if(arcade_game_level < 6)
	{
		//gra 4x3
		_free_buttons();
		important_list = create_list(12);

		int i, j;
		
		for(i = 0; i < 4; ++i)
		{
			for(j = 0; j < 3; ++j)
			{
				Evas_Object *btn = elm_button_add(game_grid_window);
				elm_object_style_set(btn, "game_grid");
				evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
				evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
				important_list = create_game_button(btn, important_list);
				evas_object_smart_callback_add(btn, "clicked", _game_btn_click, game_grid_window);
				elm_layout_table_pack(game_grid_layout, "game_board", btn, i, j, 1, 1);
				evas_object_show(btn);
			}
		}
		elm_clock_time_set(game_grid_clock, 0, mins, secs+20);
		arcade_game_result += 10;
		_update_scores();
	}
	else if(arcade_game_level < 9)
	{
		//gra 4x4
		_free_buttons();
		important_list = create_list(16);

		int i, j;
		
		for(i = 0; i < 4; ++i)
		{
			for(j = 0; j < 4; ++j)
			{
				Evas_Object *btn = elm_button_add(game_grid_window);
				elm_object_style_set(btn, "game_grid");
				evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
				evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
				important_list = create_game_button(btn, important_list);
				evas_object_smart_callback_add(btn, "clicked", _game_btn_click, game_grid_window);
				elm_layout_table_pack(game_grid_layout, "game_board", btn, i, j, 1, 1);
				evas_object_show(btn);
			}
		}
		elm_clock_time_set(game_grid_clock, 0, mins, secs+25);
		arcade_game_result += 15;
		_update_scores();
	}
	else if(arcade_game_level < 12)
	{
		//gra 5x4
		_free_buttons();
		important_list = create_list(20);

		int i, j;
		
		for(i = 0; i < 5; ++i)
		{
			for(j = 0; j < 4; ++j)
			{
				Evas_Object *btn = elm_button_add(game_grid_window);
				elm_object_style_set(btn, "game_grid");
				evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
				evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
				important_list = create_game_button(btn, important_list);
				evas_object_smart_callback_add(btn, "clicked", _game_btn_click, game_grid_window);
				elm_layout_table_pack(game_grid_layout, "game_board", btn, i, j, 1, 1);
				evas_object_show(btn);
			}
		}
		elm_clock_time_set(game_grid_clock, 0, mins, secs+25);
		arcade_game_result += 20;
		_update_scores();
	}
	else if(arcade_game_level < 20)
	{
		//gra 6x4
		_free_buttons();
		important_list = create_list(24);

		int i, j;
		
		for(i = 0; i < 6; ++i)
		{
			for(j = 0; j < 4; ++j)
			{
				Evas_Object *btn = elm_button_add(game_grid_window);
				elm_object_style_set(btn, "game_grid");
				evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
				evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
				important_list = create_game_button(btn, important_list);
				evas_object_smart_callback_add(btn, "clicked", _game_btn_click, game_grid_window);
				elm_layout_table_pack(game_grid_layout, "game_board", btn, i, j, 1, 1);
				evas_object_show(btn);
			}
		}
		elm_clock_time_set(game_grid_clock, 0, mins, secs+25);
		arcade_game_result += 30;
		_update_scores();
	}
	else
	{
		arcade_game_result += 50;
		_show_arcade_game_finish(EINA_TRUE);
	}
}

void _restart_button_gg_cb(void *data, Evas_Object *obj, void *event_info)
{
	game_grid_is_visible = EINA_FALSE;
	_free_buttons();
	switch(game_type) {
		case CLASSIC_GAME_SMALL_GRID:
			_show_classic_small_gg(NULL, NULL, NULL);
			break;
		case CLASSIC_GAME_MEDIUM_GRID:
			_show_classic_medium_gg(NULL, NULL, NULL);
			break;
		case CLASSIC_GAME_BIG_GRID:
			_show_classic_big_gg(NULL, NULL, NULL);
			break;
		case ARCADE_GAME:
			_show_arcade_gg(NULL, NULL, NULL);
			break;
	}
}



void _prepare_grid(int rows, int cols, int cards)
{
	if(!game_grid_window)
	{
		game_grid_window = elm_win_add(NULL, "Game Grid", ELM_WIN_BASIC);
		elm_win_autodel_set(game_grid_window, EINA_TRUE);
		elm_win_alpha_set(game_grid_window, EINA_TRUE);
		ea_object_event_callback_add(game_grid_window, EA_CALLBACK_BACK, _close_gg_cb, NULL);
		evas_object_show(game_grid_window);
	}
    
    game_grid_layout = elm_layout_add(game_grid_window);
    evas_object_size_hint_weight_set(game_grid_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(game_grid_window, game_grid_layout);
	elm_layout_file_set(game_grid_layout, GAME_GRID_LAYOUT, "game_board_classic_layout");
	
	important_list = create_list(cards);

	int i, j;
	
	for(i = 0; i < rows; ++i)
	{
		for(j = 0; j < cols; ++j)
		{
			Evas_Object *btn = elm_button_add(game_grid_window);
			elm_object_style_set(btn, "game_grid");
			evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
			important_list = create_game_button(btn, important_list);
			evas_object_smart_callback_add(btn, "clicked", _game_btn_click, game_grid_window);
			elm_layout_table_pack(game_grid_layout, "game_board", btn, i, j, 1, 1);
			evas_object_show(btn);
		}
	}
	
	game_grid_pause_btn = elm_button_add(game_grid_window);
	elm_object_text_set(game_grid_pause_btn, "Pause");
	elm_object_part_content_set(game_grid_layout, "pause_btn", game_grid_pause_btn);
    evas_object_smart_callback_add(game_grid_pause_btn, "clicked", _pause_button_cb, NULL);

	game_grid_restart_btn = elm_button_add(game_grid_window);
	elm_object_text_set(game_grid_restart_btn, "Restart");
	elm_object_part_content_set(game_grid_layout, "restart_btn", game_grid_restart_btn);
    evas_object_smart_callback_add(game_grid_restart_btn, "clicked", _restart_button_gg_cb, NULL);
    
    game_grid_clock = elm_clock_add(game_grid_window);
    elm_object_part_content_set(game_grid_layout, "clock", game_grid_clock);
    elm_clock_show_seconds_set(game_grid_clock, EINA_TRUE);
    elm_clock_time_set(game_grid_clock, 0, 0, 0);
	
	visible_elements = 0;
	game_grid_is_visible = EINA_TRUE;
	
	evas_object_show(game_grid_layout);
}

void _update_scores()
{
    char buf[32];
    snprintf(buf, strlen(buf), "%d", arcade_game_result);
    elm_object_text_set(game_grid_scores, buf);
}

void _prepare_arcade_grid(int rows, int cols, int cards)
{
	if(!game_grid_window)
	{
		game_grid_window = elm_win_add(NULL, "Game Grid", ELM_WIN_BASIC);
		elm_win_autodel_set(game_grid_window, EINA_TRUE);
		elm_win_alpha_set(game_grid_window, EINA_TRUE);
		ea_object_event_callback_add(game_grid_window, EA_CALLBACK_BACK, _close_gg_cb, NULL);
		evas_object_show(game_grid_window);
	}
    
    game_grid_layout = elm_layout_add(game_grid_window);
    evas_object_size_hint_weight_set(game_grid_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(game_grid_window, game_grid_layout);
	elm_layout_file_set(game_grid_layout, GAME_GRID_LAYOUT, "game_board_arcade_layout");
	
	important_list = create_list(cards);

	int i, j;
	
	for(i = 0; i < rows; ++i)
	{
		for(j = 0; j < cols; ++j)
		{
			Evas_Object *btn = elm_button_add(game_grid_window);
			elm_object_style_set(btn, "game_grid");
			evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
			important_list = create_game_button(btn, important_list);
			evas_object_smart_callback_add(btn, "clicked", _game_btn_click, game_grid_window);
			elm_layout_table_pack(game_grid_layout, "game_board", btn, i, j, 1, 1);
			evas_object_show(btn);
		}
	}
	
	game_grid_pause_btn = elm_button_add(game_grid_window);
	elm_object_text_set(game_grid_pause_btn, "Pause");
	elm_object_part_content_set(game_grid_layout, "pause_btn", game_grid_pause_btn);
    evas_object_smart_callback_add(game_grid_pause_btn, "clicked", _pause_button_cb, NULL);

	game_grid_restart_btn = elm_button_add(game_grid_window);
	elm_object_text_set(game_grid_restart_btn, "Restart");
	elm_object_part_content_set(game_grid_layout, "restart_btn", game_grid_restart_btn);
    evas_object_smart_callback_add(game_grid_restart_btn, "clicked", _restart_button_gg_cb, NULL);
    
    arcade_game_result = 0;
    game_grid_scores = elm_label_add(game_grid_window);
    elm_object_part_content_set(game_grid_layout, "scores", game_grid_scores);
    _update_scores();
    
    game_grid_clock = elm_clock_add(game_grid_window);
    elm_object_part_content_set(game_grid_layout, "clock", game_grid_clock);
    elm_clock_show_seconds_set(game_grid_clock, EINA_TRUE);
    elm_clock_time_set(game_grid_clock, 0, 0, 10);
    elm_clock_pause_set(game_grid_clock, EINA_TRUE);

	arcade_game_timer = ecore_timer_add(1.0, _arcade_timer_cb, NULL);
	
	visible_elements = 0;
	game_grid_is_visible = EINA_TRUE;
	
	evas_object_show(game_grid_layout);
}


void _show_classic_small_gg(void *data, Evas_Object *obj, void *event_info)
{
	game_type = CLASSIC_GAME_SMALL_GRID;
	
	_prepare_grid(4, 3, 12);
}

void _show_classic_medium_gg(void *data, Evas_Object *obj, void *event_info)
{
	game_type = CLASSIC_GAME_MEDIUM_GRID;
	
	_prepare_grid(4, 4, 16);
}

void _show_classic_big_gg(void *data, Evas_Object *obj, void *event_info)
{
	game_type = CLASSIC_GAME_BIG_GRID;

	_prepare_grid(6, 4, 24);
}

void _show_arcade_gg(void *data, Evas_Object *obj, void *event_info)
{
	game_type = ARCADE_GAME;
	arcade_game_level = 1;

	_prepare_arcade_grid(2, 2, 4);
}
