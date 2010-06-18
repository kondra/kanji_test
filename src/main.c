#include <gtk/gtk.h>
#include <stdlib.h>

#include "kanji.h"
#include "add_kanji.h"
#include "view_kanji.h"

//static void button1_clicked (GtkButton*, GArray*);
static void button2_clicked (GtkButton*, GArray*);
static void destroy (GtkWidget*, GArray*);

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

//		button1 = gtk_button_new_with_mnemonic ("_Add Kanji");
		button2 = gtk_button_new_with_mnemonic ("_View Kanji List");

//		g_signal_connect (G_OBJECT (button1), "clicked", G_CALLBACK (button1_clicked), (gpointer) arr);
		g_signal_connect (G_OBJECT (button2), "clicked", G_CALLBACK (button2_clicked), (gpointer) arr);

		vbox = gtk_vbox_new (FALSE, 5);

//		gtk_box_pack_start_defaults (GTK_BOX (vbox), button1);
		gtk_box_pack_start_defaults (GTK_BOX (vbox), button2);

		gtk_container_add (GTK_CONTAINER (window), vbox);
		gtk_widget_show_all (window);

		gtk_main ();
		return 0;
}
/*
static void button1_clicked (GtkButton *button, GArray *arr)
{
		if (arr == NULL)
				g_error ("null pointer received");

		Kanji *tmp = create_dialog (NULL, FALSE);

		if (tmp == NULL)
				return;

		arr = kanji_array_append (arr, tmp);
}
*/
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
