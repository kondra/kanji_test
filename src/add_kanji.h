#ifndef ADD_KANJI_INCLUDE

#define ADD_KANJI_INCLUDE

typedef struct
{
		GtkWidget *dialog, *table1, *table2, *lbl, *expander;
		GtkWidget *kanji_label, *on_label, *kun_label, *meaning_label, *jlpt_label, *grade_label, *radical_label, *stroke_label, *rst_label;
		GtkWidget *kanji_entry, *on_entry, *kun_entry, *meaning_entry, *radical_entry, *jlpt_spin, *grade_spin, *stroke_spin, *rst_spin;
} Widgets;

Kanji* create_dialog (void);

#endif
