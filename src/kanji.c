#include <gtk/gtk.h>
#include <gio/gio.h>
#include <string.h>

#include "kanji.h"
/*
Kanji* kanji_create_empty (void)
{
		Kanji *k = (Kanji*) g_malloc0 (sizeof (Kanji));
		return k;
}
*/
Collocations* collocations_create (gint num, gchar **writings, gchar **readings, gchar **meanings, guint8 *levels, gboolean alloc)
{
		gint i;

		Collocations *tmp;

		tmp = (Collocations*) g_malloc0 (sizeof (Collocations));

		tmp->num = num;

		tmp->writing = (gchar**) g_malloc0 (num * sizeof (gchar*));
		tmp->reading = (gchar**) g_malloc0 (num * sizeof (gchar*));
		tmp->meaning = (gchar**) g_malloc0 (num * sizeof (gchar*));
		tmp->level = (guint8*) g_malloc0 (num * sizeof (guint8));
		
		for (i = 0; i < num; i++)
		{
				if (alloc)
				{
						tmp->writing[i] = g_strdup (writings[i]);
						tmp->reading[i] = g_strdup (readings[i]);
						tmp->meaning[i] = g_strdup (meanings[i]);
				}
				else
				{
						tmp->writing[i] = writings[i];
						tmp->reading[i] = readings[i];
						tmp->meaning[i] = meanings[i];
				}
				tmp->level[i] = levels[i];
		}

		return tmp;
}

