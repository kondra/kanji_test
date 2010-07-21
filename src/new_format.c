//gen rindex and kindex from radkfile and kradfile

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
		gchar *rad;

		int stroke;
		guint8 stroke_new;

		GArray *kanji;
		guint16 num;
		guint16 *kanji_new;
} Radical;

typedef struct
{
		gchar *kanji;
		guint8 stroke;
		guint8 num;
		gchar *radicals[20];
		guint16 radicals_new[20];
} KanjiDecomposition;

static GArray* radicals_process (gchar*, gsize);
static gsize radicals_load (gchar**, const gchar*, GError*);

//gets strokes from kanjidic file
static void strokes (GArray *kan, gchar *filename)
{
		FILE *f;

		gchar buf[10000];
		gchar kanji[10];
		gint i, j, k;

		f = fopen (filename, "r");

		while (!feof (f))
		{
				fgets (buf, 10000, f);
				if (buf[0] == '#')
						continue;

				for (i = 0; buf[i] != ' '; i++)
						kanji[i] = buf[i];
				kanji[i] = 0;

				for (i = 0; buf[i] != 'S'; i++);
				for (j = i; buf[j] != ' '; j++);

				buf[j] = 0;
				sscanf (buf + i + 1, "%d", &k);
		
				for (i = 0; i < kan->len; i++)
						if (strcmp (kanji, g_array_index (kan, KanjiDecomposition, i).kanji) == 0)
								g_array_index (kan, KanjiDecomposition, i).stroke = k;
		}

		fclose (f);
}

static void kanji_index_build (GArray *rad, GArray *kan)
{
		guint i, j;
		guint16 k;

		for (i = 0; i < kan->len; i++)
				for (k = 0; k < g_array_index (kan, KanjiDecomposition, i).num; k++)
						for (j = 0; j < rad->len; j++)
								if (strcmp (g_array_index (rad, Radical, j).rad, g_array_index (kan, KanjiDecomposition, i).radicals[k]) == 0)
								{
										g_array_index (kan, KanjiDecomposition, i).radicals_new[k] = j;
										break;
								}
}

static void kanji_index_save (GArray *kan)
{
		FILE *f;
		guint i;

		f = fopen ("kindex", "wb");
		fwrite (&(kan->len), sizeof (int), 1, f);
		for (i = 0; i < kan->len; i++)
		{
				fwrite (g_array_index (kan, KanjiDecomposition, i).kanji, sizeof (gchar), strlen (g_array_index (kan, KanjiDecomposition, i).kanji) + 1, f);
				fwrite (&(g_array_index (kan, KanjiDecomposition, i).stroke), sizeof (guint8), 1, f);
				fwrite (&(g_array_index (kan, KanjiDecomposition, i).num), sizeof (guint8), 1, f);
				fwrite (g_array_index (kan, KanjiDecomposition, i).radicals_new, sizeof (guint16), g_array_index (kan, KanjiDecomposition, i).num, f);
		}
		
		fclose (f);
}

//parses binary output and prints kanji index in text format
static void kanji_index_test ()
{
		FILE *f, *ft;

		int l;
		guint i, j;
		guint8 num, strk;
		guint16 rs[20];
		gchar buf[10];

		f = fopen ("kindex", "rb");
		ft = fopen ("text", "wb");

		fread (&l, sizeof (int), 1, f);
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
				fprintf (ft, "%s : ", buf);

				fread (&strk, sizeof (guint8), 1, f);
				fread (&num, sizeof (guint8), 1, f);
				fread (rs, sizeof (guint16), num, f);

				for (j = 0; j < num; j++)
						fprintf (ft, "%d ", rs[j]);
				fprintf (ft, "\n");
		}

		fclose (f);
		fclose (ft);
}

static void radical_index_build (GArray *rad, GArray *kan)
{
		guint i, j;
		guint16 k;

		for (i = 0; i < rad->len; i++)
		{
				g_array_index (rad, Radical, i).num = g_array_index (rad, Radical, i).kanji->len;
				g_array_index (rad, Radical, i).kanji_new = (guint16*) g_malloc0 (sizeof (guint16) * g_array_index (rad, Radical, i).num);
				for (j = 0; j < g_array_index (rad, Radical, i).kanji->len; j++)
						for (k = 0; k < kan->len; k++)
								if (g_utf8_collate (g_array_index (g_array_index (rad, Radical, i).kanji, gchar*, j), g_array_index (kan, KanjiDecomposition, k).kanji) == 0)
								{
										g_array_index (rad, Radical, i).kanji_new[j] = k;
										break;
								}
		}
}

static void radical_index_save (GArray *rad)
{
		FILE *f;

		guint i;

		f = fopen ("rindex", "wb");

		fwrite (&(rad->len), sizeof (int), 1, f);
		for (i = 0; i < rad->len; i++)
		{
				fwrite (g_array_index (rad, Radical, i).rad, sizeof (gchar), strlen (g_array_index (rad, Radical, i).rad) + 1, f);
				g_array_index (rad, Radical, i).stroke_new = (guint8) g_array_index (rad, Radical, i).stroke;
				fwrite (&(g_array_index (rad, Radical, i).stroke_new), sizeof (guint8), 1, f);
				fwrite (&(g_array_index (rad, Radical, i).num), sizeof (guint16), 1, f);
				fwrite (g_array_index (rad, Radical, i).kanji_new, sizeof (guint16), g_array_index (rad, Radical, i).num, f);
		}

		fclose (f);
}

