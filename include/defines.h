#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>
#include <curses.h>

typedef enum { FRONT_ROOM, STUDY, HIDDEN_ROOM, FR_PASSAGE, NUM_ROOMS } ROOMS;

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
  bool davidIsInventory;
  bool davidIsVisible;
  bool bookcaseIsLoose;
  bool pictureIsOpen;
  bool hiddenRoomTreasure;
  bool passageTreasure;
};

struct MainCharacter {
  int locationNumber;
  char location[22];
  char name[22];
  char namehold[22];
  // bool overpowered;
};

typedef struct {
	const char asciiArtDefinitions[6][30];
	const char roomTextDefinitions[5][30];
	const char audioDefinitions[7][30];
	const char randomMovementAudioDefinitions[4][30];
} AssetDefinitions;

#endif