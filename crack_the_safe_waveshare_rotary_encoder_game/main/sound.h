/*
 * The game's sounds and the volume setting.
 *
 * Every sound goes to the Bluetooth headphones while they are connected
 * (the ESP32 mutes the wired output for as long as they are), and to the
 * 3.5 mm jack otherwise. The volume applies to both and is remembered
 * across power cycles.
 */
#pragma once

/** Volume levels: 0 (off) .. SOUND_VOLUME_MAX. */
#define SOUND_VOLUME_MAX     10
#define SOUND_VOLUME_DEFAULT 3

/** Build the sounds, load the saved volume. Call once, after bt_link_init(). */
void sound_init(void);

/** A lock pin dropping into place: a number was hit. */
void sound_pin(void);

/** The police siren, for the whole of the loss hold (GAME_WRONG_HOLD_MS). */
void sound_siren(void);

/** The vault door opening: play the moment the safe opens. */
void sound_vault_door(void);

/** The prize behind the door at `level`: play as the result panel appears. */
void sound_prize(int level);

/** Cut off whatever is playing. */
void sound_stop(void);

int sound_volume(void);

/** Set the volume (clamped to 0..SOUND_VOLUME_MAX), save it to NVS at once,
 *  and play the pin click at the new level so the player hears it. */
void sound_set_volume(int level);
