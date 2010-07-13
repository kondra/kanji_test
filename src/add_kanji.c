#include <gtk/gtk.h>

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
		WORD_WRITING = 0,
		WORD_READING,
		WORD_MEANING,
		COLUMNS
};

const gchar *column_names[] = {"Writing", "Reading", "Meaning"};

static void input_check (GtkWidget*, Widgets*);
static void setup_tree_view (GtkTreeView*);
static void cell_edited (GtkCellRendererText*, gchar*, gchar*, GtkTreeView*);
static void row_add (GtkMenuItem*, GtkTreeView*);
static void row_remove (GtkMenuItem*, GtkTreeView*);

static Kanji* create_dialog (Kanji*, gboolean);

static void setup_data (Kanji*, GtkListStore*, Widgets*);
static Kanji* get_data (GtkTreeView*, Widgets*);

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

static void input_check (GtkWidget *cw, Widgets *w)
{
		guint16 l1 = gtk_entry_get_text_length (GTK_ENTRY (w->kanji_entry));
		guint16 l2 = gtk_entry_get_text_length (GTK_ENTRY (w->radical_entry));
		guint16 l3 = gtk_entry_get_text_length (GTK_ENTRY (w->on_entry));

		gint val1 = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (w->stroke_spin));
		gint val2 = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (w->rst_spin));

		if (val1 && val2 && l1 && l2 && l3)
				gtk_dialog_set_response_sensitive (GTK_DIALOG (w->dialog), GTK_RESPONSE_OK, TRUE);
		else
				gtk_dialog_set_response_sensitive (GTK_DIALOG (w->dialog), GTK_RESPONSE_OK, FALSE);
}

static Kanji* get_data (GtkTreeView *treeview, Widgets *w)
{
		GtkTreeIter iter;

		const gchar *kanji_str = gtk_entry_get_text (GTK_ENTRY (w->kanji_entry));
		const gchar *on_str = gtk_entry_get_text (GTK_ENTRY (w->on_entry));
		const gchar *radical_str = gtk_entry_get_text (GTK_ENTRY (w->radical_entry));
		
		gint stroke_cnt = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (w->stroke_spin));
		gint rst_cnt = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (w->rst_spin));
		gint jlpt_lvl = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (w->jlpt_spin));
		gint grade = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (w->grade_spin));

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
						gtk_tree_model_get (model, &iter, 
										WORD_WRITING, &writing[i], 
										WORD_READING, &reading[i], 
										WORD_MEANING, &meaning[i], -1);
						i++;
				} while (gtk_tree_model_iter_next (model, &iter));
		}

		Kanji *tmp = kanji_create (kanji_str, radical_str, on_str, NULL, jlpt_lvl, grade, stroke_cnt, rst_cnt, num, writing, reading, meaning);

		g_free (writing);
		g_free (reading);
		g_free (meaning);

		return tmp;
}

static void setup_data (Kanji *old, GtkListStore *store, Widgets *w)
{
		GtkTreeIter iter;
		gint i;

		if (old == NULL)
				g_error ("add_kanji: old kanji NULL pointer received (setup_data)");

		gtk_entry_set_text (GTK_ENTRY (w->kanji_entry), old->kanji);
		gtk_entry_set_text (GTK_ENTRY (w->on_entry), old->on);
		gtk_entry_set_text (GTK_ENTRY (w->radical_entry), old->radical);

		gtk_spin_button_set_value (GTK_SPIN_BUTTON (w->stroke_spin), old->kanji_stroke);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (w->rst_spin), old->radical_stroke);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (w->grade_spin), old->grade);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (w->jlpt_spin), old->jlpt_level);

		for (i = 0; i < old->num; i++)
		{
				gtk_list_store_append (store, &iter);
				gtk_list_store_set (store, &iter, 
								WORD_WRITING, old->word_writing[i], 
								WORD_READING, old->word_reading[i], 
								WORD_MEANING, old->word_meaning[i], -1);
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

		model = gtk_tree_view_get_model (treeview);
		if (gtk_tree_model_get_iter_from_string (model, &iter, path))
				gtk_list_store_set (GTK_LIST_STORE (model), &iter, num, new_text, -1);
}

