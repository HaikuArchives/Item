/* Newsgroup.h */

#ifndef _H_Newsgroup_
#define _H_Newsgroup_

#include "string_slice.h"
#include "qstring.h"
#include "IntRange.h"
#include <parsedate.h>
#include <SupportDefs.h>
#include <List.h>

class Newsrc;
class SubjectSubjectTree;
class SubjectFilterTree;
class ArticleSet;
class NNTPConnection;
class NNTPResponse;
class Subject;
class ListableTree;
class NewsgroupWind;
class Task;
class BFile;

class Newsgroup {
public:
	Newsgroup(string_slice groupNameIn, Newsrc* newsrcIn);
	~Newsgroup();
	void          	SetAvailableArticles(int firstArticle, int lastArticle);
	void          	SetReadArticles(ArticleSet* newReadArticles);
	status_t      	ReadHeaders(NNTPConnection* connection, ArticleSet* cachedArticles, Task* task,
                     volatile bool &stopRequested);
	void          	ArticleRead(int articleNo);
	void          	ArticleUnread(int articleNo);
	void          	ReadArticlesChanged();
	void          	Close();
	void          	WriteArticleCache();
	ArticleSet*   	ReadArticleCache();
	void          	RemoveReadArticles();
	string_slice  	Name();
	int           	NumUnreadArticles();
	bool          	HaveAvailableArticlesInfo();
	ArticleSet*   	ReadArticles();
	ListableTree* 	SubjectTree();
	ListableTree* 	FilterTree();
	void          	AttachedToWindow(NewsgroupWind* window);
	NewsgroupWind*	Window();
	void          	SetNewsrc(Newsrc* newNewsrc);
	status_t      	GetOneHeaderRange(NNTPConnection* connection, IntRange range, Task* task);
	string_slice  	TrimSubject(string_slice subject);
	time_t        	SliceToDate(string_slice strIn);
	void          	CalcNumUnreadArticles();
	BFile*        	OpenHeaderCacheFile(uint32 openMode);

protected:
	static void   	WriteSubject(Subject* subject, void* data);

	qstring                  	groupName;
	Newsrc*                  	newsrc;
	IntRange                 	availableArticles;
	ArticleSet*              	readArticles;
	int                      	numUnreadArticles;
	SubjectSubjectTree*      	subjectTree;
	SubjectFilterTree*       	filterTree;
	BList*                   	headersResponses;
	char*                    	cachedHeaderInfo;
	NewsgroupWind*           	wind;
	static const unsigned int	maxHeaders;
};


#endif
