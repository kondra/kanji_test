#include <gtk/gtk.h>

#include "kanji.h"
#include "add_kanji.h"
#include "view_kanji.h"

#include <string.h>
#include <stdlib.h>

gchar katakana[]="ァアィイゥウェエォオカガキギク\
		グケゲコゴサザシジスズセゼソゾタ\
		ダチヂッツヅテデトドナニヌネノハ\
		バパヒビピフブプヘベペホボポマミ\
		ムメモャヤュユョヨラリルレロヮワ\
		ヰヱヲンヴヵヶヷヸヹヺ・ーヽヾ";

gchar *rad[500];
gint strad[500];

static gboolean is_letter (gchar c)
{
		if (c >= 'a' && c <= 'z')
				return TRUE;
		if (c >= 'A' && c <= 'Z')
				return TRUE;
		return FALSE;
}

static gboolean is_digit (gchar c)
{
		if (c >= '0' && c <= '9')
				return TRUE;
		return FALSE;
}

static void load_radicals ()
{
		FILE *f;

		gchar buf[100];
		gchar kanji[10];
		gint i, j;
		gint strk;

		f = fopen ("radicals", "r");

		while (!feof (f))
		{
				fgets (buf, 20, f);
				if (buf[0] == '#')
						continue;

				for (i = 0; buf[i] != ' '; i++)
						kanji[i] = buf[i];
				kanji[i] = 0;

				sscanf (buf + i, "%d %d", &j, &strk);

				rad[j] = g_strdup (kanji);
				strad[j] = strk;
		}

		fclose (f);
}

static void kanjidic_parse (Kanji *tmp)
{
		FILE *f;

		gchar buf[10000];
		gchar kanji[10];
		gint i, j;
		gchar *p;
		gunichar uc;
		gint strk, len, grade, rnum, off;

		f = fopen ("kanjidic", "r");

		len = strlen (katakana);
		while (!feof (f))
		{
				fgets (buf, 10000, f);
				if (buf[0] == '#')
						continue;

				for (i = 0; buf[i] != ' '; i++)
						kanji[i] = buf[i];
				kanji[i] = 0;

				if (strcmp (kanji, tmp->kanji) != 0)
						continue;

				for (i = 0; buf[i] != 'S'; i++);
				for (j = i; buf[j] != ' '; j++);

				buf[j] = 0;
				sscanf (buf + i + 1, "%d", &strk);
				buf[j] = ' ';
		
				for (i = 0; buf[i] != 'G'; i++);
				for (j = i; buf[j] != ' '; j++);

				buf[j] = 0;
				sscanf (buf + i + 1, "%d", &grade);
				buf[j] = ' ';

				for (i = 0; buf[i] != 'B'; i++);
				for (j = i; buf[j] != ' '; j++);

				buf[j] = 0;
				sscanf (buf + i + 1, "%d", &rnum);
				buf[j] = ' ';

				tmp->grade = grade;
				tmp->kanji_stroke = strk;
				tmp->radical_stroke = strad[rnum];
				if (rad[rnum] == NULL)
						g_warning ("fail");
				tmp->radical = strdup (rad[rnum]);

				tmp->on = NULL; 
				for (off = 0, i = strlen (kanji); i < strlen (buf); i++)
				{
						if (is_digit (buf[i]) || is_letter (buf[i]) || buf[i] == ' ' || buf[i] == '.' || buf[i] == '-')
								continue;
						p = buf + i;
						uc = g_utf8_get_char (p);
						if (g_utf8_strchr (katakana, len, uc) == NULL)
						{
								g_debug ("%s", tmp->kanji);
								break;
						}
						for (; buf[i] != ' '; i++);
						buf[i] = 0;
						if (tmp->on == NULL)
								tmp->on = g_strdup_printf ("%s", p);
						else
								tmp->on = g_strdup_printf ("%s, %s", tmp->on, p);
						off += strlen (tmp->on);
				}
				if (tmp->on == NULL)
						tmp->on = g_strdup (" ");
				break;
		}

		fclose (f);
}

