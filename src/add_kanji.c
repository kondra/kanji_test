#include <gtk/gtk.h>
#include <string.h>

#include "kanji.h"
#include "add_kanji.h"

typedef struct
{
		GtkWidget *dialog;
		GtkWidget *kanji_entry;
		GtkWidget *on_entry; 
		GtkWidget *meaning_entry;
		GtkWidget *radical_entry;
		GtkWidget *jlpt_spin;
		GtkWidget *grade_spin;
		GtkWidget *stroke_spin;
		GtkWidget *rst_spin;
} Widgets;

enum
{
		KUN_WRITING = 0,
		KUN_READING,
		KUN_MEANING,
		COLUMNS
};

static void upd_entry (GtkWidget*, Widgets*);
static void setup_tree_view (GtkTreeView*);
static void cell_edited (GtkCellRendererText*, gchar*, gchar*, GtkTreeView*);

static void row_add (GtkMenuItem*, GtkTreeView*);
static void row_remove (GtkMenuItem*, GtkTreeView*);
static Kanji* create_dialog (Kanji*, gboolean);

Kanji* kanji_add_dialog (void)
{
		return create_dialog (NULL, FALSE);
}

Kanji* kanji_edit_dialog (Kanji *old)
{
		return create_dialog (old, TRUE);
}

static void row_add (GtkMenuItem *item, GtkTreeView *treeview)
{
		GtkTreeModel *model = gtk_tree_view_get_model (treeview);
		GtkTreeIter iter;

		gtk_list_store_append (GTK_LIST_STORE (model), &iter);

		GtkTreePath *path = gtk_tree_model_get_path (model, &iter);
		GtkTreeViewColumn *column = gtk_tree_view_get_column (treeview, KUN_WRITING);

		gtk_tree_view_set_cursor_on_cell (treeview, path, column, NULL, TRUE);
}

static void row_remove (GtkMenuItem *item, GtkTreeView *treeview)
{
		GtkTreeSelection *selection;
		GtkTreeModel *model;
		GtkTreeIter iter;

		model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
		selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

		if (gtk_tree_selection_get_selected (selection, &model, &iter) == FALSE)
				return;
		
		gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
}

static void upd_entry (GtkWidget *cw, Widgets *w)
{
		gint val1, val2;
		guint16 l1, l2, l3;

		l1 = gtk_entry_get_text_length (GTK_ENTRY (w->kanji_entry));
		l2 = gtk_entry_get_text_length (GTK_ENTRY (w->radical_entry));
		l3 = gtk_entry_get_text_length (GTK_ENTRY (w->on_entry));
//		l4 = gtk_entry_get_text_length (GTK_ENTRY (w->meaning_entry));

		val1 = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (w->stroke_spin));
		val2 = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (w->rst_spin));

		if (val1 && val2 && l1 && l2)
				gtk_dialog_set_response_sensitive (GTK_DIALOG (w->dialog), GTK_RESPONSE_OK, TRUE);
		else
				gtk_dialog_set_response_sensitive (GTK_DIALOG (w->dialog), GTK_RESPONSE_OK, FALSE);
}

