#include <gtk/gtk.h>
#include <string.h>

#include "kanji.h"
#include "view_kanji.h"
#include "add_kanji.h"

typedef struct
{
		GArray *arr;
		GtkWidget *tview;
} Pair;

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

static void setup_tree_view (GtkWidget*, Pair*);
//static void view_kanji_flash_card (Kanji*);
static void row_activated (GtkTreeView*, GtkTreePath*, GtkTreeViewColumn*, GArray*);
static void row_add (GtkButton*, Pair*);
static void row_remove (GtkButton*, Pair*);
static void row_edit (GtkButton*, Pair*);
static void close_dialog (GtkButton*, GtkDialog*);

void kanji_list_view (GArray *arr)
{
		//GtkWidget *entry;
		GtkWidget *dialog, *treeview, *scrolled_win;
		GtkWidget *close_button, *edit_button, *add_button, *remove_button, *hbox;
		GtkListStore *store;
		GtkTreeIter iter;
		guint i, result;

		Kanji *tmp;
		Pair p;

		dialog = gtk_dialog_new_with_buttons ("Kanji List", NULL, GTK_DIALOG_MODAL, NULL);
		gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
		gtk_widget_set_size_request (GTK_WIDGET (dialog), 750, 400);

		treeview = gtk_tree_view_new ();
		setup_tree_view (treeview, &p);

		store = gtk_list_store_new (COLUMNS, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);

		tmp = &g_array_index (arr, Kanji, i = 0);
//		while (tmp == NULL || !kanji_is_null (tmp))
		while (i < arr->len)
		{
				gtk_list_store_append (store, &iter);
				gtk_list_store_set (store, &iter, NUMBER, i + 1, KANJI, tmp->str, KANJI_STROKE, tmp->stroke, RADICAL, tmp->radical, 
								RADICAL_STROKE, tmp->radical_stroke, JLPT_LEVEL, tmp->jlpt_level, SCHOOL_GRADE, tmp->grade, -1);

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

		add_button = gtk_button_new_from_stock (GTK_STOCK_ADD);
		remove_button = gtk_button_new_from_stock (GTK_STOCK_REMOVE);
		edit_button = gtk_button_new_from_stock (GTK_STOCK_EDIT);
		close_button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);

		g_signal_connect (G_OBJECT (add_button), "clicked", G_CALLBACK (row_add), (gpointer) &p);
		g_signal_connect (G_OBJECT (remove_button), "clicked", G_CALLBACK (row_remove), (gpointer) &p);
		g_signal_connect (G_OBJECT (edit_button), "clicked", G_CALLBACK (row_edit), (gpointer) &p);
		g_signal_connect (G_OBJECT (close_button), "clicked", G_CALLBACK (close_dialog), (gpointer) dialog);

		hbox = gtk_hbox_new (FALSE, 5);

		gtk_box_pack_start (GTK_BOX (hbox), add_button, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox), remove_button, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox), edit_button, FALSE, FALSE, 5);

		gtk_box_pack_start (GTK_BOX (hbox), close_button, FALSE, FALSE, 5);

		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), scrolled_win, TRUE, TRUE, 5);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), hbox, FALSE, FALSE, 5);

		gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview), FALSE);
	//	gtk_tree_view_set_search_column (GTK_TREE_VIEW (treeview), ON);
	//	entry = gtk_entry_new ();
	//	gtk_tree_view_set_search_entry (GTK_TREE_VIEW (treeview), GTK_ENTRY (entry));

	//	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), entry, FALSE, FALSE, 5);

		gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);

		gtk_widget_show_all (dialog);
		result = gtk_dialog_run (GTK_DIALOG (dialog));

		gtk_widget_destroy (dialog);
}

static void close_dialog (GtkButton *button, GtkDialog *dialog)
{
		gtk_dialog_response (dialog, GTK_RESPONSE_CLOSE);
}

static void row_edit (GtkButton *button, Pair *p)
{
		GtkTreeSelection *selection;
		GtkTreeModel *model;
		GtkTreeIter iter;
		GtkWidget *treeview = p->tview;

		int i;

		model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
		selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

		if (gtk_tree_selection_get_selected (selection, &model, &iter) == FALSE)
				return;

		gtk_tree_model_get (model, &iter, NUMBER, &i, -1);

		Kanji old = g_array_index (p->arr, Kanji, i - 1);

		Kanji *tmp = kanji_edit_dialog (&old);// create_dialog (&old, TRUE);
		if (tmp == NULL)
				return;

		g_array_index (p->arr, Kanji, i - 1) = *tmp;
		
		gtk_list_store_set (GTK_LIST_STORE (model), &iter, KANJI, tmp->str, KANJI_STROKE, tmp->stroke, RADICAL, tmp->radical, 
								RADICAL_STROKE, tmp->radical_stroke, JLPT_LEVEL, tmp->jlpt_level, SCHOOL_GRADE, tmp->grade, -1);
}

