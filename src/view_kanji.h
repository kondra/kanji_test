#ifndef VIEW_KANJI_INCLUDE

#define VIEW_KANJI_INCLUDE

enum
{
		NUMBER = 0,
		KANJI,
		KANJI_STROKE,
		RADICAL,
		RADICAL_STROKE,
		JLPT_LEVEL,
		SCHOOL_GRADE,
		COLUMNS
};

void view_kanji (GArray*);

#endif