static void radical_index_test ()
{
		FILE *f;

		int l;
		guint i, j;
		guint8 str;
		guint16 rs1[2000], num1;

		f = fopen ("rindex", "rb");
		ft = fopen ("text1", "wb");

		fread (&l, sizeof (int), 1, f);
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
				fprintf (ft, "%s : ", buf);

				fread (&str, sizeof (guint8), 1, f);
				fprintf (ft, "%d ", str);
				
				fread (&num1, sizeof (guint16), 1, f);
				fread (rs1, sizeof (guint16), num1, f);

				fprintf (ft, "\n");
		}

		fclose (f);
		fclose (ft);
}

static void new_format (GArray *rad, GArray *kan)
{
		kanji_index_build (rad, kan);
		kanji_index_save (kan);
//		kanji_index_test ();

		radical_index_build (rad, kan);
		radical_index_save (rad);
//		radical_index_test ();
}

static gsize radicals_load (gchar **contents, const gchar *filename, GError *error)
{
		gsize bytes;

		if (!g_file_test (filename, G_FILE_TEST_EXISTS))
				g_error ("Error: file %s does not exist", filename);

		g_file_get_contents (filename, contents, &bytes, &error);
		
		if (error != NULL)
				g_error ("Error: radicals_load function");

		return bytes;
}

static GArray* radicals_process (gchar *contents, gsize bytes)
{
		GArray *arr, *kanji;
		gchar *next, *cur, *buf, c;
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
						g_array_index (arr, Radical, k).state = TRUE;
						contents[j] = ' ';
						sscanf (contents + j, "%d", &(g_array_index (arr, Radical, k).stroke));
						i = j;

						kanji = g_array_new (TRUE, TRUE, sizeof (gchar*));
						g_array_index (arr, Radical, k).kanji = kanji;

						for (; contents[i] != '\n'; i++);
						i++;
						while (1)
						{
								cur = contents + i;

								next = g_utf8_next_char (cur);
								i += next - cur ;

								c = *next;
								*next = 0;
								buf = g_strdup (cur);
								if (*buf != '\n')
										g_array_append_val (kanji, buf);
								*next = c;

								if (i == bytes - 1)
										break;

								if (contents[i] == '\n' && contents[i + 1] == '$')
								{
										i--;
										break;
								}
						}
						k++;
				}
		}
		arr->len = k;

		return arr;
}

static gsize kanji_decomposition_load (gchar **contents, const gchar *filename, GError *error)
{
		gsize bytes;

		if (!g_file_test (filename, G_FILE_TEST_EXISTS))
				g_error ("Error: file %s does not exist", filename);

		g_file_get_contents (filename, contents, &bytes, &error);
		
		if (error != NULL)
				g_error ("Error: radicals_load function");

		return bytes;
}

static GArray* kanji_decomposition_process (gchar *contents, gsize bytes)
{
		GArray *arr;
		gchar c;
		int i, j, k, cnt;

		arr = g_array_sized_new (TRUE, TRUE, sizeof (KanjiDecomposition), 6500);

		for (k = 0, i = 1; i < bytes; i++)
		{
				if (contents[i - 1] == '\n' && contents[i] != '#')
				{
						j = i;
						while (contents[j] != ' ')
								j++;
						contents[j] = 0;
						g_array_index (arr, KanjiDecomposition, k).kanji = g_strdup (contents + i);
						contents[j] = ' ';
						j = i = j + 3;

						cnt = 0;
						while (1)
						{
								while (contents[j] != ' ' && contents[j] != '\n')
										j++;

								c = contents[j];
								contents[j] = 0;
								
								g_array_index (arr, KanjiDecomposition, k).radicals[cnt] = g_strdup (contents + i);

								contents[j] = c;

								i = j + 1;
								if (j == bytes - 1)
										break;

								if (contents[j] == '\n')
								{
										i = j - 1;
										break;
								}

								j++;
								cnt ++;
						}
						g_array_index (arr, KanjiDecomposition, k).num = cnt + 1;
						k++;
				}
		}
		arr->len = k;

		return arr;
}

int main (int argc, char *argv[])
{
		GError *error1 = NULL, *error2 = NULL;;
		GArray *radicals, *kanji;

		gchar *contents1 = NULL, *contents2 = NULL;;
		gsize bytes1, bytes2;

		gtk_init (&argc, &argv);

		bytes1 = radicals_load (&contents1, "../data/radkfile", error1);
		g_message ("Radicals loaded");
		radicals = radicals_process (contents1, bytes1);
		g_message ("Radicals processed");

		bytes2 = kanji_decomposition_load (&contents2, "../data/kradfile", error2);
		g_message ("Decompositions loaded");
		kanji = kanji_decomposition_process (contents2, bytes2);
		g_message ("Decompositions processed");

		strokes (kanji, "../data/kanjidic");
		g_message ("Strokes information loaded");
		new_format (radicals, kanji);

		return 0;
}
