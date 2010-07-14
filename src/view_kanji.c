#include <gtk/gtk.h>

#include "kanji.h"
#include "kanji_article_view.h"
#include "view_kanji.h"
#include "add_kanji.h"

typedef struct
{
		gboolean changed;

		GArray *arr;
		GtkWidget *tview;
} Data;

struct ColumnProperties
{
		gchar *name;

		gboolean resizable;
		gboolean visible;
		gboolean reorderable;
		gboolean sort_indicator;
}
properties[] =
{
		{ "Number", FALSE, FALSE, FALSE, FALSE },
		{ "Kanji", TRUE, TRUE, FALSE, FALSE },
		{ "K. Stroke", TRUE, TRUE, TRUE, TRUE },
		{ "Radical", TRUE, TRUE, TRUE, TRUE },
		{ "R. Stroke", TRUE, TRUE, TRUE, TRUE },
		{ "JLPT", TRUE, TRUE, TRUE, TRUE },
		{ "Grade", TRUE, TRUE, TRUE, TRUE },
};

enum
{
		NUMBER = 0,
		KANJI,
		KANJI_STROKE,
		RADICAL,
		RADICAL_STROKE,
		JLPT_LEVEL,
		SCHOOL_GRADE,
		COLUMNS
};

static void setup_tree_view (GtkWidget*, Data*);
static void row_activated (GtkTreeView*, GtkTreePath*, GtkTreeViewColumn*, GArray*);
static void row_add (GtkButton*, Data*);
static void row_remove (GtkButton*, Data*);
static void row_edit (GtkButton*, Data*);
static void save_dict (GtkButton*, Data*);
static void close_dialog (GtkButton*, GtkDialog*);

gboolean kanji_list_view (GArray *arr)
{
		GtkWidget *dialog;
		GtkWidget *treeview;
		GtkWidget *scrolled_win;
		GtkWidget *hbox;

		GtkListStore *store;
		GtkTreeIter iter;

		guint i, result;

		Kanji *tmp;
		Data p;

		dialog = gtk_dialog_new_with_buttons ("Kanji List", NULL, GTK_DIALOG_MODAL, NULL);
		gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
		gtk_widget_set_size_request (GTK_WIDGET (dialog), 500, 500);

		treeview = gtk_tree_view_new ();
		setup_tree_view (treeview, &p);

		store = gtk_list_store_new (COLUMNS, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);

		tmp = &g_array_index (arr, Kanji, i = 0);
		while (i < arr->len)
		{
				gtk_list_store_append (store, &iter);
				gtk_list_store_set (store, &iter, NUMBER, i + 1,
								KANJI, tmp->kanji,
								KANJI_STROKE, tmp->kanji_stroke,
								RADICAL, tmp->radical, 
								RADICAL_STROKE, tmp->radical_stroke,
								JLPT_LEVEL, tmp->jlpt_level,
								SCHOOL_GRADE, tmp->grade, -1);

				tmp = &g_array_index (arr, Kanji, ++i);
		}

		gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
		g_object_unref (store);

		g_signal_connect (G_OBJECT (treeview), "row-activated", G_CALLBACK (row_activated), (gpointer)arr);

		scrolled_win = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

		gtk_container_add (GTK_CONTAINER (scrolled_win), treeview);

		p.arr = arr;
		p.tview = treeview;
		p.changed = FALSE;

		GtkWidget *add_button = gtk_button_new_from_stock (GTK_STOCK_ADD);
		GtkWidget *remove_button = gtk_button_new_from_stock (GTK_STOCK_REMOVE);
		GtkWidget *edit_button = gtk_button_new_from_stock (GTK_STOCK_EDIT);
		GtkWidget *save_button = gtk_button_new_from_stock (GTK_STOCK_SAVE);
		GtkWidget *close_button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);

		g_signal_connect (G_OBJECT (add_button), "clicked", G_CALLBACK (row_add), (gpointer) &p);
		g_signal_connect (G_OBJECT (remove_button), "clicked", G_CALLBACK (row_remove), (gpointer) &p);
		g_signal_connect (G_OBJECT (edit_button), "clicked", G_CALLBACK (row_edit), (gpointer) &p);
		g_signal_connect (G_OBJECT (save_button), "clicked", G_CALLBACK (save_dict), (gpointer) &p);
		g_signal_connect (G_OBJECT (close_button), "clicked", G_CALLBACK (close_dialog), (gpointer) dialog);

		hbox = gtk_hbox_new (FALSE, 5);

		gtk_box_pack_start (GTK_BOX (hbox), add_button, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox), remove_button, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox), edit_button, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox), save_button, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox), close_button, FALSE, FALSE, 5);

		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), scrolled_win, TRUE, TRUE, 5);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), hbox, FALSE, FALSE, 5);

		gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview), FALSE);

		gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);

		gtk_widget_show_all (dialog);
		result = gtk_dialog_run (GTK_DIALOG (dialog));

		gtk_widget_destroy (dialog);

		return p.changed;
}

