//testing version of parser
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

gchar buf[3000];
gchar buft[3000];
gchar *p;

int state, i;

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

int main (int argc, char *argv[])
{
		gchar *cur;
		gboolean flag;
		FILE *f;
		f = fopen ("1", "rb");

		while (!feof (f))
		{
				fgets (buf, 2000, f);

				cur = buft;
				flag = FALSE;
				for (state = 0, p = buf, i = 0; i < strlen (buf); i++)
				{
						if (buf[i - 1] == '(' && buf[i] == 'P')
						{
								buf[i - 1] = 0;
								flag = TRUE;
								break;
						}
						if (buf[i] == ' ' && state == 0)
						{
								buf[i] = 0;
								sprintf (cur, "Writing: %s\n", p);
								cur += strlen (cur);
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
								sprintf (cur, "Reading: %s\n", p);
								cur += strlen (cur);
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
						buf[i - 2] = 0;
				sprintf (cur, "%s", p);
				printf ("%s\n", buft);
		}

		return 0;
}
