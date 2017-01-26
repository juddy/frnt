/* FRNT Framework (Run Program Module)
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
#include <ctype.h>

#define RUN_PROGRAM_MAX_LENGTH 20



static char run_program_buffer[RUN_PROGRAM_MAX_LENGTH];
static int run_program_count;



void run_program_event(frnt_module_t *m, int event)
{
  if (isprint(event)) {
    run_program_buffer[run_program_count] = event;
    run_program_count++;
    if (run_program_count > RUN_PROGRAM_MAX_LENGTH - 1)
      run_program_count--;
    run_program_buffer[run_program_count] = '\0';
  } else if (event == KEY_BACKSPACE) {
    run_program_count--;
    if (run_program_count < 0)
      run_program_count = 0;
    run_program_buffer[run_program_count] = '\0';
  } else if (event == '\n') {
    if (strlen(run_program_buffer) > 0) {
      /* Shutdown ncurses temporarily and run program. */
      endwin();
      system(run_program_buffer);
      fprintf(stderr, "Press any key to return to FRNT.\n");
      fgetc(stdin);
      refresh();
    }
    m->manager_command = FRNT_COMMAND_UNLOAD;
    strncpy(m->manager_argument, "run_program", FRNT_MAX_ARG_LENGTH);
  }
}



void run_program_tick(frnt_module_t *m)
{
}



void run_program_redraw(frnt_module_t *m, int focus)
{
  int i;
  if (has_colors())
    wbkgd(m->w, COLOR_PAIR(focus ? FRNT_COLOR_FOCUS : FRNT_COLOR_COMMON));
  else
    wbkgd(m->w, focus ? A_BOLD : A_NORMAL);

  box(m->w, ACS_VLINE, ACS_HLINE);
  mvwprintw(m->w, 0, 1, "RUN PROGRAM");
  for (i = 0; i < RUN_PROGRAM_MAX_LENGTH; i++)
    mvwaddch(m->w, 1, i + 1, ' ');
  mvwprintw(m->w, 1, 1, "%s", run_program_buffer);
}



void run_program_unload(void)
{
}



void run_program_init(frnt_module_t *m)
{
  m->size_y = 3;
  m->size_x = RUN_PROGRAM_MAX_LENGTH + 2;

  run_program_buffer[0] = '\0';
  run_program_count = 0;
}