static Kanji* create_dialog (Kanji *old, gboolean mod)
{
		GtkWidget *hbox_required, *hbox_additional;
		GtkWidget *vbox1_additional, *vbox2_additional;
		GtkWidget *vbox1_required, *vbox2_required;
		GtkWidget *expander;
		GtkWidget *scrolled_win;
		GtkWidget *hbox;

		GtkWidget *treeview;
		GtkListStore *store;
		gint result;

		Widgets w;

		if (mod == FALSE)
				w.dialog = gtk_dialog_new_with_buttons ("Add New Kanji", NULL, GTK_DIALOG_MODAL, 
								GTK_STOCK_OK, GTK_RESPONSE_OK,
								GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
		else
				w.dialog = gtk_dialog_new_with_buttons ("Edit Kanji", NULL, GTK_DIALOG_MODAL,
								GTK_STOCK_OK, GTK_RESPONSE_OK,
								GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

		gtk_container_set_border_width (GTK_CONTAINER (w.dialog), 5);
		gtk_widget_set_size_request (GTK_WIDGET (w.dialog), 600, 500);
		
		GtkWidget* kanji_label = gtk_label_new ("Kanji:");
		GtkWidget* on_label = gtk_label_new ("ON reading:");
		GtkWidget* jlpt_label = gtk_label_new ("JLPT Level:");
		GtkWidget* grade_label = gtk_label_new ("School Grade:");
		GtkWidget* radical_label = gtk_label_new ("Kanji Radical:");
		GtkWidget* stroke_label = gtk_label_new ("Kanji Strokes:");
		GtkWidget* rst_label = gtk_label_new ("Radical Strokes:");

		w.kanji_entry = gtk_entry_new ();
		w.on_entry = gtk_entry_new ();
		w.radical_entry = gtk_entry_new ();

		g_signal_connect (G_OBJECT (w.kanji_entry), "changed", G_CALLBACK (input_check), (gpointer) &w);
		g_signal_connect (G_OBJECT (w.on_entry), "changed", G_CALLBACK (input_check), (gpointer) &w);
		g_signal_connect (G_OBJECT (w.radical_entry), "changed", G_CALLBACK (input_check), (gpointer) &w);

		w.jlpt_spin = gtk_spin_button_new_with_range (0.0, 5.0, 1.0);
		w.grade_spin = gtk_spin_button_new_with_range (0.0, 9.0, 1.0);
		w.stroke_spin = gtk_spin_button_new_with_range (0.0, 30.0, 1.0);
		w.rst_spin = gtk_spin_button_new_with_range (0.0, 30.0, 1.0);

		gtk_spin_button_set_value (GTK_SPIN_BUTTON (w.jlpt_spin), 0.0);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (w.grade_spin), 0.0);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (w.stroke_spin), 0.0);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (w.rst_spin), 0.0);

		g_signal_connect (G_OBJECT (w.stroke_spin), "value_changed", G_CALLBACK (input_check), (gpointer) &w);
		g_signal_connect (G_OBJECT (w.rst_spin), "value_changed", G_CALLBACK (input_check), (gpointer) &w);

		treeview = gtk_tree_view_new ();
		setup_tree_view (GTK_TREE_VIEW (treeview));

		store = gtk_list_store_new (COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

		gtk_dialog_set_response_sensitive (GTK_DIALOG (w.dialog), GTK_RESPONSE_OK, FALSE);
		if (mod == TRUE)
		{
				setup_data (old, store, &w);
				input_check (NULL, &w);
		}

		gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
		gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview), FALSE);
		g_object_unref (store);

		GtkWidget* add_button = gtk_button_new_from_stock (GTK_STOCK_ADD);
		GtkWidget* remove_button = gtk_button_new_from_stock (GTK_STOCK_REMOVE);

		g_signal_connect (G_OBJECT (add_button), "clicked", G_CALLBACK (row_add), (gpointer) treeview);
		g_signal_connect (G_OBJECT (remove_button), "clicked", G_CALLBACK (row_remove), (gpointer) treeview);

		hbox = gtk_hbox_new (FALSE, 5);

		gtk_box_pack_start (GTK_BOX (hbox), add_button, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox), remove_button, FALSE, FALSE, 5);

		scrolled_win = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		
		gtk_container_add (GTK_CONTAINER (scrolled_win), treeview);

		vbox1_additional = gtk_vbox_new (FALSE, 5);
		vbox2_additional = gtk_vbox_new (FALSE, 5);

		gtk_box_pack_start (GTK_BOX (vbox1_additional), jlpt_label, FALSE, FALSE, 6);
		gtk_box_pack_start (GTK_BOX (vbox1_additional), grade_label, FALSE, FALSE, 10);

		gtk_box_pack_start (GTK_BOX (vbox2_additional), w.jlpt_spin, FALSE, FALSE, 3);
		gtk_box_pack_start (GTK_BOX (vbox2_additional), w.grade_spin, FALSE, FALSE, 3);
		
		hbox_additional = gtk_hbox_new (FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox_additional), vbox1_additional, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox_additional), vbox2_additional, FALSE, FALSE, 5);

		expander = gtk_expander_new ("Optional fields");
		gtk_container_add (GTK_CONTAINER (expander), hbox_additional);

		vbox1_required = gtk_vbox_new (FALSE, 5);

		gtk_box_pack_start (GTK_BOX (vbox1_required), kanji_label, TRUE, TRUE, 2);
		gtk_box_pack_start (GTK_BOX (vbox1_required), radical_label, TRUE, TRUE, 2);
		gtk_box_pack_start (GTK_BOX (vbox1_required), stroke_label, TRUE, TRUE, 2);
		gtk_box_pack_start (GTK_BOX (vbox1_required), rst_label, TRUE, TRUE, 2);
		gtk_box_pack_start (GTK_BOX (vbox1_required), on_label, TRUE, TRUE, 2);

		vbox2_required = gtk_vbox_new (FALSE, 5);

		gtk_box_pack_start (GTK_BOX (vbox2_required), w.kanji_entry, FALSE, FALSE, 2);
		gtk_box_pack_start (GTK_BOX (vbox2_required), w.radical_entry, FALSE, FALSE, 2);
		gtk_box_pack_start (GTK_BOX (vbox2_required), w.stroke_spin, FALSE, FALSE, 2);
		gtk_box_pack_start (GTK_BOX (vbox2_required), w.rst_spin, FALSE, FALSE, 2);
		gtk_box_pack_start (GTK_BOX (vbox2_required), w.on_entry, FALSE, FALSE, 2);

		hbox_required = gtk_hbox_new (FALSE, 5);

		gtk_box_pack_start (GTK_BOX (hbox_required), vbox1_required, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox_required), vbox2_required, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (hbox_required), expander, FALSE, FALSE, 5);

		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (w.dialog)->vbox), hbox_required, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (w.dialog)->vbox), scrolled_win, TRUE, TRUE, 5);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (w.dialog)->vbox), hbox, FALSE, FALSE, 5);
		
		gtk_dialog_set_has_separator (GTK_DIALOG (w.dialog), FALSE);

		gtk_widget_show_all (w.dialog);
		result = gtk_dialog_run (GTK_DIALOG (w.dialog));

		Kanji *tmp= NULL;
		if (result == GTK_RESPONSE_OK)
				tmp = get_data (GTK_TREE_VIEW (treeview), &w);
		
		gtk_widget_destroy (w.dialog);

		return tmp;
}
