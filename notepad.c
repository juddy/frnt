/* FRNT Framework (Notepad Module)
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
#include <ctype.h>



static int notepad_y, notepad_x;



void notepad_event(frnt_module_t *m, int event)
{
  int y, x;

  getyx(m->w, y, x);

  if (isprint(event)) {
    waddch(m->w, event);
    if (x >= m->size_x - 2)
      wmove(m->w, y + 1, 1);
    getyx(m->w, y, x);
    if (y + 2 > m->size_y)
      wmove(m->w, 1, 1); /* Move back to top. */
  } else if (event == '\n') {
    wmove(m->w, y + 1, 1);
    getyx(m->w, y, x);
    if (y + 2 > m->size_y)
      wmove(m->w, 1, 1);
  } else if (event == KEY_BACKSPACE) {
    if (x <= 1) {
      if (y > 1) { /* Prevent moving to top. */
        wmove(m->w, y - 1, m->size_x - 2);
        waddch(m->w, ' ');
        wmove(m->w, y - 1, m->size_x - 2);
      }
    } else {
      wmove(m->w, y, x - 1);
      waddch(m->w, ' ');
      wmove(m->w, y, x - 1);
    }
  } else if (event == KEY_DOWN) {
    if (y < m->size_y - 2)
      wmove(m->w, y + 1, x);
  } else if (event == KEY_UP) {
    if (y > 1)
      wmove(m->w, y - 1, x);
  } else if (event == KEY_LEFT) {
    if (x > 1)
      wmove(m->w, y, x - 1);
  } else if (event == KEY_RIGHT) {
    if (x < m->size_x - 2)
      wmove(m->w, y, x + 1);
  }

  /* Need to save since redraw will cause more cursor movement. */
  getyx(m->w, notepad_y, notepad_x);
}



void notepad_tick(frnt_module_t *m)
{
}



void notepad_redraw(frnt_module_t *m, int focus)
{
  if (has_colors())
    wbkgd(m->w, COLOR_PAIR(focus ? FRNT_COLOR_FOCUS : FRNT_COLOR_COMMON));
  else
    wbkgd(m->w, focus ? A_BOLD : A_NORMAL);

  box(m->w, ACS_VLINE, ACS_HLINE);
  mvwprintw(m->w, 0, 1, "NOTEPAD");
  wmove(m->w, notepad_y, notepad_x);
}



void notepad_unload(void)
{
}



void notepad_init(frnt_module_t *m)
{
  m->size_y = 10;
  m->size_x = 30;
  notepad_y = 1;
  notepad_x = 1;
}