Kanji* kanji_create (const gchar *kanji, const gchar *radical, const gchar *on, const gchar *meaning, 
				gint jlpt_level, gint grade, gint kanji_stroke, gint radical_stroke,
				gint num, gchar **writings, gchar **readings, gchar **meanings)
{
		int i;

		Kanji *k = (Kanji*) g_malloc0 (sizeof (Kanji));

		k->state = TRUE;
		k->col = NULL;

		k->word_writing = (gchar**) g_malloc0 (sizeof (gchar*) * num);
		k->word_reading = (gchar**) g_malloc0 (sizeof (gchar*) * num);
		k->word_meaning = (gchar**) g_malloc0 (sizeof (gchar*) * num);

		for (i = 0; i < num; i++)
		{
				if (writings[i] == NULL)
						k->word_writing[i] = g_strdup (" ");
				else
						k->word_writing[i] = g_strdup (writings[i]);

				if (readings[i] == NULL)
						k->word_reading[i] = g_strdup (" ");
				else
						k->word_reading[i] = g_strdup (readings[i]);

				if (meanings[i] == NULL)
						k->word_meaning[i] = g_strdup (" ");
				else
						k->word_meaning[i] = g_strdup (meanings[i]);
		}

		k->kanji = g_strdup (kanji);
		
		if (radical == NULL)
				k->radical = g_strdup (" ");
		else
				k->radical = g_strdup (radical);
		
		if (on == NULL)
				k->on = g_strdup (" ");
		else
				k->on = g_strdup (on);
		
		if (meaning == NULL)
				k->meaning = g_strdup (" ");
		else
				k->meaning = g_strdup (meaning);

		k->jlpt_level = jlpt_level;
		k->grade = grade;
		k->kanji_stroke = kanji_stroke;
		k->radical_stroke = radical_stroke;
		k->num = num;

		return k;
}
/*
GArray* kanji_array_append (GArray *arr, Kanji *k)
{
		return g_array_append_vals (arr, k, 1);
}
*/
GArray* kanji_array_load (const gchar *filename)
{
		gchar b, *buf;
		gint pos, len, curpos, i, cnum;
		gsize size;

		GError *error = NULL;
		GInputStream *in;
		GArray *arr = g_array_sized_new (TRUE, TRUE, sizeof (Kanji), 100);

		gchar **cwritings;
		gchar **creadings;
		gchar **cmeanings;
		guint8 *clevels;

		Kanji *k;

		if (!g_file_test (filename, G_FILE_TEST_EXISTS))
		{
				g_warning ("File %s does not exist", filename);
				kanji_array_free (arr);
				return NULL;
		}

		g_file_get_contents (filename, &buf, &size, &error);

		if (error != NULL)
		{
				g_warning ("(kanji_array_load): Unable to read file %s", error->message);
				kanji_array_free (arr);
				return NULL;
		}
		
		g_type_init ();

		in = g_memory_input_stream_new_from_data (buf, size, NULL);

		k = g_malloc0 (sizeof (Kanji));

		k->state = TRUE;
		k->col = NULL;

		pos = curpos = 0;

		while (pos < size)
		{
				g_input_stream_read (in, &(k->jlpt_level), sizeof (gint), NULL, NULL);
				g_input_stream_read (in, &(k->grade), sizeof (gint), NULL, NULL);
				g_input_stream_read (in, &(k->kanji_stroke), sizeof (gint), NULL, NULL);
				g_input_stream_read (in, &(k->radical_stroke), sizeof (gint), NULL, NULL);
				g_input_stream_read (in, &(k->num), sizeof (gint), NULL, NULL);
				g_input_stream_read (in, &(cnum), sizeof (gint), NULL, NULL);
				g_input_stream_read (in, &b, sizeof (gchar), NULL, NULL);

				pos = g_seekable_tell (G_SEEKABLE (in));

				k->word_writing = (gchar**) g_malloc0 (sizeof (gchar*) * k->num);
				k->word_reading = (gchar**) g_malloc0 (sizeof (gchar*) * k->num);
				k->word_meaning = (gchar**) g_malloc0 (sizeof (gchar*) * k->num);

				if (cnum)
				{
						cwritings = (gchar**) g_malloc (sizeof (gchar*) * cnum);
						creadings = (gchar**) g_malloc (sizeof (gchar*) * cnum);
						cmeanings = (gchar**) g_malloc (sizeof (gchar*) * cnum);
						clevels = (guint8*) g_malloc (sizeof (guint8) * cnum);
				}

				for (i = 0; i < k->num; i++)
				{
						k->word_writing[i] = g_strdup(buf + pos);
						pos += strlen (k->word_writing[i]) + 1;

						k->word_reading[i] = g_strdup(buf + pos);
						pos += strlen (k->word_reading[i]) + 1;

						k->word_meaning[i] = g_strdup(buf + pos);
						pos += strlen (k->word_meaning[i]) + 1;
				}

				for (i = 0; i < cnum; i++)
				{
						cwritings[i] = g_strdup(buf + pos);
						pos += strlen (cwritings[i]) + 1;

						creadings[i] = g_strdup(buf + pos);
						pos += strlen (creadings[i]) + 1;

						cmeanings[i] = g_strdup(buf + pos);
						pos += strlen (cmeanings[i]) + 1;

						clevels[i] = (guint8)*(buf + pos);
						pos++;
				}

				if (cnum)
						k->col = collocations_create (cnum, cwritings, creadings, cmeanings, clevels, FALSE);
				else
						k->col = NULL;

				len = strlen (buf + pos);
				k->kanji = g_strdup (buf + pos);
				if (k->kanji == NULL)
				{
						g_warning ("kanji: unable to parse file %s (kanji_array_load)", filename);
						kanji_array_free (arr);
						arr = NULL;
						break;
				}
				pos += len + 1;
				len = strlen (buf + pos);

				k->on = g_strdup (buf + pos);
				if (k->on == NULL)
				{
						g_warning ("kanji: unable to parse file %s (kanji_array_load)", filename);
						kanji_array_free (arr);
						arr = NULL;
						break;
				}
				pos += len + 1;

				len = strlen (buf + pos);
				k->meaning = g_strdup (buf + pos);
				if (k->meaning == NULL)
				{
						g_warning ("kanji: unable to parse file %s (kanji_array_load)", filename);
						kanji_array_free (arr);
						arr = NULL;
						break;
				}
				pos += len + 1;
				
				len = strlen (buf + pos);
				k->radical = g_strdup (buf + pos);
				if (k->radical == NULL)
				{
						g_warning ("kanji: unable to parse file %s (kanji_array_load)", filename);
						kanji_array_free (arr);
						arr = NULL;
						break;
				}
				pos += len + 1;

				g_seekable_seek (G_SEEKABLE (in), pos, G_SEEK_SET, NULL, NULL);
				
				arr = g_array_append_vals (arr, k, 1);
		}

		g_input_stream_close (in, NULL, NULL);
		g_object_unref (G_OBJECT (in));

		g_free (buf);
		g_free (k);

		return arr;
}

