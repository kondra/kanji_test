#ifndef KANJI_INCLUDE

#define KANJI_INCLUDE

typedef struct
{
		gint num;

		gchar **writing;
		gchar **reading;
		gchar **meaning;
		guint8 *level;
} Collocations;

typedef struct
{
		gint num;
		gchar **word_writing;
		gchar **word_reading;
		gchar **word_meaning;

		Collocations *col;

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

typedef struct
{
		gint ind;
		gchar *word;
} IndexEntry;

//#define kanji_array_create g_array_sized_new (TRUE, TRUE, sizeof (Kanji), 100)

//Kanji* kanji_create_empty (void);

Collocations* collocations_create (gint, gchar**, gchar**, gchar**, guint8*, gboolean);

Kanji* kanji_create (const gchar*, const gchar*, const gchar*, const gchar*, gint, gint, gint, gint, gint, gchar**, gchar**, gchar**);

//GArray* kanji_array_append (GArray*, Kanji*);

GArray* kanji_array_load (const gchar*);

void kanji_array_save (const gchar*, GArray*);

void kanji_array_free (GArray*);

void kanji_free (Kanji*);

void kanji_index_save (const gchar*, GArray*);

GArray* kanji_index_generate (GArray*);

GArray* kanji_index_load (const gchar*);

GArray* kanji_search (GArray*, const gchar*);

#endif
