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
		
		guint8 stroke;
		guint8 num;
		guint16 *radicals;
} KanjiDecomposition;

typedef struct
{
		gboolean cleared;
		GArray *radicals;
		GArray *kanji;
		GArray *result;
		GtkWidget *textview;
		GtkTextBuffer *buffer;
		GtkWidget *buttons[14][50];
		GtkWidget *spin_min, *spin_max;
} Widgets;

static void destroy (GtkWidget*, gpointer);
static void radical_button_toggled (GtkWidget*, Widgets*);
static void clear_button_pressed (GtkWidget*, Widgets*);
static void stroke_range_changed (GtkWidget*, Widgets*);
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

				fread (&(g_array_index (arr, KanjiDecomposition, i).stroke), sizeof (guint8), 1, f);
				fread (&(g_array_index (arr, KanjiDecomposition, i).num), sizeof (guint8), 1, f);
				
				g_array_index (arr, KanjiDecomposition, i).radicals = (guint16*) g_malloc0 (sizeof (guint16) * g_array_index (arr, KanjiDecomposition, i).num);
				fread (g_array_index (arr, KanjiDecomposition, i).radicals, sizeof (guint16), g_array_index (arr, KanjiDecomposition, i).num, f);
		}
		fclose (f);

		return arr;
}

gint compare (gconstpointer a, gconstpointer b, gpointer k)
{
		GArray *kanji = (GArray*) k;
		if (g_array_index (kanji, KanjiDecomposition, *(guint16*)a).stroke < g_array_index (kanji, KanjiDecomposition, *(guint16*)b).stroke)
				return -1;
		if (g_array_index (kanji, KanjiDecomposition, *(guint16*)a).stroke > g_array_index (kanji, KanjiDecomposition, *(guint16*)b).stroke)
				return 1;
		return 0;
}

