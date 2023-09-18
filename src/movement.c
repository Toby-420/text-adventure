#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "../include/miniaudio.h"

#define FRONT_ROOM 0
#define STUDY 1
#define HIDDEN_ROOM 2
#define FR_PASSAGE 3

void randomMovementString(int location, bool insulting, int inputRow) {
	srand(time(NULL));
	char insultingResponsesForWalls[5][255] = {
		"You walk into the wall and headbutt it. Wall -10hp",
		"How does the wall taste?",
		"You must have smashed your head on these walls so many times now",
		"Nice wall, eh?",
		"Haiyaa. You are bad at this"
	};
	char normalResponsesForWalls[5][255] = {
		"You walk into the wall",
		"The path is blocked in that direction",
		"It's a dead end in that direction",
		"You can't go that way",
		"Nope, can't go that way"
	};
	char insultingResponsesForSmallRoom[5][255] = {
		"I guess small spaces are not easy to navigate",
		"Your eyes are open, right?",
		"Well, if walking into walls was an art form, you'd be a master by now",
		"Wow, you managed to pick the one direction that's off-limits. Impressive",
		"You're like a homing pigeon for wrong paths, aren't you?"
	};
	char normalResponsesForSmallRoom[5][255] = {
		"You can practically feel the wall's disappointment as you run into it",
		"You walk straight into a solid wall. Nice try",
		"Congratulations, you've discovered a new wall texture up close",
		"Walls tend to be rather wall-like. You've encountered one",
		"You bump your nose against the wall"
	};
	int responseNumber = rand() % 5;
	
	if (insulting) {
	  switch(location) {
	  	  case FRONT_ROOM:
	  	  case STUDY:
		    mvwprintw(stdscr, inputRow + 1, 2, "%s", insultingResponsesForWalls[responseNumber]);
		    break;
	  	  case HIDDEN_ROOM:
	  	  case FR_PASSAGE:
		    mvwprintw(stdscr, inputRow + 1, 2, "%s", insultingResponsesForSmallRoom[responseNumber]);
		    break;
		  default:
		    break;
	  }
	} else {
	  switch(location) {
	  	  case FRONT_ROOM:
	  	  case STUDY:
		    mvwprintw(stdscr, inputRow + 1, 2, "%s", normalResponsesForWalls[responseNumber]);
		    break;
	  	  case HIDDEN_ROOM:
	  	  case FR_PASSAGE:
		    mvwprintw(stdscr, inputRow + 1, 2, "%s", normalResponsesForSmallRoom[responseNumber]);
		    break;
		  default:
		    break;
	  }
	}
}

void randomMovementSound(ma_engine *engine) {
	srand(time(NULL));
	int number = rand() % 3;

	int totalLength = snprintf(NULL, 0, "assets/audio/walking%d.mp3", number + 1) + 1;

    char *combinedFilename = (char *)malloc(totalLength);

    snprintf(combinedFilename, totalLength, "assets/audio/walking%d.mp3", number);

	ma_engine_play_sound(engine, combinedFilename, NULL);

    free(combinedFilename);
}

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        return;
    }

    /* Reading PCM frames will loop based on what we specified when called ma_data_source_set_looping(). */
    ma_data_source_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

    (void)pInput;
}