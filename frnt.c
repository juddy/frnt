/* FRNT Framework (The Framework Itself)
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
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#define FRNT_DEFAULT_INIT_FILE "frnt.init"
#define FRNT_MODULE_DIR "./" /* Current directory. */
#define FRNT_DEFAULT_START_Y 1
#define FRNT_DEFAULT_START_X 1



static int frnt_module_load(frnt_module_t *ma, char *name, int initial_y, int initial_x)
{
  char name_buffer[FRNT_MAX_MODULE_NAME + 10];
  void (*init_function)(frnt_module_t *m);
  frnt_module_t *m = NULL;
  int i;

  for (i = 0; i < FRNT_MAX_MODULES; i++) {
    if (strcmp(ma[i].name, name) == 0) {
      fprintf(stderr, "Module already loaded.\n");
      return -1;
    }
    if (ma[i].w == NULL) {
      m = &ma[i];
      break;
    }
  }
  if (m == NULL) {
    fprintf(stderr, "No more space left to load more modules.\n");
    return -1;
  }

  snprintf(name_buffer, FRNT_MAX_MODULE_NAME + 10, "%s%s.so", FRNT_MODULE_DIR,
    name);
  if ((m->dl_handle = dlopen(name_buffer, RTLD_NOW)) == NULL) {
    fprintf(stderr, "%s\n", dlerror());
    return -1;
  }

  snprintf(name_buffer, FRNT_MAX_MODULE_NAME + 10, "%s_event", name);
  if ((m->event_function = dlsym(m->dl_handle, name_buffer)) == NULL) {
    fprintf(stderr, "%s\n", dlerror());
    dlclose(m->dl_handle);
    return -1;
  }

  snprintf(name_buffer, FRNT_MAX_MODULE_NAME + 10, "%s_tick", name);
  if ((m->tick_function = dlsym(m->dl_handle, name_buffer)) == NULL) {
    fprintf(stderr, "%s\n", dlerror());
    dlclose(m->dl_handle);
    return -1;
  }

  snprintf(name_buffer, FRNT_MAX_MODULE_NAME + 10, "%s_unload", name);
  if ((m->unload_function = dlsym(m->dl_handle, name_buffer)) == NULL) {
    fprintf(stderr, "%s\n", dlerror());
    dlclose(m->dl_handle);
    return -1;
  }

  snprintf(name_buffer, FRNT_MAX_MODULE_NAME + 10, "%s_redraw", name);
  if ((m->redraw_function = dlsym(m->dl_handle, name_buffer)) == NULL) {
    fprintf(stderr, "%s\n", dlerror());
    dlclose(m->dl_handle);
    return -1;
  }

  snprintf(name_buffer, FRNT_MAX_MODULE_NAME + 10, "%s_init", name);
  if ((init_function = dlsym(m->dl_handle, name_buffer)) == NULL) {
    fprintf(stderr, "%s\n", dlerror());
    dlclose(m->dl_handle);
    return -1;
  }

  /* Set up initial data, may be overwritten by the init function though. */
  m->y = initial_y;
  m->x = initial_x;
  m->manager_command = FRNT_COMMAND_NONE;

  init_function(m);
  if ((m->w = newwin(m->size_y, m->size_x, m->y, m->x)) == NULL) {
    fprintf(stderr, "Could not create new window.\n");
    dlclose(m->dl_handle);
    return -1;
  }

  /* This has to run last, since the name indicates for other functions the
     module is loaded. A zero size string is used otherwise. */
  strncpy(m->name, name, FRNT_MAX_MODULE_NAME);

  return 0;
}



static int frnt_module_unload(frnt_module_t *ma, char *name)
{
  frnt_module_t *m = NULL;
  int i;

  for (i = 0; i < FRNT_MAX_MODULES; i++) {
    if (strcmp(ma[i].name, name) == 0) {
      m = &ma[i];
      break;
    }
  }
  if (m == NULL) {
    fprintf(stderr, "Specified module could not be found in memory.\n");
    return -1;
  }

  /* Execute module specific cleanup function. */
  (m->unload_function)();

  delwin(m->w);
  dlclose(m->dl_handle);

  /* Move all modules together to keep the correct integrity. */
  for (; i < FRNT_MAX_MODULES - 1; i++)
    ma[i] = ma[i + 1];
  ma[i].w = NULL;
  ma[i].name[0] = '\0';

  return 0;
}



