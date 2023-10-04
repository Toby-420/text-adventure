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
#include <time.h>
#include <windows.h>
#include "../include/miniaudio.h"
#include "../include/movement.h"
#include "../include/structs.h"
#include "../include/interface.h"
#include "../include/interaction.h"
#include "../include/system.h"
#include "../include/files.h"

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

int main(int argc, char *argv[]) {
  HWND consoleWindow = GetConsoleWindow();
  ShowWindow(consoleWindow, SW_MAXIMIZE);

  SetConsoleTitle("The Room of the Locked Door");

  struct MainCharacter character;
  struct Windows window;
  struct System system;
  struct WindowText text;

  ma_decoder decoder;
  ma_device_config deviceConfig;
  ma_device device;
  ma_engine engine;
  ma_engine_init(NULL, &engine);
  
  const char audioFile[] = "assets/audio/track0.mp3";

  if (ma_decoder_init_file(audioFile, NULL, &decoder) != MA_SUCCESS) {
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
  bool insulting = false;
  int currentSelection = 0;
  int chosenOption = -1;
  system.exitFlag = false;

  system.nameAsked = false;
  system.keyIsVisible = false;
  system.keyIsInventory = false;
  system.bookcaseIsLoose = false;
  system.davidIsVisible = false;
  system.davidIsInventory = false;
  system.pictureIsOpen = false;
  system.hiddenRoomTreasure = false;
  system.passageTreasure = false;
  system.soundPlayed = false;
  character.locationNumber = 0;

  strcpy(character.location, "front room");

  FILE *historyFile = fopen("text\\history.txt", "w");

  text.asciiFrontRoomContents = readFileContents("assets\\ascii\\frontRoom.txt");
  text.textFrontRoomContents = readFileContents("assets\\text\\frontRoom.txt");
  text.asciiStudyContents = readFileContents("assets\\ascii\\study.txt");
  text.textStudyContents = readFileContents("assets\\text\\study.txt");
  text.asciiChestContents = readFileContents("assets\\ascii\\chest.txt");
  text.asciiChestOpenContents = readFileContents("assets\\ascii\\chestOpen.txt");
  text.textHiddenRoomContents = readFileContents("assets\\text\\hiddenRoom.txt");
  text.asciiSkullContents = readFileContents("assets\\ascii\\skull.txt");
  text.asciiPassageContents = readFileContents("assets\\ascii\\passage.txt");
  text.textPassageContents = readFileContents("assets\\text\\passage.txt");
  text.helpFileContents = readFileContents("assets\\text\\help.txt");

  if (argc > 1 && strcmp(argv[1], "load") == 0) {
    loadGame(&system, &character);
    loadText(&window, &system, &character, &text, true);
  } else if (argc > 1 && strcmp(argv[1], "load") && stringIsSimilar(argv[1], "load", 2)) {
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
      inputRow++;

      // if (strcmp(character.name, "YBSFCFS") == 0) {
      // character.overpowered = true;
      // }

      firstToUpper(character.namehold);
	  
	  mvwprintw(stdscr, inputRow + 1, 2, "Choose a response mode:");
      while (chosenOption == -1) {
        for (int i = 0; i < 4; i++) {
          if (i == currentSelection) {
            attron(A_UNDERLINE);
          }

          if (i == 0) {
            mvprintw(i + 6, 2, "Normal");
          } else if (i == 1) {
            mvprintw(i + 6, 2, "Insulting");
          }
          attroff(A_UNDERLINE);
        }

        int ch = getch();

        switch (ch) {
          case KEY_UP:
            currentSelection = (currentSelection + 1) % 2;
            break;
          case KEY_DOWN:
            currentSelection = (currentSelection + 1) % 2;
            break;
          case '\n':
            if (currentSelection == 0) {
			  insulting = false;
              chosenOption = 1;
            } else if (currentSelection == 1) {
			  insulting = true;
              chosenOption = 2;
            } 

            break;
          default:
            break;
        }
      }
      currentSelection = 0;
      chosenOption = -1;

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
		SetConsoleTitle("The Room of the Locked Door - Front Room");
	    break;
  	  case 1:
  	    strcpy(character.location, "study");
		SetConsoleTitle("The Room of the Locked Door - Study");
	    break;
	  case 2:
	    strcpy(character.location, "hidden");
		SetConsoleTitle("The Room of the Locked Door - Hidden Room");
	    break;
	  case 3:
	    strcpy(character.location, "passage");
		SetConsoleTitle("The Room of the Locked Door - Hidden Passage");
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
      inputRow = handleMoveCommand(inputRow, mainInput, parameters, &character, &system, &window, &text, &engine, insulting);
      inputRow += 2;
    } else if (stringIsSimilar(mainInput, "open", 1) || stringIsSimilar(mainInput, "shift", 1)) {
      inputRow = handleOpenCommand(inputRow, mainInput, parameters, &character, &system, &window, &text, &engine);
      inputRow += 2;
    } else if (stringIsSimilar(mainInput, "take", 1) || stringIsSimilar(mainInput, "pick", 1)) {
      inputRow = handleTakeCommand(inputRow, mainInput, parameters, &character, &system, &window, &text, &engine);
      inputRow += 2;
    } else if (strcmp(mainInput, "cory-nonce") == 0) {
      ma_engine_play_sound(&engine, "assets/audio/cory-nonce.mp3", NULL);
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
    } else {
      loadText(&window, &system, &character, &text, false);
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