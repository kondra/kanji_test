//this program looks throw edict file and saves articles with one kanji and hiragana
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

gchar kanji[100][10];
gunichar arr[100], uc;
gchar buf[3000];
gchar str[100];
FILE *files[100];
gchar *p;
gint len, k;
gboolean flag;
gchar obuf[100];
gchar name[20];

gchar hiragana[]="ぁあぃいぅうぇえぉおかがきぎくぐけげこごさざしじすずせぜそぞだちぢっつづてでとどなにぬねのはばぱひびぴふぶぷへべぺ\
				  ほぼぽまみむめもゃやゅゆょよらりるれろゎわゐゑをんゔゕゖ゛゜ゝゞゟた";

int main (int argc, char *argv[])
{
		FILE *f;
		f = fopen ("kanji", "rb");
		int i = 0, j;
		gchar c;

		len = strlen (hiragana);

		while (!feof (f))
		{
				fgets (kanji[i], 100, f);
				sprintf (name, "%d", i);
				files[i] = fopen (name, "wb");
				arr[i] = g_utf8_get_char (kanji[i]);
				i++;
		}
		i--;

		fclose (f);

		f = fopen ("edict", "rb");

		int cnt = 0;
		int l;
		int g = 0;
		while (!feof (f))
		{
				fgets (buf, 2000, f);
				k = 0;
				while (buf[k] != ' ')
						k++;
				c = buf[k];
				buf[k] = 0;
				g_stpcpy (str, buf);
				buf[k] = c;

				for (j = 0; j < i; j++)
				{
						if (g_utf8_strchr (buf, 2000, arr[j]) != NULL)
						{
								p = str;
								flag = TRUE;
								l = g_utf8_strlen (p, 100);
								cnt = 0;
								while (cnt < l)
								{
										uc = g_utf8_get_char (p);
										if (cnt == 0 && uc != arr[j])
										{
												flag = FALSE;
												break;
										}
										if (g_utf8_strchr (hiragana, len, uc) == NULL && uc != arr[j])
										{
												flag = FALSE;
												break;
										}
										p = g_utf8_next_char (p);
										cnt ++;
								}
								if (flag == TRUE && l != 0)
								{
										fprintf (files[j], "%s\n", buf);
										g ++;
								}
						}
				}
		}

		for (j = 0; j < i; j++)
				fclose (files[j]);

		fclose (f);

		return 0;
}