static int frnt_module_find_index(frnt_module_t *ma, char *name)
{
  int i;
  for (i = 0; i < FRNT_MAX_MODULES; i++) {
    if (strcmp(ma[i].name, name) == 0)
      return i;
  }
  return -1;
}



static int frnt_init_load(frnt_module_t *ma, char *filename)
{
  FILE *fh;
  int loaded = 0;
  int initial_y, initial_x;
  char module[FRNT_MAX_MODULE_NAME];
  char line[64]; /* More than enough for module name and geometries. */

  fh = fopen(filename, "r");
  if (fh == NULL) {
    fprintf(stderr, "Init file '%s' not found.\n", filename);
    return 0;
  }

  while (fgets(line, 64, fh) != NULL) {
    if (sscanf(line, "%d:%d",  &initial_y, &initial_x) != 2) {
      fprintf(stderr, "Bad format on geometry in init file, line skipped.\n");
      continue;
    }

    strncpy(module, strrchr(line, ':') + 1, FRNT_MAX_MODULE_NAME);

    if (strlen(module) > 0) {
      if (module[strlen(module) - 1] == '\n')
        module[strlen(module) - 1] = '\0'; /* Remove newline. */
    }

    if (strlen(module) == 0) {
      fprintf(stderr,
        "Bad format on module name in init file, line skipped.\n");
      continue;
    }

    if (frnt_module_load(ma, module, initial_y, initial_x) == 0)
      loaded++;
  }
  fclose(fh);

  return loaded;
}



static int frnt_init_save(frnt_module_t *ma, char *filename, int focus)
{
  int i;
  FILE *fh;

  fh = fopen(filename, "w");
  if (fh == NULL) {
    fprintf(stderr, "Init file '%s' not found.\n", filename);
    return -1;
  }

  for (i = 0; ma[i].name[0] != '\0'; i++) {
    if (i != focus)
      fprintf(fh, "%d:%d:%s\n", ma[i].y, ma[i].x, ma[i].name);
  }
  fprintf(fh, "%d:%d:%s\n", ma[focus].y, ma[focus].x, ma[focus].name);

  fclose(fh);
  
  return 0;
}



