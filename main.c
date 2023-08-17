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
#define PASSAGE 3

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
  bool nameAsked;
  bool keyIsVisible;
  bool keyIsInventory;
  bool bookcaseIsLoose;
  bool chestIsLoose;
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

void setupUi(struct Windows window) {
  (void)clear();
  (void)wclear(window.bar);
  (void)wclear(window.photo);
  (void)wclear(window.compass);
  (void)wclear(window.inventory);
  (void)wclear(window.roomdescription);
  (void)refresh();
  (void)wborder(window.bar, '|', '|', '-', '-', '+', '+', '+', '+');
  (void)box(window.inventory, 0, 0);
  (void)wrefresh(window.inventory);
  (void)box(window.photo, 0, 0);
  (void)wrefresh(window.photo);
  (void)refresh();
  (void)box(window.compass, 0, 0);
  (void)wrefresh(window.compass);
  (void)refresh();
  (void)box(window.roomdescription, 0, 0);
  (void)wrefresh(window.roomdescription);
  (void)refresh();
  (void)mvwprintw(window.bar, 0, 0, "Simple commands are: view, move, open, take, save, room, help, exit");
  (void)wrefresh(window.bar);
  (void)refresh();
}

void loadText(struct Windows window, struct System system, struct MainCharacter character, struct WindowText text) {
  setupUi(window);

  window.bar = newwin(1, COLS, LINES - 1, 0);
  wborder(window.bar, '|', '|', '-', '-', '+', '+', '+', '+');

  mvwprintw(window.roomdescription, 1, 2, "ROOM DESCRIPTION:");
  mvwprintw(window.inventory, 1, 2, "INVENTORY:");
  mvwprintw(window.photo, 1, 2, "PLAYER VIEW:");
  mvwprintw(window.compass, 1, 2, "COMPASS:");
  mvwprintw(window.compass, 4, 8, "|");  // Add compass arms
  mvwprintw(window.compass, 6, 8, "|");
  mvwprintw(window.compass, 5, 6, "-- --");
  mvwprintw(window.compass, 5, 8, "O");  // Add center of compass
  mvwprintw(window.bar, 0, 0, "Commands are: view, move, open, take, save, room, help, exit");

  if (system.keyIsInventory == true) {
    mvwprintw(window.inventory, system.inventoryStringPlace, 1, "- Old, but shiny key");
    system.inventoryStringPlace++;
  }

  if (system.davidTreasure == true) {
    mvwprintw(window.inventory, system.inventoryStringPlace, 1, "- David's lost treasure");
    system.inventoryStringPlace++;
  }

  if (system.hiddenRoomTreasure == true) {
    mvwprintw(window.inventory, system.inventoryStringPlace, 1, "- Treasure from the hidden room");
    system.inventoryStringPlace++;
  }

  if (system.passageTreasure == true) {
    mvwprintw(window.inventory, system.inventoryStringPlace, 1, "- Treasure from the passage");
    system.inventoryStringPlace++;
  }

  if (strcmp(character.location, "front room") == 0) {
    mvwprintw(window.compass, 5, 2, "WEST ");
    mvwprintw(window.compass, 5, 11, "EAST ");

    if (system.pictureIsOpen == true) {
      mvwprintw(window.compass, 7, 6, "SOUTH*");
    }

    mvwprintw(window.photo, 3, 0, text.asciiFrontRoomContents);

    mvwprintw(window.roomdescription, 3, 0, text.textFrontRoomContents);

  } else if (strcmp(character.location, "study") == 0) {
    if (system.bookcaseIsLoose == true) {
      mvwprintw(window.compass, 5, 11, "EAST*");
    }

    mvwprintw(window.compass, 5, 2, "WEST");

    mvwprintw(window.photo, 3, 0, text.asciiStudyContents);

    mvwprintw(window.roomdescription, 3, 0, text.textStudyContents);
    if (system.bookcaseIsLoose == true) {
      mvwprintw(window.roomdescription, 11, 2, "The bookcases also move it seems");
    }

  } else if (strcmp(character.location, "passage") == 0) {
    mvwprintw(window.compass, 3, 6, "NORTH");

    mvwprintw(window.photo, 3, 2, text.asciiPassageContents);

    mvwprintw(window.roomdescription, 3, 2, text.textPassageContents);

  } else if (strcmp(character.location, "hidden") == 0) {
    if (system.davidTreasure == false) {
      mvwprintw(window.photo, 3, 2, text.asciiChestContents);
    } else {
      mvwprintw(window.photo, 3, 2, text.asciiChestOpenContents);
    }

    mvwprintw(window.compass, 5, 2, "WEST");

    mvwprintw(window.roomdescription, 3, 2, text.textHiddenRoomContents);
  }

  box(window.inventory, 0, 0);
  box(window.photo, 0, 0);
  box(window.roomdescription, 0, 0);
  box(window.compass, 0, 0);
  wrefresh(window.bar);
  wrefresh(window.photo);
  wrefresh(window.compass);
  wrefresh(window.inventory);
  wrefresh(window.roomdescription);
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
  (void)json_object_object_add(root, "location", json_object_new_string(character.location));
  (void)json_object_object_add(root, "name", json_object_new_string(character.name));          // ciphered
  (void)json_object_object_add(root, "namehold", json_object_new_string(character.namehold));  // readable
  (void)json_object_object_add(root, "nameAsked", json_object_new_boolean(system.nameAsked));
  // (void)json_object_object_add(root, "overpowered", json_object_new_boolean(character.overpowered));

  const char *jsonString = json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY);

  FILE *file = fopen("text\\savefile.json", "w");
  if (file == NULL) {
    (void)printf("Error opening file!\n");
    return 1;
  }
  (void)fprintf(file, "%s", jsonString);
  (void)fclose(file);

  (void)json_object_put(root);
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
  strcpy(character->name, json_object_get_string(json_object_object_get(root, "name")));
  strcpy(character->namehold, json_object_get_string(json_object_object_get(root, "namehold")));
  system->nameAsked = json_object_get_boolean(json_object_object_get(root, "nameAsked"));

  json_object_put(root);
  return 0;
}

