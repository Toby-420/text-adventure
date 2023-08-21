#define MINIAUDIO_IMPLEMENTATION
#include <ctype.h>
#include <curses.h>
#include <json-c/json.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <windows.h>
#include "miniaudio.h"

#define MAX_HISTORY_ITEMS 1000
#define MAX_COMMAND_LENGTH 20
#define FRONT_ROOM 0
#define STUDY 1
#define HIDDEN_ROOM 2
#define FR_PASSAGE 3

/*

COMPILATION:

- compile.bat (gcc -o play icon/icon.o main.c -lncurses -Wall -Werror -Wpedantic -O3 -ljson-c -fstack-protector -C -H -g -trigraphs)
    └This should give you absolutely no errors or warnings

- powershell -noprofile -executionpolicy bypass -file ./textalteration.ps1
    └This changes the text displayed when the program is run in a DOS environment

LICENSE:

GNU General Public License version 3 (./COPYING)

*/

struct Windows {
  WINDOW *inventory;
  WINDOW *bar;
  WINDOW *photo;
  WINDOW *compass;
  WINDOW *roomdescription;
};

struct WindowText {
  char *asciiFrontRoomContents;
  char *textFrontRoomContents;
  char *asciiStudyContents;
  char *textStudyContents;
  char *asciiChestContents;
  char *asciiChestOpenContents;
  char *textHiddenRoomContents;
  char *asciiSkullContents;
  char *asciiPassageContents;
  char *textPassageContents;
  char *helpFileContents;
};

struct System {
  bool exitFlag;
  bool nameAsked;
  bool keyIsVisible;
  bool keyIsInventory;
  bool bookcaseIsLoose;
  bool davidTreasure;
  bool pictureIsOpen;
  bool hiddenRoomTreasure;
  bool passageTreasure;
  bool soundPlayed;
  int inventoryStringPlace;
};

struct MainCharacter {
  int locationNumber;
  char location[22];
  char name[22];
  char namehold[22];
  // bool overpowered;
};

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

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        return;
    }

    /* Reading PCM frames will loop based on what we specified when called ma_data_source_set_looping(). */
    ma_data_source_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

    (void)pInput;
}

void setupUi(struct Windows *window) {
  clear();
  wborder(window->bar, '|', '|', '-', '-', '+', '+', '+', '+');
  box(window->inventory, 0, 0);
  wrefresh(window->inventory);
  box(window->photo, 0, 0);
  wrefresh(window->photo);
  box(window->compass, 0, 0);
  wrefresh(window->compass);
  box(window->roomdescription, 0, 0);
  wrefresh(window->roomdescription);
  mvwprintw(window->bar, 0, 0, "Simple commands are: view, move, open, take, save, room, help, exit");
  wrefresh(window->bar);
  refresh();
}

