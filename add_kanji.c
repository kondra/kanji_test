#include <gtk/gtk.h>

static int create_dialog (void);

int main (int argc, char *argv[])
{
		gtk_init (&argc, &argv);
		create_dialog ();
		
		return 0;
}

static int create_dialog (void)
{
		GtkWidget *dialog;
		gint result;

		dialog = gtk_dialog_new_with_buttons ("Add New Kanji", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

		GtkWidget *table;
		GtkWidget *kanji_label, *on_label, *kun_label, *trans_label, *jlpt_label, *grade_label, *key_label, *stroke_label;
		GtkWidget *kanji_entry, *on_entry, *kun_entry, *trans_entry, *jlpt_entry, *grade_entry, *key_entry, *stroke_entry;

		kanji_label = gtk_label_new ("Kanji:");
		on_label = gtk_label_new ("ON reading:");
		kun_label = gtk_label_new ("KUN reading:");
		trans_label = gtk_label_new ("Translation:");
		jlpt_label = gtk_label_new ("JLPT Level:");
		grade_label = gtk_label_new ("School Grade:");
		key_label = gtk_label_new ("Kanji Key:");
		stroke_label = gtk_label_new ("Number of Strokes:");

		kanji_entry = gtk_entry_new ();
		on_entry = gtk_entry_new ();
		kun_entry = gtk_entry_new ();
		trans_entry = gtk_entry_new ();
		jlpt_entry = gtk_entry_new ();
		grade_entry = gtk_entry_new ();
		key_entry = gtk_entry_new ();
		stroke_entry = gtk_entry_new ();

		table = gtk_table_new (8, 2, FALSE);
		gtk_table_attach_defaults (GTK_TABLE (table), kanji_label, 0, 1, 0, 1);
		gtk_table_attach_defaults (GTK_TABLE (table), on_label, 0, 1, 1, 2);
		gtk_table_attach_defaults (GTK_TABLE (table), kun_label, 0, 1, 2, 3);
		gtk_table_attach_defaults (GTK_TABLE (table), trans_label, 0, 1, 3, 4);
		gtk_table_attach_defaults (GTK_TABLE (table), jlpt_label, 0, 1, 4, 5);
		gtk_table_attach_defaults (GTK_TABLE (table), grade_label, 0, 1, 5, 6);
		gtk_table_attach_defaults (GTK_TABLE (table), key_label, 0, 1, 6, 7);
		gtk_table_attach_defaults (GTK_TABLE (table), stroke_label, 0, 1, 7, 8);

		gtk_table_attach_defaults (GTK_TABLE (table), kanji_entry, 1, 2, 0, 1);
		gtk_table_attach_defaults (GTK_TABLE (table), on_entry, 1, 2, 1, 2);
		gtk_table_attach_defaults (GTK_TABLE (table), kun_entry, 1, 2, 2, 3);
		gtk_table_attach_defaults (GTK_TABLE (table), trans_entry, 1, 2, 3, 4);
		gtk_table_attach_defaults (GTK_TABLE (table), jlpt_entry, 1, 2, 4, 5);
		gtk_table_attach_defaults (GTK_TABLE (table), grade_entry, 1, 2, 5, 6);
		gtk_table_attach_defaults (GTK_TABLE (table), key_entry, 1, 2, 6, 7);
		gtk_table_attach_defaults (GTK_TABLE (table), stroke_entry, 1, 2, 7, 8);

		gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dialog)->vbox), table);
		gtk_widget_show_all (dialog);

		result = gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
		return 0;
}
