#include <curses.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <SDL2/SDL_mixer.h>
#include "../include/defines.h"
#include "../include/movement.h"
#include "../include/interface.h"

int minFunction(int a, int b, int c) {
    int smallest = a < b ? a : b;
    return smallest < c ? smallest : c;
}

int distanceAlgorithm(const char *s1, const char *s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    int dp[len1 + 1][len2 + 1];

    for (int i = 0; i <= len1; i++) {
        for (int j = 0; j <= len2; j++) {
            if (i == 0)
                dp[i][j] = j;
            else if (j == 0)
                dp[i][j] = i;
            else if (s1[i - 1] == s2[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
            else
                dp[i][j] = 1 + minFunction(dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]);
        }
    }

    return dp[len1][len2];
}

int stringIsSimilar(const char *s1, const char *s2, int threshold) {
    int distance = distanceAlgorithm(s1, s2);
    return distance <= threshold;
}

int handleViewCommand(int inputRow, char *mainInput, char *parameters, struct MainCharacter *character, struct System *system, struct Windows *window, struct WindowText *text) { 
  if (stringIsSimilar(parameters, "window", 2)) {
	  switch(character->locationNumber) {
		case FRONT_ROOM:
          mvwprintw(stdscr, inputRow + 1, 2, "There is a large, stained glass window with pictures of Jesus and sheep on it");
		  break;
		case STUDY:
          mvwprintw(stdscr, inputRow + 1, 2, "A crusty, wooden window frame houses a small sheet of glass. It seems pretty drafty");
		  break;
		case HIDDEN_ROOM:
		case FR_PASSAGE:
          mvwprintw(stdscr, inputRow + 1, 2, "There are no windows in here");
		  break;
		default:
          mvwprintw(stdscr, inputRow + 1, 2, "Location not found");
		  break;
	  }
  } else if (stringIsSimilar(parameters, "picture", 2)) {
	  switch(character->locationNumber) {
		case FRONT_ROOM:
          mvwprintw(stdscr, inputRow + 1, 2, "A big, hand-painted picture of a guy sits on the wall");
		  break;
		case STUDY:
          mvwprintw(stdscr, inputRow + 1, 2, "There are no pictures in here. Only Zuul");
		  break;
		case HIDDEN_ROOM:
          mvwprintw(stdscr, inputRow + 1, 2, "There are no pictures");
		  break;
		case FR_PASSAGE:
          mvwprintw(stdscr, inputRow + 1, 2, "You can't find any pictures. There is a lot of dust though");
		  break;
		default:
          mvwprintw(stdscr, inputRow + 1, 2, "Location not found");
		  break;
	  }
  } else if (stringIsSimilar(parameters, "key", 2)) {
	  if (system->keyIsInventory || system->keyIsVisible) {
	    mvwprintw(stdscr, inputRow + 1, 2, "You see an old key which is still shiny. What is it here for?");
	  } else {
	    mvwprintw(stdscr, inputRow + 1, 2, "There are no keys here");
	  }
  } else if (stringIsSimilar(parameters, "treasure", 2)) {
	  if (system->davidIsInventory || system->davidIsVisible) {
	    mvwprintw(stdscr, inputRow + 1, 2, "It looks like a small tin with something quite dense inside");
	  } else {
  	    mvwprintw(stdscr, inputRow + 1, 2, "You see no treasure in here");
  	  }
  } else if (stringIsSimilar(parameters, "bookcase", 2)) {
	  switch(character->locationNumber) {
		case FRONT_ROOM:
          mvwprintw(stdscr, inputRow + 1, 2, "There are no bookcases");
		  break;
		case STUDY:
          mvwprintw(stdscr, inputRow + 1, 2, "Beautifully crafted bookcases line the walls. One looks a little loose");
          system->bookcaseIsLoose = true;
		  break;
		case HIDDEN_ROOM:
		case FR_PASSAGE:
          mvwprintw(stdscr, inputRow + 1, 2, "There are no bookcases");
		  break;
		default:
          mvwprintw(stdscr, inputRow + 1, 2, "Location not found");
		  break;
	  }
  } else if (stringIsSimilar(parameters, "room", 2)) {
	  switch(character->locationNumber) {
		case FRONT_ROOM:
          mvwprintw(stdscr, inputRow + 1, 2, "A large and ornate, yet run-down front room with a window, picture and doors to the east and west");
		  break;
		case STUDY:
          mvwprintw(stdscr, inputRow + 1, 2, "A fancy study with bookcases along the east wall, a broken window, and a drawer");
		  break;
		case HIDDEN_ROOM:
          mvwprintw(stdscr, inputRow + 1, 2, "A tiny room which only has a chest in");
		  break;
		case FR_PASSAGE:
          mvwprintw(stdscr, inputRow + 1, 2, "A small room you found hidden behind a picture");
		  break;
		default:
          mvwprintw(stdscr, inputRow + 1, 2, "Location not found");
		  break;
	  }
  } else if (stringIsSimilar(parameters, "drawer", 2)) {
	  switch(character->locationNumber) {
		case FRONT_ROOM:
          mvwprintw(stdscr, inputRow + 1, 2, "There is no drawer in here");
		  break;
		case STUDY:
          mvwprintw(stdscr, inputRow + 1, 2, "A rough-looking drawer with a handle");
		  break;
		case HIDDEN_ROOM:
          mvwprintw(stdscr, inputRow + 1, 2, "You can't see any drawers");
		  break;
		case FR_PASSAGE:
          mvwprintw(stdscr, inputRow + 1, 2, "There is nothing but darkness in here");
		  break;
		default:
          mvwprintw(stdscr, inputRow + 1, 2, "Location not found");
		  break;
	  }
  } else {
	  mvwprintw(stdscr, inputRow + 1, 2, "You cannot view the %s", parameters);
  }

  return inputRow;
}

int handleMoveCommand(int inputRow, char *mainInput, char *parameters, struct MainCharacter *character, struct System *system, struct Windows *window, struct WindowText *text, bool insulting) {
  if (parameters[0] == 'n') {
	  switch(character->locationNumber) {
		case FRONT_ROOM:
		  randomMovementString(character->locationNumber, insulting, inputRow);
		  break;
		case STUDY:
		  loadText(window, system, character, text, false);
		  mvwprintw(stdscr, inputRow + 1, 2, "You carelessly fall out of the window onto the rose bush and die");
		  mvwprintw(stdscr, inputRow + 2, 2, "Well done. You died where there were no enemies");
		  mvwprintw(window->photo, 3, 0, text->asciiSkullContents);
		  box(window->photo, 0, 0);
		  wrefresh(window->photo);
		  wrefresh(stdscr);
		  sleep(3);
		  system->exitFlag = true;
		  break;
		case HIDDEN_ROOM:
		  randomMovementString(character->locationNumber, insulting, inputRow);
		  break;
		case FR_PASSAGE:
		  inputRow = 0;
		  character->locationNumber = FRONT_ROOM;
		  loadText(window, system, character, text, true);
		  mvwprintw(stdscr, inputRow, 0, "> [%s@%s] %s %s", character->namehold, character->location, mainInput, parameters);
		  randomMovementSound();
		  mvwprintw(stdscr, inputRow + 1, 2, "You move into the front room. There is a large painting and 2 doors");
		  wrefresh(window->compass);
		  break;
		default:
		  mvwprintw(stdscr, inputRow + 1, 2, "Location not found");
		  break;
	  }
  } else if (parameters[0] == 'e') {
	  switch(character->locationNumber) {
		case FRONT_ROOM:
		  inputRow = 0;
		  character->locationNumber = STUDY;
		  loadText(window, system, character, text, true);
		  mvwprintw(stdscr, inputRow, 0, "> [%s@%s] %s %s", character->namehold, character->location, mainInput, parameters);
		  randomMovementSound();
          mvwprintw(stdscr, inputRow + 1, 2, "You move into a small study. Bookcases line the walls, along with a window overseeing the garden");
          inputRow++;
          mvwprintw(stdscr, inputRow + 1, 2, "There is a table with an old, solid oak drawer");
		  wrefresh(window->compass);
		  break;
		case STUDY:
		  if (system->bookcaseIsLoose) {
            inputRow = 0;
		    character->locationNumber = HIDDEN_ROOM;
		    loadText(window, system, character, text, true);
            mvwprintw(stdscr, inputRow, 0, "> [%s@%s] %s %s", character->namehold, character->location, mainInput, parameters);
		  randomMovementSound();
            mvwprintw(stdscr, inputRow + 1, 2, "You slide behind the bookcases and find yourself in a very small room with a chest");
          } else {
            mvwprintw(stdscr, inputRow + 1, 2, "There are some bookcases in the way. Try looking for one that can move");
          }
		  break;
		case HIDDEN_ROOM:
		case FR_PASSAGE:
		  randomMovementString(character->locationNumber, insulting, inputRow);
		  break;
		default:
		  mvwprintw(stdscr, inputRow + 1, 2, "Location not found");
		  break;
	  }
  } else if (parameters[0] == 's') {
	  switch(character->locationNumber) {
		case FRONT_ROOM:
		  if (system->pictureIsOpen) {
            inputRow = 0;
		    character->locationNumber = FR_PASSAGE;
		    loadText(window, system, character, text, true);
            mvwprintw(stdscr, inputRow, 0, "> [%s@%s] %s %s", character->namehold, character->location, mainInput, parameters);
		    randomMovementSound();
            mvwprintw(stdscr, inputRow + 1, 2, "You shift your body up into the hole in the wall where the painting was");
			inputRow++;
            mvwprintw(stdscr, inputRow + 1, 2, "It doesn't look like there is much here (No more content here in this game version)");
          } else {
            mvwprintw(stdscr, inputRow + 1, 2, "You can't move through a painting");
          }
		  break;
		case STUDY:
		  randomMovementString(character->locationNumber, insulting, inputRow);
		  break;
		case HIDDEN_ROOM:
		  randomMovementString(character->locationNumber, insulting, inputRow);
		  break;
		case FR_PASSAGE:
		  randomMovementString(character->locationNumber, insulting, inputRow);
		  break;
		default:
		  mvwprintw(stdscr, inputRow + 1, 2, "Location not found");
		  break;
	  }
  } else if (parameters[0] == 'w') {
	  switch(character->locationNumber) {
		case FRONT_ROOM:
		  if (system->keyIsInventory) {
            mvwprintw(stdscr, inputRow + 1, 2, "Well done, you have completed 'The Room of the Locked Door'");
			inputRow++;
            mvwprintw(stdscr, inputRow + 1, 2, "I hope you enjoyed your stay!");
			if (system->davidIsInventory) {
              mvwprintw(stdscr, inputRow + 1, 2, "You even found David's cake tin!");
			}
			system->exitFlag = true;
          } else {
            mvwprintw(stdscr, inputRow + 1, 2, "You need a key to exit this door. Maybe there is one somewhere here");
          }
		  break;
		case STUDY:
		  inputRow = 0;
		  character->locationNumber = FRONT_ROOM;
		  loadText(window, system, character, text, true);
		  mvwprintw(stdscr, inputRow, 0, "> [%s@%s] %s %s", character->namehold, character->location, mainInput, parameters);
		  randomMovementSound();
		  mvwprintw(stdscr, inputRow + 1, 2, "You move into the front room. There is a large painting and 2 doors");
		  wrefresh(window->compass);
		  break;
		case HIDDEN_ROOM:
		  inputRow = 0;
		  character->locationNumber = STUDY;
		  loadText(window, system, character, text, true);
		  mvwprintw(stdscr, inputRow, 0, "> [%s@%s] %s %s", character->namehold, character->location, mainInput, parameters);
		  randomMovementSound();
          mvwprintw(stdscr, inputRow + 1, 2, "You move into a small study. Bookcases line the walls, along with a window overseeing the garden");
          inputRow++;
          mvwprintw(stdscr, inputRow + 1, 2, "There is a table with an old, solid oak drawer");
		  wrefresh(window->compass);
		  break;
		case FR_PASSAGE:
		  randomMovementString(character->locationNumber, insulting, inputRow);
		  break;
		default:
		  mvwprintw(stdscr, inputRow + 1, 2, "Location not found");
		  break;
	  }
  } else if (stringIsSimilar(parameters, "picture", 2) || stringIsSimilar(parameters, "passage", 2)) {
	  switch(character->locationNumber) {
		case FRONT_ROOM:
          if (system->pictureIsOpen) {
            inputRow = 0;
		    character->locationNumber = FR_PASSAGE;
		    mvwprintw(stdscr, inputRow, 0, "> [%s@%s] %s %s", character->namehold, character->location, mainInput, parameters);
		    randomMovementSound();
            mvwprintw(stdscr, inputRow + 1, 2, "You squeeze behind the picture, and find yourself in a small room");
		    wrefresh(window->compass);
          } else {
            mvwprintw(stdscr, inputRow + 1, 2, "You can't find any %ss to enter", parameters);
          }
		  break;
		case STUDY:
		  mvwprintw(stdscr, inputRow + 1, 2, "There are no %ss to enter", parameters);
		  break;
		case HIDDEN_ROOM:
		  mvwprintw(stdscr, inputRow + 1, 2, "You search high and low but fail to find a %s to enter", parameters);
		  break;
		case FR_PASSAGE:
		  mvwprintw(stdscr, inputRow + 1, 2, "You are already in here", parameters);
		  break;
		default:
		  mvwprintw(stdscr, inputRow + 1, 2, "Location not found");
		  break;
	  }
  } else if (stringIsSimilar(parameters, "bookcase", 2)) {
	  switch(character->locationNumber) {
		case FRONT_ROOM:
		  break;
		case STUDY:
		  if (system->bookcaseIsLoose) {
            inputRow = 0;
		    character->locationNumber = HIDDEN_ROOM;
		    loadText(window, system, character, text, true);
            mvwprintw(stdscr, inputRow, 0, "> [%s@%s] %s %s", character->namehold, character->location, mainInput, parameters);
		    randomMovementSound();
            mvwprintw(stdscr, inputRow + 1, 2, "You slide behind the bookcases and find yourself in a very small room with a chest");
          } else {
            mvwprintw(stdscr, inputRow + 1, 2, "There are some bookcases in the way. Try looking for one that can move");
          }
		  break;
		case HIDDEN_ROOM:
          mvwprintw(stdscr, inputRow + 1, 2, "There is nothing in this room other than the chest");
		  break;
		case FR_PASSAGE:
          mvwprintw(stdscr, inputRow + 1, 2, "It seems like this passage has no %s", parameters);
		  break;
		default:
		  mvwprintw(stdscr, inputRow + 1, 2, "Location not found");
		  break;
	  }
  } else if (parameters[0] == '\0' || isspace(parameters[0])) {
      mvwprintw(stdscr, inputRow + 1, 2, "You decide to stay still");
  } else {
      mvwprintw(stdscr, inputRow + 1, 2, "You cannot move %s however much you try", parameters);
  }

  return inputRow;
}

int handleOpenCommand(int inputRow, char *mainInput, char *parameters, struct MainCharacter *character, struct System *system, struct Windows *window, struct WindowText *text) { 
  switch(character->locationNumber) {
	case FRONT_ROOM:
	  if (stringIsSimilar(parameters, "key", 2)) {
	    if (system->keyIsInventory) {
		  mvwprintw(stdscr, inputRow + 1, 2, "You try with all your might, but the key doesn't want to open");
	    } else {
		  mvwprintw(stdscr, inputRow + 1, 2, "You do not have a key to open");
	    }
	  } else if (stringIsSimilar(parameters, "door", 2)) {
          mvwprintw(stdscr, inputRow + 1, 2, "Try walking through it");
      } else if (stringIsSimilar(parameters, "treasure", 2)) {
	    if (system->davidIsInventory) {
		  mvwprintw(stdscr, inputRow + 1, 2, "You open the treasure... And it immediately slaps you in the face and closes");
	    } else {
		  mvwprintw(stdscr, inputRow + 1, 2, "You have no treasure");
	    }
	  } else if (stringIsSimilar(parameters, "picture", 2) || stringIsSimilar(parameters, "painting", 2)) {
	    if (!system->pictureIsOpen) {
		  mvwprintw(stdscr, inputRow + 1, 2, "You slide the picture to the side, revealing a passage");
          system->pictureIsOpen = true;
	    } else {
		  mvwprintw(stdscr, inputRow + 1, 2, "You already opened the picture");
	    }
	  } else {
          mvwprintw(stdscr, inputRow + 1, 2, "You pull as hard as you can on the %s, but it just won't open", parameters);
      }
	  break;
	case STUDY:
	  if (stringIsSimilar(parameters, "drawer", 2)) {
	    if (!system->keyIsVisible) {
		  mvwprintw(stdscr, inputRow + 1, 2, "You open the old, dusty drawer whose handle is nearly broken and you find an old, but shiny key");
		  system->keyIsVisible = true;
	    } else {
		  mvwprintw(stdscr, inputRow + 1, 2, "There is only a key. Oh, and some Opal Fruit wrappers");
	    }
	  } else if (stringIsSimilar(parameters, "window", 2)) {
          mvwprintw(stdscr, inputRow + 1, 2, "You open the creaky window and smell a gust of stale air.");
          inputRow++;
          mvwprintw(stdscr, inputRow + 1, 2, "It seems like it has been closed for years. There is a very sharp looking");
          inputRow++;
          mvwprintw(stdscr, inputRow + 1, 2, "rose bush which looks like it would hurt. There is nothing of use. You close the window");
	  } else if (stringIsSimilar(parameters, "door", 2)) {
          mvwprintw(stdscr, inputRow + 1, 2, "There is no door here");
	  } else if (stringIsSimilar(parameters, "bookcase", 2)) {
		  mvwprintw(stdscr, inputRow + 1, 2, "Ornate bookcases line the walls of this room. Could they have other uses?");
	  } else if (stringIsSimilar(parameters, "key", 2)) {
	    if (system->keyIsInventory) {
		  mvwprintw(stdscr, inputRow + 1, 2, "You try with all your might, but the key doesn't want to open");
	    } else {
		  mvwprintw(stdscr, inputRow + 1, 2, "You do not have a key to open");
	    }
	  } else if (stringIsSimilar(parameters, "treasure", 2)) {
	    if (system->davidIsInventory) {
		  mvwprintw(stdscr, inputRow + 1, 2, "You open the treasure... And it immediately slaps you in the face and closes");
	    } else {
		  mvwprintw(stdscr, inputRow + 1, 2, "You have no treasure");
	    }
	  } else {
        mvwprintw(stdscr, inputRow + 1, 2, "You pull as hard as you can on the %s, but it just won't open", parameters);
       }
	  break;
	case HIDDEN_ROOM:
      if (stringIsSimilar(parameters, "chest", 2)) {
		if (!system->davidIsVisible) {
          mvwprintw(stdscr, inputRow + 1, 2, "You open the chest and there is a small box inside");
          system->davidIsVisible = true;
		} else {
          mvwprintw(stdscr, inputRow + 1, 2, "You already opened the chest");
		}
      } else if (stringIsSimilar(parameters, "key", 2)) {
	    if (system->keyIsInventory) {
		  mvwprintw(stdscr, inputRow + 1, 2, "You try with all your might, but the key doesn't want to open");
	    } else {
		  mvwprintw(stdscr, inputRow + 1, 2, "You do not have a key to open");
	    }
	  } else if (stringIsSimilar(parameters, "treasure", 2)) {
	    if (system->davidIsInventory) {
		  mvwprintw(stdscr, inputRow + 1, 2, "You open the treasure... And it immediately slaps you in the face and closes");
	    } else {
		  mvwprintw(stdscr, inputRow + 1, 2, "You have no treasure");
	    }
	  } else {
          mvwprintw(stdscr, inputRow + 1, 2, "There is literally nothing else in here");
      }
	  break;
	case FR_PASSAGE:
	  if (stringIsSimilar(parameters, "key", 2)) {
	    if (system->keyIsInventory) {
		  mvwprintw(stdscr, inputRow + 1, 2, "You try with all your might, but the key doesn't want to open");
	    } else {
		  mvwprintw(stdscr, inputRow + 1, 2, "You do not have a key to open");
	    }
	  } else if (stringIsSimilar(parameters, "treasure", 2)) {
	    if (system->davidIsInventory) {
		  mvwprintw(stdscr, inputRow + 1, 2, "You open the treasure... And it immediately slaps you in the face and closes");
	    } else {
		  mvwprintw(stdscr, inputRow + 1, 2, "You have no treasure");
	    }
	  } else {
          mvwprintw(stdscr, inputRow + 1, 2, "There is literally nothing else in here");
      }
	  break;
	default:
	  break;
  }

  return inputRow;
}

int handleTakeCommand(int inputRow, char *mainInput, char *parameters, struct MainCharacter *character, struct System *system, struct Windows *window, struct WindowText *text) {
  switch(character->locationNumber) {
	case FRONT_ROOM:
	  if (stringIsSimilar(parameters, "picture", 1) || stringIsSimilar(parameters, "painting", 1)) {
		mvwprintw(stdscr, inputRow + 1, 2, "It is too large to carry anywhere");
	  } else {
        mvwprintw(stdscr, inputRow + 1, 2, "There is no %s to pick up", parameters);
	  }
	  break;
	case STUDY:
	  if (stringIsSimilar(parameters, "key", 1)) {
	    if (system->keyIsVisible && !system->keyIsInventory) {
		  mvwprintw(stdscr, inputRow + 1, 2, "You pick up the old key");
		  system->keyIsInventory = true;
	    } else {
	  	  mvwprintw(stdscr, inputRow + 1, 2, "There is no key to pick up");
	    }
      } else {
        mvwprintw(stdscr, inputRow + 1, 2, "There is no %s to pick up", parameters);
	  }
	  break;
	case HIDDEN_ROOM:
	  if (stringIsSimilar(parameters, "treasure", 1) || stringIsSimilar(parameters, "box", 1)) {
	    if (system->davidIsVisible && !system->davidIsInventory) {
		  mvwprintw(stdscr, inputRow + 1, 2, "You pick up the treasure");
		  system->davidIsInventory = true;
	    } else {
	  	  mvwprintw(stdscr, inputRow + 1, 2, "There is no treasure to pick up");
	    }
      } else {
        mvwprintw(stdscr, inputRow + 1, 2, "There is no %s to pick up", parameters);
	  }
	  break;
	case FR_PASSAGE:
      mvwprintw(stdscr, inputRow + 1, 2, "There is no %s to pick up", parameters);
	  break;
  }
  
  return inputRow;
}