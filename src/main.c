#include <gtk/gtk.h>

#include "kanji.h"
#include "add_kanji.h"
#include "view_kanji.h"

static void button1_clicked (GtkButton*, GtkWindow*);
static void button2_clicked (GtkButton*, GtkWindow*);
static void destroy (GtkWidget*, gpointer);

int main (int argc, char *argv[])
{
		GtkWidget *window, *button1, *button2;
		GtkWidget *vbox;

		gtk_init (&argc, &argv);

		window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title (GTK_WINDOW (window), "KanjiTest");
		gtk_container_set_border_width (GTK_CONTAINER (window), 10);

		g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy), NULL);

		button1 = gtk_button_new_with_mnemonic ("_Add Kanji");
		button2 = gtk_button_new_with_mnemonic ("_View Kanji List");

		g_signal_connect (G_OBJECT (button1), "clicked", G_CALLBACK (button1_clicked), (gpointer) window);
		g_signal_connect (G_OBJECT (button2), "clicked", G_CALLBACK (button2_clicked), (gpointer) window);

		vbox = gtk_vbox_new (FALSE, 5);

		gtk_box_pack_start_defaults (GTK_BOX (vbox), button1);
		gtk_box_pack_start_defaults (GTK_BOX (vbox), button2);

		gtk_container_add (GTK_CONTAINER (window), vbox);
		gtk_widget_show_all (window);
		
		gtk_main ();
		return 0;
}

static void button1_clicked (GtkButton *button, GtkWindow *parent)
{
		Kanji *tmp = create_dialog ();

		if (tmp == NULL)
				return;

		GArray *arr = kanji_array_load ("output");
		if (arr == NULL)
		{
				arr = kanji_array_create;
				arr = kanji_array_append (arr, tmp);
				kanji_array_save ("output", arr);
		}
		else
		{
				arr = kanji_array_append (arr, tmp);
				kanji_array_save ("output", arr);
		}

		kanji_array_free (arr);
}

static void button2_clicked (GtkButton *button, GtkWindow *parent)
{
		GArray *arr = kanji_array_load ("output");
		if (arr == NULL)
				return;

		view_kanji (arr);

		kanji_array_free (arr);
}
static void destroy (GtkWidget *window, gpointer data)
{
		gtk_main_quit ();
}
