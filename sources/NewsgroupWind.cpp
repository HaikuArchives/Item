/* NewsgroupWind.cpp */

#include "NewsgroupWind.h"
#include "Newsgroup.h"
#include "SubjectsView.h"
#include "SubjectTree.h"
#include "Subject.h"
#include "NNTPApp.h"
#include "Filters.h"
#include "FilterGroup.h"
#include "Filter.h"
#include "ExtractBinariesThread.h"
#include "Prefs.h"
#include "qstring.h"
#include "Messages.h"
#include <MenuBar.h>
#include <Menu.h>
#include <MenuItem.h>
#include <ScrollView.h>

const float NewsgroupWind::DefaultWidth = 500;
const float NewsgroupWind::DefaultHeight = 560;
const float NewsgroupWind::DefaultLeft = 80;
const float NewsgroupWind::DefaultTop = 80;


NewsgroupWind::NewsgroupWind(Newsgroup* newsgroupIn)
	: BWindow(BRect(0, 0, DefaultWidth, DefaultHeight), "", B_DOCUMENT_WINDOW, 0),
	  newsgroup(newsgroupIn)
{
	// menu bar
	BMenuBar* menuBar = new BMenuBar(BRect(0, 0, 0, 0), "");
	BMenu* newsgroupMenu = new BMenu("Newsgroup");
	newsgroupMenu->AddItem(new BMenuItem("View By Filter Score/Date",
	                                     new BMessage(ViewByScoreMessage),
	                                     'F', B_SHIFT_KEY));
	newsgroupMenu->AddItem(new BMenuItem("View By Subject",
	                                     new BMessage(ViewBySubjectMessage),
	                                     'S', B_SHIFT_KEY));
	newsgroupMenu->AddSeparatorItem();
	BMenuItem* filtersItem =
		new BMenuItem("Show Filters", new BMessage(ShowFiltersMessage), 'F');
	filtersItem->SetTarget(be_app_messenger);
	newsgroupMenu->AddItem(filtersItem);
	BMenuItem* prefsItem =
		new BMenuItem("Show Preferences", new BMessage(ShowPrefsMessage), ';');
	prefsItem->SetTarget(be_app_messenger);
	newsgroupMenu->AddItem(prefsItem);
	menuBar->AddItem(newsgroupMenu);
	BMenu* articleMenu = new BMenu("Article");
	articleMenu->AddItem(new BMenuItem("Mark", new BMessage(MarkMessage), 'M'));
	articleMenu->AddItem(new BMenuItem("Unmark", new BMessage(UnmarkMessage), 'U'));
	articleMenu->AddItem(new BMenuItem("Kill Selected Subjects", new BMessage(KillSubjectsMessage), 'K'));
	articleMenu->AddItem(new BMenuItem("Hilite Selected Subjects", new BMessage(HiliteSubjectsMessage), 'H'));
	articleMenu->AddItem(new BMenuItem("Remove Read Articles", new BMessage(RemoveReadMessage),
	                                   'R', B_SHIFT_KEY));
	articleMenu->AddSeparatorItem();
	articleMenu->AddItem(new BMenuItem("Save...", new BMessage(SaveArticlesMessage), 'S'));
	articleMenu->AddItem(new BMenuItem("Append To...", new BMessage(AppendArticlesMessage), 'D'));
	articleMenu->AddItem(new BMenuItem("Extract Binaries", new BMessage(ExtractBinariesMessage), 'B'));
	articleMenu->AddItem(new BMenuItem("Extract Multipart Binary", new BMessage(ExtractMultipartBinaryMessage), 'E'));
	articleMenu->AddSeparatorItem();
	articleMenu->AddItem(new BMenuItem("Select All", new BMessage(B_SELECT_ALL), 'A'));
	menuBar->AddItem(articleMenu);
	AddChild(menuBar);

	// make the listView
	BRect listViewFrame(0, menuBar->Frame().Height() + 1,
	                    DefaultWidth - B_V_SCROLL_BAR_WIDTH, DefaultHeight);
	listView = new SubjectsView(listViewFrame, newsgroup->FilterTree());

	// make the scrollView
	BScrollView* scrollView =
		new BScrollView("", listView, B_FOLLOW_ALL_SIDES, 0, false, true, B_NO_BORDER);
	AddChild(scrollView);

	// set the title
	qstring title = newsgroup->Name();
	SetTitle(title.c_str());

	newsgroup->AttachedToWindow(this);

	// place it on the screen
	MoveTo(DefaultLeft, DefaultTop);
	BRect savedFrame = Prefs()->GetRectPref("newsgroupWind.frame");
	if (savedFrame.IsValid()) {
		MoveTo(savedFrame.LeftTop());
		ResizeTo(savedFrame.Width(), savedFrame.Height());
		}

	listView->MakeFocus();
	Show();
}


NewsgroupWind::~NewsgroupWind()
{
	newsgroup->Close();
	newsgroup->AttachedToWindow(NULL);
	((NNTPApp*) be_app)->WindowClosing(this);
}


void NewsgroupWind::MessageReceived(BMessage* message)
{
	ListableTree* newTree;

	switch (message->what) {
		case ViewByScoreMessage:
			newTree = newsgroup->FilterTree();
			newTree->UpdateAllObjects();
			listView->SetTree(newTree);
			break;

		case ViewBySubjectMessage:
			newTree = newsgroup->SubjectTree();
			newTree->UpdateAllObjects();
			listView->SetTree(newTree);
			break;

		case MarkMessage:
			MarkSelected(true);
			break;

		case UnmarkMessage:
			MarkSelected(false);
			break;

		case KillSubjectsMessage:
			KillSubjects();
			break;

		case HiliteSubjectsMessage:
			HiliteSubjects();
			break;

		case RemoveReadMessage:
			newsgroup->RemoveReadArticles();
			break;

		case B_SELECT_ALL:
			listView->SelectAll();
			break;

		case AppendArticlesMessage:
			AppendArticles();
			break;

		case SaveArticlesMessage:
			SaveArticles();
			break;

		case ExtractBinariesMessage:
			ExtractBinaries(false);
			break;

		case ExtractMultipartBinaryMessage:
			ExtractBinaries(true);
			break;

		default:
			BWindow::MessageReceived(message);
			break;
		}
}


