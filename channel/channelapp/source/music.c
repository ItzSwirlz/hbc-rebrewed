#include "../config.h"
#ifdef USE_MUSIC
#include <ogc/machine/processor.h>

#include "music.h"
#include <asndlib.h>
#include "oggplayer.h"
#include <stdio.h>


#include "theme.h"
#include "xml.h"

const theme_font_t default_music[MUSIC_MAX] = {
	{menu_music_ogg, &menu_music_ogg_size},
	{credit_music_ogg, &credit_music_ogg_size}};

int current_track = MUSIC_TRACK_MENU; // default

void music_init() {
    ASND_Init();
    SetVolumeOgg(127);
}

void play_music() {
    // Just play whatever is playing.
    // This is used in the main loop so if we are in the main menu, keep playing
    // the music: But if we are in the 'about' section, the main loop will
    // replace the credits loop. So we need a control - and this is our solution
    play_music_track(current_track);
}

void play_music_track(int track) {
    if (track != current_track)
		current_track = track;
    if (StatusOgg() != 1) {
        PlayOgg(theme_music[current_track].data, theme_music[current_track].data_len, 0, OGG_INFINITE_TIME);
    }
}

void music_deinit() {
    StopOgg();
    ASND_End();
}

#endif
