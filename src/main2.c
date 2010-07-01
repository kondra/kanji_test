//it's a testing version of multiradical search

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
		guint x, y; // coordinates in buttons table
		gchar *rad;
		guint state;

		guint8 stroke;
		guint16 num;
		guint16 *kanji;
} Radical;

typedef struct
{
		gchar *kanji;
		guint state;
		
		guint8 num;
		guint16 *radicals;
} KanjiDecomposition;

typedef struct
{
		GArray *radicals;
		GArray *kanji;
		GtkWidget *label;
		GtkWidget *buttons[14][50];
} Widgets;

static void destroy (GtkWidget*, gpointer);
static void radical_button_toggled (GtkWidget*, Widgets*);
static GArray* radicals_process (const gchar*);
static GArray* kanji_decomposition_process (const gchar*);

static GArray* radicals_process (const gchar *filename)
{
		GArray *arr;
		FILE *f;
		f = fopen (filename, "rb");

		gchar buf[20];

		int l, i, j;

		fread (&l, sizeof (int), 1, f);
		arr = g_array_sized_new (TRUE, TRUE, sizeof (Radical), l);
		arr->len = l;

		for (i = 0; i < l; i++)
		{
				j = 0;
				while (1)	
				{
						fread (&buf[j], sizeof (gchar), 1, f);

						if (buf[j] == 0)
								break;

						j++;
				}
				g_array_index (arr, Radical, i).rad = g_strdup (buf);
				g_array_index (arr, Radical, i).state = 0;

				fread (&(g_array_index (arr, Radical, i).stroke), sizeof (guint8), 1, f);
				fread (&(g_array_index (arr, Radical, i).num), sizeof (guint16), 1, f);

				g_array_index (arr, Radical, i).kanji = (guint16*) g_malloc0 (sizeof (guint16) * g_array_index (arr, Radical, i).num);
				fread (g_array_index (arr, Radical, i).kanji, sizeof (guint16), g_array_index (arr, Radical, i).num, f);
		}
		fclose (f);

		return arr;
}


static GArray* kanji_decomposition_process (const gchar *filename)
{
		GArray *arr;
		FILE *f;
		f = fopen (filename, "rb");

		gchar buf[20];

		int l, i, j;

		fread (&l, sizeof (int), 1, f);
		arr = g_array_sized_new (TRUE, TRUE, sizeof (KanjiDecomposition), l);
		arr->len = l;

		for (i = 0; i < l; i++)
		{
				j = 0;
				while (1)	
				{
						fread (&buf[j], sizeof (gchar), 1, f);

						if (buf[j] == 0)
								break;

						j++;
				}
				g_array_index (arr, KanjiDecomposition, i).kanji = g_strdup (buf);
				g_array_index (arr, KanjiDecomposition, i).state = 1;

				fread (&(g_array_index (arr, KanjiDecomposition, i).num), sizeof (guint8), 1, f);
				
				g_array_index (arr, KanjiDecomposition, i).radicals = (guint16*) g_malloc0 (sizeof (guint16) * g_array_index (arr, KanjiDecomposition, i).num);
				fread (g_array_index (arr, KanjiDecomposition, i).radicals, sizeof (guint16), g_array_index (arr, KanjiDecomposition, i).num, f);
		}
		fclose (f);

		return arr;
}

