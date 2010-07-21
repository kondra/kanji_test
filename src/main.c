#include <gtk/gtk.h>

#include "kanji.h"
#include "add_kanji.h"
#include "view_kanji.h"

int main (int argc, char *argv[])
{
		GArray *arr;

		gboolean changed;

		gtk_init (&argc, &argv);

//		Memory Profiling
//		g_mem_set_vtable (glib_mem_profiler_table);
//		g_atexit (g_mem_profile);

		arr = kanji_array_load ("kanjidict");
		if (arr == NULL)
				arr = g_array_sized_new (TRUE, TRUE, sizeof (Kanji), 100);

		changed = kanji_list_view (arr);

		if (changed)
				kanji_array_save ("kanjidict", arr);

		return 0;
}