static GArray* edict_parse (GArray *arr)
{
		FILE *f;

		gchar name[100];
		gchar buf[3000], *p;
		gchar buft[1000], *cur;
		gchar kanji[100];

		gchar *writing[200];
		gchar *reading[200];
		gchar *meaning[200];

		gint state, i, j, k, num;
		gint u_len;

		gboolean flag;
		gunichar uc;

		Kanji *tmp;

		for (i = 0; i < 100; i++)
				writing[i] = meaning[i] = reading[i] = 0;

		for (j = 0; j < 181; j++)
		{
				sprintf (name, "utils/%d", j);
				f = fopen (name, "rb");
				k = 0;
			
				while (!feof (f))
				{
						fgets (buf, 2000, f);

						cur = buft;
						flag = FALSE;
						for (state = 0, p = buf, i = 0; i < strlen (buf); i++)
						{
								if (buf[i - 1] == '(' && buf[i] == 'P')
								{
										buf[i - 2] = 0;
										flag = TRUE;
										break;
								}
								if (buf[i] == ' ' && state == 0)
								{
										buf[i] = 0;
										writing[k] = g_strdup_printf ("%s", p);
										buf[i] = ' ';
										state = 1;
										continue;
								}
								if (buf[i] == '[' && state == 1)
								{
										p = buf + i + 1;
										state = 2;
										continue;
								}
								if (buf[i] == ']' && state == 2)
								{
										buf[i] = 0;
										reading[k] = g_strdup_printf ("%s", p);
										buf[i] = ']';
										state = 3;
										continue;
								}
								if (state == 3 && buf[i] == '/')
								{
										state = 4;
										p = buf + i + 1;
										continue;
								}
								if (buf[i] == '/' && ((is_letter(buf[i - 1]) && is_letter (buf[i+1])) || (buf[i - 1] == ')' && is_letter (buf[i + 1]))))
								{
										buf[i] = ',';
										continue;
								}
								if (buf[i] == '/' && buf[i + 1] == '(')
								{
										buf[i] = ';';
										buf[i + 1] = 0;
										sprintf (cur, "%s", p);
										cur += strlen (cur);
										buf[i + 1] = '(';
										p = buf + i + 1;
										continue;
								}
								if (buf[i] == '(' && is_digit (buf[i + 1]))
								{
										if (buf[i + 2] == ')')
										{
												if (buf[i - 1] == ' ')
												{
														buf[i - 1] = 0;
														sprintf (cur, "%s", p);
														cur += strlen (cur);
														p = buf + i + 3;
														buf[i - 1] = ' ';
														continue;
												}else
												{
														buf[i] = 0;
														sprintf (cur, "%s", p);
														cur += strlen (cur);
														p = buf + i + 3;
														buf[i] = '(';
														continue;
												}
										}
										if (is_digit (buf[i + 1]) && buf[i + 3] == ')')
										{
												if (buf[i - 1] == ' ')
												{
														buf[i - 1] = 0;
														sprintf (cur, "%s", p);
														cur += strlen (cur);
														p = buf + i + 4;
														buf[i - 1] = ' ';
														continue;
												}else
												{
														buf[i] = 0;
														sprintf (cur, "%s", p);
														cur += strlen (cur);
														p = buf + i + 4;
														buf[i] = '(';
														continue;
												}
												continue;
										}
								}
						}
						if (!flag)
						{
								buf[i - 2] = 0;
								sprintf (cur, "%s", p);
						}
						if (flag)
								*(cur - 1) = 0;
						meaning[k] = g_strdup (buft);
						k++;
				}
				num = k;
				uc = g_utf8_get_char (writing[0]);
				u_len = g_unichar_to_utf8 (uc, kanji);
				kanji[u_len] = 0;
				tmp = kanji_create (kanji, " ", " ", " ", 4, 1, 1, 1, num, writing, reading, meaning);

				kanjidic_parse (tmp);

				arr = kanji_array_append (arr, tmp);
				for (k = 0; k < num; k++)
				{
						g_free (reading[k]);
						reading[k] = NULL;
						g_free (writing[k]);
						writing[k] = NULL;
						g_free (meaning[k]);
						meaning[k] = NULL;
				}
				fclose (f);
		}

		return arr;
}

