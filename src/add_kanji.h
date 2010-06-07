#ifndef ADD_KANJI_INCLUDE

#define ADD_KANJI_INCLUDE

typedef struct
{
		GtkWidget *dialog, *table1, *table2, *lbl, *expander;
		GtkWidget *kanji_label, *on_label, *kun_label, *trans_label, *jlpt_label, *grade_label, *radical_label, *stroke_label;
		GtkWidget *kanji_entry, *on_entry, *kun_entry, *trans_entry, *radical_entry, *jlpt_spin, *grade_spin, *stroke_spin;
} Widgets;

Kanji* create_dialog (void);

void upd_entry (GtkWidget *cw, Widgets *w);

#endif