char *handleLocation(int locationNumber) {
  char *location = NULL;

  switch (locationNumber) {
    case 0:
      location = strdup("front room");
      break;
    case 1:
      location = strdup("study");
      break;
    case 2:
      location = strdup("hidden");
      break;
    case 3:
      location = strdup("passage");
      break;
    default:
      break;
  }

  return location;
}

int handleViewCommand(int inputRow, char *mainInput, char *parameters, struct MainCharacter character, struct System system, struct Windows window) { return 0; }

int handleMoveCommand(int inputRow, char *mainInput, char *parameters, struct MainCharacter character, struct System system, struct Windows window) {
  if (parameters[0] == 'n') {
    mvwprintw(stdscr, inputRow + 1, 2, "North chosen");
  } else if (parameters[0] == 'e') {
    mvwprintw(stdscr, inputRow + 1, 2, "East chosen");
    if (character.locationNumber == FRONT_ROOM) {
    } else if (character.locationNumber == STUDY) {
    } else if (character.locationNumber == HIDDEN_ROOM) {
    } else {
    }
  } else if (strcmp(parameters, "passage") == 0 || strcmp(parameters, "picture") == 0) {
    if (character.locationNumber == HIDDEN_ROOM) {
    } else if (character.locationNumber == STUDY) {
    } else {
    }
  } else if (strcmp(parameters, "bookcases") == 0 || strcmp(parameters, "bookcase") == 0) {
  } else if (parameters[0] == 'w') {
    mvwprintw(stdscr, inputRow + 1, 2, "West chosen");
  } else {
    mvwprintw(stdscr, inputRow + 1, 2, "You cannot move %s however much you try", parameters);
  }

  return 0;
}

int handleOpenCommand(int inputRow, char *mainInput, char *parameters, struct MainCharacter character, struct System system, struct Windows window) { return 0; }

int handleTakeCommand(int inputRow, char *mainInput, char *parameters, struct MainCharacter character, struct System system, struct Windows window) { return 0; }

int processInput(int inputRow, char *mainInput, char *parameters, bool exitFlag, struct MainCharacter character, struct System system, struct Windows window, struct WindowText text) {
  if (strcmp(mainInput, "view") == 0 || strcmp(mainInput, "look") == 0) {
    handleViewCommand(inputRow, mainInput, parameters, character, system, window);
    inputRow += 2;
  } else if (strcmp(mainInput, "move") == 0 || strcmp(mainInput, "walk") == 0 || strcmp(mainInput, "run") == 0 || strcmp(mainInput, "enter") == 0 || strcmp(mainInput, "go") == 0 || strcmp(mainInput, "cd") == 0) {
    handleMoveCommand(inputRow, mainInput, parameters, character, system, window);
    inputRow += 2;
  } else if (strcmp(mainInput, "open") == 0 || strcmp(mainInput, "shift") == 0) {
    handleOpenCommand(inputRow, mainInput, parameters, character, system, window);
    inputRow += 2;
  } else if (strcmp(mainInput, "take") == 0 || strcmp(mainInput, "pick") == 0) {
    handleTakeCommand(inputRow, mainInput, parameters, character, system, window);
    inputRow += 2;
  } else if (strcmp(mainInput, "help") == 0) {
    clear();
    printw(text.helpFileContents);
    wrefresh(stdscr);
    getch();
    inputRow = 0;
    setupUi(window);
  } else if (mainInput[0] == '\0' || isspace(mainInput[0])) {
    mvwprintw(stdscr, inputRow + 1, 2, "I'm not psychic you know");
    inputRow += 2;
  } else {
    mvwprintw(stdscr, inputRow + 1, 2, "Try using one of the commands below");
    inputRow += 2;
  }
  return inputRow;
}

