#include <gtk/gtk.h>
#include <gio/gio.h>
#include <string.h>

#include "kanji.h"

Kanji* kanji_create_empty (void)
{
		Kanji *k = (Kanji*) g_malloc0 (sizeof (Kanji));
		return k;
}

Kanji* kanji_create (const gchar *kanji, const gchar *radical, const gchar *on, const gchar *meaning, 
				gint jlpt_level, gint grade, gint kanji_stroke, gint radical_stroke,
				gint num, gchar **writings, gchar **readings, gchar **meanings)
{
		int i;

		Kanji *k = (Kanji*) g_malloc0 (sizeof (Kanji));

		k->state = TRUE;

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
		
		k->radical = g_strdup (radical);
		
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

//TODO: write a macro
GArray* kanji_array_append (GArray *arr, Kanji *k)
{
		return g_array_append_vals (arr, k, 1);
}

GArray* kanji_array_load (const gchar *filename)
{
		gchar b, *buf;
		gint pos, len, curpos, i;
		gsize size;

		GError *error = NULL;
		GInputStream *in;
		GArray *arr = kanji_array_create;

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

		pos = curpos = 0;

		while (pos < size)
		{
				g_input_stream_read (in, &(k->jlpt_level), sizeof (gint), NULL, NULL);
				g_input_stream_read (in, &(k->grade), sizeof (gint), NULL, NULL);
				g_input_stream_read (in, &(k->kanji_stroke), sizeof (gint), NULL, NULL);
				g_input_stream_read (in, &(k->radical_stroke), sizeof (gint), NULL, NULL);
				g_input_stream_read (in, &(k->num), sizeof (gint), NULL, NULL);
				g_input_stream_read (in, &b, sizeof (gchar), NULL, NULL);

				pos = g_seekable_tell (G_SEEKABLE (in));

				k->word_writing = (gchar**) g_malloc0 (sizeof (gchar*) * k->num);
				k->word_reading = (gchar**) g_malloc0 (sizeof (gchar*) * k->num);
				k->word_meaning = (gchar**) g_malloc0 (sizeof (gchar*) * k->num);

				for (i = 0; i < k->num; i++)
				{
						k->word_writing[i] = g_strdup(buf + pos);
						pos += strlen (k->word_writing[i]) + 1;

						k->word_reading[i] = g_strdup(buf + pos);
						pos += strlen (k->word_reading[i]) + 1;

						k->word_meaning[i] = g_strdup(buf + pos);
						pos += strlen (k->word_meaning[i]) + 1;
				}

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
				
				arr = kanji_array_append (arr, k);
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
				g_output_stream_write (G_OUTPUT_STREAM (out), &b, sizeof (gchar), NULL, NULL);

				for (j = 0; j < k->num; j++)
				{
						g_output_stream_write (G_OUTPUT_STREAM (out), k->word_writing[j], sizeof (gchar) * (strlen (k->word_writing[j]) + 1), NULL, NULL);
						g_output_stream_write (G_OUTPUT_STREAM (out), k->word_reading[j], sizeof (gchar) * (strlen (k->word_reading[j]) + 1), NULL, NULL);
						g_output_stream_write (G_OUTPUT_STREAM (out), k->word_meaning[j], sizeof (gchar) * (strlen (k->word_meaning[j]) + 1), NULL, NULL);
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
