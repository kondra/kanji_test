#ifndef KANJI_INCLUDE

#define KANJI_INCLUDE

typedef struct
{
		int num;
		gchar **kun_writing;
		gchar **kun_reading;
		gchar **kun_meaning;
		
		int jlpt_level, grade, stroke, radical_stroke;
		gchar *str, *kun, *on, *meaning, *radical;
} Kanji;

#define kanji_array_create g_array_sized_new (TRUE, TRUE, sizeof (Kanji), 100)

Kanji* kanji_create_empty (void);

Kanji* kanji_create (const gchar *str, const gchar *radical, const gchar *on, const gchar *meaning, 
				int jlpt_level, int grade, int stroke, int radical_stroke, int num, gchar **writings, gchar **readings, gchar **meanings);

GArray* kanji_array_append (GArray *arr, Kanji *k);

GArray* kanji_array_load (const gchar *filename);

void kanji_array_save (const gchar *filename, GArray *arr);

void kanji_array_free (GArray *arr);

void kanji_free (Kanji *k);

#endif
