/* FRNT Framework (Calculator Module)
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



static int calculator_input_number, calculator_stored_number,
  calculator_ghost_number;
static char calculator_operator;
static int calculator_cursor_y, calculator_cursor_x;

static char calculator_buttons[4][4] = {
  {'1', '2', '3', '+'},
  {'4', '5', '6', '-'},
  {'7', '8', '9', '*'},
  {'0', '=', 'C', '/'},
};



void calculator_event(frnt_module_t *m, int event)
{
  /* Check button activity first. */
  if (event == '\n') {
    /* "Fake" a new event. */
    event = calculator_buttons[calculator_cursor_y][calculator_cursor_x];
  } else if (event == KEY_DOWN) {
    calculator_cursor_y++;
    if (calculator_cursor_y > 3)
      calculator_cursor_y = 3;
  } else if (event == KEY_UP) {
    calculator_cursor_y--;
    if (calculator_cursor_y < 0)
      calculator_cursor_y = 0;
  } else if (event == KEY_LEFT) {
    calculator_cursor_x--;
    if (calculator_cursor_x < 0)
      calculator_cursor_x = 0;
  } else if (event == KEY_RIGHT) {
    calculator_cursor_x++;
    if (calculator_cursor_x > 3)
      calculator_cursor_x = 3;
  }

  /* Check for actual input afterwards. */
  if (isdigit(event)) {
    calculator_ghost_number = 0;
    calculator_input_number *= 10;
    /* ASCII to number conversion trick. */
    calculator_input_number += (event - 0x30);
    if (calculator_input_number > 9999999) {
      /* Do not allow values larger than display. */
      calculator_input_number -= (event - 0x30);
      calculator_input_number /= 10;
    }
  } else if (event == '+' || event == '-' || event == '*' || event == '/') {
    calculator_stored_number = calculator_input_number;
    calculator_ghost_number = calculator_input_number;
    calculator_input_number = 0;
    calculator_operator = event;
  } else if (event == '=') {
    switch (calculator_operator) {
    case '+':
      calculator_ghost_number = calculator_stored_number +
        calculator_input_number;
      break;
    case '-':
      calculator_ghost_number = calculator_stored_number -
        calculator_input_number;
      break;
    case '*':
      calculator_ghost_number = calculator_stored_number *
        calculator_input_number;
      break;
    case '/':
      if (calculator_input_number != 0) {
        calculator_ghost_number = calculator_stored_number
          / calculator_input_number;
      } else {
        /* Prevent division by zero. */
        calculator_ghost_number = 0; 
      }
      break;
    }
    if (calculator_ghost_number > 9999999)
      calculator_ghost_number = 9999999;
    if (calculator_ghost_number < -999999)
      calculator_ghost_number = -999999;
    calculator_input_number = calculator_ghost_number;
    calculator_operator = '\0';
  } else if (event == 'C' || event == 'c') {
    calculator_input_number = 0;
    calculator_ghost_number = 0;
  }


}



void calculator_tick(frnt_module_t *m)
{
}



void calculator_redraw(frnt_module_t *m, int focus)
{
  int i, j;

  if (has_colors())
    wbkgd(m->w, COLOR_PAIR(focus ? FRNT_COLOR_FOCUS : FRNT_COLOR_COMMON));
  else
    wbkgd(m->w, focus ? A_BOLD : A_NORMAL);

  box(m->w, ACS_VLINE, ACS_HLINE);

  /* Draw the lines. */
  mvwaddch(m->w, 2, 0, ACS_LTEE);
  mvwaddch(m->w, 2, 1, ACS_HLINE);
  mvwaddch(m->w, 2, 2, ACS_TTEE);
  mvwaddch(m->w, 2, 3, ACS_HLINE);
  mvwaddch(m->w, 2, 4, ACS_TTEE);
  mvwaddch(m->w, 2, 5, ACS_HLINE);
  mvwaddch(m->w, 2, 6, ACS_TTEE);
  mvwaddch(m->w, 2, 7, ACS_HLINE);
  mvwaddch(m->w, 2, 8, ACS_RTEE);
  for (i = 3; i < 10; i++) {
    if (i % 2 == 0) {
      mvwaddch(m->w, i, 0, ACS_LTEE);
      mvwaddch(m->w, i, m->size_x - 1, ACS_RTEE);
      for (j = 1; j < 8; j++)
        mvwaddch(m->w, i, j, (j % 2 == 1) ? ACS_HLINE : ACS_PLUS);
    } else {
      for (j = 0; j < 8; j += 2)
        mvwaddch(m->w, i, j, ACS_VLINE);
    }
  }
  mvwaddch(m->w, 10, 2, ACS_BTEE);
  mvwaddch(m->w, 10, 4, ACS_BTEE);
  mvwaddch(m->w, 10, 6, ACS_BTEE);

  /* Draw the buttons. */
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      if (i == calculator_cursor_y && j == calculator_cursor_x)
        wattron(m->w, A_REVERSE);
      mvwaddch(m->w, (i * 2) + 3, (j * 2) + 1, calculator_buttons[i][j]);
      if (i == calculator_cursor_y && j == calculator_cursor_x)
        wattroff(m->w, A_REVERSE);
    }
  }

  /* Draw the display. */
  mvwprintw(m->w, 1, 1, "%7d", (calculator_ghost_number != 0) ?
    calculator_ghost_number : calculator_input_number);
}



void calculator_unload(void)
{
}



void calculator_init(frnt_module_t *m)
{
  m->size_y = 11;
  m->size_x = 9;

  calculator_cursor_y = 0;
  calculator_cursor_x = 0;
  calculator_input_number = 0;
  calculator_stored_number = 0;
  calculator_ghost_number = 0;
  calculator_operator = '\0';
}

