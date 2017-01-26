/* FRNT Framework (Log Viewer Module)
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

#define LOG_VIEWER_WIDTH 60
#define LOG_VIEWER_HEIGHT 5



static char log_viewer_buffer[LOG_VIEWER_HEIGHT][LOG_VIEWER_WIDTH];
static char log_viewer_filename[LOG_VIEWER_WIDTH];
static int log_viewer_count;
static FILE *log_viewer_fh;



void log_viewer_event(frnt_module_t *m, int event)
{
  if (isprint(event)) {
    log_viewer_filename[log_viewer_count] = event;
    log_viewer_count++;
    if (log_viewer_count > LOG_VIEWER_WIDTH - 1)
      log_viewer_count--;
    log_viewer_filename[log_viewer_count] = '\0';
  } else if (event == KEY_BACKSPACE) {
    log_viewer_count--;
    if (log_viewer_count < 0)
      log_viewer_count = 0;
    log_viewer_filename[log_viewer_count] = '\0';
  } else if (event == '\n') {
    if (strlen(log_viewer_filename) > 0) {
      if ((log_viewer_fh = fopen(log_viewer_filename, "r")) != NULL)
        return;
    }
    m->manager_command = FRNT_COMMAND_UNLOAD;
    strncpy(m->manager_argument, "log_viewer", FRNT_MAX_ARG_LENGTH);
  }
}



void log_viewer_tick(frnt_module_t *m)
{
  char temp_buffer[LOG_VIEWER_WIDTH];
  int i;
  if (log_viewer_fh != NULL) {
    if ((fgets(temp_buffer, LOG_VIEWER_WIDTH, log_viewer_fh)) != NULL) {
      for (i = 0; i < LOG_VIEWER_HEIGHT - 1; i++)
        strncpy(log_viewer_buffer[i], log_viewer_buffer[i + 1],
          LOG_VIEWER_WIDTH);
      strncpy(log_viewer_buffer[LOG_VIEWER_HEIGHT - 1], temp_buffer,
        LOG_VIEWER_WIDTH);
    }
  }
}



void log_viewer_redraw(frnt_module_t *m, int focus)
{
  int i;

  if (has_colors())
    wbkgd(m->w, COLOR_PAIR(focus ? FRNT_COLOR_FOCUS : FRNT_COLOR_COMMON));
  else
    wbkgd(m->w, focus ? A_BOLD : A_NORMAL);

  for (i = 0; i < LOG_VIEWER_HEIGHT; i++)
    mvwprintw(m->w, i + 3, 1, "%s", log_viewer_buffer[i]);

  box(m->w, ACS_VLINE, ACS_HLINE);
  mvwprintw(m->w, 0, 1, "LOG VIEWER");
  mvwaddch(m->w, 2, 0, ACS_LTEE);
  mvwaddch(m->w, 2, m->size_x - 1, ACS_RTEE);
  for (i = 1; i < LOG_VIEWER_WIDTH + 1; i++)
    mvwaddch(m->w, 2, i, ACS_HLINE);

  for (i = 0; i < LOG_VIEWER_WIDTH; i++)
    mvwaddch(m->w, 1, i + 1, ' ');
  mvwprintw(m->w, 1, 1, "%s", log_viewer_filename);
}



void log_viewer_unload(void)
{
  if (log_viewer_fh != NULL)
    fclose(log_viewer_fh);
}



void log_viewer_init(frnt_module_t *m)
{
  int i;

  m->size_y = LOG_VIEWER_HEIGHT + 4;
  m->size_x = LOG_VIEWER_WIDTH + 2;

  for (i = 0; i < LOG_VIEWER_HEIGHT; i++)
    log_viewer_buffer[i][0] = '\0';
  log_viewer_filename[0] = '\0';
  log_viewer_count = 0;
  log_viewer_fh = NULL;
}