static void radical_button_toggled (GtkWidget *button, Widgets *p)
{
		GArray *radicals = p->radicals;
		GArray *kanji = p->kanji;
		GtkTextIter start, end;
		GtkTextBuffer *buffer = p->buffer;

		static guint count = 1;
		guint save;

		guint k = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (button), "index"));
		guint x, y, cnt;
		guint16 j, l, i;

		GArray *result = p->result;
		gchar buf[5000];
		guint off = 0;
		gint len = 0;

		gint min = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (p->spin_min));
		gint max = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (p->spin_max));

		if (p->cleared)
		{
				p->cleared = FALSE;
				count = 1;
		}

		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
		{
				for (i = 0; i < g_array_index (radicals, Radical, k).num; i++)
				{
						l = g_array_index (radicals, Radical, k).kanji[i];
						if (g_array_index (kanji, KanjiDecomposition, l).state == count)
						{
								g_array_index (result, guint16, len) = l;
								len++;

								g_array_index (kanji, KanjiDecomposition, l).state ++;
								for (j = 0; j < g_array_index (kanji, KanjiDecomposition, l).num; j++)
										g_array_index (radicals, Radical, g_array_index (kanji, KanjiDecomposition, l).radicals[j]).state = count;
						}
				}

				for (i = 0; i < radicals->len; i++)
				{
						x = g_array_index (radicals, Radical, i).x;
						y = g_array_index (radicals, Radical, i).y;
						gtk_widget_set_sensitive (p->buttons[x][y], (g_array_index (radicals, Radical, i).state == count));
				}

				result->len = len;
				g_array_sort_with_data (result, compare, (gpointer) kanji);
				cnt = 0;
				for (i = 0; i < len; i++)
				{
						l = g_array_index (result, guint16, i);
						if (g_array_index (kanji, KanjiDecomposition, l).stroke > max || g_array_index (kanji, KanjiDecomposition, l).stroke < min)
								continue;
						sprintf (buf + off, "%s ", g_array_index (kanji, KanjiDecomposition, l).kanji);
						off += strlen (g_array_index (kanji, KanjiDecomposition, l).kanji) + 1;
						cnt++;
						if (cnt >= 10)
						{
								buf[off++] = '\n';
								cnt = 0;
						}
				}
				buf[off] = 0;

				gtk_text_buffer_get_start_iter (buffer, &start);
				gtk_text_buffer_get_end_iter (buffer, &end);
				gtk_text_buffer_delete (buffer, &start, &end);
				gtk_text_buffer_insert_with_tags_by_name (buffer, &start, buf, -1, "kanji_font", NULL);
				count++;
		}
		else
		{
				for (k = 0; k < kanji->len; k++)
						g_array_index (kanji, KanjiDecomposition, k).state = 1;

				for (i = 0; i < radicals->len; i++)
						g_array_index (radicals, Radical, i).state = 0;

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
						gtk_text_buffer_get_start_iter (buffer, &start);
						gtk_text_buffer_get_end_iter (buffer, &end);
						gtk_text_buffer_delete (buffer, &start, &end);
						p->result->len = 0;
						return;
				}

				save = count - 2;
				count = 1;

				for (k = 0; k < radicals->len; k++)
				{
						x = g_array_index (radicals, Radical, k).x;
						y = g_array_index (radicals, Radical, k).y;
						if ( gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (p->buttons[x][y])) == FALSE)
								continue;

						cnt = 0;
						for (i = 0; i < g_array_index (radicals, Radical, k).num; i++)
						{
								l = g_array_index (radicals, Radical, k).kanji[i];
								if (g_array_index (kanji, KanjiDecomposition, l).state == count)
								{
										if (count == save)
										{
												g_array_index (result, guint16, l) = len;
												len++;
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

				result->len = len;
				g_array_sort_with_data (result, compare, (gpointer) kanji);
				cnt = 0;
				off = 0;
				for (i = 0; i < len; i++)
				{
						l = g_array_index (result, guint16, i);
						if (g_array_index (kanji, KanjiDecomposition, l).stroke > max || g_array_index (kanji, KanjiDecomposition, l).stroke < min)
								continue;
						sprintf (buf + off, "%s ", g_array_index (kanji, KanjiDecomposition, l).kanji);
						off += strlen (g_array_index (kanji, KanjiDecomposition, l).kanji) + 1;
						cnt++;
						if (cnt >= 10)
						{
								buf[off++] = '\n';
								cnt = 0;
						}
				}
				buf[off] = 0;

				gtk_text_buffer_get_start_iter (buffer, &start);
				gtk_text_buffer_get_end_iter (buffer, &end);
				gtk_text_buffer_delete (buffer, &start, &end);
				gtk_text_buffer_insert_with_tags_by_name (buffer, &start, buf, -1, "kanji_font", NULL);
		}
}

static void clear_button_pressed (GtkWidget *button, Widgets *p)
{
		GtkTextIter start, end;
		guint i, x, y;
		p->cleared = TRUE;
		for (i = 0; i < p->kanji->len; i++)
				g_array_index (p->kanji, KanjiDecomposition, i).state = 1;

		for (i = 0; i < p->radicals->len; i++)
		{
				g_array_index (p->radicals, Radical, i).state = 0;
				x = g_array_index (p->radicals, Radical, i).x;
				y = g_array_index (p->radicals, Radical, i).y;
				gtk_widget_set_sensitive (p->buttons[x][y], TRUE);
				if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (p->buttons[x][y])) == TRUE)
						gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (p->buttons[x][y]), FALSE);
		}
		gtk_text_buffer_get_start_iter (p->buffer, &start);
		gtk_text_buffer_get_end_iter (p->buffer, &end);
		gtk_text_buffer_delete (p->buffer, &start, &end);
}

