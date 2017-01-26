/* FRNT Framework (Header File)
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

#include <ncurses.h>

#define FRNT_COLOR_COMMON 1
#define FRNT_COLOR_FOCUS 2

#define FRNT_MAX_MODULES 10
#define FRNT_MAX_MODULE_NAME 16
#define FRNT_MAX_ARG_LENGTH 16

#define FRNT_CMD_RESULT_OK 0
#define FRNT_CMD_RESULT_FAIL -1

enum {
  FRNT_COMMAND_NONE        = 0,
  FRNT_COMMAND_EXIT        = 1,
  FRNT_COMMAND_LOAD        = 2,
  FRNT_COMMAND_UNLOAD      = 3,
  FRNT_COMMAND_MOD_EXISTS  = 4,
  FRNT_COMMAND_NEW_FOCUS   = 5,
  FRNT_COMMAND_SAVE_LAYOUT = 6,
};

typedef struct frnt_module_s {
  WINDOW *w;
  void *dl_handle;
  char name[FRNT_MAX_MODULE_NAME];
  int y, x; /* Position. */
  int size_y, size_x;
  void (*event_function)(struct frnt_module_s *m, int event);
  void (*tick_function)(struct frnt_module_s *m);
  void (*redraw_function)(struct frnt_module_s *m, int focus);
  void (*unload_function)(void);
  int manager_command;
  int manager_return;
  char manager_argument[FRNT_MAX_ARG_LENGTH];
} frnt_module_t;