void loadText(struct Windows *window, struct System *system, struct MainCharacter *character, struct WindowText *text, bool setupCheck) {
  if (setupCheck == true) {
    setupUi(window);
  }
  wclear(window->bar);
  wclear(window->photo);
  wclear(window->compass);
  wclear(window->inventory);
  wclear(window->roomdescription);

  window->bar = newwin(1, COLS, LINES - 1, 0);
  wborder(window->bar, '|', '|', '-', '-', '+', '+', '+', '+');

  mvwprintw(window->roomdescription, 1, 2, "ROOM DESCRIPTION:");
  mvwprintw(window->inventory, 1, 2, "INVENTORY:");
  mvwprintw(window->photo, 1, 2, "PLAYER VIEW:");
  mvwprintw(window->compass, 1, 2, "COMPASS:");
  mvwprintw(window->compass, 4, 8, "|");  // Add compass arms
  mvwprintw(window->compass, 6, 8, "|");
  mvwprintw(window->compass, 5, 6, "-- --");
  mvwprintw(window->compass, 5, 8, "O");  // Add center of compass
  mvwprintw(window->bar, 0, 0, "Commands are: view, move, open, take, save, room, help, exit");

  if (system->keyIsInventory) {
    mvwprintw(window->inventory, system->inventoryStringPlace, 1, "- Old, but shiny key");
    system->inventoryStringPlace++;
  }

  if (system->davidTreasure) {
    mvwprintw(window->inventory, system->inventoryStringPlace, 1, "- David's lost treasure");
    system->inventoryStringPlace++;
  }

  if (system->hiddenRoomTreasure) {
    mvwprintw(window->inventory, system->inventoryStringPlace, 1, "- Treasure from the hidden room");
    system->inventoryStringPlace++;
  }

  if (system->passageTreasure) {
    mvwprintw(window->inventory, system->inventoryStringPlace, 1, "- Treasure from the passage");
    system->inventoryStringPlace++;
  }
  
  switch(character->locationNumber) {
	case FRONT_ROOM:
	  mvwprintw(window->compass, 5, 2, "WEST");
	  mvwprintw(window->compass, 5, 11, "EAST");
	  if (system->pictureIsOpen) {
	    mvwprintw(window->compass, 7, 6, "SOUTH*");
	  }
	  mvwprintw(window->photo, 3, 0, text->asciiFrontRoomContents);
	  mvwprintw(window->roomdescription, 3, 0, text->textFrontRoomContents);
	  break;
	case STUDY:
      mvwprintw(window->compass, 5, 2, "WEST");
      if (system->bookcaseIsLoose) {
        mvwprintw(window->compass, 5, 11, "EAST*");
      }
      mvwprintw(window->photo, 3, 0, text->asciiStudyContents);
      mvwprintw(window->roomdescription, 3, 0, text->textStudyContents);
      if (system->bookcaseIsLoose) {
        mvwprintw(window->roomdescription, 11, 0, "The bookcases also move it seems");
      }
	  break;
	case HIDDEN_ROOM:
	  mvwprintw(window->compass, 5, 2, "WEST");
	  if (!system->davidTreasure) {
	    mvwprintw(window->photo, 3, 0, text->asciiChestContents);
	  } else {
	    mvwprintw(window->photo, 3, 0, text->asciiChestOpenContents);
	  }
	  mvwprintw(window->roomdescription, 3, 0, text->textHiddenRoomContents);
	  break;
	case FR_PASSAGE:
	  mvwprintw(window->compass, 3, 6, "NORTH");
	  mvwprintw(window->photo, 3, 0, text->asciiPassageContents);
	  mvwprintw(window->roomdescription, 3, 0, text->textPassageContents);
	  break;
	default:
	  break;
  }

  box(window->inventory, 0, 0);
  box(window->photo, 0, 0);
  box(window->roomdescription, 0, 0);
  box(window->compass, 0, 0);
  wrefresh(window->bar);
  wrefresh(window->photo);
  wrefresh(window->compass);
  wrefresh(window->inventory);
  wrefresh(window->roomdescription);
  refresh();
}

char *readFileContents(const char *filename) {
  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    perror("Error opening file");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *file_contents = (char *)malloc(size + 1);
  if (file_contents == NULL) {
    perror("Error allocating memory");
    fclose(file);
    return NULL;
  }

  size_t read_size = fread(file_contents, 1, size, file);
  file_contents[read_size] = '\0';

  fclose(file);

  return file_contents;
}

void toLowerCase(char *str) {
  if (str == NULL) return;

  while (*str) {
    *str = tolower(*str);
    str++;
  }
}

void firstToUpper(char *str) {
  *str = toupper(*str);
  // This does not have error checking as it is always called after the toLowerCase function
}

void xorCipher(char *input, int key) {
  while (*input) {
    *input = *input ^ key;
    input++;
  }
}