static void radical_button_toggled (GtkWidget *button, Widgets *p)
{
		GArray *radicals = p->radicals;
		GArray *kanji = p->kanji;

		static guint count = 1;
		guint save;

		GtkLabel *label = GTK_LABEL (p->label);
		guint k = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (button), "index"));
		guint x, y;
		guint16 j, l, i;

		gchar buf[10000];
		guint off = 0;

		g_debug ("%d", count);
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
		{
				for (i = 0; i < g_array_index (radicals, Radical, k).num; i++)
				{
						l = g_array_index (radicals, Radical, k).kanji[i];
						if (g_array_index (kanji, KanjiDecomposition, l).state == count)
						{
								sprintf (buf + off, "%s ", g_array_index (kanji, KanjiDecomposition, l).kanji);
								off += strlen (g_array_index (kanji, KanjiDecomposition, l).kanji) + 1;

								g_array_index (kanji, KanjiDecomposition, l).state ++;
								for (j = 0; j < g_array_index (kanji, KanjiDecomposition, l).num; j++)
										g_array_index (radicals, Radical, g_array_index (kanji, KanjiDecomposition, l).radicals[j]).state = count;
						}
				}

				for (i = 0; i < radicals->len; i++)
				{
						x = g_array_index (radicals, Radical, i).x;
						y = g_array_index (radicals, Radical, i).y;
						gtk_widget_set_sensitive (p->buttons[x][y], g_array_index (radicals, Radical, i).state == count);
				}

				gtk_label_set_text (label, buf);
				count++;
		}
		else
		{
				if (count == 2)
				{
						for (i = 0; i < radicals->len; i++)
						{
								x = g_array_index (radicals, Radical, i).x;
								y = g_array_index (radicals, Radical, i).y;
								gtk_widget_set_sensitive (p->buttons[x][y], TRUE);
						}
						count--;
						buf[0] = 0;
//						gtk_label_set_text (label, " ");
						return;
				}
				save = count - 2;
				count = 1;

				for (k = 0; k < kanji->len; k++)
						g_array_index (kanji, KanjiDecomposition, k).state = 1;

				for (k = 0; k < radicals->len; k++)
				{
						x = g_array_index (radicals, Radical, k).x;
						y = g_array_index (radicals, Radical, k).y;
						if ( gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (p->buttons[x][y])) == FALSE)
								continue;

						g_debug ("%d", k);
						for (i = 0; i < g_array_index (radicals, Radical, k).num; i++)
						{
								l = g_array_index (radicals, Radical, k).kanji[i];
								if (g_array_index (kanji, KanjiDecomposition, l).state == count)
								{
										if (count == save)
										{
												sprintf (buf + off, "%s ", g_array_index (kanji, KanjiDecomposition, l).kanji);
												off += strlen (g_array_index (kanji, KanjiDecomposition, l).kanji) + 1;
										}

										g_array_index (kanji, KanjiDecomposition, l).state ++;
										for (j = 0; j < g_array_index (kanji, KanjiDecomposition, l).num; j++)
												g_array_index (radicals, Radical, g_array_index (kanji, KanjiDecomposition, l).radicals[j]).state = count;
								}
						}

						count++;
				}

				count--;
				for (i = 0; i < radicals->len; i++)
				{
						x = g_array_index (radicals, Radical, i).x;
						y = g_array_index (radicals, Radical, i).y;
						gtk_widget_set_sensitive (p->buttons[x][y], g_array_index (radicals, Radical, i).state == count);
				}
				count++;

				gtk_label_set_text (label, buf);
		}
}

int main (int argc, char *argv[])
{
		GtkWidget *window;
		GtkWidget *table, *scrolled;
		GtkWidget *vbox;

		guint16 i, j;
		guint k;

		const gchar filename1[] = "rindex";
		const gchar filename2[] = "kindex";
		GArray *radicals, *kanji;
		Widgets p;

//		Memory Profiling
//		g_mem_set_vtable (glib_mem_profiler_table);
//		g_atexit (g_mem_profile);

		gtk_init (&argc, &argv);

		radicals = radicals_process (filename1);
		g_debug ("radicals processed");

		kanji = kanji_decomposition_process (filename2);
		g_debug ("decompositions processed");

		window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title (GTK_WINDOW (window), "Kanji");
		gtk_widget_set_size_request (window, 450, 600);
		gtk_container_set_border_width (GTK_CONTAINER (window), 10);

		table = gtk_table_new (50, 14, FALSE);
		gtk_table_set_row_spacings (GTK_TABLE (table), 2);
		gtk_table_set_col_spacings (GTK_TABLE (table), 2);

		p.label = gtk_label_new (NULL);

		p.radicals = radicals;
		p.kanji = kanji;

		for (k = 0, i = 0; i < 14; i++)
		{
				for (j = 0; j < 50; j++)
				{
						p.buttons[i][j] = NULL;
						if (g_array_index (radicals, Radical, k).stroke == i + 1)
						{
								p.buttons[i][j] = gtk_toggle_button_new_with_label (g_array_index (radicals, Radical, k).rad);
								g_object_set_data (G_OBJECT (p.buttons[i][j]), "index", GUINT_TO_POINTER (k));
								g_signal_connect (G_OBJECT (p.buttons[i][j]), "toggled", G_CALLBACK (radical_button_toggled), (gpointer) &p);
								gtk_table_attach_defaults (GTK_TABLE (table), p.buttons[i][j], i, i + 1, j, j + 1);
								g_array_index (radicals, Radical, k).x = i;
								g_array_index (radicals, Radical, k).y = j;
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

		g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy), NULL);

		vbox = gtk_vbox_new (FALSE, 5);
		gtk_box_pack_start (GTK_BOX (vbox), p.label, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (vbox), scrolled, TRUE, TRUE, 5);

		gtk_container_add (GTK_CONTAINER (window), vbox);
		gtk_widget_show_all (window);

		gtk_main ();
		return 0;
}

static void destroy (GtkWidget *window, gpointer data)
{
		gtk_main_quit ();
}
