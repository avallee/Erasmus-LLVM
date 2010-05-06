/** \file error.cpp */

#include "error.h"

int numErrors = 0;

Error & Error::operator<<(const ErrorReporter & rep)
{
    ++numErrors;
    if (rep == REPORT)
        cerr << os.str();
    else
        throw Glib::ustring(os.str());
    return *this;
}

int errorCount()
{
    return numErrors;
}

