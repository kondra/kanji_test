//gen rindex and kindex from radkfile and kradfile

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
		int stroke;
		int x, y; // coordinates in buttons table
		gchar *rad;
		gboolean state;

		guint8 stroke2;
		guint16 num;
		guint16 *kanji2;

		GArray *kanji;
} Radical;

typedef struct
{
		gchar *kanji;
		guint16 radicals2[20];
		guint8 num;
		gchar *radicals[20];
} KanjiDecomposition;

static GArray* radicals_process (gchar*, gsize);
static gsize radicals_load (gchar**, const gchar*, GError*);

static void new_format (GArray *rad, GArray *kan)
{
		int i, j;
		guint16 k;

// * building index
//		g_debug ("%d", kan->len);
		for (i = 0; i < kan->len; i++)
		{
//				g_debug ("\n%s - %d", g_array_index (kan, KanjiDecomposition, i).kanji, g_array_index (kan, KanjiDecomposition, i).num);
				for (k = 0; k < g_array_index (kan, KanjiDecomposition, i).num; k++)
				{
						for (j = 0; j < rad->len; j++)
						{
								if (strcmp (g_array_index (rad, Radical, j).rad, g_array_index (kan, KanjiDecomposition, i).radicals[k]) == 0)
								{
										if (i==58)
										{
												printf ("%d %s %s\n", j, g_array_index (rad, Radical, j).rad, g_array_index (kan, KanjiDecomposition, i).radicals[k]);
										}
										g_array_index (kan, KanjiDecomposition, i).radicals2[k] = j;
//										g_debug ("%s -%d", g_array_index (kan, KanjiDecomposition, i).radicals[k], j);
										break;
								}
						}
				}
//				if (i == 2)
//						exit (0);
		}

		//writind index file
		FILE *f;
		f = fopen ("kindex", "wb");
		fwrite (&(kan->len), sizeof (int), 1, f);
		for (i = 0; i < kan->len; i++)
		{
//				g_debug ("%s", g_array_index (kan, KanjiDecomposition, i).kanji);
//				g_debug ("%d", strlen (g_array_index (kan, KanjiDecomposition, i).kanji));

				fwrite (g_array_index (kan, KanjiDecomposition, i).kanji, sizeof (gchar), strlen (g_array_index (kan, KanjiDecomposition, i).kanji) + 1, f);
//				g_debug ("1");
				fwrite (&(g_array_index (kan, KanjiDecomposition, i).num), sizeof (guint8), 1, f);
//				g_debug ("2");
				fwrite (g_array_index (kan, KanjiDecomposition, i).radicals2, sizeof (guint16), g_array_index (kan, KanjiDecomposition, i).num, f);
//				g_debug ("3");
		}
		fclose (f);


// * test  reading
		FILE *f1;
		f = fopen ("kindex", "rb");
		f1 = fopen ("text", "wb");

		int l;
		guint8 num;
		gchar buf[10];
		guint16 rs[20];
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
				fprintf (f1, "%s : ", buf);
				fread (&num, sizeof (guint8), 1, f);
				fread (rs, sizeof (guint16), num, f);
				for (j = 0; j < num; j++){
						fprintf (f1, "%d ", rs[j]);
				}
				fprintf (f1, "\n");
		}

// build radical index

		for (i = 0; i < rad->len; i++)
		{
				g_array_index (rad, Radical, i).num = g_array_index (rad, Radical, i).kanji->len;
				g_array_index (rad, Radical, i).kanji2 = (guint16*) g_malloc0 (sizeof (guint16) * g_array_index (rad, Radical, i).num);
				for (j = 0; j < g_array_index (rad, Radical, i).kanji->len; j++)
				{
						for (k = 0; k < kan->len; k++)
						{
								if (g_utf8_collate (g_array_index (g_array_index (rad, Radical, i).kanji, gchar*, j), g_array_index (kan, KanjiDecomposition, k).kanji) == 0)
								{
										g_array_index (rad, Radical, i).kanji2[j] = k;
//										g_debug ("%d", g_array_index (rad, Radical, i).kanji2[j]);
//										g_debug ("%s -%d", g_array_index (kan, KanjiDecomposition, i).radicals[k], j);
										break;
								}
						}
				}
			//	for (j = 0; j < g_array_index (rad, Radical, i).num; j++)
			//			g_debug ("%d", g_array_index (rad, Radical, i).kanji2[j]);
//				g_array_index (rad, Radical, i).num = j;
				//g_debug ("%d - %d", i, j);
		}

