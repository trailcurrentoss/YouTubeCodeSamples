/*
 * The actions EEZ Studio declares in ui/actions.h.
 *
 * Tap:   the title and vault screens' SHORT_CLICKED handler. Shows the
 *        rules from the title; starts the next game from the vault (the game
 *        ignores it during the replay lockout).
 * Start: the Start button on the instructions. Deals the first code.
 * Menu:  every game screen's LONG_PRESSED handler -- touch and hold for the
 *        menu: New Game, Settings, Back.
 * Menu*, Settings*: the menu's and the Settings screen's buttons (the
 *        volume's - and + among them).
 * Headphones*: the Bluetooth pairing screen. HeadphonesSelect carries the
 *        list row in userData.
 */

#include "actions.h"

#include <stdint.h>

#include "app.h"

void action_tap(lv_event_t *e)
{
    (void)e;
    app_tap();
}

void action_start(lv_event_t *e)
{
    (void)e;
    app_start();
}

void action_menu(lv_event_t *e)
{
    (void)e;
    app_menu();
}

void action_menu_new_game(lv_event_t *e)
{
    (void)e;
    app_menu_new_game();
}

void action_menu_settings(lv_event_t *e)
{
    (void)e;
    app_menu_settings();
}

void action_menu_back(lv_event_t *e)
{
    (void)e;
    app_back();
}

void action_settings_headphones(lv_event_t *e)
{
    (void)e;
    app_headphones_open();
}

void action_settings_back(lv_event_t *e)
{
    (void)e;
    app_back();
}

void action_settings_volume_down(lv_event_t *e)
{
    (void)e;
    app_volume_step(-1);
}

void action_settings_volume_up(lv_event_t *e)
{
    (void)e;
    app_volume_step(1);
}

void action_headphones(lv_event_t *e)
{
    (void)e;
    app_headphones_open();
}

void action_headphones_search(lv_event_t *e)
{
    (void)e;
    app_headphones_search();
}

void action_headphones_select(lv_event_t *e)
{
    app_headphones_select((int)(intptr_t)lv_event_get_user_data(e));
}

void action_headphones_done(lv_event_t *e)
{
    (void)e;
    app_back();
}

void action_headphones_info(lv_event_t *e)
{
    (void)e;
    app_headphones_info(true);
}

void action_headphones_info_close(lv_event_t *e)
{
    (void)e;
    app_headphones_info(false);
}

void action_headphones_forget(lv_event_t *e)
{
    (void)e;
    app_headphones_forget();
}