static void stroke_range_changed (GtkWidget *spin, Widgets *p)
{
		GtkTextIter start, end;

		gint i, off, cnt;
		gint min, max;
		guint16 l;
		gchar buf[5000];

		if (spin == p->spin_min)
		{
				min = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (p->spin_min));
				max = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (p->spin_max));
				if (max < min)
				{
						gtk_spin_button_set_value (GTK_SPIN_BUTTON (p->spin_max), min);
						return;
				}
		}
		if (spin == p->spin_max)
		{
				min = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (p->spin_min));
				max = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (p->spin_max));
				if (max < min)
				{
						gtk_spin_button_set_value (GTK_SPIN_BUTTON (p->spin_min), max);
						return;
				}
		}

		if (p->result->len == 0)
				return;

		cnt = off = 0;
		buf[0] = 0;
		for (i = 0; i < p->result->len; i++)
		{
				l = g_array_index (p->result, guint16, i);
				if (g_array_index (p->kanji, KanjiDecomposition, l).stroke > max || g_array_index (p->kanji, KanjiDecomposition, l).stroke < min)
						continue;
				sprintf (buf + off, "%s ", g_array_index (p->kanji, KanjiDecomposition, l).kanji);
				off += strlen (g_array_index (p->kanji, KanjiDecomposition, l).kanji) + 1;
				cnt++;
				if (cnt >= 10)
				{
						buf[off++] = '\n';
						cnt = 0;
				}
		}
		buf[off] = 0;

		gtk_text_buffer_get_start_iter (p->buffer, &start);
		gtk_text_buffer_get_end_iter (p->buffer, &end);
		gtk_text_buffer_delete (p->buffer, &start, &end);
		gtk_text_buffer_get_start_iter (p->buffer, &start);
		gtk_text_buffer_insert_with_tags_by_name (p->buffer, &start, buf, -1, "kanji_font", NULL);
}
int main (int argc, char *argv[])
{
		GtkWidget *window;
		GtkWidget *table, *scrolled1, *scrolled2, *textview;
		GtkWidget *vbox, *hbox1, *hbox2, *reset_button;
		GtkWidget *label1, *label2;
		GtkTextBuffer *buffer;

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
		gtk_widget_set_size_request (window, 900, 600);
		gtk_container_set_border_width (GTK_CONTAINER (window), 10);

		table = gtk_table_new (50, 14, FALSE);
		gtk_table_set_row_spacings (GTK_TABLE (table), 2);
		gtk_table_set_col_spacings (GTK_TABLE (table), 2);

		//p.label = gtk_label_new (NULL);
		//gtk_label_set_selectable (GTK_LABEL (p.label), TRUE);

		textview = gtk_text_view_new ();
		gtk_text_view_set_editable (GTK_TEXT_VIEW (textview), FALSE);
		buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
		gtk_text_buffer_create_tag (buffer, "kanji_font", "font", "20", NULL);

		p.result = g_array_sized_new (TRUE, TRUE, sizeof (guint16), 2000);
		p.result->len = 0;
		p.cleared = FALSE;
		p.radicals = radicals;
		p.kanji = kanji;
		p.buffer = buffer;
		p.textview = textview;

		for (k = 0, i = 0; i < 14; i++)
		{
				for (j = 0; j < 50; j++)
				{
						p.buttons[i][j] = NULL;
						if (g_array_index (radicals, Radical, k).stroke == i + 1 || (i == 13 && g_array_index (radicals, Radical, k).stroke >= i + 1))
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

		scrolled1 = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_container_set_border_width (GTK_CONTAINER (scrolled1), 5);
		gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled1), table);

		scrolled2 = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled2), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_container_set_border_width (GTK_CONTAINER (scrolled2), 5);
		gtk_container_add (GTK_CONTAINER (scrolled2), textview);

		g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy), NULL);

		reset_button = gtk_button_new_from_stock (GTK_STOCK_CLEAR);
		g_signal_connect (G_OBJECT (reset_button), "clicked", G_CALLBACK (clear_button_pressed), (gpointer) &p);

		label1 = gtk_label_new ("Stroke range:");
		label2 = gtk_label_new ("-");
		p.spin_min = gtk_spin_button_new_with_range (1, 30, 1);
		g_signal_connect (G_OBJECT (p.spin_min), "value-changed", G_CALLBACK (stroke_range_changed), (gpointer) &p);
		p.spin_max = gtk_spin_button_new_with_range (1, 30, 1);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (p.spin_max), 30);
		g_signal_connect (G_OBJECT (p.spin_max), "value-changed", G_CALLBACK (stroke_range_changed), (gpointer) &p);

		hbox1 = gtk_hbox_new (FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox1), reset_button, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox1), label1, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox1), p.spin_min, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox1), label2, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox1), p.spin_max, FALSE, FALSE, 5);

		hbox2 = gtk_hbox_new (FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox2), scrolled1, TRUE, TRUE, 5);
		gtk_box_pack_start (GTK_BOX (hbox2), scrolled2, TRUE, TRUE, 5);

		vbox = gtk_vbox_new (FALSE, 5);
		gtk_box_pack_start (GTK_BOX (vbox), hbox1, FALSE, FALSE, 5);
//		gtk_box_pack_start (GTK_BOX (vbox), p.label, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (vbox), hbox2, TRUE, TRUE, 5);

		gtk_container_add (GTK_CONTAINER (window), vbox);
		gtk_widget_show_all (window);

		gtk_main ();
		return 0;
}

static void destroy (GtkWidget *window, gpointer data)
{
		gtk_main_quit ();
}
