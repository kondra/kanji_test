#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include "kanji.h"
#include "add_kanji.h"
#include "view_kanji.h"

//static void button2_clicked (GtkButton*, GArray*);
//static void destroy (GtkWidget*, GArray*);

int main (int argc, char *argv[])
{
//		GtkWidget *window, *button2;
//		GtkWidget *vbox;
		GArray *arr;

//		Memory Profiling
//		g_mem_set_vtable (glib_mem_profiler_table);
//		g_atexit (g_mem_profile);

		arr = kanji_array_load ("kanjidict");
		if (arr == NULL)
				arr = kanji_array_create;

		gtk_init (&argc, &argv);

//		window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
//		gtk_window_set_title (GTK_WINDOW (window), "KanjiTest");
//		gtk_container_set_border_width (GTK_CONTAINER (window), 10);
//
//		g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy), (gpointer) arr);
//
//		button2 = gtk_button_new_with_mnemonic ("_View Kanji List");
//
//		g_signal_connect (G_OBJECT (button2), "clicked", G_CALLBACK (button2_clicked), (gpointer) arr);
//
//		vbox = gtk_vbox_new (FALSE, 5);
//
//		gtk_box_pack_start_defaults (GTK_BOX (vbox), button2);
//
//		gtk_container_add (GTK_CONTAINER (window), vbox);
//		gtk_widget_show_all (window);

		if (arr == NULL)
				return -1;

		kanji_list_view (arr);

		kanji_array_save ("kanjidict", arr);
		//gtk_main ();
		return 0;
}
/*
static void button2_clicked (GtkButton *button, GArray *arr)
{
		if (arr == NULL)
				return;

		kanji_list_view (arr);
}

static void destroy (GtkWidget *window, GArray *arr)
{
	//	GtkWidget *dialog;
	//	int res;

		if (arr != NULL)
		{
//				dialog = gtk_message_dialog_new (GTK_WINDOW (window), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Do you want to save dictionary before exit?");
//				res = gtk_dialog_run (GTK_DIALOG (dialog));
//				
//				if (res == GTK_RESPONSE_YES)
						kanji_array_save ("kanjidict", arr);

//				gtk_widget_destroy (dialog);
		}

		gtk_main_quit ();
}*/
