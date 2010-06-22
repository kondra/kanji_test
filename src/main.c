#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include "kanji.h"
#include "add_kanji.h"
#include "view_kanji.h"

static void button2_clicked (GtkButton*, GArray*);
static void destroy (GtkWidget*, GArray*);

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

int main (int argc, char *argv[])
{
		GtkWidget *window, *button2;
		GtkWidget *vbox;
		GArray *arr;
		
		arr = kanji_array_load ("kanjidict");
		if (arr == NULL)
				arr = kanji_array_create;

		gtk_init (&argc, &argv);

		window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title (GTK_WINDOW (window), "KanjiTest");
		gtk_container_set_border_width (GTK_CONTAINER (window), 10);

		g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy), (gpointer) arr);

		button2 = gtk_button_new_with_mnemonic ("_View Kanji List");

		g_signal_connect (G_OBJECT (button2), "clicked", G_CALLBACK (button2_clicked), (gpointer) arr);

		vbox = gtk_vbox_new (FALSE, 5);

		gtk_box_pack_start_defaults (GTK_BOX (vbox), button2);

		gtk_container_add (GTK_CONTAINER (window), vbox);
		gtk_widget_show_all (window);

		gtk_main ();
		return 0;
}

static void button2_clicked (GtkButton *button, GArray *arr)
{
		if (arr == NULL)
				return;

		view_kanji (arr);
}

static void destroy (GtkWidget *window, GArray *arr)
{
		if (arr != NULL)
				kanji_array_save ("kanjidict", arr);

		gtk_main_quit ();
}
