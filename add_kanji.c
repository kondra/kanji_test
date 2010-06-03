#include <gtk/gtk.h>

typedef struct
{
		GtkWidget *kanji_label, *on_label, *kun_label, *trans_label, *jlpt_label, *grade_label, *key_label, *stroke_label;
		GtkWidget *kanji_entry, *on_entry, *kun_entry, *trans_entry, *jlpt_entry, *grade_entry, *key_entry, *stroke_entry;
} Widgets;

static int create_dialog (void);

int main (int argc, char *argv[])
{
		gtk_init (&argc, &argv);
		create_dialog ();
		
		return 0;
}

static int create_dialog (void)
{
		gint result;
		guint16 length;
		GtkWidget *dialog, *table;
		Widgets w;

		dialog = gtk_dialog_new_with_buttons ("Add New Kanji", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

		w.kanji_label = gtk_label_new ("Kanji:");
		w.on_label = gtk_label_new ("ON reading:");
		w.kun_label = gtk_label_new ("KUN reading:");
		w.trans_label = gtk_label_new ("Translation:");
		w.jlpt_label = gtk_label_new ("JLPT Level:");
		w.grade_label = gtk_label_new ("School Grade:");
		w.key_label = gtk_label_new ("Kanji Key:");
		w.stroke_label = gtk_label_new ("Number of Strokes:");

		w.kanji_entry = gtk_entry_new ();
		w.on_entry = gtk_entry_new ();
		w.kun_entry = gtk_entry_new ();
		w.trans_entry = gtk_entry_new ();
		w.jlpt_entry = gtk_entry_new ();
		w.grade_entry = gtk_entry_new ();
		w.key_entry = gtk_entry_new ();
		w.stroke_entry = gtk_entry_new ();

		table = gtk_table_new (8, 2, FALSE);
		gtk_table_attach_defaults (GTK_TABLE (table), w.kanji_label, 0, 1, 0, 1);
		gtk_table_attach_defaults (GTK_TABLE (table), w.on_label, 0, 1, 1, 2);
		gtk_table_attach_defaults (GTK_TABLE (table), w.kun_label, 0, 1, 2, 3);
		gtk_table_attach_defaults (GTK_TABLE (table), w.trans_label, 0, 1, 3, 4);
		gtk_table_attach_defaults (GTK_TABLE (table), w.jlpt_label, 0, 1, 4, 5);
		gtk_table_attach_defaults (GTK_TABLE (table), w.grade_label, 0, 1, 5, 6);
		gtk_table_attach_defaults (GTK_TABLE (table), w.key_label, 0, 1, 6, 7);
		gtk_table_attach_defaults (GTK_TABLE (table), w.stroke_label, 0, 1, 7, 8);

		gtk_table_attach_defaults (GTK_TABLE (table), w.kanji_entry, 1, 2, 0, 1);
		gtk_table_attach_defaults (GTK_TABLE (table), w.on_entry, 1, 2, 1, 2);
		gtk_table_attach_defaults (GTK_TABLE (table), w.kun_entry, 1, 2, 2, 3);
		gtk_table_attach_defaults (GTK_TABLE (table), w.trans_entry, 1, 2, 3, 4);
		gtk_table_attach_defaults (GTK_TABLE (table), w.jlpt_entry, 1, 2, 4, 5);
		gtk_table_attach_defaults (GTK_TABLE (table), w.grade_entry, 1, 2, 5, 6);
		gtk_table_attach_defaults (GTK_TABLE (table), w.key_entry, 1, 2, 6, 7);
		gtk_table_attach_defaults (GTK_TABLE (table), w.stroke_entry, 1, 2, 7, 8);

		gtk_table_set_row_spacings (GTK_TABLE (table), 5);
		gtk_table_set_col_spacings (GTK_TABLE (table), 5);

		gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dialog)->vbox), table);
		gtk_widget_show_all (dialog);

		while (1)
		{
				result = gtk_dialog_run (GTK_DIALOG (dialog));

				if (result == GTK_RESPONSE_OK)
				{
						length = gtk_entry_get_text_length (GTK_ENTRY (w.kanji_entry));

						if (length == 0)
								g_print ("FFFUUU");
						else
								break;
				}
				else
						break;
		}

		gtk_widget_destroy (dialog);
		return 0;
}
