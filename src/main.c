#include <gtk/gtk.h>

#include "kanji.h"
#include "add_kanji.h"
#include "view_kanji.h"

int main (int argc, char *argv[])
{
		GArray *arr;

		gboolean changed;

//		Memory Profiling
//		g_mem_set_vtable (glib_mem_profiler_table);
//		g_atexit (g_mem_profile);

		arr = kanji_array_load ("kanjidict");
		if (arr == NULL)
				arr = kanji_array_create;

		gtk_init (&argc, &argv);

		changed = kanji_list_view (arr);

		if (changed)
				kanji_array_save ("kanjidict", arr);
		return 0;
}
