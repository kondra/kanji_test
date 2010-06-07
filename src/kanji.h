#ifndef KANJI_INCLUDE

#define KANJI_INCLUDE

typedef struct
{
		int jlpt_level, grade, stroke;
		gchar *str, *kun, *on, *trans, *radical;
} Kanji;

#define kanji_array_create g_array_new (TRUE, TRUE, sizeof (Kanji))

Kanji* kanji_create_empty (void);

Kanji* kanji_create (const gchar *str, const gchar *radical, const gchar *kun, const gchar *on, const gchar *trans, int jlpt_level, int grade, int stroke);

GArray* kanji_array_append (GArray *arr, Kanji *k);

GArray* kanji_array_load (const gchar *filename);

void kanji_array_save (const gchar *filename, GArray *arr);

void kanji_array_free (GArray *arr);

void kanji_free (Kanji *k);

#endif
