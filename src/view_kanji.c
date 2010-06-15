#include <gtk/gtk.h>

#include "kanji.h"
#include "view_kanji.h"

static void setup_tree_view (GtkWidget*);

void view_kanji (GArray *arr)
{
		GtkWidget *dialog, *treeview, *scrolled_win;
		GtkListStore *store;
		GtkTreeIter iter;
		guint i, result;

		Kanji *tmp;

		dialog = gtk_dialog_new_with_buttons ("Kanji List", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
		gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
		gtk_widget_set_size_request (GTK_WIDGET (dialog), 750, 400);

		treeview = gtk_tree_view_new ();
		setup_tree_view (treeview);

		store = gtk_list_store_new (COLUMNS, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);

		tmp = &g_array_index (arr, Kanji, i = 0);
		while (tmp == NULL || !kanji_is_null (tmp))
		{
				gtk_list_store_append (store, &iter);
				gtk_list_store_set (store, &iter, NUMBER, i + 1, KANJI, tmp->str, KANJI_STROKE, tmp->stroke, RADICAL, tmp->radical, 
								RADICAL_STROKE, tmp->radical_stroke, ON, tmp->on, KUN, tmp->kun, MEANING, tmp->meaning, JLPT_LEVEL, tmp->jlpt_level, 
								SCHOOL_GRADE, tmp->grade, -1);

				tmp = &g_array_index (arr, Kanji, ++i);
		}

		gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
		g_object_unref (store);

		scrolled_win = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

		gtk_container_add (GTK_CONTAINER (scrolled_win), treeview);

		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), scrolled_win, TRUE, TRUE, 5);

		gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);

		gtk_widget_show_all (dialog);
		result = gtk_dialog_run (GTK_DIALOG (dialog));

		gtk_widget_destroy (dialog);
}

static void setup_tree_view (GtkWidget *treeview)
{
		GtkCellRenderer *renderer;
		GtkTreeViewColumn *column;

		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Num", renderer, "text", NUMBER, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Kanji", renderer, "text", KANJI, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
		
		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Stroke", renderer, "text", KANJI_STROKE, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
		
		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Radical", renderer, "text", RADICAL, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
		
		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Stroke", renderer, "text", RADICAL_STROKE, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
		
		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("On", renderer, "text", ON, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
		
		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Kun", renderer, "text", KUN, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
		
		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Meaning", renderer, "text", MEANING, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
		
		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("JLPT", renderer, "text", JLPT_LEVEL, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
		
		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Grade", renderer, "text", SCHOOL_GRADE, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
}
