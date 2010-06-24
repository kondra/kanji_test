#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include "kanji.h"

static GArray* transform (GArray *arr)
{
		FILE *f;
		gchar name[100];
		gchar buf[3000], *p;
		//gchar rbuf[1000];

		gchar *writing[200];
		gchar *reading[200];
		gchar *meaning[200];

		int state, i, j, k, sz, num;

		Kanji *tmp;

		for (i = 0; i < 100; i++)
				writing[i] = meaning[i] = reading[i] = 0;

		for (j = 0; j < 80; j++)
		{
				sprintf (name, "dict/%d", j);
				f = fopen (name, "rb");
				k = 0;
			
				while (!feof (f))
				{
						fgets (buf, 2000, f);

						for (sz = 0, state = 0, p = buf, i = 0; i < strlen (buf); i++)
						{
								if (buf[i] == ' ' && state == 0)
								{
										buf[i] = 0;
										writing[k] = g_strdup_printf ("%s", p);
				//						printf ("Writing: %s\n", p);
										buf[i] = ' ';
										state = 1;
								}
								if (buf[i] == '[' && state == 1)
								{
										p = buf + i + 1;
										state = 2;
								}
								if (buf[i] == ']' && state == 2)
								{
										buf[i] = 0;
										reading[k] = g_strdup_printf ("%s", p);
				//						printf ("Reading: %s\n", p);
										buf[i] = ']';
										state = 3;
								}
								if (buf[i] == '/' && state == 3)
								{
										p = buf + i + 1;
										state = 4;
										meaning[k] = g_strdup_printf ("%s", p);
										meaning[k][strlen (p) - 1] = 0;
										break;
								}
/*								if (buf[i] == '/' && state == 4)
								{
										buf[i] = 0;
//										sprintf (rbuf + sz, "%s,", p);
//										sz += strlen (p)+1;
				//						printf ("%s\n", p);
										buf[i] = '/';
										p = buf + i + 1;
								}
								if (buf[i] == '(' && state == 4)
								{
										state = 5;
								}
								if (buf[i] == ')' && state == 5)
								{
										state = 4;
										p = buf + i + 2;
								}
*/
						}
						k++;
				//		printf ("\n");
				}
				num = k;
				tmp = kanji_create (writing[0], " ", " ", " ", 5, 1, 1, 1, num, writing, reading, meaning);
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

static void parse (GArray *arr)
{
		Kanji *tmp, *tmp2;
		int i, j, k, pos, num;
		gchar c;
		gchar *writing[100];
		gchar *reading[100];
		gchar *meaning[100];


		for (i = 0; i < 100; i++)
				writing[i] = meaning[i] = reading[i] = 0;

		tmp = &g_array_index (arr, Kanji, i = 0);
		while (i < arr->len)
		{
				num = 0;
				j = 0;
				pos = 0;
				for (k = 0; k < strlen (tmp->kun) + 1; k++)
				{
						if (tmp->kun[k] == '\n' || tmp->kun[k] == ',' || tmp->kun[k] == '\0')
						{
								c = tmp->kun[k];
								tmp->kun[k] = '\0';
								if (tmp->kun[pos] == ' ')
								{
										reading[j] = g_strdup ((tmp->kun) + pos + 1);
										writing[j] = g_strdup ((tmp->kun) + pos + 1);
								}
								else
								{
										reading[j] = g_strdup ((tmp->kun) + pos);
										writing[j] = g_strdup ((tmp->kun) + pos);
								}
								tmp->kun[k] = c;
								j++;
								pos = k + 1;
						}
				}
				num = j;
				
				pos = 0;
				j = 0;
				for (k = 0; k < strlen (tmp->meaning) + 1; k++)
				{
						if (tmp->meaning[k] == '\n' || tmp->meaning[k] == ',' || tmp->meaning[k] == '\0')
						{
								c = tmp->meaning[k];
								tmp->meaning[k] = '\0';
								if (tmp->meaning[pos] == ' ')
										meaning[j] = g_strdup ((tmp->meaning) + pos + 1);
								else
										meaning[j] = g_strdup ((tmp->meaning) + pos);
								tmp->meaning[k] = c;
								j++;
								pos = k + 1;
						}
				}
				num = MAX (num, j);

				tmp2 = kanji_create (tmp->str, tmp->radical, tmp->on, NULL, tmp->jlpt_level, tmp->grade, tmp->stroke, tmp->radical_stroke, num, 
								writing, reading, meaning);

				g_array_index (arr, Kanji, i) = *tmp2;
				tmp = &g_array_index (arr, Kanji, ++i);

				for (k = 0; k < num; k++)
				{
						g_free (reading[k]);
						reading[k] = NULL;
						g_free (writing[k]);
						writing[k] = NULL;
						g_free (meaning[k]);
						meaning[k] = NULL;
				}
		}
}

