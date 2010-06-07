#include <gtk/gtk.h>

#include "kanji.h"
#include "add_kanji.h"

static void button_clicked (GtkButton*, GtkWindow*);
static void destroy (GtkWidget*, gpointer);

int main (int argc, char *argv[])
{
		GtkWidget *window, *button;

		gtk_init (&argc, &argv);

		window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title (GTK_WINDOW (window), "KanjiTest");
		gtk_container_set_border_width (GTK_CONTAINER (window), 10);

		g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy), NULL);

		button = gtk_button_new_with_mnemonic ("_Add Kanji");

		g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (button_clicked), (gpointer) window);

		gtk_container_add (GTK_CONTAINER (window), button);
		gtk_widget_show_all (window);
		
		gtk_main ();
		return 0;
}

static void button_clicked (GtkButton *button, GtkWindow *parent)
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

static void destroy (GtkWidget *window, gpointer data)
{
		gtk_main_quit ();
}
