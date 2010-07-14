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

int main (int argc, char *argv[])
{
		GArray *arr;

		gtk_init (&argc, &argv);

		arr = kanji_array_load ("kanjidict");
		if (arr == NULL)
				arr = kanji_array_create;

		load_radicals ();

		arr = edict_parse (arr);

		kanji_array_save ("kanjidict", arr);

		return 0;
}
