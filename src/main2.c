//it's a testing version of multiradical search

#include <gtk/gtk.h>
#include <glib/gprintf.h>
#include <gio/gio.h>
#include <string.h>

#include "kanji.h"
#include "kanji_article_view.h"

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
		gint index;
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
		GArray *dict;
		GArray *result;
		GArray *ind;
		GtkWidget *textview;
		GtkTextBuffer *buffer;
		GtkWidget *buttons[14][50];
		GtkWidget *spin_min, *spin_max;
		GtkWidget *search_entry;
} Data;

static void destroy (GtkWidget*, gpointer);
static void radical_button_toggled (GtkWidget*, Data*);
static void find_button_pressed (GtkWidget *button, Data *p);
static void clear_button_pressed (GtkWidget*, Data*);
static gboolean kanji_button_pressed (GtkWidget*, GdkEventButton*, Data*);
static void stroke_range_changed (GtkWidget*, Data*);
static GArray* radicals_process (const gchar*);
static GArray* kanji_decomposition_process (const gchar*);

static GArray* radicals_process (const gchar *filename)
{
		GArray *arr;

		GFile *f;
		GFileInputStream *in;
		GError *err = NULL;

		gchar buf[20];
		int l, i, j;

		f = g_file_new_for_path (filename);

		in = g_file_read (f, NULL, &err);
		if (err != NULL)
				g_error ("Unable to read file: %s\n", err->message);

		g_input_stream_read (G_INPUT_STREAM (in), &l, sizeof (int), NULL, NULL);
		arr = g_array_sized_new (TRUE, TRUE, sizeof (Radical), l);
		arr->len = l;

		for (i = 0; i < l; i++)
		{
				j = 0;
				while (1)	
				{
						g_input_stream_read (G_INPUT_STREAM (in), &buf[j], sizeof (gchar), NULL, NULL);

						if (buf[j] == 0)
								break;

						j++;
				}
				g_array_index (arr, Radical, i).rad = g_strdup (buf);
				g_array_index (arr, Radical, i).state = 0;

				g_input_stream_read (G_INPUT_STREAM (in), &(g_array_index (arr, Radical, i).stroke), sizeof (guint8), NULL, NULL);
				g_input_stream_read (G_INPUT_STREAM (in), &(g_array_index (arr, Radical, i).num), sizeof (guint16), NULL, NULL);

				g_array_index (arr, Radical, i).kanji = (guint16*) g_malloc0 (sizeof (guint16) * g_array_index (arr, Radical, i).num);
				g_input_stream_read (G_INPUT_STREAM (in), g_array_index (arr, Radical, i).kanji, sizeof (guint16) * g_array_index (arr, Radical, i).num, NULL, NULL);
		}

		g_input_stream_close (G_INPUT_STREAM (in), NULL, NULL);
		g_object_unref (G_OBJECT (in));

		return arr;
}


static GArray* kanji_decomposition_process (const gchar *filename)
{
		GArray *arr;

		GFile *f;
		GFileInputStream *in;
		GError *err = NULL;

		gchar buf[20];
		int l, i, j;

		f = g_file_new_for_path (filename);

		in = g_file_read (f, NULL, &err);
		if (err != NULL)
				g_error ("Unable to read file: %s\n", err->message);

		g_input_stream_read (G_INPUT_STREAM (in), &l, sizeof (int), NULL, NULL);

		arr = g_array_sized_new (TRUE, TRUE, sizeof (KanjiDecomposition), l);
		arr->len = l;

		for (i = 0; i < l; i++)
		{
				j = 0;
				while (1)	
				{
						g_input_stream_read (G_INPUT_STREAM (in), &buf[j], sizeof (gchar), NULL, NULL);

						if (buf[j] == 0)
								break;

						j++;
				}
				g_array_index (arr, KanjiDecomposition, i).kanji = g_strdup (buf);
				g_array_index (arr, KanjiDecomposition, i).state = 1;
				g_array_index (arr, KanjiDecomposition, i).index = -1;

				g_input_stream_read (G_INPUT_STREAM (in), &(g_array_index (arr, KanjiDecomposition, i).stroke), sizeof (guint8), NULL, NULL);
				g_input_stream_read (G_INPUT_STREAM (in), &(g_array_index (arr, KanjiDecomposition, i).num), sizeof (guint8), NULL, NULL);
				
				g_array_index (arr, KanjiDecomposition, i).radicals = (guint16*) g_malloc0 (sizeof (guint16) * g_array_index (arr, KanjiDecomposition, i).num);
				g_input_stream_read (G_INPUT_STREAM (in), g_array_index (arr, KanjiDecomposition, i).radicals, sizeof (guint16) * g_array_index (arr, KanjiDecomposition, i).num, NULL, NULL);
		}
		
		g_input_stream_close (G_INPUT_STREAM (in), NULL, NULL);
		g_object_unref (G_OBJECT (in));

		return arr;
}

