#include "frnt.h"
void skeleton_event(frnt_module_t *m, int event) { }
void skeleton_tick(frnt_module_t *m) { }
void skeleton_redraw(frnt_module_t *m, int focus)
{
  if (has_colors())
    wbkgd(m->w, COLOR_PAIR(focus ? FRNT_COLOR_FOCUS : FRNT_COLOR_COMMON));
  else
    wbkgd(m->w, focus ? A_BOLD : A_NORMAL);
  box(m->w, ACS_VLINE, ACS_HLINE);
  mvwprintw(m->w, 0, 1, "SKELETON");
}
void skeleton_unload(void) { }
void skeleton_init(frnt_module_t *m)
{
  m->size_y = 10;
  m->size_x = 10;
}
