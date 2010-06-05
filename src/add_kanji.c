#include <gtk/gtk.h>

typedef struct
{
		GtkWidget *dialog, *table1, *table2, *lbl, *expander;
		GtkWidget *kanji_label, *on_label, *kun_label, *trans_label, *jlpt_label, *grade_label, *radical_label, *stroke_label;
		GtkWidget *kanji_entry, *on_entry, *kun_entry, *trans_entry, *radical_entry, *jlpt_spin, *grade_spin, *stroke_spin;
} Widgets;

static int create_dialog (void);
static void upd_entry (GtkWidget*, Widgets*);

int main (int argc, char *argv[])
{
		gtk_init (&argc, &argv);
		create_dialog ();
		
		return 0;
}

static int create_dialog (void)
{
		gint result;
		PangoFontDescription *font_desc;
		Widgets *w;

		w = g_slice_new (Widgets);

		w->dialog = gtk_dialog_new_with_buttons ("Add New Kanji", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

		gtk_container_set_border_width (GTK_CONTAINER (w->dialog), 5);
		
		w->lbl = gtk_label_new ("Required fields:");

		w->kanji_label = gtk_label_new ("Kanji:");
		w->on_label = gtk_label_new ("ON reading:");
		w->kun_label = gtk_label_new ("KUN reading:");
		w->trans_label = gtk_label_new ("Translation:");
		w->jlpt_label = gtk_label_new ("JLPT Level:");
		w->grade_label = gtk_label_new ("School Grade:");
		w->radical_label = gtk_label_new ("Kanji Radical:");
		w->stroke_label = gtk_label_new ("Number of Strokes:");

		w->kanji_entry = gtk_entry_new ();
		w->on_entry = gtk_entry_new ();
		w->kun_entry = gtk_entry_new ();
		w->trans_entry = gtk_entry_new ();
		w->radical_entry = gtk_entry_new ();

		font_desc = pango_font_description_new ();
		pango_font_description_set_size (font_desc, 45000);
		gtk_widget_modify_font (GTK_WIDGET (w->kanji_entry), font_desc);
		gtk_widget_modify_font (GTK_WIDGET (w->radical_entry), font_desc);

		g_signal_connect (G_OBJECT (w->kanji_entry), "changed", G_CALLBACK (upd_entry), (gpointer) w);
		g_signal_connect (G_OBJECT (w->on_entry), "changed", G_CALLBACK (upd_entry), (gpointer) w);
		g_signal_connect (G_OBJECT (w->kun_entry), "changed", G_CALLBACK (upd_entry), (gpointer) w);
		g_signal_connect (G_OBJECT (w->trans_entry), "changed", G_CALLBACK (upd_entry), (gpointer) w);
		g_signal_connect (G_OBJECT (w->radical_entry), "changed", G_CALLBACK (upd_entry), (gpointer) w);

		w->jlpt_spin = gtk_spin_button_new_with_range (0.0, 5.0, 1.0);
		w->grade_spin = gtk_spin_button_new_with_range (0.0, 9.0, 1.0);
		w->stroke_spin = gtk_spin_button_new_with_range (0.0, 30.0, 1.0);

		gtk_spin_button_set_value (GTK_SPIN_BUTTON (w->jlpt_spin), 0.0);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (w->grade_spin), 0.0);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (w->stroke_spin), 0.0);

		g_signal_connect (G_OBJECT (w->stroke_spin), "value_changed", G_CALLBACK (upd_entry), (gpointer) w);

		w->table2 = gtk_table_new (2, 2, FALSE);

		gtk_table_attach (GTK_TABLE (w->table2), w->jlpt_label, 0, 1, 1, 2, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (w->table2), w->grade_label, 0, 1, 2, 3, GTK_EXPAND, GTK_SHRINK, 0, 0);

		gtk_table_attach (GTK_TABLE (w->table2), w->jlpt_spin, 1, 2, 1, 2, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (w->table2), w->grade_spin, 1, 2, 2, 3, GTK_EXPAND, GTK_SHRINK, 0, 0);
		
		w->expander = gtk_expander_new ("Optional fields");
		gtk_container_add (GTK_CONTAINER (w->expander), w->table2);

		gtk_table_set_row_spacings (GTK_TABLE (w->table2), 5);
		gtk_table_set_col_spacings (GTK_TABLE (w->table2), 5);

		w->table1 = gtk_table_new (7, 4, FALSE);

		gtk_table_attach (GTK_TABLE (w->table1), w->lbl, 0, 1, 0, 1, GTK_EXPAND, GTK_SHRINK, 0, 0);

		gtk_table_attach (GTK_TABLE (w->table1), w->kanji_label, 0, 1, 1, 2, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (w->table1), w->radical_label, 0, 1, 4, 5, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (w->table1), w->stroke_label, 2, 3, 1, 2, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (w->table1), w->on_label, 2, 3, 2, 3, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (w->table1), w->kun_label, 2, 3, 3, 4, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (w->table1), w->trans_label, 2, 3, 4, 5, GTK_EXPAND, GTK_SHRINK, 0, 0);

		gtk_table_attach (GTK_TABLE (w->table1), w->kanji_entry, 1, 2, 1, 4, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (w->table1), w->radical_entry, 1, 2, 4, 7, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (w->table1), w->stroke_spin, 3, 4, 1, 2, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (w->table1), w->on_entry, 3, 4, 2, 3, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (w->table1), w->kun_entry, 3, 4, 3, 4, GTK_EXPAND, GTK_SHRINK, 0, 0);
		gtk_table_attach (GTK_TABLE (w->table1), w->trans_entry, 3, 4, 4, 5, GTK_EXPAND, GTK_SHRINK, 0, 0);

		gtk_table_set_row_spacings (GTK_TABLE (w->table1), 5);
		gtk_table_set_col_spacings (GTK_TABLE (w->table1), 5);

		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (w->dialog)->vbox), w->table1, FALSE, FALSE, 5);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (w->dialog)->vbox), w->expander, FALSE, FALSE, 5);
		
		gtk_dialog_set_response_sensitive (GTK_DIALOG (w->dialog), GTK_RESPONSE_OK, FALSE);
		gtk_dialog_set_has_separator (GTK_DIALOG (w->dialog), FALSE);

		gtk_widget_show_all (w->dialog);
		result = gtk_dialog_run (GTK_DIALOG (w->dialog));
		
		gtk_widget_destroy (w->dialog);
		g_slice_free (Widgets, w);

		return 0;
}

static void upd_entry (GtkWidget *cw, Widgets *w)
{
		gint val;
		guint16 l1, l2, l3, l4, l5;

		l1 = gtk_entry_get_text_length (GTK_ENTRY (w->kanji_entry));
		l2 = gtk_entry_get_text_length (GTK_ENTRY (w->radical_entry));
		l3 = gtk_entry_get_text_length (GTK_ENTRY (w->on_entry));
		l4 = gtk_entry_get_text_length (GTK_ENTRY (w->kun_entry));
		l5 = gtk_entry_get_text_length (GTK_ENTRY (w->trans_entry));

		val = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (w->stroke_spin));

		if (val && l1 && l2 && (l3 || l4) && l5)
				gtk_dialog_set_response_sensitive (GTK_DIALOG (w->dialog), GTK_RESPONSE_OK, TRUE);
		else
				gtk_dialog_set_response_sensitive (GTK_DIALOG (w->dialog), GTK_RESPONSE_OK, FALSE);
}
