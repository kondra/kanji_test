#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include "kanji.h"

int main (int argc, char *argv[])
{
		Kanji *k1 = kanji_create ("行", "行", "い・く,ゆ・く,おこな・う,おこな・い", "コウ,ギョウ,*アン", "go,come,visit,attend,leave for,lead to", 1, 2, 6);
		Kanji *k2 = kanji_create ("参", "厶", "まい・る", "サン", "go,come,visit", 1, 4, 8);
		Kanji *k3 = kanji_create ("一", "一", " ", "イチ,イツ", "one,unity", 1, 1, 1);
		GArray *arr = kanji_array_create;
		arr = kanji_array_append (arr, k1);
		arr = kanji_array_append (arr, k2);
		arr = kanji_array_append (arr, k3);
		kanji_array_save ("output", arr);
		GArray *arr1 = kanji_array_load ("output");
		if (arr1 == NULL){
				g_error ("Sorry, an error has occured during reading file output");
		}
		kanji_array_save ("output1", arr1);

		int i = 0;
		Kanji *k = &g_array_index (arr1, Kanji, i = 0);
		while (k == NULL || !kanji_is_null (k))
		{
				printf ("%d:\n", i + 1);
				printf ("Kanji: %s\n", k->str);
				printf ("ON reading: %s\n", k->on);
				printf ("Kun reading: %s\n", k->kun);
				printf ("Meaning: %s\n", k->trans);
				printf ("Radical: %s\n", k->radical);
				printf ("Stroke count: %d\n", k->stroke);
				printf ("JLPT level: %d\n", k->jlpt_level);
				printf ("School grade: %d\n", k->grade);
				k = &g_array_index (arr1, Kanji, ++i);
		}
//		kanji_array_free (arr);

		return 0;
}

static gboolean kanji_is_null (Kanji *k)
{
		if (k->jlpt_level || k->grade || k->stroke || k->str != NULL || k->kun != NULL || k->on != NULL || k->trans != NULL || k->radical != NULL)
				return FALSE;
		return TRUE;
}

Kanji* kanji_create_empty (void)
{
		Kanji *k = (Kanji*) g_malloc0 (sizeof (Kanji));

		return k;
}

Kanji* kanji_create (const gchar *str, const gchar *radical, const gchar *kun, const gchar *on, const gchar *trans, int jlpt_level, int grade, int stroke)
{
		Kanji *k = (Kanji*) g_malloc0 (sizeof (Kanji));

		k->str = (gchar*) g_malloc0 (sizeof (gchar) * strlen (str));
		strcpy (k->str, str);
		
		k->radical = (gchar*) g_malloc0 (sizeof (gchar) * strlen (radical));
		strcpy (k->radical, radical);
		
		k->kun = (gchar*) g_malloc0 (sizeof (gchar) * strlen (kun));
		strcpy (k->kun, kun);
		
		k->on = (gchar*) g_malloc0 (sizeof (gchar) * strlen (on));
		strcpy (k->on, on);
		
		k->trans = (gchar*) g_malloc0 (sizeof (gchar) * strlen (trans));
		strcpy (k->trans, trans);

		k->jlpt_level = jlpt_level;
		k->grade = grade;
		k->stroke = stroke;

		return k;
}

GArray* kanji_array_append (GArray *arr, Kanji *k)
{
		return g_array_append_val (arr, *k);
}

GArray* kanji_array_load (const gchar *filename)
{
		FILE *f;
		GArray *arr = g_array_new (TRUE, TRUE, sizeof (Kanji));
		gchar b, *buf;
		int pos, len, curpos;
		struct stat statbuf;
		unsigned int size;
		Kanji *k;

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
				fread (&b, sizeof (gchar), 1, f);

				//add more smart reading
				curpos = ftell (f);
				fread (buf, sizeof (gchar), 1000, f);

				len = strlen (buf);
				k->str = g_malloc0 (len * sizeof (gchar));
				sscanf (buf, "%s", k->str);
				if (k->str == NULL)
				{
						g_warning ("error parsing file %s", filename);
						kanji_array_free (arr);
						arr = NULL;
						break;
				}
				pos = len + 1;

				len = strlen (buf + pos);
				k->kun = g_malloc0 (len * sizeof (gchar));
				sscanf (buf + pos, "%s", k->kun);
				if (k->kun == NULL)
				{
						g_warning ("error parsing file %s", filename);
						kanji_array_free (arr);
						arr = NULL;
						break;
				}
				pos += len + 1;

				len = strlen (buf + pos);
				k->on = g_malloc0 (len * sizeof (gchar));
				sscanf (buf + pos, "%s", k->on);
				if (k->on == NULL)
				{
						g_warning ("error parsing file %s", filename);
						kanji_array_free (arr);
						arr = NULL;
						break;
				}
				pos += len + 1;

				len = strlen (buf + pos);
				k->trans = g_malloc0 (len * sizeof (gchar));
				sscanf (buf + pos, "%s", k->trans);
				if (k->trans == NULL)
				{
						g_warning ("error parsing file %s", filename);
						kanji_array_free (arr);
						arr = NULL;
						break;
				}
				pos += len + 1;
				
				len = strlen (buf + pos);
				k->radical = g_malloc0 (len * sizeof (gchar));
				sscanf (buf + pos, "%s", k->radical);
				if (k->radical == NULL)
				{
						g_warning ("error parsing file %s", filename);
						kanji_array_free (arr);
						arr = NULL;
						break;
				}
				pos += len + 1;

				fseek (f, curpos + pos, SEEK_SET);
				
				arr = g_array_append_val (arr, *k);
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
				fwrite (&b, sizeof (gchar), 1, f);

				fwrite (k->str, sizeof (gchar), strlen (k->str), f);
				fwrite (&b, sizeof (gchar), 1, f);
				fwrite (k->kun, sizeof (gchar), strlen (k->kun), f);
				fwrite (&b, sizeof (gchar), 1, f);
				fwrite (k->on, sizeof (gchar), strlen (k->on), f);
				fwrite (&b, sizeof (gchar), 1, f);
				fwrite (k->trans, sizeof (gchar), strlen (k->trans), f);
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
		int i;
		Kanji *k;
		
		k = &g_array_index (arr, Kanji, i = 0);
		while (!kanji_is_null (k))
		{
				kanji_free (k);
				k = &g_array_index (arr, Kanji, ++i);
		}

		g_array_free (arr, TRUE);
}

void kanji_free (Kanji *k)
{
		g_free (k->str);
		g_free (k->kun);
		g_free (k->on);
		g_free (k->trans);
		g_free (k->radical);

//		g_free (k);
}