static void setup_tree_view (GtkTreeView *treeview)
{
		GtkCellRenderer *renderer;
		GtkTreeViewColumn *column;

		renderer = gtk_cell_renderer_text_new ();
		g_object_set (renderer, "editable", TRUE, "editable-set", TRUE, NULL);
		g_object_set_data (G_OBJECT (renderer), "column-num", GUINT_TO_POINTER (KUN_WRITING));

		g_signal_connect (G_OBJECT (renderer), "edited", G_CALLBACK (cell_edited), (gpointer) treeview);

		column = gtk_tree_view_column_new_with_attributes ("Writing", renderer, "text", KUN_WRITING, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_column_set_sort_column_id (column, KUN_WRITING);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

		renderer = gtk_cell_renderer_text_new ();
		g_object_set (renderer, "editable", TRUE, "editable-set", TRUE, NULL);
		g_object_set_data (G_OBJECT (renderer), "column-num", GUINT_TO_POINTER (KUN_READING));

		g_signal_connect (G_OBJECT (renderer), "edited", G_CALLBACK (cell_edited), (gpointer) treeview);

		column = gtk_tree_view_column_new_with_attributes ("Reading", renderer, "text", KUN_READING, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_column_set_sort_column_id (column, KUN_READING);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
	
		renderer = gtk_cell_renderer_text_new ();
		g_object_set (renderer, "editable", TRUE, "editable-set", TRUE, NULL);
		g_object_set_data (G_OBJECT (renderer), "column-num", GUINT_TO_POINTER (KUN_MEANING));

		g_signal_connect (G_OBJECT (renderer), "edited", G_CALLBACK (cell_edited), (gpointer) treeview);

		column = gtk_tree_view_column_new_with_attributes ("Meaning", renderer, "text", KUN_MEANING, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_column_set_sort_column_id (column, KUN_MEANING);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
}

static void cell_edited (GtkCellRendererText *renderer, gchar *path, gchar *new_text, GtkTreeView *treeview)
{
		GtkTreeIter iter;
		GtkTreeModel *model;

		guint num = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (renderer), "column-num"));

		model = gtk_tree_view_get_model (treeview);
		if (gtk_tree_model_get_iter_from_string (model, &iter, path))
				gtk_list_store_set (GTK_LIST_STORE (model), &iter, num, new_text, -1);
}

static Kanji* create_dialog (Kanji *old, gboolean mod)
{
		GtkWidget *treeview, *scrolled_win;
		GtkListStore *store;
		GtkTreeIter iter;
		//GtkAccelGroup *group;
		gint result;
		GtkWidget *table1, *table2, *lbl, *expander;
		GtkWidget *kanji_label, *on_label, *kun_label, *jlpt_label, *grade_label, *radical_label, *stroke_label, *rst_label;
		GtkWidget *add_button, *remove_button, *hbox;
//		PangoFontDescription *font_desc;
//		GList *focus_chain = NULL;//temp solution
		Widgets w;

		if (mod == FALSE)
				w.dialog = gtk_dialog_new_with_buttons ("Add New Kanji", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK,
								GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
		else
				w.dialog = gtk_dialog_new_with_buttons ("Edit Kanji", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK,
								GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

		gtk_container_set_border_width (GTK_CONTAINER (w.dialog), 5);
		gtk_widget_set_size_request (GTK_WIDGET (w.dialog), 800, 600);
		
		lbl = gtk_label_new ("Required fields:");

		kanji_label = gtk_label_new ("Kanji:");
		on_label = gtk_label_new ("ON reading:");
		kun_label = gtk_label_new ("Usage:");
		//meaning_label = gtk_label_new ("Principal Meaning:");
		jlpt_label = gtk_label_new ("JLPT Level:");
		grade_label = gtk_label_new ("School Grade:");
		radical_label = gtk_label_new ("Kanji Radical:");
		stroke_label = gtk_label_new ("Number of Kanji Strokes:");
		rst_label = gtk_label_new ("Number of Radical Strokes:");

		w.kanji_entry = gtk_entry_new ();
		w.on_entry = gtk_entry_new ();
//		w.kun_entry = gtk_entry_new ();
//		w.meaning_entry = gtk_entry_new ();
		w.radical_entry = gtk_entry_new ();

//		font_desc = pango_font_description_new ();
//		pango_font_description_set_size (font_desc, 45000);
//		gtk_widget_modify_font (GTK_WIDGET (w->kanji_entry), font_desc);
//		gtk_widget_modify_font (GTK_WIDGET (w->radical_entry), font_desc);

		g_signal_connect (G_OBJECT (w.kanji_entry), "changed", G_CALLBACK (upd_entry), (gpointer) &w);
		g_signal_connect (G_OBJECT (w.on_entry), "changed", G_CALLBACK (upd_entry), (gpointer) &w);
//		g_signal_connect (G_OBJECT (w.kun_entry), "changed", G_CALLBACK (upd_entry), (gpointer) &w);
//		g_signal_connect (G_OBJECT (w.meaning_entry), "changed", G_CALLBACK (upd_entry), (gpointer) &w);
		g_signal_connect (G_OBJECT (w.radical_entry), "changed", G_CALLBACK (upd_entry), (gpointer) &w);

		w.jlpt_spin = gtk_spin_button_new_with_range (0.0, 5.0, 1.0);
		w.grade_spin = gtk_spin_button_new_with_range (0.0, 9.0, 1.0);
		w.stroke_spin = gtk_spin_button_new_with_range (0.0, 30.0, 1.0);
		w.rst_spin = gtk_spin_button_new_with_range (0.0, 30.0, 1.0);

		gtk_spin_button_set_value (GTK_SPIN_BUTTON (w.jlpt_spin), 0.0);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (w.grade_spin), 0.0);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (w.stroke_spin), 0.0);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (w.rst_spin), 0.0);

		g_signal_connect (G_OBJECT (w.stroke_spin), "value_changed", G_CALLBACK (upd_entry), (gpointer) &w);
		g_signal_connect (G_OBJECT (w.rst_spin), "value_changed", G_CALLBACK (upd_entry), (gpointer) &w);

//Tree view/////////////////////////////////////////////////
		treeview = gtk_tree_view_new ();
		setup_tree_view (GTK_TREE_VIEW (treeview));

		store = gtk_list_store_new (COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

		if (mod == TRUE)
		{
				int i;
				for (i = 0; i < old->num; i++)
				{
						gtk_list_store_append (store, &iter);
						gtk_list_store_set (store, &iter, KUN_WRITING, old->kun_writing[i], KUN_READING, old->kun_reading[i], KUN_MEANING, old->kun_meaning[i], -1);
				}
		}
//		else
//				gtk_list_store_append (store, &iter);

		gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
		gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview), FALSE);
		g_object_unref (store);

		//g_signal_connect (G_OBJECT (treeview), "row-activated", G_CALLBACK (row_activated), (gpointer)arr);