int saveGame(struct System system, struct MainCharacter character) {
  json_object *root = json_object_new_object();
  json_object_object_add(root, "location", json_object_new_string(character.location));
  json_object_object_add(root, "locationNumber", json_object_new_int(character.locationNumber));
  json_object_object_add(root, "name", json_object_new_string(character.name));          // ciphered
  json_object_object_add(root, "namehold", json_object_new_string(character.namehold));  // readable
  // json_object_object_add(root, "overpowered", json_object_new_boolean(character.overpowered));
  json_object_object_add(root, "nameAsked", json_object_new_boolean(system.nameAsked));
  json_object_object_add(root, "inventoryStringPlace", json_object_new_int(system.inventoryStringPlace));
  json_object_object_add(root, "keyIsVisible", json_object_new_boolean(system.keyIsVisible));
  json_object_object_add(root, "keyIsInventory", json_object_new_boolean(system.keyIsInventory));
  json_object_object_add(root, "bookcaseIsLoose", json_object_new_boolean(system.bookcaseIsLoose));
  json_object_object_add(root, "davidTreasure", json_object_new_boolean(system.davidTreasure));
  json_object_object_add(root, "pictureisOpen", json_object_new_boolean(system.pictureIsOpen));
  json_object_object_add(root, "hiddenRoomTreasure", json_object_new_boolean(system.hiddenRoomTreasure));
  json_object_object_add(root, "passageTreasure", json_object_new_boolean(system.passageTreasure));

  const char *jsonString = json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY);

  FILE *file = fopen("text\\savefile.json", "w");
  if (file == NULL) {
    printf("Error opening file!\n");
    return 1;
  }
  fprintf(file, "%s", jsonString);
  fclose(file);

  json_object_put(root);
  return 0;
}

int loadGame(struct System *system, struct MainCharacter *character) {
  FILE *file = fopen("text\\savefile.json", "r");
  if (file == NULL) {
    printf("Error opening file!\n");
    return 1;
  }

  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *jsonString = (char *)malloc(fileSize + 1);
  if (jsonString == NULL) {
    fclose(file);
    printf("Memory allocation error!\n");
    return 1;
  }

  fread(jsonString, 1, fileSize, file);
  fclose(file);
  jsonString[fileSize] = '\0';

  json_object *root = json_tokener_parse(jsonString);
  free(jsonString);

  if (root == NULL) {
    printf("Error parsing JSON character!\n");
    return 1;
  }

  strcpy(character->location, json_object_get_string(json_object_object_get(root, "location")));
  character->locationNumber = json_object_get_int(json_object_object_get(root, "locationNumber"));
  strcpy(character->name, json_object_get_string(json_object_object_get(root, "name")));
  strcpy(character->namehold, json_object_get_string(json_object_object_get(root, "namehold")));
  // character->overpowered = json_object_get_boolean(json_object_object_get(root, "overpowered"));
  system->nameAsked = json_object_get_boolean(json_object_object_get(root, "nameAsked"));
  system->inventoryStringPlace = json_object_get_int(json_object_object_get(root, "inventoryStringPlace"));
  system->keyIsVisible = json_object_get_boolean(json_object_object_get(root, "keyIsVisible"));
  system->keyIsInventory = json_object_get_boolean(json_object_object_get(root, "keyIsInventory"));
  system->bookcaseIsLoose = json_object_get_boolean(json_object_object_get(root, "bookcaseIsLoose"));
  system->davidTreasure = json_object_get_boolean(json_object_object_get(root, "davidTreasure"));
  system->pictureIsOpen = json_object_get_boolean(json_object_object_get(root, "pictureIsOpen"));
  system->hiddenRoomTreasure = json_object_get_boolean(json_object_object_get(root, "hiddenRoomTreasure"));
  system->passageTreasure = json_object_get_boolean(json_object_object_get(root, "passageTreasure"));

  json_object_put(root);
  return 0;
}

int handleViewCommand(int inputRow, char *mainInput, char *parameters, struct MainCharacter *character, struct System *system, struct Windows *window, struct WindowText *text, ma_engine *engine) { 
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
	  if (system->davidTreasure) {
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
          mvwprintw(stdscr, inputRow + 1, 2, "A somewhat good-looking room until you see the window and drawer which are slighty broken");
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
  }

  return inputRow;
}