int main(int argc, char *argv[]) {
  HWND consoleWindow = GetConsoleWindow();  // Get handle to console window
  ShowWindow(consoleWindow, SW_MAXIMIZE);   // Maximize console window

  struct MainCharacter character;
  struct Windows window;
  struct System system;
  struct WindowText text;

  ma_engine engine;
  ma_engine_init(NULL, &engine);

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
  bool exitFlag = false;

  system.nameAsked = false;
  system.keyIsVisible = false;
  system.keyIsInventory = false;
  system.bookcaseIsLoose = false;
  system.chestIsLoose = false;
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
    loadText(window, system, character, text);
  } else {
    loadText(window, system, character, text);
  }

  while (!exitFlag) {
    if (system.nameAsked == false) {
      (void)echo();
      mvwprintw(stdscr, inputRow, 2, "The Room of the Locked Door");
      inputRow++;
      while (1) {
        (void)mvwprintw(stdscr, inputRow, 2, "What do you want to be called? ");
        getnstr(character.name, 20);

        if (character.name[0] == '\0' || isspace(character.name[0])) {
        } else {
          break;
        }
      }

      (void)noecho();

      (void)toLowerCase(character.name);

      (void)strcpy(character.namehold, character.name);

      (void)xorCipher(character.name, 42);

      // if (strcmp(character.name, "YBSFCFS") == 0) {
      // character.overpowered = true;
      // }

      (void)firstToUpper(character.namehold);

      loadText(window, system, character, text);

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
      (void)echo();
    }
    if (system.soundPlayed == false) {
      PlaySound("audio\\track1.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
      system.soundPlayed = true;
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

    if (strcmp(mainInput, "exit") == 0) {
	  mvwprintw(stdscr, inputRow + 1, 2, "Exiting game");
      exitFlag = true;
    } else if (strcmp(mainInput, "name") == 0) {
      (void)mvwprintw(stdscr, inputRow + 1, 2, "Enter new name: ");
      getnstr(character.name, 20);
      toLowerCase(character.name);
      strcpy(character.namehold, character.name);
      xorCipher(character.name, 42);
      firstToUpper(character.namehold);
      inputRow += 2;
    } else if (strcmp(mainInput, "save") == 0) {
    (void)mvwprintw(stdscr, inputRow + 1, 2, "Saving game to gamesave.json");
    inputRow++;
    int success = saveGame(system, character);
    if (!success) {
      (void)mvwprintw(stdscr, inputRow + 1, 2, "Save succeeded");
    } else {
      (void)mvwprintw(stdscr, inputRow + 1, 2, "Save failed");
      (void)beep();
    }
    inputRow += 2;
  } else if (strcmp(mainInput, "load") == 0) {
    (void)mvwprintw(stdscr, inputRow + 1, 2, "Loading game from gamesave.json");
    inputRow++;
    int success = loadGame(&system, &character);
    if (!success) {
      (void)mvwprintw(stdscr, inputRow + 1, 2, "Load succeeded");
    } else {
      (void)mvwprintw(stdscr, inputRow + 1, 2, "Load failed");
      (void)beep();
    }
    inputRow += 2;
  } else {
      inputRow = processInput(inputRow, mainInput, parameters, exitFlag, character, system, window, text);
    }

    mainInput[0] = '\0';
    parameters[0] = '\0';

    refresh();

    if (inputRow + 1 == LINES - 4 || inputRow + 1 == LINES - 3) {
      inputRow = 0;
      loadText(window, system, character, text);
    }
  }

  sleep(2);
  delwin(window.bar);  // Delete the window.bar
  delwin(window.compass);
  delwin(window.inventory);
  delwin(window.roomdescription);
  delwin(window.photo);
  endwin();  // Clears ncurses and returns back to previous state
  return 0;  // Gives successful completion code
}