//Tree view end/////////////////////////////////////////////
//pop-up menu
/*		
		GtkWidget *menu = gtk_menu_new ();
		GtkWidget *add = gtk_menu_item_new_with_label ("Add");
		GtkWidget *del = gtk_menu_item_new_with_label ("Remove");

		g_signal_connect (G_OBJECT (add), "activate", G_CALLBACK (row_add), (gpointer) treeview);
		g_signal_connect (G_OBJECT (del), "activate", G_CALLBACK (row_remove), (gpointer) treeview);

		gtk_menu_shell_append (GTK_MENU_SHELL (menu), add);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), del);

		gtk_menu_attach_to_widget (GTK_MENU (menu), GTK_WIDGET (treeview), NULL);
		gtk_widget_show_all (menu);

		g_signal_connect (G_OBJECT (treeview), "button-press-event", G_CALLBACK (popup_handler), GTK_MENU (menu));
*/

		add_button = gtk_button_new_from_stock (GTK_STOCK_ADD);
		remove_button = gtk_button_new_from_stock (GTK_STOCK_REMOVE);

		g_signal_connect (G_OBJECT (add_button), "clicked", G_CALLBACK (row_add), (gpointer) treeview);
		g_signal_connect (G_OBJECT (remove_button), "clicked", G_CALLBACK (row_remove), (gpointer) treeview);

		hbox = gtk_hbox_new (FALSE, 5);

		gtk_box_pack_start (GTK_BOX (hbox), add_button, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox), remove_button, FALSE, FALSE, 5);

////////pop-up end//////////////////////////
//accelerators///////////////////////////
/*
		group = gtk_accel_group_new ();
		gtk_window_add_accel_group (GTK_WINDOW (w.dialog), group);
		gtk_menu_set_accel_group (GTK_MENU (menu), group);
		gtk_widget_add_accelerator (treeview, "activate", group, GDK_DELETE, 0, 0);

		g_signal_connect (G_OBJECT (treeview), "activate", G_CALLBACK (row_remove)
*/
/////////////////////////////////////////
//
		scrolled_win = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		
		gtk_container_add (GTK_CONTAINER (scrolled_win), treeview);

		table2 = gtk_table_new (2, 2, FALSE);

		gtk_table_attach (GTK_TABLE (table2), jlpt_label, 0, 1, 1, 2, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table2), grade_label, 0, 1, 2, 3, GTK_EXPAND, GTK_SHRINK, 0, 0);

		gtk_table_attach (GTK_TABLE (table2), w.jlpt_spin, 1, 2, 1, 2, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table2), w.grade_spin, 1, 2, 2, 3, GTK_EXPAND, GTK_SHRINK, 0, 0);
		
		expander = gtk_expander_new ("Optional fields");
		gtk_container_add (GTK_CONTAINER (expander), table2);

		gtk_table_set_row_spacings (GTK_TABLE (table2), 5);
		gtk_table_set_col_spacings (GTK_TABLE (table2), 5);

		table1 = gtk_table_new (10, 2, FALSE);

		gtk_table_attach (GTK_TABLE (table1), lbl, 0, 1, 0, 1, GTK_EXPAND, GTK_SHRINK, 0, 0);

		gtk_table_attach (GTK_TABLE (table1), kanji_label, 0, 1, 1, 2, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table1), radical_label, 0, 1, 2, 3, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table1), stroke_label, 0, 1, 3, 4, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table1), rst_label, 0, 1, 4, 5, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table1), on_label, 0, 1, 5, 6, GTK_EXPAND, GTK_SHRINK, 0, 0);
