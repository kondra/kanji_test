#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <gtk/gtk.h>

#include "kanji.h"

gboolean kanji_is_null (Kanji *k)
{
		if (k->radical_stroke || k->jlpt_level || k->grade || k->stroke || k->str != NULL || k->kun != NULL || k->on != NULL || k->meaning != NULL || k->radical != NULL)
				return FALSE;
		return TRUE;
}

Kanji* kanji_create_empty (void)
{
		Kanji *k = (Kanji*) g_malloc0 (sizeof (Kanji));

		return k;
}

Kanji* kanji_create (const gchar *str, const gchar *radical, const gchar *kun, const gchar *on, const gchar *meaning, 
				int jlpt_level, int grade, int stroke, int radical_stroke)
{
		Kanji *k = (Kanji*) g_malloc0 (sizeof (Kanji));

		k->str = g_strdup_printf ("%s", str);
		
		k->radical = g_strdup_printf ("%s", radical);
		
		k->kun = g_strdup_printf ("%s", kun);
		
		k->on = g_strdup_printf ("%s", on);
		
		k->meaning = g_strdup_printf ("%s", meaning);

		k->jlpt_level = jlpt_level;
		k->grade = grade;
		k->stroke = stroke;
		k->radical_stroke = radical_stroke;

		return k;
}

//TODO: write a macro
GArray* kanji_array_append (GArray *arr, Kanji *k)
{
		return g_array_append_vals (arr, k, 1);
}

GArray* kanji_array_load (const gchar *filename)
{
		FILE *f;
		GArray *arr = kanji_array_create;
		gchar b, *buf;
		int pos, len, curpos;
		struct stat statbuf;
		unsigned int size;
		Kanji *k;

		if (!g_file_test (filename, G_FILE_TEST_EXISTS))
		{
				g_warning ("file %s not exist", filename);
				return NULL;
		}

		if (lstat (filename, &statbuf))
				g_error ("could not lstat %s: %s", filename, strerror (errno));
		size = statbuf.st_size;

		if ((f = fopen (filename, "rb")) == NULL)
		{
				g_warning ("kanji dict file not exist");
				return NULL;
		}
		
		buf = g_malloc0 (1000 * sizeof (gchar));
		k = g_malloc0 (sizeof (Kanji));

		pos = curpos = 0;

		while (pos + curpos < size)
		{
				fread (&(k->jlpt_level), sizeof (int), 1, f);
				fread (&(k->grade), sizeof (int), 1, f);
				fread (&(k->stroke), sizeof (int), 1, f);
				fread (&(k->radical_stroke), sizeof (int), 1, f);
				fread (&b, sizeof (gchar), 1, f);

				//add more smart reading
				curpos = ftell (f);
				fread (buf, sizeof (gchar), 1000, f);

				len = strlen (buf);
				k->str = g_strdup_printf ("%s", buf);
				if (k->str == NULL)
				{
						g_warning ("error parsing file %s", filename);
						kanji_array_free (arr);
						arr = NULL;
						break;
				}
				pos = len + 1;

				len = strlen (buf + pos);
				k->kun = g_strdup_printf ("%s", buf + pos);
				if (k->kun == NULL)
				{
						g_warning ("error parsing file %s", filename);
						kanji_array_free (arr);
						arr = NULL;
						break;
				}
				pos += len + 1;
				g_debug ("2");

				len = strlen (buf + pos);
				k->on = g_strdup_printf ("%s", buf + pos);
				if (k->on == NULL)
				{
						g_warning ("error parsing file %s", filename);
						kanji_array_free (arr);
						arr = NULL;
						break;
				}
				pos += len + 1;
				g_debug ("3");

				len = strlen (buf + pos);
				k->meaning = g_strdup_printf ("%s", buf + pos);
				if (k->meaning == NULL)
				{
						g_warning ("error parsing file %s", filename);
						kanji_array_free (arr);
						arr = NULL;
						break;
				}
				pos += len + 1;
				g_debug ("4");
				
				len = strlen (buf + pos);
				k->radical = g_strdup_printf ("%s", buf + pos);
				if (k->radical == NULL)
				{
						g_warning ("error parsing file %s", filename);
						kanji_array_free (arr);
						arr = NULL;
						break;
				}
				pos += len + 1;
				g_debug ("5");

				fseek (f, curpos + pos, SEEK_SET);
				
				arr = kanji_array_append (arr, k);
				g_debug ("6");
		}

		g_free (buf);
		g_free (k);
		fclose (f);

		return arr;
}

void kanji_array_save (const gchar *filename, GArray *arr)
{
		FILE *f;
		int i;
		gchar b = 0;
		Kanji *k;

		if ((f = fopen (filename, "wb")) == NULL)
				g_error ("can not open file");

		k = &g_array_index (arr, Kanji, i = 0);
		while (k == NULL || !kanji_is_null (k))
		{
				fwrite (&(k->jlpt_level), sizeof (int), 1, f);
				fwrite (&(k->grade), sizeof (int), 1, f);
				fwrite (&(k->stroke), sizeof (int), 1, f);
				fwrite (&(k->radical_stroke), sizeof (int), 1, f);
				fwrite (&b, sizeof (gchar), 1, f);

				fwrite (k->str, sizeof (gchar), strlen (k->str), f);
				fwrite (&b, sizeof (gchar), 1, f);
				fwrite (k->kun, sizeof (gchar), strlen (k->kun), f);
				fwrite (&b, sizeof (gchar), 1, f);
				fwrite (k->on, sizeof (gchar), strlen (k->on), f);
				fwrite (&b, sizeof (gchar), 1, f);
				fwrite (k->meaning, sizeof (gchar), strlen (k->meaning), f);
				fwrite (&b, sizeof (gchar), 1, f);
				fwrite (k->radical, sizeof (gchar), strlen (k->radical), f);
				fwrite (&b, sizeof (gchar), 1, f);

				//g_free (k);
				k = &g_array_index (arr, Kanji, ++i);
		}

		fclose (f);
}

void kanji_array_free (GArray *arr)
{
	/*	int i;
		Kanji *k;
		
		k = &g_array_index (arr, Kanji, i = 0);
		while (!kanji_is_null (k))
		{
				kanji_free (k);
				k = &g_array_index (arr, Kanji, ++i);
		}
	*/
		g_array_free (arr, TRUE);
}

//bad function ^_^
void kanji_free (Kanji *k)
{
		g_free (k->str);
		g_free (k->kun);
		g_free (k->on);
		g_free (k->meaning);
		g_free (k->radical);

//		g_free (k);
}