void kanji_array_save (const gchar *filename, GArray *arr)
{
		gint i, j;
		gchar b = 0;
		Kanji *k;

		GFile *f;
		GFileOutputStream *out;
		GError *error = NULL;

		f = g_file_new_for_path (filename);

		out = g_file_replace (f, NULL, FALSE, G_FILE_CREATE_NONE, NULL, &error);
		if (error != NULL)
				g_error ("Unable to write file: %s\n", error->message);

		k = &g_array_index (arr, Kanji, i = 0);
		while (i < arr->len)
		{
				if (k-> state == FALSE)
				{
						k = &g_array_index (arr, Kanji, ++i);
						continue;
				}

				//add error checking
				g_output_stream_write (G_OUTPUT_STREAM (out), &(k->jlpt_level), sizeof (gint), NULL, NULL);
				g_output_stream_write (G_OUTPUT_STREAM (out), &(k->grade), sizeof (gint), NULL, NULL);
				g_output_stream_write (G_OUTPUT_STREAM (out), &(k->kanji_stroke), sizeof (gint), NULL, NULL);
				g_output_stream_write (G_OUTPUT_STREAM (out), &(k->radical_stroke), sizeof (gint), NULL, NULL);
				g_output_stream_write (G_OUTPUT_STREAM (out), &(k->num), sizeof (gint), NULL, NULL);

				if (k->col == NULL)
				{
						gint x = 0;
						g_output_stream_write (G_OUTPUT_STREAM (out), &x, sizeof (gint), NULL, NULL);
				}
				else
						g_output_stream_write (G_OUTPUT_STREAM (out), &(k->col->num), sizeof (gint), NULL, NULL);

				g_output_stream_write (G_OUTPUT_STREAM (out), &b, sizeof (gchar), NULL, NULL);

				for (j = 0; j < k->num; j++)
				{
						g_output_stream_write (G_OUTPUT_STREAM (out), k->word_writing[j], sizeof (gchar) * (strlen (k->word_writing[j]) + 1), NULL, NULL);
						g_output_stream_write (G_OUTPUT_STREAM (out), k->word_reading[j], sizeof (gchar) * (strlen (k->word_reading[j]) + 1), NULL, NULL);
						g_output_stream_write (G_OUTPUT_STREAM (out), k->word_meaning[j], sizeof (gchar) * (strlen (k->word_meaning[j]) + 1), NULL, NULL);
				}

				if (k->col != NULL)
						for (j = 0; j < k->col->num; j++)
						{
								g_output_stream_write (G_OUTPUT_STREAM (out), k->col->writing[j], sizeof (gchar) * (strlen (k->col->writing[j]) + 1), NULL, NULL);
								g_output_stream_write (G_OUTPUT_STREAM (out), k->col->reading[j], sizeof (gchar) * (strlen (k->col->reading[j]) + 1), NULL, NULL);
								g_output_stream_write (G_OUTPUT_STREAM (out), k->col->meaning[j], sizeof (gchar) * (strlen (k->col->meaning[j]) + 1), NULL, NULL);
								g_output_stream_write (G_OUTPUT_STREAM (out), &(k->col->level[j]), sizeof (guint8), NULL, NULL);
						}

				g_output_stream_write (G_OUTPUT_STREAM (out), k->kanji, sizeof (gchar) * (strlen (k->kanji) + 1), NULL, NULL);
				g_output_stream_write (G_OUTPUT_STREAM (out), k->on, sizeof (gchar) * (strlen (k->on) + 1), NULL, NULL);
				g_output_stream_write (G_OUTPUT_STREAM (out), k->meaning, sizeof (gchar) * (strlen (k->meaning) + 1), NULL, NULL);
				g_output_stream_write (G_OUTPUT_STREAM (out), k->radical, sizeof (gchar) * (strlen (k->radical) + 1), NULL, NULL);

				k = &g_array_index (arr, Kanji, ++i);
		}

		g_output_stream_close (G_OUTPUT_STREAM (out), NULL, NULL);
		g_object_unref (G_OBJECT (out));
}

void kanji_array_free (GArray *arr)
{
		g_array_free (arr, TRUE);
}

void kanji_free (Kanji *k)
{
		gint i;

		g_free (k->kanji);
		g_free (k->on);
		g_free (k->meaning);
		g_free (k->radical);

		for (i = 0; i < k->num; i++)
		{
				g_free (k->word_reading[i]);
				g_free (k->word_writing[i]);
				g_free (k->word_meaning[i]);
		}

		k->state = FALSE;
}

