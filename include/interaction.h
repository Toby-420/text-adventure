#ifndef INTERACTION_H
#define INTERACTION_H

int minFunction(int a, int b, int c);
int distanceAlgorithm(const char *s1, const char *s2);
int stringIsSimilar(const char *s1, const char *s2, int threshold);
int handleViewCommand(int inputRow, char *mainInput, char *parameters, struct MainCharacter *character, struct System *system, struct Windows *window, struct WindowText *text);
int handleMoveCommand(int inputRow, char *mainInput, char *parameters, struct MainCharacter *character, struct System *system, struct Windows *window, struct WindowText *text, bool insulting);
int handleOpenCommand(int inputRow, char *mainInput, char *parameters, struct MainCharacter *character, struct System *system, struct Windows *window, struct WindowText *text);
int handleTakeCommand(int inputRow, char *mainInput, char *parameters, struct MainCharacter *character, struct System *system, struct Windows *window, struct WindowText *text);

#endif