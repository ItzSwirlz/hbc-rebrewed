#include "credit_music_ogg.h"
#include "menu_music_ogg.h"

#define MUSIC_TRACK_MENU 0
#define MUSIC_TRACK_CREDITS 1
#define MUSIC_MAX 2

void music_init();
void play_music();
void play_music_track(int track);
void music_deinit();
