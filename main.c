#define MINIAUDIO_IMPLEMENTATION
#define MIDI_FILE "audio/track1.mid"
#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <windows.h>
#include "portmidi.h"
#include "miniaudio.h"


#define player_health 3
#define enemy_low_health 1
#define enemy_mid_health 2
#define enemy_high_health 3
#define enemy_very_high_health 5
#define MAX_HISTORY_ITEMS 1000
#define MAX_COMMAND_LENGTH 20

/*
Compiled with GNU Compiler Collection (MinGW) on Windows 10 with command 'gcc -o play main.c icon/icon.o -lncurses -lwinmm'
Do I wish this was Linux-based? Yes
Can I make a Linux-compatible release? Yes, when I have finished the main version of the game

I have a computer running Arch Linux (i use arch btw) and I can make a release for that, but I have to rewrite quite
a bit of this for it to work which is why I haven't yet
*/

int setup_ui(WINDOW * inventory, WINDOW *bar, WINDOW * photo, WINDOW * compass, WINDOW * roomdescription) {
	clear();
	refresh();
	wborder(bar, '|', '|', '-', '-', '+', '+', '+', '+'); // Add border
	box(inventory, 0, 0);
	wrefresh(inventory);
	box(photo, 0, 0);
	wrefresh(photo);
	refresh();
	box(compass, 0, 0);
	wrefresh(compass);
	refresh();
	box(roomdescription, 0, 0);
	wrefresh(roomdescription);
	refresh();
	mvwprintw(bar, 0, 0, "Commands are: view, move, open, take, save, room, help, exit");
	wrefresh(bar);
	refresh();
}

int save_game(int input_row, char name[20], char location[255], bool key_visibility, bool key_inventory, bool bookcases, bool chestloose, bool david, bool pictureopen, bool hiddentreasure, bool passagetreasure, int key_visibility_int, int key_inventory_int, int bookcases_int, int chestloose_int, int david_int, int pictureopen_int, int hiddentreasure_int, int passagetreasure_int) {
	FILE * fp = fopen("text\\gamesave.txt", "w");
	mvwprintw(stdscr, input_row + 1, 2, "Saving game data to gamesave.txt");
	key_visibility_int = key_visibility; // Set the visibility of the key in the room to false (since the drawer is closed by default)
	key_inventory_int = key_inventory; // Set the status of the key to not in inventory (since it is in the closed drawer)
	bookcases_int = bookcases; // Set the status of the loose bookcase to not found (the player has to find the loose bookcase before opening it)
	chestloose_int = chestloose; // Set the status of the chest to fully closed (so the player has to open it twice)
	david_int = david; // Set david's treasure in the inventory to false
	pictureopen_int = pictureopen; // Set frontroom picture to be closed (passage)
	hiddentreasure_int = hiddentreasure;
	passagetreasure_int = passagetreasure;
	fprintf(fp, "%s %s %d %d %d %d %d %d %d %d", name, location, key_visibility, key_inventory, bookcases, chestloose, david, pictureopen, hiddentreasure, passagetreasure);
	fclose(fp);
}

int load_game(int input_row, char name[20], char location[255], bool key_visibility, bool key_inventory, bool bookcases, bool chestloose, bool david, bool pictureopen, bool hiddentreasure, bool passagetreasure, int key_visibility_int, int key_inventory_int, int bookcases_int, int chestloose_int, int david_int, int pictureopen_int, int hiddentreasure_int, int passagetreasure_int) {
	FILE * fp = fopen("text\\gamesave.txt", "r");
	mvwprintw(stdscr, input_row + 1, 2, "Loading game data from gamesave.txt");
	fscanf(fp, "%s %s %d %d %d %d %d %d %d %d", name, location, & key_visibility_int, & key_inventory_int, & bookcases_int, & chestloose_int, & david_int, & pictureopen_int, & hiddentreasure_int, & passagetreasure_int);
	fclose(fp);
	key_visibility = (key_visibility_int == 0) ? false : true;
	key_inventory = (key_inventory_int == 0) ? false : true;
	bookcases = (bookcases_int == 0) ? false : true;
	chestloose = (chestloose_int == 0) ? false : true;
	david = (david_int == 0) ? false : true;
	pictureopen = (pictureopen_int == 0) ? false : true;
	hiddentreasure = (hiddentreasure_int == 0) ? false : true;
	passagetreasure = (passagetreasure_int == 0) ? false : true;
	return name, location, key_visibility, key_inventory, bookcases, chestloose, david, pictureopen, hiddentreasure, passagetreasure;
}