void NewsgroupWind::MenusBeginning()
{
	BMenuBar* menuBar = KeyMenuBar();
	ListableTree* viewTree = listView->Tree();
	menuBar->FindItem(ViewByScoreMessage)->SetMarked(viewTree == newsgroup->FilterTree());
	menuBar->FindItem(ViewBySubjectMessage)->SetMarked(viewTree == newsgroup->SubjectTree());
}


void NewsgroupWind::FrameMoved(BPoint screenPoint)
{
	BWindow::FrameMoved(screenPoint);
	SavePosition();
}


void NewsgroupWind::FrameResized(float width, float height)
{
	BWindow::FrameResized(width, height);
	SavePosition();
}


Newsgroup* NewsgroupWind::GetNewsgroup()
{
	return newsgroup;
}


void NewsgroupWind::SubjectChanged(Subject* subject)
{
	listView->ItemDisplayChanged(((SubjectTree*) listView->Tree())->AdaptorFor(subject));
}


void NewsgroupWind::NumSubjectsChanged()
{
	listView->NumItemsChanged();
}


void NewsgroupWind::MarkSelected(bool marked)
{
	SubjectTree* tree = (SubjectTree*) listView->Tree();
	int numSelections = tree->NumSelections();
	for (int i=0; i<numSelections; i++)
		tree->SelectedSubjectAt(i)->MarkSelected(marked, newsgroup);
	newsgroup->ReadArticlesChanged();
	listView->Invalidate();
}


void NewsgroupWind::AppendArticles()
{
	// fill up the message
	BMessage* message = new BMessage();
	qstring groupName(newsgroup->Name());
	message->AddString("groupName", groupName.c_str());
	SubjectTree* tree = (SubjectTree*) listView->Tree();
	int numSelections = tree->NumSelections();
	for (int i=0; i<numSelections; i++)
		tree->SelectedSubjectAt(i)->SetupSaveArticles(message);

	((NNTPApp*) be_app)->ShowAppendArticlesPanel(message);
}


void NewsgroupWind::SaveArticles()
{
	// fill up the message
	BMessage* message = new BMessage();
	qstring groupName(newsgroup->Name());
	message->AddString("groupName", groupName.c_str());
	SubjectTree* tree = (SubjectTree*) listView->Tree();
	int numSelections = tree->NumSelections();
	for (int i=0; i<numSelections; i++)
		tree->SelectedSubjectAt(i)->SetupSaveArticles(message);

	((NNTPApp*) be_app)->ShowSaveArticlesPanel(message);
}


void NewsgroupWind::ExtractBinaries(bool multipart)
{
	// set up the articlesList
	BList* articlesList = new BList();
	SubjectTree* tree = (SubjectTree*) listView->Tree();
	int numSelections = tree->NumSelections();
	for (int i=0; i<numSelections; i++)
		tree->SelectedSubjectAt(i)->SetupExtractBinaries(articlesList);

	// run the thread
	ExtractBinariesThread* binariesThread =
		new ExtractBinariesThread(newsgroup, articlesList, listView, multipart);
	binariesThread->Go();
}


void NewsgroupWind::SavePosition()
{
	// don't do it if we're the only NewsgroupWind
	for (int i=0; ; i++) {
		BWindow* wind = be_app->WindowAt(i);
		if (wind == NULL)
			break;
		if (wind != this && dynamic_cast<NewsgroupWind*>(wind) != NULL)
			return;
		}

	Prefs()->SetRectPref("newsgroupWind.frame", Frame());
}


void NewsgroupWind::KillSubjects()
{
	// set up
	Filters* filters = ((NNTPApp*) be_app)->GetFilters();
	string_slice groupName = newsgroup->Name();

	// kill and mark the subjects
	SubjectTree* tree = (SubjectTree*) listView->Tree();
	int numSelections = tree->NumSelections();
	for (int i=0; i<numSelections; i++) {
		// kill subject
		Subject* subject = tree->SelectedSubjectAt(i);
		FilterGroup* filterGroup = filters->GetFilterGroup(groupName);
		if (filterGroup == NULL) {
			filterGroup = new FilterGroup(groupName);
			filters->AddFilterGroup(filterGroup);
			}
		filterGroup->AddFilter(new Filter(Kill, BySubject, subject->GetSubject()));

		// mark subject
		subject->MarkSelected(true, newsgroup);
		}

	// update from marking
	newsgroup->ReadArticlesChanged();
	listView->Invalidate();
}


void NewsgroupWind::HiliteSubjects()
{
	// set up
	Filters* filters = ((NNTPApp*) be_app)->GetFilters();
	string_slice groupName = newsgroup->Name();

	// kill the subjects
	SubjectTree* tree = (SubjectTree*) listView->Tree();
	int numSelections = tree->NumSelections();
	for (int i=0; i<numSelections; i++) {
		// hilite subject
		Subject* subject = tree->SelectedSubjectAt(i);
		FilterGroup* filterGroup = filters->GetFilterGroup(groupName);
		if (filterGroup == NULL) {
			filterGroup = new FilterGroup(groupName);
			filters->AddFilterGroup(filterGroup);
			}
		filterGroup->AddFilter(new Filter(Hilite, BySubject, subject->GetSubject()));
		}
}