static void row_add (GtkButton *button, Pair *p)
{
		Kanji *tmp = kanji_add_dialog (); //create_dialog (NULL, FALSE);

		if (tmp == NULL)
				return;

		p->arr = kanji_array_append (p->arr, tmp);

		GtkWidget *treeview = p->tview;
		GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
		GtkTreeIter iter;

		guint i = p->arr->len;

		gtk_list_store_append (GTK_LIST_STORE (model), &iter);
		gtk_list_store_set (GTK_LIST_STORE (model), &iter, NUMBER, i, KANJI, tmp->str, KANJI_STROKE, tmp->stroke, RADICAL, tmp->radical, 
								RADICAL_STROKE, tmp->radical_stroke, JLPT_LEVEL, tmp->jlpt_level, SCHOOL_GRADE, tmp->grade, -1);
}

static void row_remove (GtkButton *button, Pair *p)
{
		GtkTreeSelection *selection;
		GtkTreeModel *model;
		GtkTreeIter iter;//, it;
		GtkWidget *treeview = p->tview;

		int i;

		model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
		selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

		if (gtk_tree_selection_get_selected (selection, &model, &iter) == FALSE)
				return;

		gtk_tree_model_get (model, &iter, NUMBER, &i, -1);
		kanji_free (&g_array_index (p->arr, Kanji, i - 1));

		gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
}

static void view_kanji_article (Kanji *kanji)
{
		GtkWidget *dialog, *scrolled, *textview;
		GtkTextBuffer *buffer;
		GtkTextIter start;//, end;
		gchar *buf;

		dialog = gtk_dialog_new_with_buttons ("Kanji Flash Card", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
		gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
		gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);
		gtk_widget_set_size_request (GTK_WIDGET (dialog), 400, 600);

		textview = gtk_text_view_new ();
		gtk_text_view_set_editable (GTK_TEXT_VIEW (textview), FALSE);
		gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (textview), FALSE);
		buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));

		gtk_text_buffer_create_tag (buffer, "kanji_font", "font", "46", NULL);
		gtk_text_buffer_create_tag (buffer, "on_font", "font", "20", NULL);
		gtk_text_buffer_create_tag (buffer, "on_color", "foreground", "#443FD1", NULL);

		gtk_text_buffer_get_start_iter (buffer, &start);
		gtk_text_buffer_insert_with_tags_by_name (buffer, &start, kanji->str, -1, "kanji_font", NULL);

		gtk_text_buffer_get_end_iter (buffer, &start);
		buf = g_strdup_printf (" %d\n", kanji->stroke);
		gtk_text_buffer_insert (buffer, &start, buf, -1);
		g_free (buf);

		gtk_text_buffer_get_end_iter (buffer, &start);
		buf = g_strdup_printf ("    %s\n", kanji->on);
		gtk_text_buffer_insert_with_tags_by_name (buffer, &start, buf, -1, "on_font", "on_color", NULL);

		scrolled = gtk_scrolled_window_new (NULL, NULL);
		gtk_container_add (GTK_CONTAINER (scrolled), textview);

		gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dialog)->vbox), scrolled);
		
		gtk_widget_show_all (dialog);
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
}
/*
static void view_kanji_flash_card (Kanji *kanji)
{
		GtkWidget *dialog;
		GtkWidget *kanji_label, *on_label, *kun_label, *meaning_label, *jlpt_label, *grade_label, *radical_label, *stroke_label;
		GtkWidget *on_lbl, *kun_lbl, *meaning_lbl, *radical_lbl, *descr_lbl, *jlpt_lbl, *grade_lbl; 
		GtkWidget *table1;

		int result;

		dialog = gtk_dialog_new_with_buttons ("Kanji Flash Card", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
		gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

		kanji_label = gtk_label_new (NULL);
		gtk_label_set_markup (GTK_LABEL (kanji_label), g_strconcat ("<span font_desc='76'>", kanji->str, "</span>", NULL));

		on_label = gtk_label_new (NULL);
		gtk_label_set_selectable (GTK_LABEL (on_label), TRUE);
		gtk_label_set_markup (GTK_LABEL (on_label), g_strconcat ("<span font_desc='14'>", simple_coma_parser (kanji->on, TRUE), "</span>", NULL));
		
		kun_label = gtk_label_new (NULL);
		gtk_label_set_selectable (GTK_LABEL (kun_label), TRUE);
		gtk_label_set_markup (GTK_LABEL (kun_label), g_strconcat ("<span font_desc='14'>", simple_coma_parser (kanji->kun, TRUE), "</span>", NULL));
	
		meaning_label = gtk_label_new (simple_coma_parser (kanji->meaning, TRUE));

		jlpt_label = gtk_label_new (g_strdup_printf ("%d", kanji->jlpt_level));
		
		grade_label = gtk_label_new (g_strdup_printf ("%d", kanji->grade));
	
		radical_label = gtk_label_new (NULL);
		gtk_label_set_selectable (GTK_LABEL (radical_label), TRUE);
		gtk_label_set_markup (GTK_LABEL (radical_label), g_strconcat ("<span font_desc='20'>", kanji->radical, "</span>", NULL));
	
		stroke_label = gtk_label_new (g_strdup_printf ("%d-%d", kanji->stroke, kanji->radical_stroke));

		on_lbl = gtk_label_new ("On:");
		kun_lbl = gtk_label_new ("Kun:");
		meaning_lbl = gtk_label_new ("Meaning:");
		radical_lbl = gtk_label_new ("Radical:");
		descr_lbl = gtk_label_new ("Kanji-Radical Stroke:");
		jlpt_lbl = gtk_label_new ("JLPT Level:");
		grade_lbl = gtk_label_new ("School Grade:");

		gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dialog)->vbox), kanji_label);

		table1 = gtk_table_new (7, 2, FALSE);

		gtk_table_attach (GTK_TABLE (table1), radical_lbl, 0, 1, 0, 1, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table1), descr_lbl, 0, 1, 1, 2, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table1), on_lbl, 0, 1, 2, 3, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table1), kun_lbl, 0, 1, 3, 4, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table1), meaning_lbl, 0, 1, 4, 5, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table1), jlpt_lbl, 0, 1, 5, 6, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table1), grade_lbl, 0, 1, 6, 7, GTK_EXPAND, GTK_SHRINK, 0, 0);
//		gtk_table_attach (GTK_TABLE (table1), _lbl, 0, 1, 7, 8, GTK_EXPAND, GTK_SHRINK, 0, 0);

		gtk_table_attach (GTK_TABLE (table1), radical_label, 1, 2, 0, 1, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table1), stroke_label, 1, 2, 1, 2, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table1), on_label, 1, 2, 2, 3, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table1), kun_label, 1, 2, 3, 4, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table1), meaning_label, 1, 2, 4, 5, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table1), jlpt_label, 1, 2, 5, 6, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table1), grade_label, 1, 2, 6, 7, GTK_EXPAND, GTK_SHRINK, 0, 0);
//		gtk_table_attach (GTK_TABLE (table1), _label, 1, 2, 7, 8, GTK_EXPAND, GTK_SHRINK, 0, 0);

		gtk_table_set_row_spacings (GTK_TABLE (table1), 5);
		gtk_table_set_col_spacings (GTK_TABLE (table1), 5);
		
		gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dialog)->vbox), table1);

		gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);
		
		gtk_widget_show_all (dialog);
		result = gtk_dialog_run (GTK_DIALOG (dialog));

		gtk_widget_destroy (dialog);
		
		simple_coma_parser (kanji->on, FALSE);
		simple_coma_parser (kanji->meaning, FALSE);
		simple_coma_parser (kanji->kun, FALSE);
}
*/
static void row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, GArray *arr)
{
		GtkTreeModel *model;
		GtkTreeIter iter;

		int i;

		Kanji *tmp;

		model = gtk_tree_view_get_model (treeview);
		if (gtk_tree_model_get_iter (model, &iter, path))
		{
				gtk_tree_model_get (model, &iter, NUMBER, &i, -1);
				tmp = &g_array_index (arr, Kanji, i - 1);

//				view_kanji_flash_card (tmp);
				view_kanji_article (tmp);
		}
}