int main(int argc, char *argv[]) {

  ma_result result;
  ma_engine engine;
  ma_sound track;
  ma_engine_init(NULL, & engine);

  HWND consoleWindow = GetConsoleWindow(); // Get handle to console window
  ShowWindow(consoleWindow, SW_MAXIMIZE); // Maximize console window

  initscr(); // Start ncurses
  cbreak(); // Allow user input
  keypad(stdscr, TRUE); // Enable arrow keys
  curs_set(1); // Set cursor invisible

  WINDOW * bar = newwin(1, COLS, LINES - 1, 0); // Make new window for bar
  wborder(bar, '|', '|', '-', '-', '+', '+', '+', '+'); // Add border

  WINDOW * inventory = newwin(LINES / 3 + 3, COLS / 4, 1, COLS * 6 / 8); // Make new window for inventory
  box(inventory, 0, 0); // Add box around inventory

  WINDOW * photo = newwin(27, 45, LINES - 28, COLS - 85); // Make new window for player's view
  box(photo, 0, 0); // Add a box around it

  WINDOW * roomdescription = newwin(27, 38, LINES - 28, COLS - 39); // Make a window for description
  box(roomdescription, 0, 0); // make a box around it

  WINDOW * compass = newwin(9, 17, 1, COLS * 6 / 8 - 19); // Make a window for the compass
  box(compass, 0, 0); // Make a box around compass

  mvwprintw(roomdescription, 1, 2, "ROOM DESCRIPTION:"); // Add titles to all the boxes
  mvwprintw(inventory, 1, 2, "INVENTORY:");
  mvwprintw(photo, 1, 2, "PLAYER VIEW:");
  mvwprintw(compass, 1, 2, "COMPASS:");
  mvwprintw(compass, 4, 8, "|"); // Add compass arms
  mvwprintw(compass, 6, 8, "|");
  mvwprintw(compass, 5, 6, "-- --");
  mvwprintw(compass, 5, 8, "O"); // Add center of compass
  mvwprintw(bar, 0, 0, "Commands are: view, move, open, take, save, room, help, exit"); // Add text to bar at bottom of screen
  wrefresh(bar); // Display the windows
  wrefresh(photo);
  wrefresh(inventory);
  wrefresh(compass);
  wrefresh(roomdescription);
  refresh(); // Display the result to the screen

  char location[255]; // Make a character array for the location
  char input[255]; // Declare a character array to hold user input
  char * str = NULL;
  char main_input[6]; // Declare an array for the main command (e.g. move, view, take, etc...)
  char params[10]; // Declare an array for parameters such as key and drawer
  char * token; // Make a pointer character for the tokens later on
  char name[20];

  signed int input_row = 2; // Keep track of the row for input
  int len = 0;
  int key_visibility_int; // Set the visibility of the key in the room to false (since the drawer is closed by default)
  int key_inventory_int; // Set the status of the key to not in inventory (since it is in the closed drawer)
  int bookcases_int; // Set the status of the loose bookcase to not found (the player has to find the loose bookcase before opening it)
  int chestloose_int; // Set the status of the chest to fully closed (so the player has to open it twice)
  int david_int; // Set david's treasure in the inventory to false
  int pictureopen_int; // Set frontroom picture to be closed (passage)
  int hiddentreasure_int;
  int passagetreasure_int;

  bool nameask = false;
  bool key_visibility = false; // Set the visibility of the key in the room to false (since the drawer is closed by default)
  bool key_inventory = false; // Set the status of the key to not in inventory (since it is in the closed drawer)
  bool bookcases = false; // Set the status of the loose bookcase to not found (the player has to find the loose bookcase before opening it)
  bool chestloose = false; // Set the status of the chest to fully closed (so the player has to open it twice)
  bool david = false; // Set david's treasure in the inventory to false
  bool pictureopen = false; // Set frontroom picture to be closed (passage)
  bool hiddentreasure = false;
  bool passagetreasure = false;
  bool soundplayed = false;


  strcpy(location, "frontroom"); // Set location to be frontroom (the starting room)

  FILE * history_file = fopen("text\\history.txt", "w"); // Open file for history

  while (1) { // Continue looping until user quits

    if (nameask == false) {
	  mvwprintw(stdscr, input_row, 2, "The Room of the Locked Door");
	  input_row++;
	  mvwprintw(stdscr, input_row, 2, "What do you want to be called? ");
      getstr(name);
      setup_ui(inventory, bar,  photo, compass, roomdescription);

      int n = 0;
      while (name[n]) {
        name[n] = toupper(name[n]);
        n++;
      }
      mvwprintw(stdscr, input_row - 1, 0, "\t\t\t\tWelcome to The Room of the Locked Door"); // Print starting things
      input_row++; // Increment input_row by 1
      mvwprintw(stdscr, input_row - 1, 0, "\t\t\t\t\tESCAPE OR DIE");
      input_row++;
      mvwprintw(stdscr, input_row - 1, 0, "\t*or type exit if you're a useless roadman who can't type and think at the same time*");
      input_row += 2; // Increment input_row by 2
      mvwprintw(stdscr, input_row - 1, 0, "You find yourself in an ornate and spacious front room with a window and a picture");
      input_row++;
      mvwprintw(stdscr, input_row - 1, 0, "There is a door that leads east and one that leads west");
      input_row++;
      nameask = true;
    }
    if (soundplayed == false) {
      PlaySound("audio\\track1.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); ///////////////////////////////////////////////////////////////////////// use sdl
	   // Start the playback thread and wait for it to finish
      soundplayed = true;
    }
    mvwprintw(stdscr, input_row, 0, "> [%s] ", location); // Put a terminal-like prompt with the current location at the top of the screen

    if (key_inventory == true) {
      mvwprintw(inventory, 2, 1, "- Old, but shiny key"); // Add the key to the inventory at line 1
      wrefresh(inventory);
    } else {
      mvwprintw(inventory, 2, 1, "                    "); // Clear the key from inventory (not used yet but might later)
      wrefresh(inventory);
    }

    if (david == true) {
      mvwprintw(inventory, 3, 1, "- David's lost treasure"); // Add david to the inventory at line 3
      wrefresh(inventory);
    } else {
      mvwprintw(inventory, 3, 1, "                       "); // Clear from inventory
      wrefresh(inventory);
    }

    if (hiddentreasure == true) {
      mvwprintw(inventory, 4, 1, "- Treasure from the picture"); // Add david to the inventory at line 4
      wrefresh(inventory);
    } else {
      mvwprintw(inventory, 4, 1, "                       "); // Clear from inventory
      wrefresh(inventory);
    }
    if (passagetreasure == true) {
      mvwprintw(inventory, 5, 1, "- Treasure from the passage"); // Add david to the inventory at line 5
      wrefresh(inventory);
    } else {
      mvwprintw(inventory, 5, 1, "                       "); // Clear from inventory
      wrefresh(inventory);
    }

    if (strcmp(location, "frontroom") == 0) {
      mvwprintw(compass, 5, 2, "WEST"); // Add possible directions onto the compass
      mvwprintw(compass, 5, 11, "EAST ");
      mvwprintw(compass, 3, 6, "     ");

      if (pictureopen == true) {
        mvwprintw(compass, 7, 6, "SOUTH*");
      } else {

        mvwprintw(compass, 7, 6, "      ");
      }

      wrefresh(compass);
      mvwprintw(photo, 3, 2, "        |                 |");
      mvwprintw(photo, 4, 2, "        |                 |");
      mvwprintw(photo, 5, 2, "        |                 |  ");
      mvwprintw(photo, 6, 2, "     /| |                 | |\\");
      mvwprintw(photo, 7, 2, "    / | |                 | | \\");
      mvwprintw(photo, 8, 2, "   //|| |      _____      | |  \\");
      mvwprintw(photo, 9, 2, "  // || |     |  o  |     | |   \\");
      mvwprintw(photo, 10, 2, " ||  || |     | -H- |     | |   |");
      mvwprintw(photo, 11, 2, " ||  || |     |  X  |     | |   |");
      mvwprintw(photo, 12, 2, " ||  || |     | / \\ |     | |   |");
      mvwprintw(photo, 13, 2, " ||  || |     |_____|     | |   |");
      mvwprintw(photo, 14, 2, " ||  || |                 | |   |"); // Add the ASCII art in the PLAYER VIEW box (photo window)
      mvwprintw(photo, 15, 2, " ||o || |_________________| |  o|");
      mvwprintw(photo, 16, 2, " ||  || /                 \\ |   |   ");
      mvwprintw(photo, 17, 2, " ||  ||/                   \\|   |   ");
      mvwprintw(photo, 18, 2, " ||  |/                     \\   |   ");
      mvwprintw(photo, 19, 2, " || //                       \\  |   ");
      mvwprintw(photo, 20, 2, " ||//                         \\ |   ");
      mvwprintw(photo, 21, 2, " | /                           \\|   ");
      mvwprintw(photo, 22, 2, " |/                             \\   ");
      mvwprintw(photo, 23, 2, " /                               \\   ");
      wrefresh(photo);
      mvwprintw(roomdescription, 3, 2, "This is an ornate, spacious front");
      mvwprintw(roomdescription, 4, 2, "room with a large window.         "); // Add description about front room into the description box
      mvwprintw(roomdescription, 5, 2, "Two doors are here with one");
      mvwprintw(roomdescription, 6, 2, "leading east, the other west.   ");
      mvwprintw(roomdescription, 7, 2, "A picture hangs on the wall and");
      mvwprintw(roomdescription, 8, 2, "makes the place seem very pretty");
      mvwprintw(roomdescription, 9, 2, "until you see the rest of the   ");
      mvwprintw(roomdescription, 10, 2, "room which is quite run-down");
      wrefresh(roomdescription);

    } else if (strcmp(location, "study") == 0) {
      if (bookcases == false) { // If the secret room has been discovered
        mvwprintw(compass, 5, 11, "     "); // Cover up EAST as a direction
      } else {
        mvwprintw(compass, 5, 11, "EAST*"); // Write EAST onto the compass

      }
      mvwprintw(compass, 5, 2, "WEST"); // Add possible direction onto compass
      mvwprintw(compass, 3, 6, "     ");
      mvwprintw(compass, 7, 6, "      ");
      mvwprintw(compass, 3, 6, "     ");

      wrefresh(compass);
      mvwprintw(photo, 3, 2, "        |                 |");
      mvwprintw(photo, 4, 2, "        |                 |   ");
      mvwprintw(photo, 5, 2, "        |                 |      ");
      mvwprintw(photo, 6, 2, "     /| |                 |     ");
      mvwprintw(photo, 7, 2, "    / | |                 |       ");
      mvwprintw(photo, 8, 2, "   //|| |     _______     |       ");
      mvwprintw(photo, 9, 2, "  // || |    |ooo    |    |       ");
      mvwprintw(photo, 10, 2, " ||  || |    | I  n  |    |       ");
      mvwprintw(photo, 11, 2, " ||  || |    |_______|    |       ");
      mvwprintw(photo, 12, 2, " ||  || |    _________    |        ");
      mvwprintw(photo, 13, 2, " ||  || |   /         \\   ||\\ \\     ");
      mvwprintw(photo, 14, 2, " ||  || |  /___________\\  |||\\ \\     ");
      mvwprintw(photo, 15, 2, " ||o || |__|------|----|__||||\\ \\      ");
      mvwprintw(photo, 16, 2, " ||  || /  |      |---o|  \\|\\||\\_\\    "); // Add the ASCII art in the PLAYER VIEW box (photo window)
      mvwprintw(photo, 17, 2, " ||  ||/   |      |----|   ||\\|| |    ");
      mvwprintw(photo, 18, 2, " ||  |/                    |\\|\\| |     ");
      mvwprintw(photo, 19, 2, " || //                     \\|\\|| |    ");
      mvwprintw(photo, 20, 2, " ||//                       \\|\\| |    ");
      mvwprintw(photo, 21, 2, " | /                         \\|| |    ");
      mvwprintw(photo, 22, 2, " |/                           \\| |    ");
      mvwprintw(photo, 23, 2, " /                             |_|\\    ");
      wrefresh(photo);
      mvwprintw(roomdescription, 3, 2, "You look around and see a small,  ");
      mvwprintw(roomdescription, 4, 2, "comfy room with bookcases around");
      mvwprintw(roomdescription, 5, 2, "the walls.                       ");
      mvwprintw(roomdescription, 6, 2, "There is a broken window and a"); // Add description about front room into the description box
      mvwprintw(roomdescription, 7, 2, "whose drawer has seen better.  ");
      mvwprintw(roomdescription, 8, 2, "The window is quite drafty and   ");
      mvwprintw(roomdescription, 9, 2, "bland with a small wooden frame");
      mvwprintw(roomdescription, 10, 2, "It looks like a study.         ");
      if (bookcases == true) {
        mvwprintw(roomdescription, 11, 2, "The bookcases also move it seems");
      } else {
        mvwprintw(roomdescription, 11, 2, "                               ");
      }

      wrefresh(roomdescription);

    } else if (strcmp(location, "picture") == 0) {
      mvwprintw(compass, 5, 11, "     "); // Write EAST onto the compass
      mvwprintw(compass, 5, 2, "    "); // Add possible direction onto compass
      mvwprintw(compass, 3, 6, "NORTH");

      mvwprintw(compass, 7, 6, "      ");
      wrefresh(compass);
      mvwprintw(photo, 3, 2, "        |                 |");
      mvwprintw(photo, 4, 2, "        |                 |   ");
      mvwprintw(photo, 5, 2, "        |                 |      ");
      mvwprintw(photo, 6, 2, "     /| |                 |     ");
      mvwprintw(photo, 7, 2, "    / | |                 |       ");
      mvwprintw(photo, 8, 2, "   //|| |     _______     |       ");
      mvwprintw(photo, 9, 2, "  // || |    |ooo    |    |       ");
      mvwprintw(photo, 10, 2, " ||  || |    | I  n  |    |       ");
      mvwprintw(photo, 11, 2, " ||  || |    |_______|    |       ");
      mvwprintw(photo, 12, 2, " ||  || |    _________    |        ");
      mvwprintw(photo, 13, 2, " ||  || |   /         \\   ||\\ \\     ");
      mvwprintw(photo, 14, 2, " ||  || |  /___________\\  |||\\ \\     ");
      mvwprintw(photo, 15, 2, " ||o || |__|------|----|__||||\\ \\      ");
      mvwprintw(photo, 16, 2, " ||  || /  |      |---o|  \\|\\||\\_\\    "); // Add the ASCII art in the PLAYER VIEW box (photo window)
      mvwprintw(photo, 17, 2, " ||  ||/   |      |----|   ||\\|| |    ");
      mvwprintw(photo, 18, 2, " ||  |/                    |\\|\\| |     ");
      mvwprintw(photo, 19, 2, " || //                     \\|\\|| |    ");
      mvwprintw(photo, 20, 2, " ||//                       \\|\\| |    ");
      mvwprintw(photo, 21, 2, " | /                         \\|| |    ");
      mvwprintw(photo, 22, 2, " |/                           \\| |    ");
      mvwprintw(photo, 23, 2, " /                             |_|\\    ");
      wrefresh(photo);
      mvwprintw(roomdescription, 3, 2, "You look around and see a small,  ");
      mvwprintw(roomdescription, 4, 2, "comfy room with bookcases around");
      mvwprintw(roomdescription, 5, 2, "the walls.                       ");
      mvwprintw(roomdescription, 6, 2, "There is a broken window and a"); // Add description about front room into the description box
      mvwprintw(roomdescription, 7, 2, "whose drawer has seen better.  ");
      mvwprintw(roomdescription, 8, 2, "The window is quite drafty and   ");
      mvwprintw(roomdescription, 9, 2, "bland with a small wooden frame");
      mvwprintw(roomdescription, 10, 2, "It looks like a study.         ");
      if (bookcases == true) {
        mvwprintw(roomdescription, 11, 2, "The bookcases also move it seems");
      } else {
        mvwprintw(roomdescription, 11, 2, "                               ");
      }

      wrefresh(roomdescription);

    } else if (strcmp(location, "hidden") == 0) {
      if (david == false) {

        mvwprintw(compass, 5, 11, "     ");
        mvwprintw(compass, 5, 2, "WEST");

        wrefresh(compass);
        mvwprintw(photo, 3, 2, "                                      ");
        mvwprintw(photo, 4, 2, "                                      ");
        mvwprintw(photo, 5, 2, "                                      ");
        mvwprintw(photo, 6, 2, "                                      ");
        mvwprintw(photo, 7, 2, "         _____________                ");
        mvwprintw(photo, 8, 2, "        /-------------\\               ");
        mvwprintw(photo, 9, 2, "       /_______________\\              ");
        mvwprintw(photo, 10, 2, "       |------| |------|              ");
        mvwprintw(photo, 11, 2, "       |------|?|------|              ");
        mvwprintw(photo, 12, 2, "       |------\\-/------|              ");
        mvwprintw(photo, 13, 2, "       |---------------|              ");
        mvwprintw(photo, 14, 2, "       |_______________|              ");
        mvwprintw(photo, 15, 2, "                                      ");
        mvwprintw(photo, 16, 2, "                                      ");
        mvwprintw(photo, 17, 2, "                                      ");
        mvwprintw(photo, 18, 2, "                                      ");
        mvwprintw(photo, 19, 2, "                                      ");
        mvwprintw(photo, 20, 2, "                                      ");
        mvwprintw(photo, 21, 2, "                                      ");
        mvwprintw(photo, 22, 2, "                                      ");
        mvwprintw(photo, 23, 2, "                                      ");
        wrefresh(photo);
      } else {
        mvwprintw(compass, 5, 11, "      ");
        mvwprintw(compass, 5, 2, "WEST ");

        wrefresh(compass);
        mvwprintw(photo, 3, 2, "          ___________                   ");
        mvwprintw(photo, 4, 2, "         |-----------|                  ");
        mvwprintw(photo, 5, 2, "         |-----------|                  ");
        mvwprintw(photo, 6, 2, "         |-----------|                  ");
        mvwprintw(photo, 7, 2, "         |___________|                 ");
        mvwprintw(photo, 8, 2, "        /#############\\               ");
        mvwprintw(photo, 9, 2, "       /_______________\\              ");
        mvwprintw(photo, 10, 2, "       |---------------|              ");
        mvwprintw(photo, 11, 2, "       |-------?-------|              ");
        mvwprintw(photo, 12, 2, "       |---------------|              ");
        mvwprintw(photo, 13, 2, "       |---------------|              ");
        mvwprintw(photo, 14, 2, "       |_______________|              ");
        mvwprintw(photo, 15, 2, "                                      ");
        mvwprintw(photo, 16, 2, "                                      ");
        mvwprintw(photo, 17, 2, "                                      ");
        mvwprintw(photo, 18, 2, "                                      ");
        mvwprintw(photo, 19, 2, "                                      ");
        mvwprintw(photo, 20, 2, "                                      ");
        mvwprintw(photo, 21, 2, "                                      ");
        mvwprintw(photo, 22, 2, "                                      ");
        mvwprintw(photo, 23, 2, "                                      ");
        wrefresh(photo);

      }

      mvwprintw(roomdescription, 3, 2, "Shhhh, this is a hidden room.     ");
      mvwprintw(roomdescription, 4, 2, "It is very small with the only    ");
      mvwprintw(roomdescription, 5, 2, "remarkable thing being a chest in ");
      mvwprintw(roomdescription, 6, 2, "the center of it.                 ");
      mvwprintw(roomdescription, 7, 2, "There isn't much else.           ");
      mvwprintw(roomdescription, 8, 2, "                                 ");
      mvwprintw(roomdescription, 9, 2, "                                  ");
      mvwprintw(roomdescription, 10, 2, "                                 ");
      mvwprintw(roomdescription, 11, 2, "                                 ");
      wrefresh(roomdescription);

    } else {
      mvwprintw(compass, 5, 2, "      ");
      mvwprintw(compass, 5, 11, "      ");
      wrefresh(compass);

    }

    getstr(input); // Get a string of input from the user

    fprintf(history_file, "%s\n", input);

    token = strtok(input, " ");
    if (token != NULL) {
      strcpy(main_input, token);

      token = strtok(NULL, " ");
      if (token != NULL) {
        strcpy(params, token);
      }
    }

    int i;
    for (i = 0; main_input[i]; i++) {
      main_input[i] = tolower(main_input[i]);
    }
    int f;
    for (f = 0; params[f]; f++) {
      params[f] = tolower(params[f]);
    }

    if (strcmp(main_input, "exit") == 0) { // Check if main_input matches the word "exit"
      // Respond to the user
      goto exit;
    } else if (strcmp(main_input, "view") == 0 || strcmp(main_input, "look") == 0) {
      if (strcmp(params, "window") == 0) {
        if (strcmp(location, "frontroom") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "There is a large, stained glass window with pictures of a naked Jesus and sheep on it");
        } else if (strcmp(location, "study") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "A crusty, wooden window frame houses a small sheet of glass. It seems pretty drafty");
        } else if (strcmp(location, "hidden") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "A small, quite unremarkable room housing a chest");
        } else {
          mvwprintw(stdscr, input_row + 1, 2, "Location not found");
        }
      } else if (strcmp(params, "picture") == 0) {
        if (strcmp(location, "frontroom") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "A big, hand-painted picture of a guy sits on the wall");
          // include ascii art for window 'photo'
        } else if (strcmp(location, "study") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "There are no pictures in here. Only Zuul");
        } else {
          mvwprintw(stdscr, input_row + 1, 2, "Location not found");
        }
      } else if (strcmp(params, "key") == 0) {
        if (key_inventory == true || key_visibility == true) {
          mvwprintw(stdscr, input_row + 1, 2, "You see an old key which is still shiny. What is it here for?");
        } else {
          mvwprintw(stdscr, input_row + 1, 2, "Key what? Key procedures?");
        }
      } else if (strcmp(params, "treasure") == 0) {
        if (david == true) {
          mvwprintw(stdscr, input_row + 1, 2, "It looks like a small tin with something quite dense inside");
        } else {
          mvwprintw(stdscr, input_row + 1, 2, "You see no treasure in here");
        }
      } else if (strcmp(params, "bookcase") == 0 || strcmp(params, "bookcases") == 0) {
        if (strcmp(location, "frontroom") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "There are no bookcases in here");
        } else if (strcmp(location, "study") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "Beautifully crafted bookcases line the walls. One looks a little loose");
          bookcases = true;
        } else {
          mvwprintw(stdscr, input_row + 1, 2, "There are no bookcases");
        }
      } else if (strcmp(params, "room") == 0) {
        if (strcmp(location, "frontroom") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "A large and ornate, yet run-down front room with a window, picture and doors to the east and west");
        } else if (strcmp(location, "study") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "A somewhat good-looking room until you see the window and drawer which are slighty broken");
        } else if (strcmp(location, "hidden") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "A tiny room which only has a chest in");
        } else if (strcmp(location, "picture") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "A small room you found hidden behind a picture");
        } else if (strcmp(location, "passage") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "A little passageway that seemingly goes through quite a large area");

        } else {
          mvwprintw(stdscr, input_row + 1, 2, "There has been no description for this room yet");
        }
      } else {
        mvwprintw(stdscr, input_row + 1, 2, "There is nothing of interest here");
      }

      input_row += 2;
      if (input_row + 1 == LINES - 4 || input_row + 1 == LINES - 3 || input_row + 1 == LINES - 3) {
        input_row = 0;
        setup_ui(inventory, bar,  photo, compass, roomdescription);
      }

    } else if (strcmp(main_input, "cory-nonce") == 0) {

      input_row++;
      if (input_row + 1 == LINES - 4 || input_row + 1 == LINES - 3) {
        input_row = 0;
        setup_ui(inventory, bar,  photo, compass, roomdescription);
      }

    } else if (strcmp(main_input, "move") == 0 || strcmp(main_input, "walk") == 0 || strcmp(main_input, "run") == 0 || strcmp(main_input, "enter") == 0 || strcmp(main_input, "go") == 0) {

      if (strcmp(params, "east") == 0 || strcmp(params, "e") == 0) {

        if (strcmp(location, "frontroom") == 0) {
          input_row = 0;
          setup_ui(inventory, bar,  photo, compass, roomdescription);
          mvwprintw(stdscr, input_row, 0, "> [%s] %s %s", location, main_input, params);
          mvwprintw(stdscr, input_row + 1, 2, "You move into a small study. bookcases line the walls, along with a window overseeing the garden");
          input_row++;
          mvwprintw(stdscr, input_row + 1, 2, "There is a table with an old, solid oak drawer");
          strcpy(location, "study");
          wrefresh(compass);

        } else if (strcmp(location, "study") == 0 || strcmp(location, "hidden") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "You walk into a wall");
        } else {
          mvwprintw(stdscr, input_row + 1, 2, "Location not found");
        }
      } else if (strcmp(params, "passage") == 0 || strcmp(params, "picture") == 0) {
        if (strcmp(location, "hidden") == 0) {
          input_row = 0;
          setup_ui(inventory, bar,  photo, compass, roomdescription);
          mvwprintw(stdscr, input_row, 0, "> [%s] %s %s", location, main_input, params);
          mvwprintw(stdscr, input_row + 1, 2, "You move into the tight passage, previously blocked by the darkness");
          strcpy(location, "passage");
        } else if (strcmp(location, "frontroom") == 0) {
          if (pictureopen == true) {
            input_row = 0;
            setup_ui(inventory, bar,  photo, compass, roomdescription);
            mvwprintw(stdscr, input_row, 0, "> [%s] %s %s", location, main_input, params);
            mvwprintw(stdscr, input_row + 1, 2, "You squeeze behind the picture, and find yourself in a small room");
            strcpy(location, "picture");
          } else {
            mvwprintw(stdscr, input_row + 1, 2, "There is no %s to enter", params);
          }
        } else if (strcmp(location, "study") == 0) {

        } else {

        }
      } else if (strcmp(params, "bookcases") == 0 || (strcmp(params, "bookcase") == 0)) {

        if (strcmp(location, "frontroom") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "What bookcases...");
        } else if (strcmp(location, "study") == 0) {
          if (bookcases == true) {
            input_row = 0;
            setup_ui(inventory, bar,  photo, compass, roomdescription);
            mvwprintw(stdscr, input_row, 0, "> [%s] %s %s", location, main_input, params);
            mvwprintw(stdscr, input_row + 1, 2, "You slide behind the bookcases and find yourself in a very small room with a chest");
            strcpy(location, "hidden");
          } else {
            mvwprintw(stdscr, input_row + 1, 2, "There are no bookcases to move");
          }
        } else {
          mvwprintw(stdscr, input_row + 1, 2, "Location not found");
        }

      } else if (strcmp(params, "north") == 0) {
        if (strcmp(location, "study") == 0) {
          input_row = 0;
          setup_ui(inventory, bar,  photo, compass, roomdescription);
          mvwprintw(stdscr, input_row, 0, "> [%s] %s %s", location, main_input, params);
          mvwprintw(stdscr, input_row + 1, 2, "You fall out of the window and die");
          mvwprintw(stdscr, LINES - 2, 0, "EXITING PROGRAM");
          mvwprintw(photo, 3, 2, "             _..~~~~~~~.._           ");
          mvwprintw(photo, 4, 2, "         .:~'             '~:.       ");
          mvwprintw(photo, 5, 2, "       .:                     :.     ");
          mvwprintw(photo, 6, 2, "      :                         :    ");
          mvwprintw(photo, 7, 2, "     .'                         '.   ");
          mvwprintw(photo, 8, 2, "    .:                           :.  ");
          mvwprintw(photo, 9, 2, "    : :                         : :  ");
          mvwprintw(photo, 10, 2, "    | :   _____         _____   : |  ");
          mvwprintw(photo, 11, 2, "    |  '/~     ~   .   ~     ~\'  |  ");
          mvwprintw(photo, 12, 2, "    |  ~  .-~~~~~. | .~~~~~-.  ~  |  ");
          mvwprintw(photo, 13, 2, "     |   |        }:{        |   |   ");
          mvwprintw(photo, 14, 2, "     |   !       / | \\       !   |   ");
          mvwprintw(photo, 15, 2, "     .~   \\_..--' .^. '--.._/  ~.    ");
          mvwprintw(photo, 16, 2, "      |        :' /|\\ ':        |    ");
          mvwprintw(photo, 17, 2, "       \\~~.__     U^U     __.~~/     ");
          mvwprintw(photo, 18, 2, "        \\I \\#\\           /#/ I/      ");
          mvwprintw(photo, 19, 2, "         | |#~\\_________/~#| |       ");
          mvwprintw(photo, 20, 2, "         | |###|||_|_|||###| |       ");
          mvwprintw(photo, 21, 2, "         |  \\,#|||||||||#,/  |       ");
          mvwprintw(photo, 22, 2, "          \\   '~~~~~~~~~'   /        ");
          mvwprintw(photo, 23, 2, "           \\    .     .    /         ");
          mvwprintw(photo, 24, 2, "            \\.     ^     ./          ");
          mvwprintw(photo, 25, 2, "              '~~~~^~~~~'            ");
          wrefresh(photo);
          mvwprintw(roomdescription, 3, 2, "You carelessly walked into the  ");
          mvwprintw(roomdescription, 4, 2, "window and fell out, collapsing "); // Add description about front room into the description box
          mvwprintw(roomdescription, 5, 2, "onto the rose bush.             ");
          mvwprintw(roomdescription, 6, 2, "Well done. You managed to die   ");
          mvwprintw(roomdescription, 7, 2, "in a room without any enemies.  ");
          mvwprintw(roomdescription, 8, 2, "*HEY EVERYBODY! %s              ", name);
          mvwprintw(roomdescription, 9, 2, "MANAGED TO DIE WITHOUT ANY         ");
          mvwprintw(roomdescription, 10, 2, "ENEMIES! LET'S GIVE A SLOW CLAP* ");
          wrefresh(roomdescription);
          refresh(); // Refresh the screen
          goto exit;

        } else if (strcmp(location, "picture") == 0) {
          input_row = 0;
          setup_ui(inventory, bar,  photo, compass, roomdescription);
          mvwprintw(stdscr, input_row, 0, "> [%s] %s %s", location, main_input, params);
          mvwprintw(stdscr, input_row + 1, 2, "You move back into the front room");
          strcpy(location, "frontroom");
          wrefresh(compass);
        } else if (strcmp(location, "hidden") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "You headbutt the wall");
        } else if (strcmp(location, "frontroom") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "The window cannot be traveled through");
        } else {
          mvwprintw(stdscr, input_row + 1, 2, "Location not found");
        }
      } else if (strcmp(params, "west") == 0) {
        if (strcmp(location, "study") == 0) {
          input_row = 0;
          setup_ui(inventory, bar,  photo, compass, roomdescription);
          mvwprintw(stdscr, input_row, 0, "> [%s] %s %s", location, main_input, params);
          mvwprintw(stdscr, input_row + 1, 2, "You move back into the front room");
          strcpy(location, "frontroom");
          wrefresh(compass);
        } else if (strcmp(location, "hidden") == 0) {
          input_row = 0;
          setup_ui(inventory, bar,  photo, compass, roomdescription);
          mvwprintw(stdscr, input_row, 0, "> [%s] %s %s", location, main_input, params);
          mvwprintw(stdscr, input_row + 1, 2, "You move back into the study");
          strcpy(location, "study");
          wrefresh(compass);
        } else if (strcmp(location, "frontroom") == 0) {
          if (key_inventory == false) {
            mvwprintw(stdscr, input_row + 1, 2, "This door is locked. Maybe there is a key somewhere");
          } else {
            mvwprintw(stdscr, input_row + 1, 2, "Well done, you have completed 'The Room of the Locked Door'");
            if (david == true) {
              input_row++;
              mvwprintw(stdscr, input_row + 1, 2, "You even found David's old cake tin");
            }
            mvwprintw(stdscr, LINES - 2, 0, "EXITING PROGRAM");
            refresh(); // Refresh the screen
            sleep(5); // Wait for a second to allow the user to read the message
            delwin(bar); // Delete the bar
            endwin(); // Clear ncurses and return back to previous state
            return 0; // Exit the program with a successful completion code
          }
        } else {
          mvwprintw(stdscr, input_row + 1, 2, "Location not found");
        }

      } else {
        mvwprintw(stdscr, input_row + 1, 2, "You walk into a wall");
      }

      input_row += 2;
      if (input_row + 1 == LINES - 4 || input_row + 1 == LINES - 3) {
        input_row = 0;
        setup_ui(inventory, bar,  photo, compass, roomdescription);
      }

    } else if (strcmp(main_input, "open") == 0 || strcmp(main_input, "shift") == 0) {

      if (strcmp(location, "study") == 0) {
        if (strcmp(params, "drawer") == 0) {
          if (key_visibility == false) {
            ma_engine_play_sound( & engine, "audio/drawer.wav", NULL);
            mvwprintw(stdscr, input_row + 1, 2, "You open the old, dusty drawer whose handle is nearly broken and you find an old, but shiny key");
            key_visibility = true;
          } else {
            mvwprintw(stdscr, input_row + 1, 2, "There is nothing else in here. Except some Opal Fruit wrappers");
          }
        } else if (strcmp(params, "window") == 0) {
          ma_engine_play_sound( & engine, "audio/wind.wav", NULL);
          mvwprintw(stdscr, input_row + 1, 2, "You open the creaky window and smell a gust of stale air.");
          input_row++;
          mvwprintw(stdscr, input_row + 1, 2, "It seems like it has been closed for years. There is a very sharp looking");
          input_row++;
          mvwprintw(stdscr, input_row + 1, 2, "rose bush which looks like it would hurt. There is nothing of use. You close the window");
        } else if (strcmp(params, "door") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "There is no door here");
        } else if (strcmp(params, "key") == 0) {
          if (key_inventory == true) {
            mvwprintw(stdscr, input_row + 1, 2, "You try with all your might, but the key doesn't want to open");
          } else {
            mvwprintw(stdscr, input_row + 1, 2, "You do not have a key to open");
          }
        } else if (strcmp(params, "treasure") == 0) {
          if (david == true) {
            mvwprintw(stdscr, input_row + 1, 2, "You open the treasure... And it immediately slaps you in the face and closes");
          } else {
            mvwprintw(stdscr, input_row + 1, 2, "You have no treasure");
          }
        } else {
          mvwprintw(stdscr, input_row + 1, 2, "The %s is too difficult to move. It just won't budge", params);
        }
      } else if (strcmp(params, "bookcases") == 0 || (strcmp(params, "bookcase") == 0)) {

        if (strcmp(location, "frontroom") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "What bookcases...");
        } else if (strcmp(location, "study") == 0) {
          if (bookcases == true) {
            input_row = 0;
            setup_ui(inventory, bar,  photo, compass, roomdescription);
            mvwprintw(stdscr, input_row + 1, 2, "You slide behind the bookcases and find yourself in a very small room with a chest");
            strcpy(location, "hidden");
          } else {
            mvwprintw(stdscr, input_row + 1, 2, "There are no bookcases to move");
          }
        } else {
          mvwprintw(stdscr, input_row + 1, 2, "Location not found");
        }

      } else if (strcmp(location, "frontroom") == 0) {
        if (strcmp(params, "key") == 0) {
          if (key_inventory == true) {
            mvwprintw(stdscr, input_row + 1, 2, "You try with all your might, but the key doesn't want to open");
          } else {
            mvwprintw(stdscr, input_row + 1, 2, "You do not have a key to open");
          }
        } else if (strcmp(params, "treasure") == 0) {
          if (david == true) {
            mvwprintw(stdscr, input_row + 1, 2, "You open the treasure... And it immediately slaps you in the face and closes");
          } else {
            mvwprintw(stdscr, input_row + 1, 2, "You have no treasure");
          }
        } else if (strcmp(params, "picture") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "You slide the picture to the side, revealing a passage", params);
          pictureopen = true;
        } else {
          mvwprintw(stdscr, input_row + 1, 2, "You pull as hard as you can on the %s, but it just won't open", params);
        }
      } else if (strcmp(location, "hidden") == 0) {
        if (strcmp(params, "chest") == 0) {
          if (chestloose == true) {
            mvwprintw(stdscr, input_row + 1, 2, "You found the Lost Treasure of David Spindler", params);
            david = true;
          } else {
            mvwprintw(stdscr, input_row + 1, 2, "The chest is really stiff", params);
            chestloose = true;
          }
        } else if (strcmp(params, "door") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "Try walking through it");
        } else if (strcmp(params, "key") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "You try with all your might, but the key doesn't want to open");
        } else if (strcmp(params, "treasure") == 0) {
          mvwprintw(stdscr, input_row + 1, 2, "You open the treasure... And it immediately slaps you in the face and closes");
        } else {
          mvwprintw(stdscr, input_row + 1, 2, "There is literally nothing else in here", params);
        }
      } else {
        mvwprintw(stdscr, input_row + 1, 2, "Location not found");
      }

      input_row += 2;
      if (input_row + 1 == LINES - 4 || input_row + 1 == LINES - 3) {
        input_row = 0;
        setup_ui(inventory, bar,  photo, compass, roomdescription);
      }

    } else if (strcmp(main_input, "take") == 0 || strcmp(main_input, "pick") == 0) {

      if (strcmp(location, "study") == 0) {
        if (strcmp(params, "key") == 0) {
          if (key_visibility == true && key_inventory == false) {
            ma_engine_play_sound( & engine, "audio/sparkle.wav", NULL);
            mvwprintw(stdscr, input_row + 1, 2, "You pick up the old key");
            key_inventory = true;
          } else {
            mvwprintw(stdscr, input_row + 1, 2, "There is no key to pick up");
          }
        } else {
          mvwprintw(stdscr, input_row + 1, 2, "There is no %s to pick up", params);
        }

      } else if (strcmp(location, "frontroom") == 0) {
        mvwprintw(stdscr, input_row + 1, 2, "There is nothing to take");
      } else {
        mvwprintw(stdscr, input_row + 1, 2, "Location not found");
      }

      input_row += 2;
      if (input_row + 1 == LINES - 4 || input_row + 1 == LINES - 3) {
        input_row = 0;
        setup_ui(inventory, bar,  photo, compass, roomdescription);
      }

    } else if (strcmp(main_input, "help") == 0) { // Check of main_input matches help
      clear();
      printw("------HELP SCREEN------\nCommand help:\n\n\tview/look\n\t\tViews an object.\n\t\te.g. 'view picture' would view a picture on a wall.\n\tmove/walk/run\n\t\tAllows the game character to move in a direction specified by the player.\n\t\te.g. 'move west' would move west.\n\topen\n\t\tOpens an object.\n\t\te.g. 'open drawer' would open a drawer.\n\tsave\n\t\tSaves the game.\n\ttake/pick\n\t\tTakes an object.\n\t\te.g. 'take key' would take a key.\n\thelp\n\t\tOpens this help screen.\n\texit\n\t\tSimply exits the game.\n\n\nPress enter 3 times to exit this screen.\n");
      getstr(main_input); // idk why this is here but the program doesn't work when i take it away soooo...
      getchar(); // Waits until the user presses a key
      input_row = 0;
      setup_ui(inventory, bar,  photo, compass, roomdescription);

    } else if (strcmp(main_input, "save") == 0) {
		save_game(input_row, name, location, key_visibility, key_inventory, bookcases, chestloose, david, pictureopen, hiddentreasure, passagetreasure, key_visibility_int, key_inventory_int, bookcases_int, chestloose_int, david_int, pictureopen_int, hiddentreasure_int, passagetreasure_int);
		input_row += 2;
		if (input_row + 1 == LINES - 4 || input_row + 1 == LINES - 3) {
		input_row = 0;
		setup_ui(inventory, bar,  photo, compass, roomdescription);
      }

    } else if (strcmp(main_input, "load") == 0) {
      load_game(input_row, name, location, key_visibility, key_inventory, bookcases, chestloose, david, pictureopen, hiddentreasure, passagetreasure, key_visibility_int, key_inventory_int, bookcases_int, chestloose_int, david_int, pictureopen_int, hiddentreasure_int, passagetreasure_int);
      input_row = 0;
      setup_ui(inventory, bar,  photo, compass, roomdescription);
      input_row += 2;
      if (input_row + 1 == LINES - 4 || input_row + 1 == LINES - 3) {
        input_row = 0;
        setup_ui(inventory, bar,  photo, compass, roomdescription);
      }
    } else if (main_input[0] == '\0' || isspace(main_input[0])) {
      mvwprintw(stdscr, input_row + 1, 2, "I'm not psychic you know");
      input_row += 2;
      if (input_row + 1 == LINES - 4 || input_row + 1 == LINES - 3) {
        input_row = 0;
        setup_ui(inventory, bar,  photo, compass, roomdescription);
      }
    } else {
      mvwprintw(stdscr, input_row + 1, 2, "Try using one of the commands below", main_input); // Print the user main_input on the screen
      input_row += 2;
      if (input_row + 1 == LINES - 4 || input_row + 1 == LINES - 3) {
        input_row = 0;
        setup_ui(inventory, bar,  photo, compass, roomdescription);
      }
    }

    refresh(); // Refresh the screen
  }
  exit:
  free(str); // free the allocated memory
  delwin(bar); // Delete the bar
  delwin(compass);
  delwin(inventory);
  delwin(roomdescription);
  delwin(photo);
  endwin(); // Clears ncurses and returns back to previous state
  return 0; // Gives successful completion code
}