void kanji_index_save (const gchar *filename, GArray *arr)
{
		IndexEntry *tmp;

		gint i;

		GFile *f;
		GFileOutputStream *out;
		GError *error = NULL;

		f = g_file_new_for_path (filename);

		out = g_file_replace (f, NULL, FALSE, G_FILE_CREATE_NONE, NULL, &error);
		if (error != NULL)
				g_error ("Unable to write file: %s\n", error->message);

		g_output_stream_write (G_OUTPUT_STREAM (out), &(arr->len), sizeof (int), NULL, NULL);

		tmp = &g_array_index (arr, IndexEntry, i = 0);
		while (i < arr->len)
		{
				g_output_stream_write (G_OUTPUT_STREAM (out), tmp->word, sizeof (gchar) * (strlen (tmp->word) + 1), NULL, NULL);
				g_output_stream_write (G_OUTPUT_STREAM (out), &(tmp->ind), sizeof (gint), NULL, NULL);

				tmp = &g_array_index (arr, IndexEntry, ++i);
		}

		g_output_stream_close (G_OUTPUT_STREAM (out), NULL, NULL);
		g_object_unref (G_OBJECT (out));
}

GArray* kanji_index_load (const gchar *filename)
{
		GArray *index;
		IndexEntry *tmp;

		gchar *buf;
		gsize size;
		gint l, i;
		guint pos;

		GError *error = NULL;
		GInputStream *in;

		if (!g_file_test (filename, G_FILE_TEST_EXISTS))
		{
				g_warning ("File %s does not exist", filename);
				return NULL;
		}

		g_file_get_contents (filename, &buf, &size, &error);

		if (error != NULL)
		{
				g_warning ("(kanji_index_load): Unable to read file %s", error->message);
				return NULL;
		}
		
		g_type_init ();

		in = g_memory_input_stream_new_from_data (buf, size, NULL);

		g_input_stream_read (in, &l, sizeof (int), NULL, NULL);

		index = g_array_sized_new (TRUE, TRUE, sizeof (IndexEntry), l); 
		index->len = l;

		for (i = 0; i < l; i++)
		{
				tmp = &g_array_index (index, IndexEntry, i);

				pos = g_seekable_tell (G_SEEKABLE (in));

				tmp->word = g_strdup (buf + pos);
				pos += strlen (buf + pos) + 1;
				
				g_seekable_seek (G_SEEKABLE (in), pos, G_SEEK_SET, NULL, NULL);
				
				g_input_stream_read (in, &(tmp->ind), sizeof (gint), NULL, NULL);
		}

		g_input_stream_close (in, NULL, NULL);
		g_object_unref (G_OBJECT (in));

		g_free (buf);

		return index;
}

static gint compare (gconstpointer a, gconstpointer b)
{
		return g_utf8_collate (((IndexEntry*)a)->word, ((IndexEntry*)b)->word);
}

static void entry_append (GArray *index, gint ind, gchar *word)
{
		IndexEntry *entry;
		entry = (IndexEntry*) g_malloc0 (sizeof (IndexEntry));
		entry->ind = ind;
		entry->word = word;
		g_array_append_vals (index, entry, 1);
}

GArray* kanji_index_generate (GArray *arr)
{
		Kanji *k;

		GArray *index = g_array_sized_new (TRUE, TRUE, sizeof (IndexEntry), 1000); 
		gint i, j;

		for (i = 0; i < arr->len; i++)
		{
				k = &g_array_index (arr, Kanji, i);
				entry_append (index, i, k->kanji);
				entry_append (index, i, k->on);
				for (j = 0; j < k->num; j++)
				{
						entry_append (index, i, k->word_reading[j]);
						entry_append (index, i, k->word_writing[j]);
						entry_append (index, i, k->word_meaning[j]);
				}
				if (k->col == NULL)
						continue;
				for (j = 0; j < k->col->num; j++)
				{
						entry_append (index, i, k->col->reading[j]);
						entry_append (index, i, k->col->writing[j]);
						entry_append (index, i, k->col->meaning[j]);
				}
		}

		g_array_sort (index, compare);

		return index;
}

//too slow
GArray* kanji_search (GArray *ind, const gchar *str)
{
		GArray *res = g_array_sized_new (TRUE, TRUE, sizeof (gint), 10);
		gint i;
		gboolean used[1000];

		for (i = 0; i < 1000; i++)
				used[i] = FALSE;

		for (i = 0; i < ind->len; i++)
		{
				if (used[g_array_index (ind, IndexEntry, i).ind] == FALSE && g_strstr_len (g_array_index (ind, IndexEntry, i).word, -1, str) != NULL)
				{
						g_array_append_vals (res, &g_array_index (ind, IndexEntry, i).ind, 1);
						used[g_array_index (ind, IndexEntry, i).ind] = TRUE;
				}
		}

		return res;
}
