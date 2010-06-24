#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

int main()
{
		FILE *f, *f1;
		gchar name[20], name1[20];
		gchar buf[1200];

		int k, j;

		for (j = 0; j < 80; j++)
		{
				sprintf (name, "%d", j);
				sprintf (name1, "%dx", j);
				f = fopen (name, "rb");
				f1 = fopen (name1, "wb");
				k = 0;
			
				while (!feof (f))
				{
						fgets (buf, 1000, f);
						if (strlen (buf) == 1)
								continue;
						fputs (buf, f1);

				}
				fclose (f);
				fclose (f1);
		}
		return 0;
}
