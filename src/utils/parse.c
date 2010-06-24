#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

gchar buf[3000];
gchar *p;

int state, i;

int main (int argc, char *argv[])
{
		FILE *f;
		f = fopen ("40", "rb");

		while (!feof (f))
		{
				fgets (buf, 1000, f);

				for (state = 0, p = buf, i = 0; i < strlen (buf); i++)
				{
						if (buf[i] == ' ' && state == 0)
						{
								buf[i] = 0;
								printf ("Writing: %s\n", p);
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
								printf ("Reading: %s\n", p);
								buf[i] = ']';
								state = 3;
						}
						if (buf[i] == '/' && state == 3)
						{
								p = buf + i + 1;
								state = 4;
								continue;
						}
						if (buf[i] == '/' && state == 4)
						{
								buf[i] = 0;
								printf ("%s\n", p);
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
				}
				printf ("\n");
		}

		return 0;
}
