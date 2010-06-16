#include <gtk/gtk.h>
#include <string.h>

#include "kanji.h"
#include "view_kanji.h"

static void setup_tree_view (GtkWidget*);
static void view_kanji_flash_card (Kanji*);
static void row_activated (GtkTreeView*, GtkTreePath*, GtkTreeViewColumn*, gpointer);

void view_kanji (GArray *arr)
{
		//GtkWidget *entry;
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

		store = gtk_list_store_new (COLUMNS, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, 
						G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);

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

		g_signal_connect (G_OBJECT (treeview), "row-activated", G_CALLBACK (row_activated), NULL);

		scrolled_win = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

		gtk_container_add (GTK_CONTAINER (scrolled_win), treeview);

		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), scrolled_win, TRUE, TRUE, 5);

	//	gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview), TRUE);
	//	gtk_tree_view_set_search_column (GTK_TREE_VIEW (treeview), ON);
	//	entry = gtk_entry_new ();
	//	gtk_tree_view_set_search_entry (GTK_TREE_VIEW (treeview), GTK_ENTRY (entry));

	//	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), entry, FALSE, FALSE, 5);

		//gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);

		gtk_widget_show_all (dialog);
		result = gtk_dialog_run (GTK_DIALOG (dialog));

		gtk_widget_destroy (dialog);
}

//change ',' to '\n'
static gchar* simple_coma_parser (gchar *str)
{
		int i;

		for (i = 0; i < strlen (str); i++)
				if (str[i] == ',')
						str[i] = '\n';

		return str;
}

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
		gtk_label_set_markup (GTK_LABEL (on_label), g_strconcat ("<span font_desc='14'>", simple_coma_parser (kanji->on), "</span>", NULL));
		
		kun_label = gtk_label_new (NULL);
		gtk_label_set_selectable (GTK_LABEL (kun_label), TRUE);
		gtk_label_set_markup (GTK_LABEL (kun_label), g_strconcat ("<span font_desc='14'>", simple_coma_parser (kanji->kun), "</span>", NULL));
	
		meaning_label = gtk_label_new (simple_coma_parser (kanji->meaning));

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
}

static void row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer data)
{
		GtkTreeModel *model;
		GtkTreeIter iter;

		gchar *kanji, *radical, *on, *kun, *meaning;
		gint number, k_stroke, r_stroke, jlpt_lvl, grade;

		Kanji *tmp;

		model = gtk_tree_view_get_model (treeview);
		if (gtk_tree_model_get_iter (model, &iter, path))
		{
				gtk_tree_model_get (model, &iter, NUMBER, &number, KANJI, &kanji, KANJI_STROKE, &k_stroke, RADICAL, &radical, RADICAL_STROKE, &r_stroke, 
								ON, &on, KUN, &kun, MEANING, &meaning, JLPT_LEVEL, &jlpt_lvl, SCHOOL_GRADE, &grade, -1);

				tmp = kanji_create (kanji, radical, kun, on, meaning, jlpt_lvl, grade, k_stroke, r_stroke);
				view_kanji_flash_card (tmp);
				kanji_free (tmp);
		}
}

static void setup_tree_view (GtkWidget *treeview)
{
		GtkCellRenderer *renderer;
		GtkTreeViewColumn *column;

		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Num", renderer, "text", NUMBER, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_column_set_reorderable (column, TRUE);
		gtk_tree_view_column_set_sort_indicator (column, TRUE);
		gtk_tree_view_column_set_sort_column_id (column, NUMBER);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Kanji", renderer, "text", KANJI, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
		
		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Stroke", renderer, "text", KANJI_STROKE, NULL);
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
		column = gtk_tree_view_column_new_with_attributes ("Stroke", renderer, "text", RADICAL_STROKE, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_column_set_reorderable (column, TRUE);
		gtk_tree_view_column_set_sort_indicator (column, TRUE);
		gtk_tree_view_column_set_sort_column_id (column, RADICAL_STROKE);
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