static void setup_tree_view (GtkWidget *treeview, Pair *p)
{
		GtkCellRenderer *renderer;
		GtkTreeViewColumn *column;

		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Num", renderer, "text", NUMBER, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_column_set_visible (column, FALSE);
		gtk_tree_view_column_set_reorderable (column, TRUE);
		gtk_tree_view_column_set_sort_indicator (column, TRUE);
		gtk_tree_view_column_set_sort_column_id (column, NUMBER);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Kanji", renderer, "text", KANJI, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
		
		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("K. Stroke", renderer, "text", KANJI_STROKE, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_column_set_reorderable (column, TRUE);
		gtk_tree_view_column_set_sort_indicator (column, TRUE);
		gtk_tree_view_column_set_sort_column_id (column, KANJI_STROKE);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
		
		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Radical", renderer, "text", RADICAL, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
		
		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("R. Stroke", renderer, "text", RADICAL_STROKE, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_column_set_reorderable (column, TRUE);
		gtk_tree_view_column_set_sort_indicator (column, TRUE);
		gtk_tree_view_column_set_sort_column_id (column, RADICAL_STROKE);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

		renderer = gtk_cell_renderer_text_new (); 

		column = gtk_tree_view_column_new_with_attributes ("JLPT", renderer, "text", JLPT_LEVEL, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_column_set_reorderable (column, TRUE);
		gtk_tree_view_column_set_sort_indicator (column, TRUE);
		gtk_tree_view_column_set_sort_column_id (column, JLPT_LEVEL);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
		
		renderer = gtk_cell_renderer_text_new ();

		column = gtk_tree_view_column_new_with_attributes ("Grade", renderer, "text", SCHOOL_GRADE, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_column_set_reorderable (column, TRUE);
		gtk_tree_view_column_set_sort_indicator (column, TRUE);
		gtk_tree_view_column_set_sort_column_id (column, SCHOOL_GRADE);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
}
