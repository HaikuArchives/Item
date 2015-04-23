/* Article.cpp */

#include "Article.h"



Article::Article(int articleNoIn, string_slice authorIn, time_t dateIn, int linesIn, int filterScoreIn)
	: articleNo(articleNoIn), author(authorIn), date(dateIn), lines(linesIn),
	  filterScore(filterScoreIn), read(false), selected(false)
{
}


void Article::SetFilterScore(float newFilterScore)
{
	filterScore = newFilterScore;
}


bool Article::Selected()
{
	return selected;
}


void Article::SetSelected(bool newSelected)
{
	selected = newSelected;
}


