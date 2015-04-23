/* Main.cpp */

#include "Main.h"
#include "NNTPApp.h"
#include "NNTPConnection.h"
#include "NNTPResponse.h"
#include "SubjectSubjectTree.h"
#include "Subject.h"
#include "ConnectionPool.h"
#include "OpenGroupThread.h"
#include "ArticleSet.h"
#include "Prefs.h"
#include <stdio.h>

extern void ArticleSetTest();
extern void PrintArticleSet(ArticleSet* set);
extern void PrintResponseData(NNTPResponse* response);
extern void SubjectTest(NNTPResponse* response);
extern void ContainsCITest();
extern void TestContains(const char* container, const char* contained);

int main()
{
//***	Prefs()->SetStringPref("nntpServer", "news.95net.com");
/***
	OpenGroupThread* thread = new OpenGroupThread("rec.audio.pro");
	thread->Go();

	thread->WaitFor();
***/

//*** test
/***
	ArticleSetTest();
	return 0;
***/

	// run the app
	new NNTPApp();
	be_app->Run();
	delete be_app;

	// clean up
	ConnectionPool::DeletePool();
	Preferences::DeletePreferences();
}


void ArticleSetTest()
{
	ArticleSet set;

	set.AddArticle(1);
	PrintArticleSet(&set);
	set.AddArticle(3);
	PrintArticleSet(&set);
	set.AddArticle(5);
	PrintArticleSet(&set);
	set.AddArticle(7);
	PrintArticleSet(&set);
	set.AddArticle(11);
	PrintArticleSet(&set);
	set.AddArticle(13);
	PrintArticleSet(&set);
	set.AddArticle(4);
	PrintArticleSet(&set);
	set.AddArticle(6);
	PrintArticleSet(&set);
	set.AddArticle(8);
	PrintArticleSet(&set);
	set.AddArticle(10);
	PrintArticleSet(&set);
	set.AddArticle(12);
	set.AddArticles("1-7,34,53-68,149");
	PrintArticleSet(&set);
	bool hasArticle = set.ContainsArticle(3);
	hasArticle = set.ContainsArticle(7);
	hasArticle = set.ContainsArticle(6);
}

void PrintArticleSet(ArticleSet* set)
{
	qstring* str = set->AsString();
	printf("%s\n", str->c_str());
	delete str;
/***
	bool needsComma = false;
	int numRanges = set->NumRanges();
	for (int i=0; i<numRanges; i++) {
		IntRange range = set->RangeAt(i);
		if (needsComma)
			printf(",");
		needsComma = true;
		if (range.min == range.max)
			printf("%d", range.min);
		else
			printf("%d-%d", range.min, range.max);
		}
	printf("\n");
***/
}


#ifdef NOT_ANYMORE
static void OldTest()
{
	NNTPResponse* response;
	char cmdStr[256];

	// open connection
//***	NNTPConnection* connection = new NNTPConnection("news.95net.com");
	Prefs()->SetStringPref("nntpServer", "news.95net.com");
	NNTPConnection* connection = ConnectionPool::GetPool()->GetConnection();

	// get the group
	connection->SendCommand("group rec.audio.pro");
	printf("Sent GROUP.  Response: %d\n", connection->GetResponse(&response));
	int numArticles = response->NextIntField();
	int firstArticle = response->NextIntField();
	int lastArticle = response->NextIntField();
	printf("\tNum: %d, First: %d, Last: %d\n", numArticles, firstArticle, lastArticle);
	delete response;

	// XOVER for last ten articles
	sprintf(cmdStr, "xover %d-%d", lastArticle-60, lastArticle);
	connection->SendCommand(cmdStr);
	int result = connection->GetResponse(&response);
	printf("Sent XOVER.  Response: %d\n", result);
	if (result == 224) {
/***
		PrintResponseData(response);
		printf("\n");
		response->Reset();
***/
		printf("==================================\n");
		SubjectTest(response);
		printf("==================================\n");
		delete response;
		}

	// read the last article
/***
	sprintf(cmdStr, "article %d", lastArticle);	// 242316 was one with a problem, now fixed
	connection->SendCommand(cmdStr);
	result = connection->GetResponse(&response);
	printf("Sent ARTICLE.  Response: %d\n", result);
	if (result == 220) {
		PrintResponseData(response);
		delete response;
		}
***/

	// get & use a second connection
	NNTPConnection* connection2 = ConnectionPool::GetPool()->GetConnection();
	connection2->SendCommand("group soc.history.war.world-war-ii");
	printf("Sent GROUP.  Response: %d\n", connection2->GetResponse(&response));
	numArticles = response->NextIntField();
	firstArticle = response->NextIntField();
	lastArticle = response->NextIntField();
	printf("\tNum: %d, First: %d, Last: %d\n", numArticles, firstArticle, lastArticle);
	delete response;

	// close connection
	ConnectionPool::GetPool()->ReleaseConnection(connection);
	ConnectionPool::GetPool()->ReleaseConnection(connection2);
	ConnectionPool::DeletePool();
/***
	connection->SendCommand("quit");
	printf("Sent QUIT.  Response: %d\n", connection->GetResponse());
	delete connection;
***/
}
#endif


void PrintResponseData(NNTPResponse* response)
{
	if (response == NULL)
		return;

	response->NextLine();

	while (!response->AtEOF()) {
		string_slice line = response->NextLine();
		fwrite(line.begin(), line.length(), 1, stdout);
		printf("\n");
		}
}



	static void WriteSubjectSubject(Subject* subject, void* data)
	{
		string_slice subj = subject->GetSubject();
		fwrite(subj.begin(), subj.length(), 1, stdout);
		printf("\n");
	}

void SubjectTest(NNTPResponse* response)
{
	SubjectSubjectTree	subjectTree;

	response->NextLine();

	// get the subjects
	while (!response->AtEOF()) {
		// get the subject from the XOVER response
		response->NextTabField();	// skip articleNo
		string_slice subject = response->NextTabField();
		response->NextLine();
		if (subject.length() == 0)
			continue;

		// trim "Re:"
		const char* p = subject.begin();
		const char* stopper = subject.end();
		while (p < stopper) {
			// skip whitespace
			for (; p < stopper; p++) {
				if (*p != ' ' && *p != '\t')
					break;
				}
			// check for "Re"
			if (p > stopper - 3)
				break;
			if ((*p == 'R' || *p == 'r') && (p[1] == 'e' || p[1] == 'E') && p[2] == ':')
				p += 3;
			else
				break;
			}
		subject = string_slice(p, subject.end());

		// add to the subjects if it's not already there
		if (subjectTree.Find(subject) == NULL)
			subjectTree.Insert(new Subject(subject));
		}

	// write the subjects
	subjectTree.Walk(WriteSubjectSubject, NULL);
}


void ContainsCITest()
{
	TestContains("Gabe Wiener", "wiener");
	TestContains("Gabe Wiener", "weiner");
	TestContains("Gabe Wiener", "gabe");
	TestContains("Gabe Wiener", "garb");
}


void TestContains(const char* container, const char* contained)
{
	printf("\"%s\" contains \"%s\": %s\n", container, contained,
	       (string_slice(container).containsCI(string_slice(contained)) ? "true" : "false"));
}