int handleMoveCommand(int inputRow, char *mainInput, char *parameters, struct MainCharacter *character, struct System *system, struct Windows *window, struct WindowText *text, ma_engine *engine) {
  if (parameters[0] == 'n') {
	  switch(character->locationNumber) {
		case FRONT_ROOM:
		  mvwprintw(stdscr, inputRow + 1, 2, "You can't go through the window");
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
		  mvwprintw(stdscr, inputRow + 1, 2, "You walk into the wall and headbutt it. Wall -10hp");
		  break;
		case FR_PASSAGE:
		  inputRow = 0;
		  character->locationNumber = FRONT_ROOM;
		  loadText(window, system, character, text, true);
		  mvwprintw(stdscr, inputRow, 0, "> [%s@%s] %s %s", character->namehold, character->location, mainInput, parameters);
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
            mvwprintw(stdscr, inputRow + 1, 2, "You slide behind the bookcases and find yourself in a very small room with a chest");
          } else {
            mvwprintw(stdscr, inputRow + 1, 2, "There are no bookcases in the way. Try looking for one that can move");
          }
		  break;
		case HIDDEN_ROOM:
          mvwprintw(stdscr, inputRow + 1, 2, "How does the wall taste?");
		  break;
		case FR_PASSAGE:
          mvwprintw(stdscr, inputRow + 1, 2, "You must have smashed your head on these walls so many times now");
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
            mvwprintw(stdscr, inputRow + 1, 2, "You shift your body up into the hole in the wall where the painting was");
			inputRow++;
            mvwprintw(stdscr, inputRow + 1, 2, "It doesn't look like there is much here (No more content here in this game version)");
          } else {
            mvwprintw(stdscr, inputRow + 1, 2, "You can't move through a painting");
          }
		  break;
		case STUDY:
		  mvwprintw(stdscr, inputRow + 1, 2, "HEY! DON'T WALK INTO THE CAMERA!!!");
		  break;
		case HIDDEN_ROOM:
		  mvwprintw(stdscr, inputRow + 1, 2, "I guess small spaces are not easy to navigate");
		  break;
		case FR_PASSAGE:
		  mvwprintw(stdscr, inputRow + 1, 2, "Down bad? Down diabolical? Nah you just try to go too far south don't you");
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
			if (system->davidTreasure) {
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
		  mvwprintw(stdscr, inputRow + 1, 2, "You move into the front room. There is a large painting and 2 doors");
		  wrefresh(window->compass);
		  break;
		case HIDDEN_ROOM:
		  inputRow = 0;
		  character->locationNumber = STUDY;
		  loadText(window, system, character, text, true);
		  mvwprintw(stdscr, inputRow, 0, "> [%s@%s] %s %s", character->namehold, character->location, mainInput, parameters);
          mvwprintw(stdscr, inputRow + 1, 2, "You move into a small study. Bookcases line the walls, along with a window overseeing the garden");
          inputRow++;
          mvwprintw(stdscr, inputRow + 1, 2, "There is a table with an old, solid oak drawer");
		  wrefresh(window->compass);
		  break;
		case FR_PASSAGE:
          mvwprintw(stdscr, inputRow + 1, 2, "Nice wall, eh?");
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
  } else {
    mvwprintw(stdscr, inputRow + 1, 2, "You cannot move %s however much you try", parameters);
  }

  return inputRow;
}

