/* FRNT Framework (Menu Module)
 * Version 0.1 (26/10-07)
 * Copyright 2007 Kjetil Erga (kobolt.anarion -AT- gmail -DOT- com)
 *
 *
 * This file is part of the FRNT Framework.
 *
 * The FRNT Framework is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * The FRNT Framework is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the FRNT Framework.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "frnt.h"
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>

#define MENU_MODULE_DIR "./" /* Current directory. */

#define MENU_MAX_ENTRIES 10
#define MENU_MAX_NAME 16

enum {
  MENU_TICK_ACTION_NONE,
  MENU_TICK_ACTION_LOAD,
  MENU_TICK_ACTION_NEW_FOCUS,
  MENU_TICK_ACTION_SCAVENGE,
  MENU_TICK_ACTION_SCAVENGE_2,
};



static int menu_tick_action;
static int menu_tick_module;

static char menu_entries[MENU_MAX_ENTRIES][MENU_MAX_NAME];
static int menu_total_entries;
static int menu_selected_entry;



void menu_event(frnt_module_t *m, int event)
{
  if (event == KEY_UP) {
    menu_selected_entry--;
    if (menu_selected_entry < 0)
      menu_selected_entry = 0;
  } else if (event == KEY_DOWN) {
    menu_selected_entry++;
    if (menu_selected_entry >= menu_total_entries + 2)
      menu_selected_entry = menu_total_entries + 1;
  } else if (event == '\n') {
    if (menu_selected_entry == menu_total_entries) {
      /* Save layout. */
      m->manager_command = FRNT_COMMAND_SAVE_LAYOUT;
    } else if (menu_selected_entry == menu_total_entries + 1) {
      /* Exit. */
      m->manager_command = FRNT_COMMAND_EXIT;
    } else {
      /* Check if module exists first, then load/unload on later tick. */
      menu_tick_action = MENU_TICK_ACTION_LOAD;
      menu_tick_module = menu_selected_entry;
      m->manager_command = FRNT_COMMAND_MOD_EXISTS;
      strncpy(m->manager_argument, menu_entries[menu_selected_entry],
        FRNT_MAX_ARG_LENGTH);
    }
  }
}



void menu_tick(frnt_module_t *m)
{
  switch (menu_tick_action) {
  case MENU_TICK_ACTION_LOAD:
    if (m->manager_command == FRNT_COMMAND_NONE) {
      if (m->manager_return == 0) {
        m->manager_command = FRNT_COMMAND_LOAD;
        strncpy(m->manager_argument, menu_entries[menu_tick_module],
          FRNT_MAX_ARG_LENGTH);
        /* Try to change focus to newly loaded module on next tick. */
        menu_tick_action = MENU_TICK_ACTION_NEW_FOCUS;
      } else if (m->manager_return == 1) {
        m->manager_command = FRNT_COMMAND_UNLOAD;
        strncpy(m->manager_argument, menu_entries[menu_tick_module],
          FRNT_MAX_ARG_LENGTH);
        menu_tick_action = MENU_TICK_ACTION_NONE;
      }
    } /* Else, command not processed yet. */
    break;

  case MENU_TICK_ACTION_NEW_FOCUS:
    if (m->manager_command == FRNT_COMMAND_NONE) {
      if (m->manager_return == FRNT_CMD_RESULT_OK) {
        /* Change focus if newly loaded module loaded successfully. */
        m->manager_command = FRNT_COMMAND_NEW_FOCUS;
        strncpy(m->manager_argument, menu_entries[menu_tick_module],
          FRNT_MAX_ARG_LENGTH);
        menu_tick_action = MENU_TICK_ACTION_SCAVENGE;
      } else {
        menu_tick_action = MENU_TICK_ACTION_NONE;
      }
    }
    break;

  case MENU_TICK_ACTION_SCAVENGE:
    m->manager_command = FRNT_COMMAND_MOD_EXISTS;
    strncpy(m->manager_argument, menu_entries[menu_tick_module],
      FRNT_MAX_ARG_LENGTH);
    menu_tick_action = MENU_TICK_ACTION_SCAVENGE_2;
    break;

  case MENU_TICK_ACTION_SCAVENGE_2:
    if (m->manager_command == FRNT_COMMAND_NONE) {
      /* Wait for loaded module to be unloaded, then grab back focus. */
      if (m->manager_return == 0) {
        m->manager_command = FRNT_COMMAND_NEW_FOCUS;
        strncpy(m->manager_argument, "menu", FRNT_MAX_ARG_LENGTH);
        menu_tick_action = MENU_TICK_ACTION_NONE;
      } else {
        /* Module still loaded, try to check again. */
        menu_tick_action = MENU_TICK_ACTION_SCAVENGE;
      }
    }
    break;

  default:
    break;
  }
}



void menu_redraw(frnt_module_t *m, int focus)
{
  int i, j;

  if (has_colors())
    wbkgd(m->w, COLOR_PAIR(focus ? FRNT_COLOR_FOCUS : FRNT_COLOR_COMMON));
  else
    wbkgd(m->w, focus ? A_BOLD : A_NORMAL);

  box(m->w, ACS_VLINE, ACS_HLINE);

  mvwprintw(m->w, 0, 1, "MENU");
  for (i = 0; i < menu_total_entries + 2; i++) {
    if (i == menu_selected_entry)
      wattron(m->w, A_REVERSE);
    if (i == menu_total_entries) {
      mvwprintw(m->w, i + 1, 1, "Save layout");
    } else if (i == menu_total_entries + 1) {
      mvwprintw(m->w, i + 1, 1, "Exit");
    } else {
      mvwprintw(m->w, i + 1, 1, menu_entries[i]);
      /* Force uppercase for first character. */
      mvwaddch(m->w, i + 1, 1, toupper(menu_entries[i][0]));
      /* Change underscore to space. */
      for (j = 0; j < MENU_MAX_NAME; j++) {
        if (menu_entries[i][j] == '_')
          mvwaddch(m->w, i + 1, j + 1, ' ');
      }
    }
    if (i == menu_selected_entry)
      wattroff(m->w, A_REVERSE);
  }
}



void menu_unload(void)
{
}



void menu_init(frnt_module_t *m)
{
  DIR *dh;
  struct dirent *dir_entry;
  char *so_pos;
  int length;

  /* Read module directory to generate menu run-time. */
  menu_total_entries = 0;
  if ((dh = opendir(MENU_MODULE_DIR)) != NULL) {
    while ((dir_entry = readdir(dh)) != NULL) {
      if (strcmp(dir_entry->d_name, "menu.so") == 0)
        continue; /* Do not add itself to the menu. */
      if ((so_pos = strstr(dir_entry->d_name, ".so")) != NULL) {
        length = so_pos - dir_entry->d_name; /* Pointer magic. */
        if (length <= MENU_MAX_NAME) {
          strncpy(menu_entries[menu_total_entries], dir_entry->d_name, length);
          menu_total_entries++;
        }
      }
    }
    closedir(dh);
  } else {
    fprintf(stderr, "Cannot open module directory to create menu.\n");
  }

  menu_tick_action = MENU_TICK_ACTION_NONE;
  menu_selected_entry = 0;
  
  /* Including top line, bottom line, exit and save layout. */
  m->size_y = menu_total_entries + 4;
  m->size_x = MENU_MAX_NAME + 2;
}