static void vocab_process (GArray *arr)
{
		FILE *f = fopen ("utils/nvocab2", "r");

		gchar buf[1000], outbuf[10], *p;
		gchar *reading[10000];
		gchar *writing[10000];
		gchar *meaning[10000];

		gint save[400][100];
		gint i, j, k, n, t, cnt, old;

		gunichar uc;
		gboolean flag;

		Collocations *tmp;

		for (i = 0; i < 400; i++)
				save[i][0] = 0;

		k = 0;
		while (!feof (f))
		{
				fgets (buf, 1000, f);
				n = strlen (buf);

				for (i = 0; buf[i] != ' '; i++);
				buf[i] = 0;
				reading[k] = g_strdup (buf);

				for (j = i + 1; buf[j] != ' '; j++);
				buf[j] = 0;
				writing[k] = g_strdup (buf + i + 1);

				buf[n - 2] = 0;
				meaning[k] = g_strdup (buf + j + 2);
				k++;
		}
		fclose (f);

		for (i = 0; i < k; i++)
		{
				cnt = 0;
				p = writing[i];
				n = g_utf8_strlen (p, -1);
				while (cnt < n)
				{
						uc = g_utf8_get_char (p);
						outbuf[g_unichar_to_utf8 (uc, outbuf)] = 0;
						p = g_utf8_next_char (p);
						cnt++;
						for (j = 0; j < arr->len; j++)
						{
								if (strcmp (outbuf, g_array_index (arr, Kanji, j).kanji) == 0)
								{
										flag = TRUE;
										for (t = 0; t < g_array_index (arr, Kanji, j).num; t++)
												if (strcmp (writing[i], g_array_index (arr, Kanji, j).word_writing[t]) == 0)
												{
														flag = FALSE;
														break;
												}
										if (flag)
										{
												save[j][++save[j][0]] = i;
										}
								}
						}
				}
		}

		for (i = 0; i < arr->len; i++)
		{
				if (save[i][0] == 0)
						continue;

				printf ("%s\n", g_array_index (arr, Kanji, i).kanji);

				if (g_array_index (arr, Kanji, i).col != NULL)
						old = g_array_index (arr, Kanji, i).col->num;
				else
						old = 0;

				tmp = (Collocations*) g_malloc0 (sizeof (Collocations));
		
				tmp->num = save[i][0] + old;
		
				tmp->writing = (gchar**) g_malloc0 (tmp->num * sizeof (gchar*));
				tmp->reading = (gchar**) g_malloc0 (tmp->num * sizeof (gchar*));
				tmp->meaning = (gchar**) g_malloc0 (tmp->num * sizeof (gchar*));
				tmp->level = (guint8*) g_malloc0 (tmp->num * sizeof (guint8));

				for (j = 0; j < old; j++)
				{
						tmp->level[j] = g_array_index (arr, Kanji, i).col->level[j];
						tmp->writing[j] = g_array_index (arr, Kanji, i).col->writing[j];
						tmp->reading[j] = g_array_index (arr, Kanji, i).col->reading[j];
						tmp->meaning[j] = g_array_index (arr, Kanji, i).col->meaning[j];
				}
				
				for (j = old + 1; j < old + save[i][0] + 1; j++)
				{
						tmp->level[j - 1] = 3;
						tmp->writing[j - 1] = writing[save[i][j - old]];
						tmp->reading[j - 1] = reading[save[i][j - old]];
						tmp->meaning[j - 1] = meaning[save[i][j - old]];

				}
				g_array_index (arr, Kanji, i).col = tmp;
		}
}

int main (int argc, char *argv[])
{
		GArray *arr;

		gtk_init (&argc, &argv);

		arr = kanji_array_load ("kanjidict");

		vocab_process (arr);

//		kanji_list_view (arr);

//		kanji_array_save ("kanjidict", arr);

		return 0;
}
