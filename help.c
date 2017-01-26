/* FRNT Framework (Help Module)
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



void help_event(frnt_module_t *m, int event)
{
  /* Unload self on key press. */
  m->manager_command = FRNT_COMMAND_UNLOAD;
  strncpy(m->manager_argument, "help", FRNT_MAX_ARG_LENGTH);
}



void help_tick(frnt_module_t *m)
{
}



void help_redraw(frnt_module_t *m, int focus)
{
  if (has_colors())
    wbkgd(m->w, COLOR_PAIR(focus ? FRNT_COLOR_FOCUS : FRNT_COLOR_COMMON));
  else
    wbkgd(m->w, focus ? A_BOLD : A_NORMAL);
  
  box(m->w, ACS_VLINE, ACS_HLINE);
  mvwprintw(m->w, 0, 1, "HELP");

  mvwprintw(m->w, 1, 1, "F5  = Move window in focus left.");
  mvwprintw(m->w, 2, 1, "F6  = Move window in focus down.");
  mvwprintw(m->w, 3, 1, "F7  = Move window in focus up.");
  mvwprintw(m->w, 4, 1, "F8  = Move window in focus right.");
  mvwprintw(m->w, 5, 1, "F9  = Change focus to another window.");
  mvwprintw(m->w, 6, 1, "F10 = Quit the FRNT framework.");
  mvwprintw(m->w, 7, 1, "F11 = Unload window in focus.");
  mvwprintw(m->w, 8, 1, "F12 = Save layout back to init file.");
#ifdef NCURSES_MOUSE_VERSION
  mvwprintw(m->w, 10, 1, "The mouse can be used to move windows,");
  mvwprintw(m->w, 11, 1, "or change focus with a click.");
#endif
}



void help_unload(void)
{
}



void help_init(frnt_module_t *m)
{
#ifdef NCURSES_MOUSE_VERSION
  m->size_y = 13;
#else
  m->size_y = 10;
#endif
  m->size_x = 45;
}

