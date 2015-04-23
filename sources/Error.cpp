/* Error.cpp */

#include "Error.h"
#include "qstring.h"
#include <Alert.h>
#include <string.h>


int Error::ShowError(string_slice msgIn, int err)
{
	qstring msg = msgIn;
	if (err != 0) {
		msg += "  (";
		const char* errStr = ErrorString(err);
		if (errStr != NULL)
			msg += errStr;
		else
			msg += err;
		msg += ")";
		}
	BAlert* alert =
		new BAlert("Error alert", msg.c_str(), "OK");
	alert->SetShortcut(0, ' ');
	alert->Go();

	return err;
}


int Error::ShowError(string_slice msg1, string_slice msg2, int err)
{
	qstring msg = msg1;
	msg += msg2;
	if (err != 0) {
		msg += "  (";
		const char* errStr = ErrorString(err);
		if (errStr != NULL)
			msg += errStr;
		else
			msg += err;
		msg += ")";
		}
	BAlert* alert =
		new BAlert("Error alert", msg.c_str(), "OK");
	alert->SetShortcut(0, ' ');
	alert->Go();

	return err;
}


int Error::ShowError(string_slice msg1, string_slice msg2, string_slice msg3, int err)
{
	qstring msg = msg1;
	msg += msg2;
	msg += msg3;
	if (err != 0) {
		msg += "  (";
		const char* errStr = ErrorString(err);
		if (errStr != NULL)
			msg += errStr;
		else
			msg += err;
		msg += ")";
		}
	BAlert* alert =
		new BAlert("Error alert", msg.c_str(), "OK");
	alert->SetShortcut(0, ' ');
	alert->Go();

	return err;
}


const char* Error::ErrorString(int err)
{
	/** specific to Item **/
	switch (err) {
		case 400:
			return "Service discontinued";
			break;

		case 411:
			return "No such newsgroup";
			break;

		case 412:
			return "No newsgroup selected";
			break;

		case 420:
			return "No current article has been selected";
			break;

		case 421:
			return "No next article in this group";
			break;

		case 422:
			return "No previous article in this group";
			break;

		case 423:
			return "Article expired";
			break;

		case 430:
			return "No such article found";
			break;

		case 435:
			return "Article not wanted - don't send it";
			break;

		case 436:
			return "Transfer failed - try again later";
			break;

		case 437:
			return "Article rejected - do not try again";
			break;

		case 440:
			return "Posting not allowed";
			break;

		case 441:
			return "Posting failed";
			break;

		case 500:
			return "Command not recognized";
			break;

		case 501:
			return "Command syntax error";
			break;

		case 502:
			return "Access restriction or permission denied";
			break;

		case 503:
			return "Program fault (on server) - command not performed";
			break;

		default:
			return strerror(err);
			break;
		}
}