static void close_dialog (GtkButton *button, GtkDialog *dialog)
{
		gtk_dialog_response (dialog, GTK_RESPONSE_CLOSE);
}

static void save_dict (GtkButton *button, Data *p)
{
		if (p->changed)
		{
				kanji_array_save ("kanjidict", p->arr);
				p->changed = FALSE;
		}
}

static void row_edit (GtkButton *button, Data *p)
{
		GtkTreeIter iter;
		GtkWidget *treeview = p->tview;

		gint i;

		GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
		GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

		if (gtk_tree_selection_get_selected (selection, &model, &iter) == FALSE)
				return;

		gtk_tree_model_get (model, &iter, NUMBER, &i, -1);

		Kanji old = g_array_index (p->arr, Kanji, i - 1);

		Kanji *tmp = kanji_edit_dialog (&old);
		if (tmp == NULL)
				return;

		g_array_index (p->arr, Kanji, i - 1) = *tmp;
		
		gtk_list_store_set (GTK_LIST_STORE (model), &iter, 
						KANJI, tmp->kanji, 
						KANJI_STROKE, tmp->kanji_stroke, 
						RADICAL, tmp->radical, 
						RADICAL_STROKE, tmp->radical_stroke, 
						JLPT_LEVEL, tmp->jlpt_level, 
						SCHOOL_GRADE, tmp->grade, -1);
		
		p->changed = TRUE;
}

static void row_add (GtkButton *button, Data *p)
{
		Kanji *tmp = kanji_add_dialog ();

		if (tmp == NULL)
				return;

		p->arr = kanji_array_append (p->arr, tmp);

		GtkWidget *treeview = p->tview;
		GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
		GtkTreeIter iter;

		guint i = p->arr->len;

		gtk_list_store_append (GTK_LIST_STORE (model), &iter);
		gtk_list_store_set (GTK_LIST_STORE (model), &iter, 
						NUMBER, i,
						KANJI, tmp->kanji,
						KANJI_STROKE, tmp->kanji_stroke,
						RADICAL, tmp->radical, 
						RADICAL_STROKE, tmp->radical_stroke, 
						JLPT_LEVEL, tmp->jlpt_level, 
						SCHOOL_GRADE, tmp->grade, -1);

		p->changed = TRUE;
}

static void row_remove (GtkButton *button, Data *p)
{
		GtkTreeIter iter;
		GtkWidget *treeview = p->tview;

		int i;

		GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
		GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

		if (gtk_tree_selection_get_selected (selection, &model, &iter) == FALSE)
				return;

		gtk_tree_model_get (model, &iter, NUMBER, &i, -1);
		kanji_free (&g_array_index (p->arr, Kanji, i - 1));

		gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
		
		p->changed = TRUE;
}

static void row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, GArray *arr)
{
		GtkTreeIter iter;
		gint i;

		Kanji *tmp;

		GtkTreeModel *model = gtk_tree_view_get_model (treeview);
		if (gtk_tree_model_get_iter (model, &iter, path))
		{
				gtk_tree_model_get (model, &iter, NUMBER, &i, -1);
				tmp = &g_array_index (arr, Kanji, i - 1);

				kanji_article_view (tmp);
		}
}

static void setup_tree_view (GtkWidget *treeview, Data *p)
{
		GtkCellRenderer *renderer;
		GtkTreeViewColumn *column;

		guint i = NUMBER;
		while (i != COLUMNS)
		{
				renderer = gtk_cell_renderer_text_new ();
				column = gtk_tree_view_column_new_with_attributes (properties[i].name, renderer, "text", i, NULL);

				gtk_tree_view_column_set_resizable (column, properties[i].resizable);
				gtk_tree_view_column_set_visible (column, properties[i].visible);
				gtk_tree_view_column_set_reorderable (column, properties[i].reorderable);
				gtk_tree_view_column_set_sort_indicator (column, properties[i].sort_indicator);
				if (properties[i].reorderable)
						gtk_tree_view_column_set_sort_column_id (column, i);
				gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

				i++;
		}
}