int main(int argc, char *argv[])
{
  int c, i, max_y, max_x;
  frnt_module_t modules[FRNT_MAX_MODULES];
  int module_in_focus, loaded_modules;
  char temp_name[FRNT_MAX_MODULE_NAME];
  int new_index;
#ifdef NCURSES_MOUSE_VERSION
  MEVENT mouse_event;
  int module_moused, old_mouse_y, old_mouse_x;
#endif

  initscr();
  if (has_colors()) {
    start_color();
    init_pair(FRNT_COLOR_COMMON, COLOR_WHITE, COLOR_BLUE);
    init_pair(FRNT_COLOR_FOCUS, COLOR_YELLOW, COLOR_BLUE);
    bkgd(COLOR_PAIR(FRNT_COLOR_COMMON));
  }
  atexit((void *)endwin);

  getmaxyx(stdscr, max_y, max_x);
  noecho();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
#ifdef NCURSES_MOUSE_VERSION
  mousemask(ALL_MOUSE_EVENTS, NULL);
#endif

  /* Intialize and set all modules as "empty". */
  for (i = 0; i < FRNT_MAX_MODULES; i++) {
    modules[i].w = NULL;
    modules[i].name[0] = '\0';
  }

  loaded_modules = frnt_init_load(modules,
    argc > 1 ? argv[1] : FRNT_DEFAULT_INIT_FILE);
  module_in_focus = loaded_modules - 1; /* Last one loaded from init file. */
#ifdef NCURSES_MOUSE_VERSION
  module_moused = -1; /* -1 = No module in movement by mouse. */
#endif

  if (loaded_modules == 0) {
    fprintf(stderr, "No modules were loaded during startup, aborting.\n");
    exit(1);
  }

  while (1) {
    /* Get event: */
    c = getch();
    if (c != ERR) {
      switch (c) {

      case KEY_F(10):
        /* Unload all modules. (Special trick: Unload first module X times.) */
        for (i = 0; i < loaded_modules; i++)
          frnt_module_unload(modules, modules[0].name);
        loaded_modules = 0;
        exit(0);
        break;

      case KEY_RESIZE:
        getmaxyx(stdscr, max_y, max_x);
        break;

#ifdef NCURSES_MOUSE_VERSION
      case KEY_MOUSE:
        getmouse(&mouse_event);

        if (mouse_event.bstate == BUTTON1_PRESSED) {
          if (wenclose(modules[module_in_focus].w,
            mouse_event.y, mouse_event.x)) {
            /* module in focus has precedence when moving. */
            module_moused = module_in_focus;
            old_mouse_y = mouse_event.y;
            old_mouse_x = mouse_event.x;
            break; 
          }

          /* Check if the mouse press was in the vicinity of a module. */
          for (i = 0; i < loaded_modules; i++) {
            if (wenclose(modules[i].w, mouse_event.y, mouse_event.x)) {
              module_moused = i;
              old_mouse_y = mouse_event.y;
              old_mouse_x = mouse_event.x;
              break;
            }
          }

        } else if (mouse_event.bstate == BUTTON1_RELEASED) {
          if (module_moused != -1) {
            modules[module_moused].y = modules[module_moused].y +
              (mouse_event.y - old_mouse_y);
            modules[module_moused].x = modules[module_moused].x +
              (mouse_event.x - old_mouse_x);

            touchwin(stdscr);

            /* Make sure module is not moved outside of screen. */
            if (modules[module_moused].y < 0)
              modules[module_moused].y = 0;
            if (modules[module_moused].x < 0)
              modules[module_moused].x = 0;
            if (modules[module_moused].y > 
              max_y - modules[module_moused].size_y)
              modules[module_moused].y = 
              max_y - modules[module_moused].size_y;
            if (modules[module_moused].x > 
              max_x - modules[module_moused].size_x)
              modules[module_moused].x =
              max_x - modules[module_moused].size_x;

            module_moused = -1;
          }

        } else if (mouse_event.bstate == BUTTON1_CLICKED ||
          mouse_event.bstate == BUTTON1_DOUBLE_CLICKED) {
          if (wenclose(modules[module_in_focus].w,
            mouse_event.y, mouse_event.x)) {
            /* Module window in focus was clicked, do nothing. */
            break; 
          }
          /* Check if the mouse click was in the vicinity of a module window. */
          for (i = loaded_modules - 1; i >= 0; i--) {
            /* Go backwards through modules, since the last drawed module
               will look like it is on the top visually. */
            if (wenclose(modules[i].w, mouse_event.y, mouse_event.x)) {
              module_in_focus = i;
              break;
            }
          }
        }
        break;
#endif

      case KEY_F(5):
        modules[module_in_focus].x--;
        if (modules[module_in_focus].x < 0)
          modules[module_in_focus].x = 0;
        touchwin(stdscr);
        break;

      case KEY_F(6):
        modules[module_in_focus].y++;
        if (modules[module_in_focus].y > 
          max_y - modules[module_in_focus].size_y)
          modules[module_in_focus].y = max_y - modules[module_in_focus].size_y;
        touchwin(stdscr);
        break;

      case KEY_F(7):
        modules[module_in_focus].y--;
        if (modules[module_in_focus].y < 0)
          modules[module_in_focus].y = 0;
        touchwin(stdscr); /* Touch to prevent trash. */
        break;

      case KEY_F(8):
        modules[module_in_focus].x++;
        if (modules[module_in_focus].x > 
          max_x - modules[module_in_focus].size_x)
          modules[module_in_focus].x = max_x - modules[module_in_focus].size_x;
        touchwin(stdscr);
        break;
        
      case KEY_F(9):
        module_in_focus++;
        if (module_in_focus > loaded_modules - 1)
          module_in_focus = 0;
        break;

      case KEY_F(11):
        if (frnt_module_unload(modules, modules[module_in_focus].name) == 0) {
          loaded_modules--;
          touchwin(stdscr);
          module_in_focus = 0;
        }
        break;

      case KEY_F(12):
        if (frnt_init_save(modules,
          argc > 1 ? argv[1] : FRNT_DEFAULT_INIT_FILE, module_in_focus) == -1) {
          fprintf(stderr, "Saving layout to init file failed.\n");
        }
        break;

      default:
        /* Send event to module window in focus. */
        (modules[module_in_focus].event_function)(&modules[module_in_focus], c);
        break;
      }
    }
    
    /* Send tick to all modules: */
    for (i = 0; i < loaded_modules; i++)
      (modules[i].tick_function)(&modules[i]);

    /* See if a module want to order the manger to do something: */
    for (i = 0; i < loaded_modules; i++) {
      if (modules[i].manager_command == FRNT_COMMAND_EXIT) {
        /* Unload all modules. */
        for (i = 0; i < loaded_modules; i++)
          frnt_module_unload(modules, modules[0].name);
        loaded_modules = 0;
        exit(0);
      } else if (modules[i].manager_command == FRNT_COMMAND_LOAD) {
        if (frnt_module_load(modules, modules[i].manager_argument,
          FRNT_DEFAULT_START_Y, FRNT_DEFAULT_START_X) == 0) {
          loaded_modules++;
          modules[i].manager_return = FRNT_CMD_RESULT_OK;
        } else {
          fprintf(stderr, "Loading of module '%s' failed.\n",
            modules[i].manager_argument);
          modules[i].manager_return = FRNT_CMD_RESULT_FAIL;
        }
        modules[i].manager_command = FRNT_COMMAND_NONE;
        modules[i].manager_argument[0] = '\0';
        break; /* Only one load at a time, or else loop will go insane,
                  since the iterator is changed. */
      } else if (modules[i].manager_command == FRNT_COMMAND_UNLOAD) {
        /* Need to remember name, since module may be moved during unload. */
        strncpy(temp_name, modules[i].name, FRNT_MAX_MODULE_NAME);
        if (frnt_module_unload(modules, modules[i].manager_argument) == 0) {
          loaded_modules--;
          touchwin(stdscr); /* Touch to prevent trash. */
          modules[i].manager_return = FRNT_CMD_RESULT_OK;
        } else {
          fprintf(stderr, "Unloading of module '%s' failed.\n",
            modules[i].manager_argument);
          modules[i].manager_return = FRNT_CMD_RESULT_FAIL;
        }
        if ((new_index = frnt_module_find_index(modules, temp_name)) == -1) {
          /* Module unloaded itself. */
          module_in_focus = 0;
          break;
        }
        /* Only do this if the module did not unload itself. */
        module_in_focus = new_index;
        modules[new_index].manager_command = FRNT_COMMAND_NONE;
        modules[new_index].manager_argument[0] = '\0';
        break; /* Only one unload at a time, or else loop will go insane,
                  since the iterator is changed. */
      } else if (modules[i].manager_command == FRNT_COMMAND_MOD_EXISTS) {
        if (frnt_module_find_index(modules, modules[i].manager_argument) != -1) {
          modules[i].manager_return = 1;
        } else {
          modules[i].manager_return = 0;
        }
        modules[i].manager_command = FRNT_COMMAND_NONE;
        modules[i].manager_argument[0] = '\0';
      } else if (modules[i].manager_command == FRNT_COMMAND_NEW_FOCUS) {
        if ((new_index = frnt_module_find_index(modules,
          modules[i].manager_argument)) != -1) {
          module_in_focus = new_index;
          modules[i].manager_return = FRNT_CMD_RESULT_OK;
        } else {
          modules[i].manager_return = FRNT_CMD_RESULT_FAIL;
        }
        modules[i].manager_command = FRNT_COMMAND_NONE;
        modules[i].manager_argument[0] = '\0';
      } else if (modules[i].manager_command == FRNT_COMMAND_SAVE_LAYOUT) {
        if (frnt_init_save(modules,
          argc > 1 ? argv[1] : FRNT_DEFAULT_INIT_FILE, module_in_focus) == -1) {
          fprintf(stderr, "Saving layout to init file failed.\n");
        }
      }
    }

    /* Terminate the program/manager if there are no modules loaded anymore. */
    if (loaded_modules == 0)
      exit(0);

    /* Refresh, colorize, and possibly move all modules: */
    for (i = 0; i < loaded_modules; i++) {
      mvwin(modules[i].w, modules[i].y, modules[i].x);
      (modules[i].redraw_function)(&modules[i], 0);
      wnoutrefresh(modules[i].w);
    }

    /* Module in focus is refreshed last, and has another color. */
    (modules[module_in_focus].redraw_function)(&modules[module_in_focus], 1);
    wnoutrefresh(modules[module_in_focus].w);

    doupdate(); /* Do the actual cursor movements last, do reduce flicker. */
    napms(5); /* Relax CPU. */
  }

  /* Execution will never reach this. */
}

