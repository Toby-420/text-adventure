#include <json-c/json.h>
#include <stdio.h>
#include <string.h>
#include "../include/defines.h"

int saveGame(struct System system, struct MainCharacter character) {
  json_object *root = json_object_new_object();
  json_object_object_add(root, "location", json_object_new_string(character.location));
  json_object_object_add(root, "locationNumber", json_object_new_int(character.locationNumber));
  json_object_object_add(root, "name", json_object_new_string(character.name));          // ciphered
  json_object_object_add(root, "namehold", json_object_new_string(character.namehold));  // readable
  // json_object_object_add(root, "overpowered", json_object_new_boolean(character.overpowered));
  json_object_object_add(root, "nameAsked", json_object_new_boolean(system.nameAsked));
  json_object_object_add(root, "keyIsVisible", json_object_new_boolean(system.keyIsVisible));
  json_object_object_add(root, "keyIsInventory", json_object_new_boolean(system.keyIsInventory));
  json_object_object_add(root, "bookcaseIsLoose", json_object_new_boolean(system.bookcaseIsLoose));
  json_object_object_add(root, "davidIsVisible", json_object_new_boolean(system.davidIsVisible));
  json_object_object_add(root, "davidIsInventory", json_object_new_boolean(system.davidIsInventory));
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
  system->keyIsVisible = json_object_get_boolean(json_object_object_get(root, "keyIsVisible"));
  system->keyIsInventory = json_object_get_boolean(json_object_object_get(root, "keyIsInventory"));
  system->bookcaseIsLoose = json_object_get_boolean(json_object_object_get(root, "bookcaseIsLoose"));
  system->davidIsVisible = json_object_get_boolean(json_object_object_get(root, "davidIsVisible"));
  system->davidIsInventory = json_object_get_boolean(json_object_object_get(root, "davidIsInventory"));
  system->pictureIsOpen = json_object_get_boolean(json_object_object_get(root, "pictureIsOpen"));
  system->hiddenRoomTreasure = json_object_get_boolean(json_object_object_get(root, "hiddenRoomTreasure"));
  system->passageTreasure = json_object_get_boolean(json_object_object_get(root, "passageTreasure"));

  json_object_put(root);
  return 0;
}