static void kanji_index_set (GArray *dict, GArray *kanji)
{
		gint i, j;
		for (j = 0; j < kanji->len; j++)
		{
				for (i = 0; i < dict->len; i++)
						if (strcmp (g_array_index (kanji, KanjiDecomposition, j).kanji, g_array_index (dict, Kanji, i).kanji) == 0)
								g_array_index (kanji, KanjiDecomposition, j).index = i;
		}
}

static gint compare (gconstpointer a, gconstpointer b, gpointer k)
{
		register GArray *kanji = (GArray*) k;
		if (g_array_index (kanji, KanjiDecomposition, *(guint16*)a).stroke < g_array_index (kanji, KanjiDecomposition, *(guint16*)b).stroke)
				return -1;
		if (g_array_index (kanji, KanjiDecomposition, *(guint16*)a).stroke > g_array_index (kanji, KanjiDecomposition, *(guint16*)b).stroke)
				return 1;
		return 0;
}

static void radical_button_toggled (GtkWidget *button, Data *p)
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
						if (g_array_index (kanji, KanjiDecomposition, l).stroke > max 
										|| g_array_index (kanji, KanjiDecomposition, l).stroke < min 
										|| g_array_index (kanji, KanjiDecomposition, l).index == -1)
								continue;
						g_sprintf (buf + off, "%s ", g_array_index (kanji, KanjiDecomposition, l).kanji);
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
												g_array_index (result, guint16, len) = l;
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
						if (g_array_index (kanji, KanjiDecomposition, l).stroke > max 
										|| g_array_index (kanji, KanjiDecomposition, l).stroke < min
										|| g_array_index (kanji, KanjiDecomposition, l).index == -1)
								continue;
						g_sprintf (buf + off, "%s ", g_array_index (kanji, KanjiDecomposition, l).kanji);
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

static void clear_button_pressed (GtkWidget *button, Data *p)
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

static void find_button_pressed (GtkWidget *button, Data *p)
{
		GtkTextIter start, end;

		gchar buf[5000];
		guint off = 0;
		gint i, cnt;
		guint16 l;

		const gchar *str = gtk_entry_get_text (GTK_ENTRY (p->search_entry));
		GArray *res = kanji_search (p->ind, str);

		gint min = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (p->spin_min));
		gint max = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (p->spin_max));

		clear_button_pressed (NULL, p);

		for (i = 0; i < res->len; i++)
				g_array_index (p->result, guint16, i) = (guint16) g_array_index (res, gint, i);

		p->result->len = i;
		g_array_sort_with_data (p->result, compare, (gpointer) p->kanji);
		cnt = 0;
		off = 0;
		for (i = 0; i < p->result->len; i++)
		{
				l = g_array_index (p->result, guint16, i);
				if (g_array_index (p->dict, Kanji, l).kanji_stroke > max || g_array_index (p->dict, Kanji, l).kanji_stroke < min)
						continue;
				g_sprintf (buf + off, "%s ", g_array_index (p->dict, Kanji, l).kanji);
				off += strlen (g_array_index (p->dict, Kanji, l).kanji) + 1;
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
		gtk_text_buffer_insert_with_tags_by_name (p->buffer, &start, buf, -1, "kanji_font", NULL);

		g_array_free (res, TRUE);
}

static void stroke_range_changed (GtkWidget *spin, Data *p)
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
				if (g_array_index (p->kanji, KanjiDecomposition, l).stroke > max 
								|| g_array_index (p->kanji, KanjiDecomposition, l).stroke < min 
								|| g_array_index (p->kanji, KanjiDecomposition, l).index == -1)
						continue;
				g_sprintf (buf + off, "%s ", g_array_index (p->kanji, KanjiDecomposition, l).kanji);
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

static void destroy (GtkWidget *window, gpointer data)
{
		gtk_main_quit ();
}

static gboolean kanji_button_pressed (GtkWidget *textview, GdkEventButton *event, Data *p)
{
		static gboolean state = TRUE;
		if (state)
		{
				GdkCursor *cursor = gdk_cursor_new (GDK_LEFT_PTR);
				gdk_window_set_cursor(event->window, cursor);
				state = FALSE;
		}
		if (event->type == GDK_BUTTON_PRESS && event->button == 1)
		{
				GtkTextIter iter;
				gchar buf[7];
				gint x, y, l, i;
				gunichar uc;

				gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (textview), gtk_text_view_get_window_type (GTK_TEXT_VIEW (textview), event->window), event->x, event->y, &x, &y);
				gtk_text_view_get_iter_at_position (GTK_TEXT_VIEW (textview), &iter, NULL, x, y);

				uc = gtk_text_iter_get_char (&iter);
				l = g_unichar_to_utf8 (uc, buf);
				buf[l] = 0;

				for (i = 0; i < p->dict->len; i++)
						if (strcmp (buf, g_array_index (p->dict, Kanji, i).kanji) == 0)
								kanji_article_view (&g_array_index (p->dict, Kanji, i));

				return TRUE;
		}

		return FALSE;
}

