/*
 * EEZ Studio action handlers. The .eez-project declares the actions and picks
 * the variant with each event handler's userData; the work is in mesh_ui.c.
 */

#if __has_include("ui/actions.h")
#include <stdint.h>
#include "actions.h"
#include "mesh_ui.h"

static int user_data(lv_event_t *e)
{
    return (int)(intptr_t)lv_event_get_user_data(e);
}

void action_navigate(lv_event_t *e)            { mesh_ui_navigate(user_data(e)); }
void action_select_conversation(lv_event_t *e) { mesh_ui_select_conversation(user_data(e)); }
void action_send_message(lv_event_t *e)        { (void)e; mesh_ui_send(); }
void action_show_keyboard(lv_event_t *e)       { mesh_ui_show_keyboard(user_data(e)); }
void action_message_node(lv_event_t *e)        { mesh_ui_message_node(user_data(e)); }
void action_save_owner(lv_event_t *e)          { (void)e; mesh_ui_save_owner(); }
void action_step_setting(lv_event_t *e)        { mesh_ui_step_setting(user_data(e)); }
void action_apply_radio(lv_event_t *e)         { (void)e; mesh_ui_apply_radio(); }

void action_keyboard_done(lv_event_t *e)
{
    mesh_ui_keyboard_done(user_data(e), lv_event_get_code(e) == LV_EVENT_READY);
}
#else
typedef int actions_placeholder;    /* EEZ Studio export not present yet */
#endif
