/*
 * actions.c — HAND-WRITTEN. Implements the actions EEZ Studio declares in
 * actions.h.
 *
 * Lives in main/, NOT in main/ui/: main/ui/ is EEZ Studio's export and is
 * disposable -- it can be deleted and re-exported at any time, so nothing
 * hand-written goes there. Compiled only once an export exists (see
 * main/CMakeLists.txt).
 */

#include <stdint.h>

#include "actions.h"

#include "app_model.h"
#include "crowpanel_board.h"

/*
 * A firm press on the glass also closes the ring's push switch, and that
 * press has already been handled as a push (axis change / Kdenlive cut).
 * Acting on the tap as well would fire a second, unintended shortcut.
 */
static bool tap_is_really_a_push(void)
{
    return crowpanel_board_touch_contact_pushed();
}

/* The eight ring keys; userData is the key index, clockwise from 12. */
void action_key_tap(lv_event_t *e)
{
    if (tap_is_really_a_push()) {
        return;
    }
    app_model_key_tap((int)(intptr_t)lv_event_get_user_data(e));
}

/* Kdenlive centre: frame / second scrub step. */
void action_toggle_step(lv_event_t *e)
{
    (void)e;
    if (tap_is_really_a_push()) {
        return;
    }
    app_model_toggle_step();
}
