#ifndef KANJI_INCLUDE

#define KANJI_INCLUDE

typedef struct
{
		gint num;
		gchar **word_writing;
		gchar **word_reading;
		gchar **word_meaning;

		gboolean state;
		
		gint jlpt_level;
		gint grade;
		gint kanji_stroke;
		gint radical_stroke;

		gchar *kanji;
		gchar *on;
		gchar *meaning;
		gchar *radical;
} Kanji;

#define kanji_array_create g_array_sized_new (TRUE, TRUE, sizeof (Kanji), 100)

Kanji* kanji_create_empty (void);

Kanji* kanji_create (const gchar*, const gchar*, const gchar*, const gchar*, 
				gint, gint, gint, gint, gint, gchar**, gchar**, gchar**);

GArray* kanji_array_append (GArray*, Kanji*);

GArray* kanji_array_load (const gchar*);

void kanji_array_save (const gchar*, GArray*);

void kanji_array_free (GArray*);

void kanji_free (Kanji*);

#endif
