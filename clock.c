/* FRNT Framework (Clock Module)
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
#include <time.h>



void clock_event(frnt_module_t *m, int event)
{
}



void clock_tick(frnt_module_t *m)
{
  time_t t;
  struct tm *tm;
  t = time(NULL);
  tm = localtime(&t);
  mvwprintw(m->w, 0, 0, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
}



void clock_redraw(frnt_module_t *m, int focus)
{
  if (has_colors())
    wbkgd(m->w, COLOR_PAIR(focus ? FRNT_COLOR_FOCUS : FRNT_COLOR_COMMON));
  else
    wbkgd(m->w, focus ? A_BOLD : A_NORMAL);
}



void clock_unload(void)
{
}



void clock_init(frnt_module_t *m)
{
  m->size_y = 1;
  m->size_x = 8;
}