int handleOpenCommand(int inputRow, char *mainInput, char *parameters, struct MainCharacter *character, struct System *system, struct Windows *window, struct WindowText *text, ma_engine *engine) { 
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
	    if (system->davidTreasure) {
		  mvwprintw(stdscr, inputRow + 1, 2, "You open the treasure... And it immediately slaps you in the face and closes");
	    } else {
		  mvwprintw(stdscr, inputRow + 1, 2, "You have no treasure");
	    }
	  } else if (stringIsSimilar(parameters, "picture", 2)) {
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
	  	  ma_engine_play_sound(engine, "audio/drawer.mp3", NULL);
		  mvwprintw(stdscr, inputRow + 1, 2, "You open the old, dusty drawer whose handle is nearly broken and you find an old, but shiny key");
		  system->keyIsVisible = true;
	    } else {
		  mvwprintw(stdscr, inputRow + 1, 2, "There is only a key. Oh, and some Opal Fruit wrappers");
	    }
	  } else if (stringIsSimilar(parameters, "window", 2)) {
          ma_engine_play_sound(engine, "audio/wind.mp3", NULL);
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
	    if (system->davidTreasure) {
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
		if (!system->davidTreasure) {
          mvwprintw(stdscr, inputRow + 1, 2, "You found the Lost Treasure of David Spindler");
          system->davidTreasure = true;
		}
      } else if (stringIsSimilar(parameters, "key", 2)) {
	    if (system->keyIsInventory) {
		  mvwprintw(stdscr, inputRow + 1, 2, "You try with all your might, but the key doesn't want to open");
	    } else {
		  mvwprintw(stdscr, inputRow + 1, 2, "You do not have a key to open");
	    }
	  } else if (stringIsSimilar(parameters, "treasure", 2)) {
	    if (system->davidTreasure) {
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
	    if (system->davidTreasure) {
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

int handleTakeCommand(int inputRow, char *mainInput, char *parameters, struct MainCharacter *character, struct System *system, struct Windows *window, struct WindowText *text, ma_engine *engine) {
  switch(character->locationNumber) {
	case FRONT_ROOM:
      mvwprintw(stdscr, inputRow + 1, 2, "There is no %s to pick up", parameters);
	  break;
	case STUDY:
	  if (system->keyIsVisible && !system->keyIsInventory) {
		ma_engine_play_sound(engine, "audio/sparkle.mp3", NULL);
		mvwprintw(stdscr, inputRow + 1, 2, "You pick up the old key");
		system->keyIsInventory = true;
	  } else {
		mvwprintw(stdscr, inputRow + 1, 2, "There is no key to pick up");
	  }
	  break;
	case HIDDEN_ROOM:
      mvwprintw(stdscr, inputRow + 1, 2, "There is no %s to pick up", parameters);
	  break;
	case FR_PASSAGE:
      mvwprintw(stdscr, inputRow + 1, 2, "There is no %s to pick up", parameters);
	  break;
  }
  
  return inputRow;
}

int main(int argc, char *argv[]) {
  HWND consoleWindow = GetConsoleWindow();  // Get handle to console window
  ShowWindow(consoleWindow, SW_MAXIMIZE);   // Maximize console window
  SetConsoleTitle("The Room of the Locked Door");

  struct MainCharacter character;
  struct Windows window;
  struct System system;
  struct WindowText text;

  ma_result result;
  ma_decoder decoder;
  ma_device_config deviceConfig;
  ma_device device;
  ma_engine engine;
  ma_engine_init(NULL, &engine);
  
  const char audioFile[] = "audio\\track1.mp3";

  result = ma_decoder_init_file(audioFile, NULL, &decoder);
  if (result != MA_SUCCESS) {
	printf("Audio file not found in %s", audioFile);
	return -2;
  }
  
  ma_data_source_set_looping(&decoder, MA_TRUE);
  deviceConfig = ma_device_config_init(ma_device_type_playback);
  deviceConfig.playback.format   = decoder.outputFormat;
  deviceConfig.playback.channels = decoder.outputChannels;
  deviceConfig.sampleRate        = decoder.outputSampleRate;
  deviceConfig.dataCallback      = data_callback;
  deviceConfig.pUserData         = &decoder;

  initscr();  // Start ncurses
  keypad(stdscr, TRUE);
  curs_set(1);
  set_tabsize(4);

  snprintf(character.location, sizeof(character.location), " ");
  snprintf(character.name, sizeof(character.name), " ");
  snprintf(character.namehold, sizeof(character.namehold), " ");
  // character.overpowered = false;

  window.bar = newwin(1, COLS, LINES - 1, 0);
  wborder(window.bar, '|', '|', '-', '-', '+', '+', '+', '+');

  window.inventory = newwin(LINES / 3 + 3, COLS / 4, 1, COLS * 6 / 8);
  box(window.inventory, 0, 0);

  window.photo = newwin(27, 45, LINES - 28, COLS - 85);
  box(window.photo, 0, 0);

  window.roomdescription = newwin(27, 38, LINES - 28, COLS - 39);
  box(window.roomdescription, 0, 0);

  window.compass = newwin(9, 17, 1, COLS * 6 / 8 - 19);
  box(window.compass, 0, 0);

  unsigned short inputRow = 2;
  char input[22] = " ";
  char mainInput[22] = " ";
  char parameters[22] = " ";
  char *token = " ";
  system.exitFlag = false;

  system.nameAsked = false;
  system.keyIsVisible = false;
  system.keyIsInventory = false;
  system.bookcaseIsLoose = false;
  system.davidTreasure = false;
  system.pictureIsOpen = false;
  system.hiddenRoomTreasure = false;
  system.passageTreasure = false;
  system.soundPlayed = false;
  system.inventoryStringPlace = 2;
  character.locationNumber = 0;

  strcpy(character.location, "front room");

  FILE *historyFile = fopen("text\\history.txt", "w");

  text.asciiFrontRoomContents = readFileContents("ascii\\frontRoom.txt");
  text.textFrontRoomContents = readFileContents("text\\frontRoom.txt");
  text.asciiStudyContents = readFileContents("ascii\\study.txt");
  text.textStudyContents = readFileContents("text\\study.txt");
  text.asciiChestContents = readFileContents("ascii\\chest.txt");
  text.asciiChestOpenContents = readFileContents("ascii\\chestOpen.txt");
  text.textHiddenRoomContents = readFileContents("text\\hiddenRoom.txt");
  text.asciiSkullContents = readFileContents("ascii\\skull.txt");
  text.asciiPassageContents = readFileContents("ascii\\passage.txt");
  text.textPassageContents = readFileContents("text\\passage.txt");
  text.helpFileContents = readFileContents("text\\help.txt");

  if (argc > 1 && strcmp(argv[1], "load") == 0) {
    loadGame(&system, &character);
    loadText(&window, &system, &character, &text, true);
  } else if (argc > 1 && strcmp(argv[1], "load") == 1 && stringIsSimilar(argv[1], "load", 2)) {
    printf("Did you mean 'load'");
	return 1;
  } else {
    loadText(&window, &system, &character, &text, true);
  }

  while (!system.exitFlag) {
    if (system.nameAsked == false) {
      echo();
      mvwprintw(stdscr, inputRow, 2, "The Room of the Locked Door");
      inputRow++;
      while (1) {
        mvwprintw(stdscr, inputRow, 2, "What do you want to be called? ");
        getnstr(character.name, 20);

        if (character.name[0] == '\0' || isspace(character.name[0])) {
        } else {
          break;
        }
      }

      noecho();

      toLowerCase(character.name);

      strcpy(character.namehold, character.name);

      xorCipher(character.name, 42);

      // if (strcmp(character.name, "YBSFCFS") == 0) {
      // character.overpowered = true;
      // }

      firstToUpper(character.namehold);

      loadText(&window, &system, &character, &text, true);

      mvwprintw(stdscr, inputRow - 1, 0, "\t\t\t\tWelcome to The Room of the Locked Door");
      inputRow++;
      mvwprintw(stdscr, inputRow - 1, 0, "\t\t\t\t\tESCAPE OR DIE");
      inputRow++;
      mvwprintw(stdscr, inputRow - 1, 0, "\tor type exit if you're a useless roadman who can't type and think at the same time");
      inputRow += 2;
      mvwprintw(stdscr, inputRow - 1, 0, "You find yourself in an ornate and spacious front room with a window and a picture");
      inputRow++;
      mvwprintw(stdscr, inputRow - 1, 0, "There is a door that leads east and one that leads west");
      inputRow++;
      system.nameAsked = true;
      echo();
    }
    if (system.soundPlayed == false) {
		if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
			printf("Failed to open playback device.\n");
			ma_decoder_uninit(&decoder);
			return -3;
		}

		if (ma_device_start(&device) != MA_SUCCESS) {
			printf("Failed to start playback device.\n");
			ma_device_uninit(&device);
			ma_decoder_uninit(&decoder);
			return -4;
		}
      system.soundPlayed = true;
    }
	
	switch (character.locationNumber) {
	  case 0:
	    strcpy(character.location, "front room");
	    break;
  	  case 1:
  	    strcpy(character.location, "study");
	    break;
	  case 2:
	    strcpy(character.location, "hidden");
	    break;
	  case 3:
	    strcpy(character.location, "passage");
	    break;
	  default:
	    break;
	}
  
    mvwprintw(stdscr, inputRow, 0, "> [%s@%s] ", character.namehold, character.location);

    getnstr(input, 20);

    fprintf(historyFile, "%s\n", input);

    token = strtok(input, " ");
    if (token != NULL) {
      strcpy(mainInput, token);

      token = strtok(NULL, " ");
      if (token != NULL) {
        strcpy(parameters, token);
      }
    }

    int i;
    for (i = 0; mainInput[i]; i++) {
      mainInput[i] = tolower(mainInput[i]);
    }
    int f;
    for (f = 0; parameters[f]; f++) {
      parameters[f] = tolower(parameters[f]);
    }

    if (stringIsSimilar(mainInput, "exit", 1)) {
      mvwprintw(stdscr, inputRow + 1, 2, "Exiting game");
      system.exitFlag = true;
    } else if (stringIsSimilar(mainInput, "view", 1) || stringIsSimilar(mainInput, "look", 1)) {
      inputRow = handleViewCommand(inputRow, mainInput, parameters, &character, &system, &window, &text, &engine);
      inputRow += 2;
    } else if (stringIsSimilar(mainInput, "move", 1) || stringIsSimilar(mainInput, "walk", 1) || stringIsSimilar(mainInput, "run", 1) || stringIsSimilar(mainInput, "enter", 1) || stringIsSimilar(mainInput, "go", 1) || stringIsSimilar(mainInput, "cd", 1)) {
      inputRow = handleMoveCommand(inputRow, mainInput, parameters, &character, &system, &window, &text, &engine);
      inputRow += 2;
    } else if (stringIsSimilar(mainInput, "open", 1) || stringIsSimilar(mainInput, "shift", 1)) {
      inputRow = handleOpenCommand(inputRow, mainInput, parameters, &character, &system, &window, &text, &engine);
      inputRow += 2;
    } else if (stringIsSimilar(mainInput, "take", 1) || stringIsSimilar(mainInput, "pick", 1)) {
      inputRow = handleTakeCommand(inputRow, mainInput, parameters, &character, &system, &window, &text, &engine);
      inputRow += 2;
    } else if (strcmp(mainInput, "cory-nonce") == 0) {
      ma_engine_play_sound(&engine, "audio/cory-nonce.mp3", NULL);
      inputRow++;
    } else if (stringIsSimilar(mainInput, "name", 1)) {
      mvwprintw(stdscr, inputRow + 1, 2, "Enter new name: ");
      getnstr(character.name, 20);
      toLowerCase(character.name);
      strcpy(character.namehold, character.name);
      xorCipher(character.name, 42);
      firstToUpper(character.namehold);
      inputRow += 2;
    } else if (stringIsSimilar(mainInput, "save", 1)) {
      mvwprintw(stdscr, inputRow + 1, 2, "Saving game to gamesave.json");
      inputRow++;
      int fail = saveGame(system, character);
      if (!fail) {
        mvwprintw(stdscr, inputRow + 1, 2, "Save succeeded");
      } else {
        mvwprintw(stdscr, inputRow + 1, 2, "Save failed");
        beep();
      }
      inputRow += 2;
    } else if (stringIsSimilar(mainInput, "load", 1)) {
      mvwprintw(stdscr, inputRow + 1, 2, "Loading game from gamesave.json");
      inputRow++;
      int fail = loadGame(&system, &character);
      if (!fail) {
        mvwprintw(stdscr, inputRow + 1, 2, "Load succeeded");
		loadText(&window, &system, &character, &text, false);
      } else {
        mvwprintw(stdscr, inputRow + 1, 2, "Load failed");
        beep();
      }
      inputRow += 2;
    } else if (stringIsSimilar(mainInput, "help", 1)) {
      clear();
      printw(text.helpFileContents);
      wrefresh(stdscr);
      getch();
      inputRow = 0;
      setupUi(&window);
    } else if (mainInput[0] == '\0' || isspace(mainInput[0])) {
      mvwprintw(stdscr, inputRow + 1, 2, "I'm not psychic you know");
      inputRow += 2;
    } else {
      mvwprintw(stdscr, inputRow + 1, 2, "Try using one of the commands below");
      inputRow += 2;
    }
	

    mainInput[0] = '\0';
    parameters[0] = '\0';

    refresh();

    if (inputRow + 1 == LINES - 4 || inputRow + 1 == LINES - 3) {
      inputRow = 0;
      loadText(&window, &system, &character, &text, true);
    }
  }

  sleep(1);
  delwin(window.bar);  // Delete the window.bar
  delwin(window.compass);
  delwin(window.inventory);
  delwin(window.roomdescription);
  delwin(window.photo);
  endwin();  // Clears ncurses and returns back to previous state
  return 0;  // Gives successful completion code
}