int main (int argc, char *argv[])
{
		GtkWidget *window;
		GtkWidget *table;
		GtkWidget *scrolled_table, *scrolled_text;
		GtkWidget *textview;
		GtkWidget *vbox;
		GtkWidget *hbox1, *hbox2;

		GtkTextBuffer *buffer;

		guint16 i, j;
		guint k;

		const gchar filename1[] = "rindex";
		const gchar filename2[] = "kindex";

		GArray *radicals, *kanji, *dict, *ind;

		Data p;

//		Memory Profiling
//		g_mem_set_vtable (glib_mem_profiler_table);
//		g_atexit (g_mem_profile);

		gtk_init (&argc, &argv);

		radicals = radicals_process (filename1);
		g_message ("radicals processed");

		kanji = kanji_decomposition_process (filename2);
		g_message ("decompositions processed");

		dict = kanji_array_load ("kanjidict");
		g_message ("dictionary loaded");

		kanji_index_set (dict, kanji);
		g_message ("generated index");

		ind = kanji_index_load ("index");
		g_message ("word index loaded");

		window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title (GTK_WINDOW (window), "Kanji");
		gtk_widget_set_size_request (window, 900, 500);
		gtk_container_set_border_width (GTK_CONTAINER (window), 10);

		table = gtk_table_new (50, 14, FALSE);
		gtk_table_set_row_spacings (GTK_TABLE (table), 2);
		gtk_table_set_col_spacings (GTK_TABLE (table), 2);

		textview = gtk_text_view_new ();
		gtk_text_view_set_editable (GTK_TEXT_VIEW (textview), FALSE);
		gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (textview), FALSE);
		buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
		gtk_text_buffer_create_tag (buffer, "kanji_font", "font", "20", NULL);

		g_signal_connect (G_OBJECT (textview), "button-press-event", G_CALLBACK (kanji_button_pressed), (gpointer) &p);
		
		p.result = g_array_sized_new (TRUE, TRUE, sizeof (guint16), 2000);
		p.result->len = 0;
		p.cleared = FALSE;
		p.radicals = radicals;
		p.kanji = kanji;
		p.ind = ind;
		p.dict = dict;
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

		scrolled_table = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_table), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_container_set_border_width (GTK_CONTAINER (scrolled_table), 5);
		gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_table), table);

		scrolled_text = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_text), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_container_set_border_width (GTK_CONTAINER (scrolled_text), 5);
		gtk_container_add (GTK_CONTAINER (scrolled_text), textview);

		g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy), NULL);

		GtkWidget* reset_button = gtk_button_new_from_stock (GTK_STOCK_CLEAR);
		g_signal_connect (G_OBJECT (reset_button), "clicked", G_CALLBACK (clear_button_pressed), (gpointer) &p);

		GtkWidget* find_button = gtk_button_new_from_stock (GTK_STOCK_FIND);
		g_signal_connect (G_OBJECT (find_button), "clicked", G_CALLBACK (find_button_pressed), (gpointer) &p);

		GtkWidget* label1 = gtk_label_new ("Stroke range:");
		GtkWidget* label2 = gtk_label_new ("-");
		GtkWidget* label3 = gtk_label_new ("Search:");
		
		p.spin_min = gtk_spin_button_new_with_range (1, 30, 1);
		g_signal_connect (G_OBJECT (p.spin_min), "value-changed", G_CALLBACK (stroke_range_changed), (gpointer) &p);
		
		p.spin_max = gtk_spin_button_new_with_range (1, 30, 1);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (p.spin_max), 30);
		g_signal_connect (G_OBJECT (p.spin_max), "value-changed", G_CALLBACK (stroke_range_changed), (gpointer) &p);

		p.search_entry = gtk_entry_new ();

		hbox1 = gtk_hbox_new (FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox1), reset_button, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox1), label1, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox1), p.spin_min, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox1), label2, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox1), p.spin_max, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox1), label3, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox1), p.search_entry, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox1), find_button, FALSE, FALSE, 5);

		hbox2 = gtk_hbox_new (FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox2), scrolled_table, TRUE, TRUE, 5);
		gtk_box_pack_start (GTK_BOX (hbox2), scrolled_text, TRUE, TRUE, 5);

		vbox = gtk_vbox_new (FALSE, 5);
		gtk_box_pack_start (GTK_BOX (vbox), hbox1, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (vbox), hbox2, TRUE, TRUE, 5);

		gtk_container_add (GTK_CONTAINER (window), vbox);
		gtk_widget_show_all (window);

		gtk_main ();
		return 0;
}
