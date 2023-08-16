
	else if (strcmp(mainInput, "view") == 0 || strcmp(mainInput, "look") == 0) {
      if (strcmp(parameters, "window") == 0) {
        if (strcmp(character.location, "front room") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "There is a large, stained glass window with pictures of a naked Jesus and sheep on it");
        } else if (strcmp(character.location, "study") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "A crusty, wooden window frame houses a small sheet of glass. It seems pretty drafty");
        } else if (strcmp(character.location, "hidden") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "A small, quite unremarkable room housing a chest");
        } else {
          mvwprintw(stdscr, inputRow + 1, 2, "Location not found");
        }
      } else if (strcmp(parameters, "picture") == 0) {
        if (strcmp(character.location, "front room") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "A big, hand-painted picture of a guy sits on the wall");
          // include ascii art for window 'window.photo'
        } else if (strcmp(character.location, "study") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "There are no pictures in here. Only Zuul");
        } else {
          mvwprintw(stdscr, inputRow + 1, 2, "Location not found");
        }
      } else if (strcmp(parameters, "key") == 0) {
        if (system.keyIsInventory == true || system.keyIsVisible == true) {
          mvwprintw(stdscr, inputRow + 1, 2, "You see an old key which is still shiny. What is it here for?");
        } else {
          mvwprintw(stdscr, inputRow + 1, 2, "Key what? Key procedures?");
        }
      } else if (strcmp(parameters, "treasure") == 0) {
        if (system.davidTreasure == true) {
          mvwprintw(stdscr, inputRow + 1, 2, "It looks like a small tin with something quite dense inside");
        } else {
          mvwprintw(stdscr, inputRow + 1, 2, "You see no treasure in here");
        }
      } else if (strcmp(parameters, "bookcase") == 0 || strcmp(parameters, "bookcases") == 0) {
        if (strcmp(character.location, "front room") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "There are no bookcases in here");
        } else if (strcmp(character.location, "study") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "Beautifully crafted bookcases line the walls. One looks a little loose");
          system.bookcaseIsLoose = true;
        } else {
          mvwprintw(stdscr, inputRow + 1, 2, "There are no bookcases");
        }
      } else if (strcmp(parameters, "room") == 0) {
        if (strcmp(character.location, "front room") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "A large and ornate, yet run-down front room with a window, picture and doors to the east and west");
        } else if (strcmp(character.location, "study") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "A somewhat good-looking room until you see the window and drawer which are slighty broken");
        } else if (strcmp(character.location, "hidden") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "A tiny room which only has a chest in");
        } else if (strcmp(character.location, "picture") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "A small room you found hidden behind a picture");
        } else if (strcmp(character.location, "passage") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "A little passageway that seemingly goes through quite a large area");
        } else {
          mvwprintw(stdscr, inputRow + 1, 2, "There has been no description for this room yet");
        }
      } else {
        mvwprintw(stdscr, inputRow + 1, 2, "There is nothing of interest here");
      }

      inputRow += 2;

    } else if (strcmp(mainInput, "cory-nonce") == 0) {
      ma_engine_play_sound( & engine, "audio/cory-nonce.wav", NULL);

      inputRow++;

    }
	
	
	
	else if (strcmp(mainInput, "move") == 0 || strcmp(mainInput, "walk") == 0 || strcmp(mainInput, "run") == 0 || strcmp(mainInput, "enter") == 0 || strcmp(mainInput, "go") == 0) {

      if (strcmp(parameters, "east") == 0 || strcmp(parameters, "e") == 0) {

        if (strcmp(character.location, "front room") == 0) {
          inputRow = 0;
          setupUi(window);
          mvwprintw(stdscr, inputRow, 0, "> [%s] %s %s", character.location, mainInput, parameters);
          mvwprintw(stdscr, inputRow + 1, 2, "You move into a small study. Bookcases line the walls, along with a window overseeing the garden");
          inputRow++;
          mvwprintw(stdscr, inputRow + 1, 2, "There is a table with an old, solid oak drawer");
          strcpy(character.location, "study");
          wrefresh(window.compass);

        } else if (strcmp(character.location, "study") == 0 || strcmp(character.location, "hidden") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "You walk into a wall");
        } else {
          mvwprintw(stdscr, inputRow + 1, 2, "Location not found");
        }
      } else if (strcmp(character.location, "study") == 0) {
          if (system.bookcaseIsLoose == true) {
            inputRow = 0;
            setupUi(window);
            mvwprintw(stdscr, inputRow, 0, "> [%s] %s %s", character.location, mainInput, parameters);
            mvwprintw(stdscr, inputRow + 1, 2, "You slide behind the bookcases and find yourself in a very small room with a chest");
            strcpy(character.location, "hidden");
          } else {
            mvwprintw(stdscr, inputRow + 1, 2, "There are no bookcases to move");
          }
        } else if (strcmp(parameters, "passage") == 0 || strcmp(parameters, "picture") == 0) {
        if (strcmp(character.location, "hidden") == 0) {
          inputRow = 0;
          setupUi(window);
          mvwprintw(stdscr, inputRow, 0, "> [%s] %s %s", character.location, mainInput, parameters);
          mvwprintw(stdscr, inputRow + 1, 2, "You move into the tight passage, previously blocked by the darkness");
          strcpy(character.location, "passage");
        } else if (strcmp(character.location, "front room") == 0) {
          if (system.pictureIsOpen == true) {
            inputRow = 0;
            setupUi(window);
            mvwprintw(stdscr, inputRow, 0, "> [%s] %s %s", character.location, mainInput, parameters);
            mvwprintw(stdscr, inputRow + 1, 2, "You squeeze behind the picture, and find yourself in a small room");
            strcpy(character.location, "picture");
          } else {
            mvwprintw(stdscr, inputRow + 1, 2, "There is no %s to enter", parameters);
          }
        } else if (strcmp(character.location, "study") == 0) {

        } else {

        }
      } else if (strcmp(parameters, "bookcases") == 0 || (strcmp(parameters, "bookcase") == 0)) {

        if (strcmp(character.location, "front room") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "What bookcases...");
        } else if (strcmp(character.location, "study") == 0) {
          if (system.bookcaseIsLoose == true) {
            inputRow = 0;
            setupUi(window);
            mvwprintw(stdscr, inputRow, 0, "> [%s] %s %s", character.location, mainInput, parameters);
            mvwprintw(stdscr, inputRow + 1, 2, "You slide behind the bookcases and find yourself in a very small room with a chest");
            strcpy(character.location, "hidden");
          } else {
            mvwprintw(stdscr, inputRow + 1, 2, "There are no bookcases to move");
          }
        } else {
          mvwprintw(stdscr, inputRow + 1, 2, "Location not found");
        }

      } else if (strcmp(parameters, "north") == 0) {
        if (strcmp(character.location, "study") == 0) {
          inputRow = 0;
          setupUi(window);
          mvwprintw(stdscr, inputRow, 0, "> [%s] %s %s", character.location, mainInput, parameters);
          mvwprintw(stdscr, inputRow + 1, 2, "You fall out of the window and die");
          mvwprintw(stdscr, LINES - 2, 0, "EXITING PROGRAM");
          mvwprintw(window.photo, 3, 2, "             _..~~~~~~~.._           ");
          mvwprintw(window.photo, 4, 2, "         .:~'             '~:.       ");
          mvwprintw(window.photo, 5, 2, "       .:                     :.     ");
          mvwprintw(window.photo, 6, 2, "      :                         :    ");
          mvwprintw(window.photo, 7, 2, "     .'                         '.   ");
          mvwprintw(window.photo, 8, 2, "    .:                           :.  ");
          mvwprintw(window.photo, 9, 2, "    : :                         : :  ");
          mvwprintw(window.photo, 10, 2, "    | :   _____         _____   : |  ");
          mvwprintw(window.photo, 11, 2, "    |  '/~     ~   .   ~     ~\'  |  ");
          mvwprintw(window.photo, 12, 2, "    |  ~  .-~~~~~. | .~~~~~-.  ~  |  ");
          mvwprintw(window.photo, 13, 2, "     |   |        }:{        |   |   ");
          mvwprintw(window.photo, 14, 2, "     |   !       / | \\       !   |   ");
          mvwprintw(window.photo, 15, 2, "     .~   \\_..--' .^. '--.._/  ~.    ");
          mvwprintw(window.photo, 16, 2, "      |        :' /|\\ ':        |    ");
          mvwprintw(window.photo, 17, 2, "       \\~~.__     U^U     __.~~/     ");
          mvwprintw(window.photo, 18, 2, "        \\I \\#\\           /#/ I/      ");
          mvwprintw(window.photo, 19, 2, "         | |#~\\_________/~#| |       ");
          mvwprintw(window.photo, 20, 2, "         | |###|||_|_|||###| |       ");
          mvwprintw(window.photo, 21, 2, "         |  \\,#|||||||||#,/  |       ");
          mvwprintw(window.photo, 22, 2, "          \\   '~~~~~~~~~'   /        ");
          mvwprintw(window.photo, 23, 2, "           \\    .     .    /         ");
          mvwprintw(window.photo, 24, 2, "            \\.     ^     ./          ");
          mvwprintw(window.photo, 25, 2, "              '~~~~^~~~~'            ");
          wrefresh(window.photo);
          mvwprintw(window.roomdescription, 3, 2, "You carelessly walked into the  ");
          mvwprintw(window.roomdescription, 4, 2, "window and fell out, collapsing "); // Add description about front room into the description box
          mvwprintw(window.roomdescription, 5, 2, "onto the rose bush.             ");
          mvwprintw(window.roomdescription, 6, 2, "Well done. You managed to die   ");
          mvwprintw(window.roomdescription, 7, 2, "in a room without any enemies.  ");
          mvwprintw(window.roomdescription, 8, 2, "*HEY EVERYBODY! %s              ", character.namehold);
          mvwprintw(window.roomdescription, 9, 2, "MANAGED TO DIE WITHOUT ANY         ");
          mvwprintw(window.roomdescription, 10, 2, "ENEMIES! LET'S GIVE A SLOW CLAP* ");
          wrefresh(window.roomdescription);
          refresh(); // Refresh the screen
          
		  exitFlag = true;

        } else if (strcmp(character.location, "passage") == 0) {
          inputRow = 0;
          setupUi(window);
          mvwprintw(stdscr, inputRow, 0, "> [%s] %s %s", character.location, mainInput, parameters);
          mvwprintw(stdscr, inputRow + 1, 2, "You move back into the front room");
          strcpy(character.location, "front room");
          wrefresh(window.compass);
        } else if (strcmp(character.location, "hidden") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "You headbutt the wall");
        } else if (strcmp(character.location, "front room") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "The window cannot be traveled through");
        } else {
          mvwprintw(stdscr, inputRow + 1, 2, "Location not found");
        }
      } else if (strcmp(parameters, "west") == 0) {
        if (strcmp(character.location, "study") == 0) {
          inputRow = 0;
          setupUi(window);
          mvwprintw(stdscr, inputRow, 0, "> [%s] %s %s", character.location, mainInput, parameters);
          mvwprintw(stdscr, inputRow + 1, 2, "You move back into the front room");
          strcpy(character.location, "front room");
          wrefresh(window.compass);
        } else if (strcmp(character.location, "hidden") == 0) {
          inputRow = 0;
          setupUi(window);
          mvwprintw(stdscr, inputRow, 0, "> [%s] %s %s", character.location, mainInput, parameters);
          mvwprintw(stdscr, inputRow + 1, 2, "You move back into the study");
          strcpy(character.location, "study");
          wrefresh(window.compass);
        } else if (strcmp(character.location, "front room") == 0) {
          if (system.keyIsInventory == false) {
            mvwprintw(stdscr, inputRow + 1, 2, "This door is locked. Maybe there is a key somewhere");
          } else {
            mvwprintw(stdscr, inputRow + 1, 2, "Well done, you have completed 'The Room of the Locked Door'");
            if (system.davidTreasure == true) {
              inputRow++;
              mvwprintw(stdscr, inputRow + 1, 2, "You even found David's old cake tin");
            }
            mvwprintw(stdscr, LINES - 2, 0, "EXITING PROGRAM");
			wrefresh(stdscr);
			
			exitFlag = true;
          }
        } else {
        (void)mvwprintw(stdscr, inputRow + 1, 2, "Your location of '%s' has not been found", character.location);
        inputRow++;
        (void)mvwprintw(stdscr, inputRow + 1, 2, "Set location to: ", character.location);
        getnstr(character.location, 20);
        inputRow = 0;
        setupUi(window);
      }

      } else {
        mvwprintw(stdscr, inputRow + 1, 2, "You walk into a wall");
      }

      inputRow += 2;

    }
	
	
	
	else if (strcmp(mainInput, "open") == 0 || strcmp(mainInput, "shift") == 0) {

      if (strcmp(character.location, "study") == 0) {
        if (strcmp(parameters, "drawer") == 0) {
          if (system.keyIsVisible == false) {
            ma_engine_play_sound( & engine, "audio/drawer.wav", NULL);
            mvwprintw(stdscr, inputRow + 1, 2, "You open the old, dusty drawer whose handle is nearly broken and you find an old, but shiny key");
            system.keyIsVisible = true;
          } else {
            mvwprintw(stdscr, inputRow + 1, 2, "There is nothing else in here. Except some Opal Fruit wrappers");
          }
        } else if (strcmp(parameters, "window") == 0) {
          ma_engine_play_sound( & engine, "audio/wind.wav", NULL);
          mvwprintw(stdscr, inputRow + 1, 2, "You open the creaky window and smell a gust of stale air.");
          inputRow++;
          mvwprintw(stdscr, inputRow + 1, 2, "It seems like it has been closed for years. There is a very sharp looking");
          inputRow++;
          mvwprintw(stdscr, inputRow + 1, 2, "rose bush which looks like it would hurt. There is nothing of use. You close the window");
        } else if (strcmp(parameters, "door") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "There is no door here");
        } else if (strcmp(parameters, "key") == 0) {
          if (system.keyIsInventory == true) {
            mvwprintw(stdscr, inputRow + 1, 2, "You try with all your might, but the key doesn't want to open");
          } else {
            mvwprintw(stdscr, inputRow + 1, 2, "You do not have a key to open");
          }
        } else if (strcmp(parameters, "treasure") == 0) {
          if (system.davidTreasure == true) {
            mvwprintw(stdscr, inputRow + 1, 2, "You open the treasure... And it immediately slaps you in the face and closes");
          } else {
            mvwprintw(stdscr, inputRow + 1, 2, "You have no treasure");
          }
        } else {
          mvwprintw(stdscr, inputRow + 1, 2, "The %s is too difficult to move. It just won't budge", parameters);
        }
      } else if (strcmp(parameters, "bookcases") == 0 || (strcmp(parameters, "bookcase") == 0)) {

        if (strcmp(character.location, "front room") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "What bookcases...");
        } else if (strcmp(character.location, "study") == 0) {
          if (system.bookcaseIsLoose == true) {
            inputRow = 0;
            setupUi(window);
            mvwprintw(stdscr, inputRow + 1, 2, "You slide behind the bookcases and find yourself in a very small room with a chest");
            strcpy(character.location, "hidden");
          } else {
            mvwprintw(stdscr, inputRow + 1, 2, "There are no bookcases to move");
          }
        } else {
          mvwprintw(stdscr, inputRow + 1, 2, "Location not found");
        }

      } else if (strcmp(character.location, "front room") == 0) {
        if (strcmp(parameters, "key") == 0) {
          if (system.keyIsInventory == true) {
            mvwprintw(stdscr, inputRow + 1, 2, "You try with all your might, but the key doesn't want to open");
          } else {
            mvwprintw(stdscr, inputRow + 1, 2, "You do not have a key to open");
          }
        } else if (strcmp(parameters, "treasure") == 0) {
          if (system.davidTreasure == true) {
            mvwprintw(stdscr, inputRow + 1, 2, "You open the treasure... And it immediately slaps you in the face and closes");
          } else {
            mvwprintw(stdscr, inputRow + 1, 2, "You have no treasure");
          }
        } else if (strcmp(parameters, "passage") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "You slide the picture to the side, revealing a passage", parameters);
          system.pictureIsOpen = true;
        } else {
          mvwprintw(stdscr, inputRow + 1, 2, "You pull as hard as you can on the %s, but it just won't open", parameters);
        }
      } else if (strcmp(character.location, "hidden") == 0) {
        if (strcmp(parameters, "chest") == 0) {
          if (system.chestIsLoose == true) {
            mvwprintw(stdscr, inputRow + 1, 2, "You found the Lost Treasure of David Spindler", parameters);
            system.davidTreasure = true;
          } else {
            mvwprintw(stdscr, inputRow + 1, 2, "The chest is really stiff", parameters);
            system.chestIsLoose = true;
          }
        } else if (strcmp(parameters, "door") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "Try walking through it");
        } else if (strcmp(parameters, "key") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "You try with all your might, but the key doesn't want to open");
        } else if (strcmp(parameters, "treasure") == 0) {
          mvwprintw(stdscr, inputRow + 1, 2, "You open the treasure... And it immediately slaps you in the face and closes");
        } else {
          mvwprintw(stdscr, inputRow + 1, 2, "There is literally nothing else in here", parameters);
        }
      } else {
        mvwprintw(stdscr, inputRow + 1, 2, "Location not found");
      }

      inputRow += 2;

    }
	
	
	
	else if (strcmp(mainInput, "take") == 0 || strcmp(mainInput, "pick") == 0) {

      if (strcmp(character.location, "study") == 0) {
        if (strcmp(parameters, "key") == 0) {
          if (system.keyIsVisible == true && system.keyIsInventory == false) {
            ma_engine_play_sound( & engine, "audio/sparkle.wav", NULL);
            mvwprintw(stdscr, inputRow + 1, 2, "You pick up the old key");
            system.keyIsInventory = true;
          } else {
            mvwprintw(stdscr, inputRow + 1, 2, "There is no key to pick up");
          }
        } else {
          mvwprintw(stdscr, inputRow + 1, 2, "There is no %s to pick up", parameters);
        }

      } else if (strcmp(character.location, "front room") == 0) {
        mvwprintw(stdscr, inputRow + 1, 2, "There is nothing to take");
      } else {
        mvwprintw(stdscr, inputRow + 1, 2, "Location not found");
      }

      inputRow += 2;

    }