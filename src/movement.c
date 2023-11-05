#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <SDL2/SDL_mixer.h>
#include "../include/defines.h"

extern Mix_Chunk* walkingSound0;
extern Mix_Chunk* walkingSound1;
extern Mix_Chunk* walkingSound2;
extern Mix_Chunk* walkingSound3;

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

void randomMovementSound() {
	int random = rand() % 3;
	
	switch (random) {
		case 0:
			Mix_PlayChannel(-1, walkingSound0, 0);
			break;
		case 1:
			Mix_PlayChannel(-1, walkingSound1, 0);
			break;
		case 2:
			Mix_PlayChannel(-1, walkingSound2, 0);
			break;
		case 3:
			Mix_PlayChannel(-1, walkingSound3, 0);
			break;
		default:
			break;
	}
}