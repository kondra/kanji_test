//it's a testing version of multiradical search

#include <gtk/gtk.h>

typedef struct
{
		int stroke;
		int x, y; // coordinates in buttons table
		gchar *rad;
} Radical;

static void destroy (GtkWidget*, gpointer);

static gsize radicals_load (gchar **contents, const gchar *filename, GError *error)
{
		gsize bytes;

		if (!g_file_test (filename, G_FILE_TEST_EXISTS))
				g_warning ("Error: file %s does not exist", filename);

		g_file_get_contents (filename, contents, &bytes, &error);
		
		if (error != NULL)
				g_warning ("Error: radicals_load function");

		return bytes;
}

static GArray* radicals_process (gchar *contents, gsize bytes)
{
		GArray *arr;
		int i, j, k;

		arr = g_array_sized_new (TRUE, TRUE, sizeof (Radical), 500);

		for (k = 0, i = 1; i < bytes; i++)
		{
				if (contents[i] == '\n' && contents[i + 1] == '$')
				{
						i += 3;
						j = i;
						while (contents[j] != ' ')
								j++;
						contents[j] = 0;
						g_array_index (arr, Radical, k).rad = g_strdup (contents + i);
						contents[j] = ' ';
						sscanf (contents + j, "%d", &(g_array_index (arr, Radical, k).stroke));
						i = j;
						k++;
				}
		}

		return arr;
}

int main (int argc, char *argv[])
{
		GtkWidget *window;
		GtkWidget *table, *scrolled;
		GtkWidget *buttons[50][14];

		int i, j, k;

		const gchar filename[] = "radkfile";
		gchar *contents = NULL;
		gsize bytes;
		GError *error = NULL;
		GArray *arr;

//		Memory Profiling
//		g_mem_set_vtable (glib_mem_profiler_table);
//		g_atexit (g_mem_profile);

		gtk_init (&argc, &argv);

		bytes = radicals_load (&contents, filename, error);
		g_debug ("radicals loaded");
		arr = radicals_process (contents, bytes);
		g_debug ("radicals processed");

		window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title (GTK_WINDOW (window), "Kanji");
		gtk_widget_set_size_request (window, 450, 600);
		gtk_container_set_border_width (GTK_CONTAINER (window), 10);

		table = gtk_table_new (50, 14, FALSE);
		gtk_table_set_row_spacings (GTK_TABLE (table), 2);
		gtk_table_set_col_spacings (GTK_TABLE (table), 2);

		for (k = 0, i = 0; i < 14; i++)
		{
				for (j = 0; j < 50; j++)
				{
						if (g_array_index (arr, Radical, k).stroke == i + 1)
						{
								buttons[i][j] = gtk_button_new ();
								gtk_button_set_label (GTK_BUTTON (buttons[i][j]), g_array_index (arr, Radical, k).rad);
								gtk_table_attach_defaults (GTK_TABLE (table), buttons[i][j], i, i + 1, j, j + 1);
								g_array_index (arr, Radical, k).x = i;
								g_array_index (arr, Radical, k).x = j;
								k++;
						}
						else
								break;
				}
		}

		scrolled = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_container_set_border_width (GTK_CONTAINER (scrolled), 5);
		gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled), table);

//		scrolled = gtk_viewport_new (NULL, NULL);
//		gtk_container_add (GTK_CONTAINER (scrolled), table);

		g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy), NULL);

		gtk_container_add (GTK_CONTAINER (window), scrolled);
		gtk_widget_show_all (window);

		gtk_main ();
		return 0;
}

static void destroy (GtkWidget *window, gpointer data)
{
		gtk_main_quit ();
}
