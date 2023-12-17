#include "igui.h"


//#define DEMO0

#ifdef DEMO0
#include "demo0_rs.h"
DEF_Render(Demo0_RS);
#else
#include "test_render_surface.h"
DEF_Render(TestRenderSurface);
#endif




