#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
// #include "main.h"

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
void print_history() {
    FILE* history_file = fopen("history.txt", "r");
    if (history_file) {
        clear(); // Clear the whole screen
        short i = 1;
		    WINDOW *stdscr = newwin(LINES-2, COLS, 0, 0);
    scrollok(stdscr, TRUE);      // Enable scrolling
        char line[MAX_COMMAND_LENGTH + 1];
        while (fgets(line, MAX_COMMAND_LENGTH + 1, history_file)) {
            mvwprintw(stdscr,i, 0, "%s", line); // Print each line to the screen
            i++;
        }
        fclose(history_file);
        history_file = NULL;
    }
}


int main() {
    initscr();                          // Start ncurses
	cbreak();
    keypad(stdscr, TRUE); // Enable arrow keys
    start_color();                      // Enable colour function
    init_pair(1, COLOR_WHITE, COLOR_BLACK);   // Set colours for the bar
    curs_set(1);                        // Set cursor invisible
	scrollok(stdscr, TRUE);				// Enable scrolling 
	
    refresh();                          // Initialize COLS and LINES

    // Make new window for bar
    WINDOW *bar = newwin(1, COLS, LINES-1, 0);
    wbkgd(bar, COLOR_PAIR(1));          // Set colour of the bar
    wborder(bar, '|', '|', '-', '-', '+', '+', '+', '+');   // Add border

    // Print the title of the bar
    mvwprintw(bar, 0, 0, "Commands are: move, open, use, help, exit");
    wrefresh(bar);                      // Refresh the screen
	

    
	
	// Make new window for scrollable history
    // WINDOW *stdscr = newwin(LINES-2, COLS, 0, 0);
    // scrollok(stdscr, TRUE);      // Enable scrolling
    // wborder(stdscr, '|', '|', '-', '-', '+', '+', '+', '+');  
	
    char input[20];                      // Declare a character array to hold user input
    signed int input_row = 1;                  // Keep track of the row for input
	signed int scroll_row = 0;                 // Keep track of the row for the scroll window
	
	FILE* history_file = fopen("history.txt", "w"); // Open file for history

    while (1) {                         // Continue looping until user quits
        // Print prompt at the top of the screen
        mvwprintw(stdscr,input_row, 0, "> ");
        getstr(input);                  // Get a string of input from the user
		
		fprintf(history_file, "%s\n", input);
		
		if (input_row == LINES-3) {
			input_row = 1;
			clear();
			refresh();
			WINDOW *bar = newwin(1, COLS, LINES-1, 0);
			wbkgd(bar, COLOR_PAIR(1));          // Set colour of the bar
			wborder(bar, '|', '|', '-', '-', '+', '+', '+', '+');   // Add border

			// Print the title of the bar
			mvwprintw(bar, 0, 0, "Commands are: move, open, use, help, exit");
			wrefresh(bar); 
			refresh();
		}

		
        if (strcmp(input, "exit") == 0) {   // Check if input matches the word "exit"
            // Respond to the user
            mvwprintw(stdscr,LINES-2, 0, "EXITING PROGRAM");
            refresh();                  // Refresh the screen
            sleep(1);                   // Wait for a second to allow the user to read the message
            delwin(bar);                // Delete the bar
            endwin();                   // Clear ncurses and return back to previous state
            return 0;                   // Exit the program with a successful completion code
        }
		
		else if (strcmp(input, "move") == 0) {
			mvwprintw(stdscr, input_row, 2, "Command not yet implemented.\n");
			input_row++;
			
		}
		else if (strcmp(input, "open") == 0) {
			mvwprintw(stdscr, input_row, 2, "Command not yet implemented.\n");
			input_row++;
			
		}
		else if (strcmp(input, "use") == 0) {
			mvwprintw(stdscr, input_row, 2, "Command not yet implemented.\n");
			input_row++;
			
		}
		else if (strcmp(input, "help") == 0) { // Check of input matches help
			clear();
			mvwprintw(stdscr,0,0,"------HELP SCREEN------\n");
			printw("Command help:\n\n");
			printw("\tmove\n");
			printw("\t\tAllows the game character to move to an area specified by the user.\n\t\te.g. 'move bedroom' would change the room to bedroom.\n");
			printw("\topen\n");
			printw("\t\tOpens an object.\n\t\te.g. 'open door' would open a door.\n");
			printw("\tuse\n");
			printw("\t\tUses an item.\n\t\te.g. 'use knife' would cut something with a knife.\n");
			printw("\thelp\n");
			printw("\t\tOpens this help screen.\n");
			printw("\texit\n");
			printw("\t\tSimply exits the game.\n\n\n");
			printw("Press enter 3 times to exit this screen.\n");
			getstr(input);  // idk why this is here but the program doesn't work when i take it away soooo...
			getchar(); 		// Waits until the user presses a key
			input_row = 1; 	// Resets input_row to 1
			clear(); 		// Clears ncurses screen
			refresh();
			WINDOW *bar = newwin(1, COLS, LINES-1, 0);
			wbkgd(bar, COLOR_PAIR(1));          // Set colour of the bar
			wborder(bar, '|', '|', '-', '-', '+', '+', '+', '+');   // Add border

			// Print the title of the bar
			mvwprintw(bar, 0, 0, "Commands are: move, open, use, help, exit");
			wrefresh(bar); 
			refresh();
			
		}
		else if (input[0] == '\0' || isspace(input[0])) {
			mvwprintw(stdscr, input_row, 2, "Empty command {err-0}");
			input_row++;
		}
        else {
            mvwprintw(stdscr,input_row, 2, "Illegal command: %s {err-1}", input); // Print the user input on the screen
            input_row++; // Increase the row for input
        }
        
        refresh();       // Refresh the screen
    }

    delwin(bar);         // Delete the bar
    endwin();            // Clears ncurses and returns back to previous state
    return 0;          	 // Gives successful completion code
}