//		write radial index file
		f = fopen ("rindex", "wb");
		fwrite (&(rad->len), sizeof (int), 1, f);
		for (i = 0; i < rad->len; i++)
		{
//				g_debug ("%s", g_array_index (kan, KanjiDecomposition, i).kanji);
//				g_debug ("%d", strlen (g_array_index (kan, KanjiDecomposition, i).kanji));

				fwrite (g_array_index (rad, Radical, i).rad, sizeof (gchar), strlen (g_array_index (rad, Radical, i).rad) + 1, f);
//				g_debug ("1");
				g_array_index (rad, Radical, i).stroke2 = (guint8) g_array_index (rad, Radical, i).stroke;
				fwrite (&(g_array_index (rad, Radical, i).stroke2), sizeof (guint8), 1, f);
				fwrite (&(g_array_index (rad, Radical, i).num), sizeof (guint16), 1, f);
//				g_debug ("%d", g_array_index (rad, Radical, i).num);
//				g_debug ("2");
				fwrite (g_array_index (rad, Radical, i).kanji2, sizeof (guint16), g_array_index (rad, Radical, i).num, f);
		//		for (j = 0; j < g_array_index (rad, Radical, i).num; j++)
		//				g_debug ("%d", g_array_index (rad, Radical, i).kanji2[j]);
//				g_debug ("3");
		}
		fclose (f);

//		FILE *f;
		f = fopen ("rindex", "rb");
		f1 = fopen ("text1", "wb");
		guint8 str;
		guint16 rs1[2000], num1;
		fread (&l, sizeof (int), 1, f);
		g_debug ("%d", l);
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
				fprintf (f1, "%s : ", buf);
				fread (&str, sizeof (guint8), 1, f);
				fprintf (f1, "%d ", str);
				fread (&num1, sizeof (guint16), 1, f);

				fread (rs1, sizeof (guint16), num1, f);
//				for (j = 0; j < num1; j++){
//						fprintf (f1, "%d ", rs1[j]);
//				}
				fprintf (f1, "\n");
		}
		fclose (f1);
		exit (0);
}

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
		arr->len = k ;

		return arr;
}


static gsize kanji_decomposition_load (gchar **contents, const gchar *filename, GError *error)
{
		gsize bytes;

		if (!g_file_test (filename, G_FILE_TEST_EXISTS))
				g_warning ("Error: file %s does not exist", filename);

		g_file_get_contents (filename, contents, &bytes, &error);
		
		if (error != NULL)
				g_warning ("Error: radicals_load function");

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
		//int i, j, k;

		const gchar filename1[] = "radkfile";
		const gchar filename2[] = "kradfile";
		gchar *contents1 = NULL;
		gchar *contents2 = NULL;
		gsize bytes1, bytes2;
		GError *error1 = NULL;
		GError *error2 = NULL;
		GArray *radicals, *kanji;

//		Memory Profiling
//		g_mem_set_vtable (glib_mem_profiler_table);
//		g_atexit (g_mem_profile);

		gtk_init (&argc, &argv);

		bytes1 = radicals_load (&contents1, filename1, error1);
		g_debug ("radicals loaded");
		radicals = radicals_process (contents1, bytes1);
		g_debug ("radicals processed");

		bytes2 = kanji_decomposition_load (&contents2, filename2, error2);
		g_debug ("decompositions loaded");
		kanji = kanji_decomposition_process (contents2, bytes2);
		g_debug ("decompositions processed");

		new_format (radicals, kanji);

		return 0;
}
