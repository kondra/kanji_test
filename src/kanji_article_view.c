#include <gtk/gtk.h>
#include <glib/gprintf.h>
#include <string.h>

#include "kanji.h"
#include "kanji_article_view.h"

static int kanji_meaning_parse (Kanji*, int, gchar*);

static int kanji_meaning_parse (Kanji *k, int n, gchar *dst)
{
		gchar *src = k->word_meaning[n];
		gint j, pos, state, cnt;
		for (pos = 0, j = 0; j < strlen (src); j++)
				if (src[j] == ';')
				{
						dst[pos++] = '1';
						dst[pos++] = '.';
						dst[pos++] = ' ';
						break;
				}
		for (j = 0, state = 1, cnt = 1; j < strlen (src); j++, state++)
		{
				dst[pos] = src[j];
				if (dst[pos] == ';')
				{
						cnt ++;
						state = 1;
						dst[pos] = 0;
						g_sprintf (dst + pos + 1, "%d.", cnt);
						pos += 1 + strlen (dst + pos + 1);
						continue;
				}
				if (dst[pos] == '(' && state <= 3)
				{
						state = 0;
						pos --;
						while (src[j] != ')')
								j++;
						continue;
				}
				pos ++;
		}
		dst[pos++] = 0;
		dst[pos++] = 1;
		return cnt;
}

void kanji_article_view (Kanji *kanji)
{
		GtkWidget *dialog;
		GtkWidget *scrolled;
		GtkWidget *textview;

		GtkTextBuffer *buffer;
		GtkTextIter start;

		static gchar *buf = NULL, *buf2 = NULL;

		gint i, j, f, k, off, prev = 0;
		gint spacing = 10;

		if (buf == NULL)
				buf = (gchar*) g_malloc0 (2000);
		if (buf2 == NULL)
				buf2 = (gchar*) g_malloc0 (2000);

		dialog = gtk_dialog_new_with_buttons ("Kanji Flash Card", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
		gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
		gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);
		gtk_widget_set_size_request (GTK_WIDGET (dialog), 600, 500);

		textview = gtk_text_view_new ();
		gtk_text_view_set_editable (GTK_TEXT_VIEW (textview), FALSE);
		gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (textview), FALSE);
		buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));

		gtk_text_buffer_create_tag (buffer, "kanji_font", "font", "46", NULL);
		gtk_text_buffer_create_tag (buffer, "on_font", "font", "20", NULL);
		gtk_text_buffer_create_tag (buffer, "on_color", "foreground", "#B86060", NULL);
		gtk_text_buffer_create_tag (buffer, "kun_color", "foreground", "#1E7681", NULL);
		gtk_text_buffer_create_tag (buffer, "kun_font", "font", "12", NULL);
		gtk_text_buffer_create_tag (buffer, "info_color", "foreground", "#6A6A6A", NULL);

		gtk_text_buffer_get_start_iter (buffer, &start);
		gtk_text_buffer_insert_with_tags_by_name (buffer, &start, kanji->kanji, -1, "kanji_font", NULL);

		gtk_text_buffer_get_end_iter (buffer, &start);
		g_sprintf (buf, " %d\n\n", kanji->kanji_stroke);
		gtk_text_buffer_insert (buffer, &start, buf, -1);

		gtk_text_buffer_get_end_iter (buffer, &start);
		g_sprintf (buf, " Radical %s\n Radical Stroke - %d\n JLPT Level - %d\n School Grade - %d\n\n", kanji->radical, kanji->radical_stroke, kanji->jlpt_level, kanji->grade);
		gtk_text_buffer_insert_with_tags_by_name (buffer, &start, buf, -1, "info_color", NULL);

		gtk_text_buffer_get_end_iter (buffer, &start);
		g_sprintf (buf, "    %s\n\n", kanji->on);
		gtk_text_buffer_insert_with_tags_by_name (buffer, &start, buf, -1, "on_font", "on_color", NULL);

		for (i = 0; i < kanji->num; i++)
		{
				gtk_text_buffer_get_end_iter (buffer, &start);
				f = kanji_meaning_parse (kanji, i, buf2);
				f --;
				g_sprintf (buf, " %s", kanji->word_writing[i]);
				gtk_text_buffer_insert_with_tags_by_name (buffer, &start, buf, -1, "kun_font", NULL);

				g_sprintf (buf, " [");
				for (j = 0, off = 2, prev = 0; j < strlen (kanji->word_reading[i]); j++)
				{
						if (kanji->word_reading[i][j] == ';')
						{
								kanji->word_reading[i][j] = 0;
								g_sprintf (buf + off, "%s] [", kanji->word_reading[i] + prev);
								off += j - prev + 3;
								prev = j + 1;
								kanji->word_reading[i][j] = ';';
						}
				}

				if (f)
						g_sprintf (buf + off, "%s]\n", kanji->word_reading[i] + prev);
				else
						g_sprintf (buf + off, "%s]", kanji->word_reading[i] + prev);
				
				gtk_text_buffer_get_end_iter (buffer, &start);
				gtk_text_buffer_insert_with_tags_by_name (buffer, &start, buf, -1, "kun_color", "kun_font", NULL);

				j = 0;
				while (buf2[j] != 1)
				{
						gtk_text_buffer_get_end_iter (buffer, &start);
						for (k = 0; f && k < spacing; k++)
						{
								gtk_text_buffer_insert (buffer, &start, " ", -1);
								gtk_text_buffer_get_end_iter (buffer, &start);
						}
						g_sprintf (buf, " %s\n", buf2 + j);
						gtk_text_buffer_insert (buffer, &start, buf, -1);
						j += strlen (buf) - 1;
				}
				gtk_text_buffer_get_end_iter (buffer, &start);
				gtk_text_buffer_insert (buffer, &start, "\n", -1);
		}

		scrolled = gtk_scrolled_window_new (NULL, NULL);
		gtk_container_add (GTK_CONTAINER (scrolled), textview);

		gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dialog)->vbox), scrolled);
		
		gtk_widget_show_all (dialog);
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
}
