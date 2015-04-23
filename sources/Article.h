/* Article.h */

#ifndef _H_Article_
#define _H_Article_

#include "string_slice.h"
#include <time.h>

class Article {
public:
	Article(int articleNoIn, string_slice authorIn, time_t dateIn, int linesIn,
        int filterScoreIn = 0);
	inline int         	ArticleNo();
	inline string_slice	Author();
	inline time_t      	Date();
	inline int         	Lines();
	inline float       	FilterScore();
	void               	SetFilterScore(float newFilterScore);
	inline bool        	IsRead();
	inline void        	SetRead(bool newRead);
	bool               	Selected();
	void               	SetSelected(bool newSelected);

protected:
	int         	articleNo;
	string_slice	author;
	time_t      	date;
	int         	lines;
	float       	filterScore;
	bool        	read;
	bool        	selected;
};

inline int Article::ArticleNo()
{
	return articleNo;
}



inline string_slice Article::Author()
{
	return author;
}



inline time_t Article::Date()
{
	return date;
}



inline int Article::Lines()
{
	return lines;
}



inline float Article::FilterScore()
{
	return filterScore;
}



inline bool Article::IsRead()
{
	return read;
}



inline void Article::SetRead(bool newRead)
{
	read = newRead;
}




#endif
