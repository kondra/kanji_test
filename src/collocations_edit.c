#include <gtk/gtk.h>
#include <glib/gprintf.h>

#include "kanji.h"
#include "collocations_edit.h"

enum
{
		WORD_WRITING = 0,
		WORD_READING,
		WORD_MEANING,
		WORD_LEVEL,
		COLUMNS
};

static const gchar *column_names[] = {"Writing", "Reading", "Meaning", "JLPT Level"};

static void setup_tree_view (GtkTreeView*);
static void cell_edited (GtkCellRendererText*, gchar*, gchar*, GtkTreeView*);
static void row_add (GtkMenuItem*, GtkTreeView*);
static void row_remove (GtkMenuItem*, GtkTreeView*);

static Collocations* create_dialog (Collocations*);

static void setup_data (Collocations*, GtkListStore*);
static Collocations* get_data (GtkTreeView*);

Collocations* collocations_edit_dialog (Collocations *col)
{
		return create_dialog (col);
}

static void row_add (GtkMenuItem *item, GtkTreeView *treeview)
{
		GtkTreeIter iter;

		GtkTreeModel *model = gtk_tree_view_get_model (treeview);
		gtk_list_store_append (GTK_LIST_STORE (model), &iter);

		GtkTreePath *path = gtk_tree_model_get_path (model, &iter);
		GtkTreeViewColumn *column = gtk_tree_view_get_column (treeview, WORD_WRITING);

		gtk_tree_view_set_cursor_on_cell (treeview, path, column, NULL, TRUE);
}

static void row_remove (GtkMenuItem *item, GtkTreeView *treeview)
{
		GtkTreeIter iter;

		GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
		GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

		if (gtk_tree_selection_get_selected (selection, &model, &iter) == FALSE)
				return;
		
		gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
}

static Collocations* get_data (GtkTreeView *treeview)
{
		GtkTreeIter iter;

		GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
		gint num = gtk_tree_model_iter_n_children (model, NULL);
		gint i = 0;

		gchar **writing, **reading, **meaning, *buf;
		guint64 lvl;
		guint8 *level;

		writing = (gchar**) g_malloc0 (sizeof (gchar*) * num);
		reading = (gchar**) g_malloc0 (sizeof (gchar*) * num);
		meaning = (gchar**) g_malloc0 (sizeof (gchar*) * num);
		level = (guint8*) g_malloc0 (sizeof (guint8) * num);

		if (gtk_tree_model_get_iter_first (model, &iter))
		{
				do
				{
						gtk_tree_model_get (model, &iter, 
										WORD_WRITING, &writing[i], 
										WORD_READING, &reading[i], 
										WORD_MEANING, &meaning[i],
										WORD_LEVEL, &buf, -1);

						lvl = g_ascii_strtoll (buf, NULL, 10);
						level[i] = (guint8)lvl;
						i++;
				} while (gtk_tree_model_iter_next (model, &iter));
		}

		Collocations *col = collocations_create (num, writing, reading, meaning, level, TRUE);

		g_free (writing);
		g_free (reading);
		g_free (meaning);
		g_free (level);

		return col;
}

static void setup_data (Collocations *col, GtkListStore *store)
{
		GtkTreeIter iter;
		gchar buf[10];
		gint i, j;

		for (i = 0; i < col->num; i++)
		{
				gtk_list_store_append (store, &iter);
				j = g_sprintf (buf, "%d", col->level[i]);
				buf[j] = 0;
				gtk_list_store_set (store, &iter, 
								WORD_WRITING, col->writing[i], 
								WORD_READING, col->reading[i], 
								WORD_MEANING, col->meaning[i],
								WORD_LEVEL, buf, -1);
		}
}

static void setup_tree_view (GtkTreeView *treeview)
{
		GtkCellRenderer *renderer;
		GtkTreeViewColumn *column;

		guint i = WORD_WRITING;
		while (i != COLUMNS)
		{
				renderer = gtk_cell_renderer_text_new ();
				g_object_set (renderer, "editable", TRUE, "editable-set", TRUE, NULL);
				g_object_set_data (G_OBJECT (renderer), "column-num", GUINT_TO_POINTER (i));

				g_signal_connect (G_OBJECT (renderer), "edited", G_CALLBACK (cell_edited), (gpointer) treeview);

				column = gtk_tree_view_column_new_with_attributes (column_names[i], renderer, "text", i, NULL);
				gtk_tree_view_column_set_resizable (column, TRUE);
				gtk_tree_view_column_set_reorderable (column, TRUE);
				gtk_tree_view_column_set_sort_indicator (column, TRUE);
				gtk_tree_view_column_set_sort_column_id (column, i);
				gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

				i++;
		}
}

static void cell_edited (GtkCellRendererText *renderer, gchar *path, gchar *new_text, GtkTreeView *treeview)
{
		GtkTreeIter iter;
		GtkTreeModel *model;

		guint num = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (renderer), "column-num"));

		if (num == WORD_LEVEL)
		{
				gint64 lvl;
				lvl = g_ascii_strtoll (new_text, NULL, 10);
				if (lvl > 5 || lvl < 1)
						return;
		}

		model = gtk_tree_view_get_model (treeview);
		if (gtk_tree_model_get_iter_from_string (model, &iter, path))
				gtk_list_store_set (GTK_LIST_STORE (model), &iter, num, new_text, -1);
}

static Collocations* create_dialog (Collocations *col)
{
		GtkWidget *dialog;
		GtkWidget *scrolled_win;
		GtkWidget *treeview;
		GtkListStore *store;
		gint result;

		dialog = gtk_dialog_new_with_buttons ("Edit Collocations", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
		gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
		gtk_widget_set_size_request (GTK_WIDGET (dialog), 600, 500);
		
		treeview = gtk_tree_view_new ();
		setup_tree_view (GTK_TREE_VIEW (treeview));

		store = gtk_list_store_new (COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

		if (col != NULL)
				setup_data (col, store);

		gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
		gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview), FALSE);
		g_object_unref (store);

		GtkWidget* add_button = gtk_button_new_from_stock (GTK_STOCK_ADD);
		GtkWidget* remove_button = gtk_button_new_from_stock (GTK_STOCK_REMOVE);

		g_signal_connect (G_OBJECT (add_button), "clicked", G_CALLBACK (row_add), (gpointer) treeview);
		g_signal_connect (G_OBJECT (remove_button), "clicked", G_CALLBACK (row_remove), (gpointer) treeview);

		GtkWidget* hbox = gtk_hbox_new (FALSE, 5);

		gtk_box_pack_start (GTK_BOX (hbox), add_button, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox), remove_button, FALSE, FALSE, 5);

		scrolled_win = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		
		gtk_container_add (GTK_CONTAINER (scrolled_win), treeview);

		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), scrolled_win, TRUE, TRUE, 5);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), hbox, FALSE, FALSE, 5);
		
		gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);

		gtk_widget_show_all (dialog);
		result = gtk_dialog_run (GTK_DIALOG (dialog));

		if (result == GTK_RESPONSE_OK)
		{
				Collocations *tmp = NULL;
				tmp = get_data (GTK_TREE_VIEW (treeview));
				gtk_widget_destroy (dialog);
				return tmp;
		}
		
		gtk_widget_destroy (dialog);

		return col;
}
