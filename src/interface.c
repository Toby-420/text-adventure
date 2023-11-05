#include <stdbool.h>
#include <curses.h>
#include "../include/defines.h"

#define FRONT_ROOM 0
#define STUDY 1
#define HIDDEN_ROOM 2
#define FR_PASSAGE 3

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
  mvwprintw(window->bar, 0, 0, "Simple commands are: view, move, open, name, take, save, load, help, exit");
  wrefresh(window->bar);
  refresh();
}

void loadText(struct Windows *window, struct System *system, struct MainCharacter *character, struct WindowText *text, bool setupCheck) {
  if (setupCheck) {
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
  mvwprintw(window->bar, 0, 0, "Commands are: view, move, open, take, save, clear, help, exit");

  if (system->keyIsInventory) {
    mvwprintw(window->inventory, 2, 1, "- Old, but shiny key");
  }

  if (system->davidIsInventory) {
    mvwprintw(window->inventory, 3, 1, "- David's lost treasure");
  }

  if (system->hiddenRoomTreasure) {
    mvwprintw(window->inventory, 4, 1, "- Treasure from the hidden room");
  }

  if (system->passageTreasure) {
    mvwprintw(window->inventory, 5, 1, "- Treasure from the passage");
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
	  if (!system->davidIsVisible) {
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