//		gtk_table_attach (GTK_TABLE (table1), kun_label, 0, 1, 6, 7, GTK_EXPAND, GTK_SHRINK, 0, 0);
//		gtk_table_attach (GTK_TABLE (table1), meaning_label, 0, 1, 6, 7, GTK_EXPAND, GTK_SHRINK, 0, 0);

		gtk_table_attach (GTK_TABLE (table1), kun_label, 0, 1, 7, 8, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach_defaults (GTK_TABLE (table1), scrolled_win, 0, 2, 8, 9);//, GTK_EXPAND, GTK_SHRINK, 0, 0);

		gtk_table_attach (GTK_TABLE (table1), w.kanji_entry, 1, 2, 1, 2, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table1), w.radical_entry, 1, 2, 2, 3, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table1), w.stroke_spin, 1, 2, 3, 4, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table1), w.rst_spin, 1, 2, 4, 5, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (table1), w.on_entry, 1, 2, 5, 6, GTK_EXPAND, GTK_SHRINK, 0, 0);
//		gtk_table_attach (GTK_TABLE (table1), w.kun_entry, 1, 2, 6, 7, GTK_EXPAND, GTK_SHRINK, 0, 0);
//		gtk_table_attach (GTK_TABLE (table1), w.meaning_entry, 1, 2, 6, 7, GTK_EXPAND, GTK_SHRINK, 0, 0);

		gtk_table_attach (GTK_TABLE (table1), hbox, 0, 1, 9, 10, GTK_EXPAND, GTK_SHRINK, 0, 0);

		gtk_table_set_row_spacings (GTK_TABLE (table1), 5);
		gtk_table_set_col_spacings (GTK_TABLE (table1), 5);

		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (w.dialog)->vbox), table1, TRUE, TRUE, 5);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (w.dialog)->vbox), expander, FALSE, FALSE, 5);
		
		gtk_dialog_set_response_sensitive (GTK_DIALOG (w.dialog), GTK_RESPONSE_OK, FALSE);
		gtk_dialog_set_has_separator (GTK_DIALOG (w.dialog), FALSE);

		if (mod == TRUE)
		{
				if (old == NULL)
						g_error ("add kanji: old kanji null pointer");

				gtk_entry_set_text (GTK_ENTRY (w.kanji_entry), old->str);
				gtk_entry_set_text (GTK_ENTRY (w.on_entry), old->on);
//				gtk_entry_set_text (GTK_ENTRY (w.kun_entry), old->kun);
//				gtk_entry_set_text (GTK_ENTRY (w.meaning_entry), old->meaning);
				gtk_entry_set_text (GTK_ENTRY (w.radical_entry), old->radical);

				gtk_spin_button_set_value (GTK_SPIN_BUTTON (w.stroke_spin), old->stroke);
				gtk_spin_button_set_value (GTK_SPIN_BUTTON (w.rst_spin), old->radical_stroke);
				gtk_spin_button_set_value (GTK_SPIN_BUTTON (w.grade_spin), old->grade);
				gtk_spin_button_set_value (GTK_SPIN_BUTTON (w.jlpt_spin), old->jlpt_level);
		}

		gtk_widget_show_all (w.dialog);
		result = gtk_dialog_run (GTK_DIALOG (w.dialog));

		if (result == GTK_RESPONSE_OK)
		{
				const gchar *kanji_str = gtk_entry_get_text (GTK_ENTRY (w.kanji_entry));
				const gchar *on_str = gtk_entry_get_text (GTK_ENTRY (w.on_entry));
//				const gchar *meaning_str = gtk_entry_get_text (GTK_ENTRY (w.meaning_entry));
				const gchar *radical_str = gtk_entry_get_text (GTK_ENTRY (w.radical_entry));
				
				gint stroke_cnt = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (w.stroke_spin));
				gint rst_cnt = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (w.rst_spin));
				gint jlpt_lvl = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (w.jlpt_spin));
				gint grade = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (w.grade_spin));

				GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
				gint num = gtk_tree_model_iter_n_children (model, NULL);
				gint i = 0;

				gchar **writing, **reading, **meaning;

				writing = (gchar**) g_malloc0 (sizeof (gchar*) * num);
				reading = (gchar**) g_malloc0 (sizeof (gchar*) * num);
				meaning = (gchar**) g_malloc0 (sizeof (gchar*) * num);

				if (gtk_tree_model_get_iter_first (model, &iter))
				{
						do
						{
								gtk_tree_model_get (model, &iter, KUN_WRITING, &writing[i], KUN_READING, &reading[i], KUN_MEANING, &meaning[i], -1);
								i++;
						} while (gtk_tree_model_iter_next (model, &iter));
				}

				Kanji *tmp;
				tmp = kanji_create (kanji_str, radical_str, on_str, NULL, jlpt_lvl, grade, stroke_cnt, rst_cnt, num, writing, reading, meaning);

				gtk_widget_destroy (w.dialog);

				g_free (writing);
				g_free (reading);
				g_free (meaning);

				return tmp;
		}
		
		gtk_widget_destroy (w.dialog);

		return NULL;
}
