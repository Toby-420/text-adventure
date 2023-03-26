#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

#define player_health 3
#define enemy_low_health 1
#define enemy_mid_health 2
#define enemy_high_health 3
#define enemy_very_high_health 5
#define MAX_HISTORY_ITEMS 1000
#define MAX_COMMAND_LENGTH 20

/*
VARIABLES
input 		- the user's input (limited to 20 chars for now)
input_row 	- the row that ncurses will print to (is manually reset to 0 when screen is cleared)
i 			- the line history prints on
*/

int main(int argc, char * argv[]) {
    initscr(); // Start ncurses
    cbreak();
    keypad(stdscr, TRUE); // Enable arrow keys
    start_color(); // Enable colour function
    init_pair(1, COLOR_WHITE, COLOR_BLACK); // Set colours for the bar
    curs_set(1); // Set cursor invisible
    scrollok(stdscr, TRUE); // Enable scrolling 

    refresh(); // Initialize COLS and LINES
    // Make new window for bar
    clear();
    refresh();
    WINDOW * bar = newwin(1, COLS, LINES - 1, 0);
    wbkgd(bar, COLOR_PAIR(1)); // Set colour of the bar
    wborder(bar, '|', '|', '-', '-', '+', '+', '+', '+'); // Add border

    WINDOW * inventory = newwin((LINES / 3) + 3, COLS / 4, 1, (COLS * 6) / 8);
    wbkgd(inventory, COLOR_PAIR(1)); // Set colour of the bar
    box(inventory, 0, 0);
    wrefresh(inventory);

    WINDOW * photo = newwin(LINES / 2, COLS / 4, (LINES / 2) - 1, (COLS * 6) / 8);
    wbkgd(photo, COLOR_PAIR(1)); // Set colour of the bar
    box(photo, 0, 0);
    wrefresh(photo);
    refresh();

    WINDOW * compass = newwin(8, 17, 1, ((COLS * 6) / 8) - 19);
    wbkgd(compass, COLOR_PAIR(1)); // Set colour of the bar
    box(compass, 0, 0);
    wrefresh(compass);
    refresh();
	
	time_t t;
    time(&t);
	
    // Print the title of the bar
    mvwprintw(inventory, 1, 1, "INVENTORY:");
    mvwprintw(photo, 1, 1, "PLAYER VIEW:");
    mvwprintw(compass, 1, 1, "COMPASS:");
    mvwprintw(compass, 4, 8, "O");
    mvwprintw(compass, 3, 8, "|");
    mvwprintw(compass, 5, 8, "|");
    mvwprintw(compass, 4, 7, "-");
    mvwprintw(compass, 4, 9, "-");
    mvwprintw(compass, 4, 6, "-");
    mvwprintw(compass, 4, 10, "-");
    mvwprintw(bar, 0, 0, "Commands are: view, move, open, take, save, room, help, exit");
    wrefresh(bar); // Refresh the screen
    wrefresh(photo);
    wrefresh(inventory);
    wrefresh(compass);
    refresh();

    char location[9];
    strcpy(location, "frontroom");
    char input[20]; // Declare a character array to hold user input
    char main_input[4];
    char params[10];
    char * token;
    signed int input_row = 2; // Keep track of the row for input
    signed int scroll_row = 0; // Keep track of the row for the scroll window

    bool key_visibility = false;
    bool key_inventory = false;
    bool bookcase = false;
    bool chestloose = false;
    bool david = false;

    FILE * history_file = fopen("history.txt", "w"); // Open file for history

    mvwprintw(stdscr, input_row - 1, 0, "\t\t\t\t\tWelcome to The Room of the Locked Door.");
    input_row++;
    mvwprintw(stdscr, input_row - 1, 0, "\t\t\t\t\t\tESCAPE OR DIE.");
    input_row++;
    mvwprintw(stdscr, input_row - 1, 0, "\t\t*or type exit if you're a useless roadman without the brains to type and think at the same time*");
    input_row += 2;
    mvwprintw(stdscr, input_row - 1, 0, "You find yourself in an ornate and spacious, yet decently run-down front room with a window and a picture");
    input_row++;
    mvwprintw(stdscr, input_row - 1, 0, "There is a door that leads east");

    while (1) { // Continue looping until user quits               /																											// start of loop
        // Print prompt at the top of the screen
        mvwprintw(stdscr, input_row, 0, "> [%s] ", location);

        // Extract the first token
        // Split input into two separate strings based on space

        if (key_inventory == true) {
            mvwprintw(inventory, 2, 1, "- Old, but shiny key");
            wrefresh(inventory);
        } else {
            mvwprintw(inventory, 2, 1, "                    ");
            wrefresh(inventory);
        }

        if (david == true) {
            mvwprintw(inventory, 3, 1, "- David's lost treasure");
            wrefresh(inventory);
        } else {
            mvwprintw(inventory, 3, 1, "                       ");
            wrefresh(inventory);
        }

        if (strcmp(location, "frontroom") == 0) {
            mvwprintw(compass, 4, 2, "WEST");
            mvwprintw(compass, 4, 11, "EAST");
            wrefresh(compass);
            mvwprintw(photo, 2, 1, "        |                 |");
            mvwprintw(photo, 3, 1, "        |                 |");
            mvwprintw(photo, 4, 1, "        |                 |");
            mvwprintw(photo, 5, 1, "     /| |                 | |\\");
            mvwprintw(photo, 6, 1, "    / | |                 | | \\");
            mvwprintw(photo, 7, 1, "   //|| |      _____      | |  \\");
            mvwprintw(photo, 8, 1, "  // || |     |  o  |     | |   \\");
            mvwprintw(photo, 9, 1, " ||  || |     | -H- |     | |   |");
            mvwprintw(photo, 10, 1, " ||  || |     |  X  |     | |   |");
            mvwprintw(photo, 11, 1, " ||  || |     | / \\ |     | |   |");
            mvwprintw(photo, 12, 1, " ||  || |     |_____|     | |   |");
            mvwprintw(photo, 13, 1, " ||  || |                 | |   |");
            mvwprintw(photo, 14, 1, " ||o || |_________________| |  o|");
            mvwprintw(photo, 15, 1, " ||  || /                 \\ |   |");
            mvwprintw(photo, 16, 1, " ||  ||/                   \\|   |");
            mvwprintw(photo, 17, 1, " ||  |/                     \\   |");
            mvwprintw(photo, 18, 1, " || //                       \\  |");
            mvwprintw(photo, 19, 1, " ||//                         \\ |");
            mvwprintw(photo, 20, 1, " | /                           \\|");
            mvwprintw(photo, 21, 1, " |/                             \\");
            mvwprintw(photo, 22, 1, " /                               \\");
            wrefresh(photo);

        } else if (strcmp(location, "study") == 0) {
            mvwprintw(compass, 4, 11, "    ");
            mvwprintw(compass, 4, 2, "WEST");
            wrefresh(compass);
            mvwprintw(photo, 2, 1, "        |                 |");
            mvwprintw(photo, 3, 1, "        |                 |   ");
            mvwprintw(photo, 4, 1, "        |                 |      ");
            mvwprintw(photo, 5, 1, "     /| |                 |     ");
            mvwprintw(photo, 6, 1, "    / | |                 |       ");
            mvwprintw(photo, 7, 1, "   //|| |     _______     |       ");
            mvwprintw(photo, 8, 1, "  // || |    |ooo    |    |       ");
            mvwprintw(photo, 9, 1, " ||  || |    | I  n  |    |       ");
            mvwprintw(photo, 10, 1, " ||  || |    |_______|    |       ");
            mvwprintw(photo, 11, 1, " ||  || |    _________    |        ");
            mvwprintw(photo, 12, 1, " ||  || |   /         \\   |       ");
            mvwprintw(photo, 13, 1, " ||  || |  /___________\\  |       ");
            mvwprintw(photo, 14, 1, " ||o || |__|------|----|__|         ");
            mvwprintw(photo, 15, 1, " ||  || /  |      |---o|  \\        ");
            mvwprintw(photo, 16, 1, " ||  ||/   |      |----|   \\         ");
            mvwprintw(photo, 17, 1, " ||  |/                     \\        ");
            mvwprintw(photo, 18, 1, " || //                       \\      ");
            mvwprintw(photo, 19, 1, " ||//                         \\        ");
            mvwprintw(photo, 20, 1, " | /                           \\        ");
            mvwprintw(photo, 21, 1, " |/                             \\     ");
            mvwprintw(photo, 22, 1, " /                               \\  ");
            wrefresh(photo);

        } else if (strcmp(location, "hidden") == 0) {
            mvwprintw(compass, 4, 11, "    ");
            mvwprintw(compass, 4, 2, "WEST");
            wrefresh(compass);

        } else {
            mvwprintw(compass, 4, 5, "    ");
            mvwprintw(compass, 4, 11, "    ");
            wrefresh(compass);

        }

        getstr(input); // Get a string of input from the user

        token = strtok(input, " ");
        if (token != NULL) {
            strcpy(main_input, token);

            token = strtok(NULL, " ");
            if (token != NULL) {
                strcpy(params, token);
            }
        }

        int i;
        for (i = 0; input[i]; i++) {
            input[i] = tolower(input[i]);
        }

        // Remove leading whitespace
        char * start = input;

        // Remove trailing whitespace
        char * end = input + strlen(input) - 1;
        while (end > start && isspace( * end)) {
            * end-- = '\0';
        }

        fprintf(history_file, "%s\n", input);

        if (strcmp(main_input, "exit") == 0) { // Check if main_input matches the word "exit"
            // Respond to the user
            mvwprintw(stdscr, LINES - 2, 0, "EXITING PROGRAM");
            refresh(); // Refresh the screen
            sleep(1); // Wait for a second to allow the user to read the message
            delwin(bar); // Delete the bar
            endwin(); // Clear ncurses and return back to previous state
            return 0; // Exit the program with a successful completion code
        } else if (strcmp(main_input, "view") == 0) {
            if (strcmp(params, "window") == 0) {
                if (strcmp(location, "frontroom") == 0) {
                    mvwprintw(stdscr, input_row + 1, 2, "There is a large, stained glass window with pictures of a naked Jesus and sheep on it");
                } else if (strcmp(location, "study") == 0) {
                    mvwprintw(stdscr, input_row + 1, 2, "A crusty, wooden window frame houses a small sheet of glass. It seems pretty drafty");
                } else {
                    mvwprintw(stdscr, input_row + 1, 2, "Location not found");
                }
            } else if (strcmp(params, "picture") == 0) {
                if (strcmp(location, "frontroom") == 0) {
                    mvwprintw(stdscr, input_row + 1, 2, "A big hand-painted picture of a guy sits on the wall");
                    // include ascii art for window 'photo'
                } else if (strcmp(location, "study") == 0) {
                    mvwprintw(stdscr, input_row + 1, 2, "There are no pictures in here. Only Zuul");
                } else {
                    mvwprintw(stdscr, input_row + 1, 2, "Location not found");
                }
            } else if (strcmp(params, "bookcase") == 0) {
                if (strcmp(location, "frontroom") == 0) {
                    mvwprintw(stdscr, input_row + 1, 2, "There are no bookcases in here");
                    // include ascii art for window 'photo'
                } else if (strcmp(location, "study") == 0) {
                    mvwprintw(stdscr, input_row + 1, 2, "Beautifully crafted bookcases line the walls. One looks a little loose");
                    bookcase = true;
                } else {
                    mvwprintw(stdscr, input_row + 1, 2, "Location not found");
                }
            } else if (strcmp(params, "room") == 0) {
                if (strcmp(location, "frontroom") == 0) {
                    mvwprintw(stdscr, input_row + 1, 2, "A large and ornate, yet run-down front room with a window, picture and door to the east");
                    // include ascii art for window 'photo'
                } else if (strcmp(location, "study") == 0) {
                    mvwprintw(stdscr, input_row + 1, 2, "A somewhat good-looking room until you see the window and drawer which are slighty broken");
                } else if (strcmp(location, "hidden") == 0) {
                    mvwprintw(stdscr, input_row + 1, 2, "Tiny room which only has a chest in");
                } else {
                    mvwprintw(stdscr, input_row + 1, 2, "Location not found. Are you hacking?");
                }
            } else {
                mvwprintw(stdscr, input_row + 1, 2, "Cannot view %s", params);
            }

            input_row += 2;
            if (input_row + 1 == LINES - 4) {
                clear();
                input_row = 1;
                refresh();
                WINDOW * bar = newwin(1, COLS, LINES - 1, 0);
                wbkgd(bar, COLOR_PAIR(1)); // Set colour of the bar
                wborder(bar, '|', '|', '-', '-', '+', '+', '+', '+'); // Add border
                wbkgd(inventory, COLOR_PAIR(1)); // Set colour of the bar
                box(inventory, 0, 0);
                wrefresh(inventory);

                wbkgd(photo, COLOR_PAIR(1)); // Set colour of the bar
                box(photo, 0, 0);
                wrefresh(photo);
                refresh();
                wbkgd(compass, COLOR_PAIR(1)); // Set colour of the bar
                box(compass, 0, 0);
                wrefresh(compass);
                refresh();
                // Print the title of the bar
                mvwprintw(bar, 0, 0, "Commands are: view, move, open, take, save, room, help, exit");
                wrefresh(bar);
                refresh();
            }

        } else if (strcmp(main_input, "move") == 0) {

            if (strcmp(params, "east") == 0) {

                if (strcmp(location, "frontroom") == 0) {
                    mvwprintw(stdscr, input_row + 1, 2, "You move into a small study. Bookshelves line the walls, along with a window overseeing the garden.");
                    input_row++;
                    mvwprintw(stdscr, input_row + 1, 2, "There is a table with an old, solid oak drawer");
                    strcpy(location, "study");
                    wrefresh(compass);
                } else if (strcmp(location, "study") == 0) {
                    mvwprintw(stdscr, input_row + 1, 2, "You walk into a wall");
                } else {
                    mvwprintw(stdscr, input_row + 1, 2, "Location not found");

                }
            } else if (strcmp(params, "bookcase") == 0) {

                if (strcmp(location, "frontroom") == 0) {
                    mvwprintw(stdscr, input_row + 1, 2, "What bookcase...");
                } else if (strcmp(location, "study") == 0) {
                    if (bookcase == true) {
                        mvwprintw(stdscr, input_row + 1, 2, "You slide behind the bookcase and find yourself in a very small room with a chest");
                        strcpy(location, "hidden");
                    } else {
                        mvwprintw(stdscr, input_row + 1, 2, "There are no bookcases to move");
                    }
                } else {
                    mvwprintw(stdscr, input_row + 1, 2, "Location not found");

                }
            } else if (strcmp(params, "west") == 0) {
                if (strcmp(location, "study") == 0) {
                    mvwprintw(stdscr, input_row + 1, 2, "You move back into the front room");
                    strcpy(location, "frontroom");
                    wrefresh(compass);
                } else if (strcmp(location, "hidden") == 0) {
                    mvwprintw(stdscr, input_row + 1, 2, "You move back into the study");
                    strcpy(location, "study");
                    wrefresh(compass);
                } else if (strcmp(location, "frontroom") == 0) {
                    if (key_inventory == false) {
                        mvwprintw(stdscr, input_row + 1, 2, "Door locked. Go and find a key");
                    } else {
                        if (david == false) {
                            mvwprintw(stdscr, input_row + 1, 2, "Well done, you have completed 'The Room of the Locked Door'");
                            mvwprintw(stdscr, LINES - 2, 0, "EXITING PROGRAM");
                            refresh(); // Refresh the screen
                            sleep(5); // Wait for a second to allow the user to read the message
                            delwin(bar); // Delete the bar
                            endwin(); // Clear ncurses and return back to previous state
                            return 0; // Exit the program with a successful completion code
                        } else {
                            mvwprintw(stdscr, input_row + 1, 2, "Well done, you have completed 'The Room of the Locked Door'");
                            input_row++;
                            mvwprintw(stdscr, input_row + 1, 2, "You even found David's old cake tin");
                            mvwprintw(stdscr, LINES - 2, 0, "EXITING PROGRAM");
                            refresh(); // Refresh the screen
                            sleep(5); // Wait for a second to allow the user to read the message
                            delwin(bar); // Delete the bar
                            endwin(); // Clear ncurses and return back to previous state
                            return 0; // Exit the program with a successful completion code
                        }
                    }
                } else {
                    mvwprintw(stdscr, input_row + 1, 2, "Location not found");
                }

            } else {
                mvwprintw(stdscr, input_row + 1, 2, "You walk into a wall");
            }

            input_row += 2;
            if (input_row + 1 == LINES - 4) {
                clear();
                input_row = 1;
                refresh();
                WINDOW * bar = newwin(1, COLS, LINES - 1, 0);
                wbkgd(bar, COLOR_PAIR(1)); // Set colour of the bar
                wborder(bar, '|', '|', '-', '-', '+', '+', '+', '+'); // Add border
                wbkgd(inventory, COLOR_PAIR(1)); // Set colour of the bar
                box(inventory, 0, 0);
                wrefresh(inventory);

                wbkgd(photo, COLOR_PAIR(1)); // Set colour of the bar
                box(photo, 0, 0);
                wrefresh(photo);
                refresh();
                wbkgd(compass, COLOR_PAIR(1)); // Set colour of the bar
                box(compass, 0, 0);
                wrefresh(compass);
                refresh();
                // Print the title of the bar
                mvwprintw(bar, 0, 0, "Commands are: view, move, open, take, save, room, help, exit");
                wrefresh(bar);
                refresh();
            }

        } else if (strcmp(main_input, "open") == 0) {

            if (strcmp(location, "study") == 0) {
                if (strcmp(params, "drawer") == 0) {
                    if (key_visibility == false) {
                        mvwprintw(stdscr, input_row + 1, 2, "You open the old, dusty drawer whose handle is nearly broken and you find an old, but shiny key");
                        key_visibility = true;
                    } else {
                        mvwprintw(stdscr, input_row + 1, 2, "There is nothing else in here. Except some Opal Fruit wrappers");
                    }
                } else if (strcmp(params, "window") == 0) {
                    mvwprintw(stdscr, input_row + 1, 2, "You open the creaky window and smell a gust of stale air. It seems like it has been closed for years");
                } else {
                    mvwprintw(stdscr, input_row + 1, 2, "The %s is too difficult to move. It just won't budge", params);
                }
            } else if (strcmp(location, "frontroom") == 0) {
                mvwprintw(stdscr, input_row + 1, 2, "You pull as hard as you can on the %s, but it just won't open", params);
            } else if (strcmp(location, "hidden") == 0) {
                if (strcmp(params, "chest") == 0) {
                    if (chestloose == true) {
                        mvwprintw(stdscr, input_row + 1, 2, "You found the Lost Treasure of David Spindler", params);
                        david = true;
                    } else {
                        mvwprintw(stdscr, input_row + 1, 2, "The chest is really stiff", params);
                        chestloose = true;
                    }
                } else {
                    mvwprintw(stdscr, input_row + 1, 2, "There is literally nothing else in here", params);
                }
            } else {
                mvwprintw(stdscr, input_row + 1, 2, "Location not found");
            }

            input_row += 2;
            if (input_row + 1 == LINES - 4) {
                clear();
                input_row = 1;
                refresh();
                WINDOW * bar = newwin(1, COLS, LINES - 1, 0);
                wbkgd(bar, COLOR_PAIR(1)); // Set colour of the bar
                wborder(bar, '|', '|', '-', '-', '+', '+', '+', '+'); // Add border
                wbkgd(inventory, COLOR_PAIR(1)); // Set colour of the bar
                box(inventory, 0, 0);
                wrefresh(inventory);

                wbkgd(photo, COLOR_PAIR(1)); // Set colour of the bar
                box(photo, 0, 0);
                wrefresh(photo);
                refresh();
                wbkgd(compass, COLOR_PAIR(1)); // Set colour of the bar
                box(compass, 0, 0);
                wrefresh(compass);
                refresh();
                // Print the title of the bar
                mvwprintw(bar, 0, 0, "Commands are: view, move, open, take, save, room, help, exit");
                wrefresh(bar);
                refresh();
            }

        } else if (strcmp(main_input, "take") == 0) {

            if (strcmp(location, "study") == 0) {
                if (strcmp(params, "key") == 0) {
                    if (key_visibility == true) {
                        mvwprintw(stdscr, input_row + 1, 2, "You pick up the old key");
                        key_inventory = true;
                    } else {
                        mvwprintw(stdscr, input_row + 1, 2, "There is no key to pick up");
                    }
                } else {
                    mvwprintw(stdscr, input_row + 1, 2, "Cannot take %s", params);
                }

            } else if (strcmp(location, "frontroom") == 0) {
                mvwprintw(stdscr, input_row + 1, 2, "There is nothing to take");
            } else {
                mvwprintw(stdscr, input_row + 1, 2, "Location not found");
            }

            input_row += 2;
            if (input_row + 1 == LINES - 4) {
                clear();
                input_row = 1;
                refresh();
                WINDOW * bar = newwin(1, COLS, LINES - 1, 0);
                wbkgd(bar, COLOR_PAIR(1)); // Set colour of the bar
                wborder(bar, '|', '|', '-', '-', '+', '+', '+', '+'); // Add border
                wbkgd(inventory, COLOR_PAIR(1)); // Set colour of the bar
                box(inventory, 0, 0);
                wrefresh(inventory);

                wbkgd(photo, COLOR_PAIR(1)); // Set colour of the bar
                box(photo, 0, 0);
                wrefresh(photo);
                refresh();
                wbkgd(compass, COLOR_PAIR(1)); // Set colour of the bar
                box(compass, 0, 0);
                wrefresh(compass);
                refresh();
                // Print the title of the bar
                mvwprintw(bar, 0, 0, "Commands are: view, move, open, take, save, room, help, exit");
                wrefresh(bar);
                refresh();
            }

        } else if (strcmp(main_input, "help") == 0) { // Check of main_input matches help
            clear();
            mvwprintw(stdscr, 0, 0, "------HELP SCREEN------\n");
            printw("Command help:\n\n");
            printw("\tview\n");
            printw("\t\tViews an object.\n\t\te.g. 'view picture' would view a picture on a wall.\n");
            printw("\tmove\n");
            printw("\t\tAllows the game character to move in a direction specified by the player.\n\t\te.g. 'move west' would move west.\n");
            printw("\topen\n");
            printw("\t\tOpens an object.\n\t\te.g. 'open drawer' would open a drawer.\n");
            printw("\tsave\n");
            printw("\t\tSaves the game.\n");
            printw("\ttake\n");
            printw("\t\tTakes an object.\n\t\te.g. 'take key' would take a key.\n");
            printw("\troom\n");
            printw("\t\tDisplays information about the room you are in.\n");
            printw("\thelp\n");
            printw("\t\tOpens this help screen.\n");
            printw("\texit\n");
            printw("\t\tSimply exits the game.\n\n\n");
            printw("Press enter 3 times to exit this screen.\n");
            getstr(main_input); // idk why this is here but the program doesn't work when i take it away soooo...
            getchar(); // Waits until the user presses a key
            input_row = 1; // Resets input_row to 1
            clear(); // Clears ncurses screen
            refresh();
            wbkgd(bar, COLOR_PAIR(1)); // Set colour of the bar
            wborder(bar, '|', '|', '-', '-', '+', '+', '+', '+'); // Add border

            // Print the title of the bar
            mvwprintw(bar, 0, 0, "Commands are: view, move, open, take, save, room, help, exit");
            wrefresh(bar);

            wbkgd(inventory, COLOR_PAIR(1)); // Set colour of the bar
            box(inventory, 0, 0);
            wrefresh(inventory);

            wbkgd(photo, COLOR_PAIR(1)); // Set colour of the bar
            box(photo, 0, 0);
            wrefresh(photo);
            refresh();
            wbkgd(compass, COLOR_PAIR(1)); // Set colour of the bar
            box(compass, 0, 0);
            wrefresh(compass);
            refresh();

        } else if (strcmp(main_input, "save") == 0) {

            FILE * fp = fopen("gamesave.txt", "w");
            mvwprintw(stdscr, input_row + 1, 2, "Saving game data to gamesave.txt");
            fprintf(fp, "%s %d %d", location, key_visibility, key_inventory);
            fclose(fp);
            input_row += 2;
            if (input_row + 1 == LINES - 4) {
                clear();
                input_row = 1;
                refresh();
                WINDOW * bar = newwin(1, COLS, LINES - 1, 0);
                wbkgd(bar, COLOR_PAIR(1)); // Set colour of the bar
                wborder(bar, '|', '|', '-', '-', '+', '+', '+', '+'); // Add border
                wbkgd(inventory, COLOR_PAIR(1)); // Set colour of the bar
                box(inventory, 0, 0);
                wrefresh(inventory);

                wbkgd(photo, COLOR_PAIR(1)); // Set colour of the bar
                box(photo, 0, 0);
                wrefresh(photo);
                refresh();
                // Print the title of the bar
                mvwprintw(bar, 0, 0, "Commands are: view, move, open, take, save, room, help, exit");
                wrefresh(bar);
                refresh();
                wbkgd(compass, COLOR_PAIR(1)); // Set colour of the bar
                box(compass, 0, 0);
                wrefresh(compass);
                refresh();
            }

        } else if (main_input[0] == '\0' || isspace(main_input[0])) {
            mvwprintw(stdscr, input_row + 1, 2, "Empty command {err-0}");
            input_row += 2;
            if (input_row + 1 == LINES - 4) {
                clear();
                input_row = 1;
                refresh();
                WINDOW * bar = newwin(1, COLS, LINES - 1, 0);
                wbkgd(bar, COLOR_PAIR(1)); // Set colour of the bar
                wborder(bar, '|', '|', '-', '-', '+', '+', '+', '+'); // Add border
                wbkgd(inventory, COLOR_PAIR(1)); // Set colour of the bar
                box(inventory, 0, 0);
                wrefresh(inventory);

                wbkgd(photo, COLOR_PAIR(1)); // Set colour of the bar
                box(photo, 0, 0);
                wrefresh(photo);
                refresh();
                // Print the title of the bar
                mvwprintw(bar, 0, 0, "Commands are: view, move, open, take, save, room, help, exit");
                wrefresh(bar);
                refresh();
                wbkgd(compass, COLOR_PAIR(1)); // Set colour of the bar
                box(compass, 0, 0);
                wrefresh(compass);
                refresh();
            }
        } else {
            mvwprintw(stdscr, input_row + 1, 2, "Illegal command: %s {err-1}", main_input); // Print the user main_input on the screen
            input_row += 2;
            if (input_row + 1 == LINES - 4) {
                clear();
                input_row = 1;
                refresh();
                WINDOW * bar = newwin(1, COLS, LINES - 1, 0);
                wbkgd(bar, COLOR_PAIR(1)); // Set colour of the bar
                wborder(bar, '|', '|', '-', '-', '+', '+', '+', '+'); // Add border
                wbkgd(inventory, COLOR_PAIR(1)); // Set colour of the bar
                box(inventory, 0, 0);
                wrefresh(inventory);

                wbkgd(photo, COLOR_PAIR(1)); // Set colour of the bar
                box(photo, 0, 0);
                wrefresh(photo);
                refresh();
                // Print the title of the bar
                mvwprintw(bar, 0, 0, "Commands are: view, move, open, take, save, room, help, exit");
                wrefresh(bar);
                refresh();
                wbkgd(compass, COLOR_PAIR(1)); // Set colour of the bar
                box(compass, 0, 0);
                wrefresh(compass);
                refresh();
            }
        }

        refresh(); // Refresh the screen
    }

    delwin(bar); // Delete the bar
    endwin(); // Clears ncurses and returns back to previous state
    return 0; // Gives successful completion code
}