/** \file utilities.cpp */

#include "utilities.h"

#include <iostream>
#include <map>
#include <sstream>


using namespace std;

/** Return a string with quotes around it. */
string quoteString(string s)
{
    return " '" + s + "'";
}

string str(int n)
{
    ostringstream os;
    os << n;
    return os.str();
}

string chr(char c)
{
    string fmt = "'";
    if (c == '\n')
        fmt += "\\n";
    else if (c == '\t')
        fmt += "\\t";
    else if (c == '\'')
        fmt += "\\\'";
    else
        fmt += c;
    return fmt + "'";
}

void showChar(ostream & os, char c, bool withConstructor)
{
    if (withConstructor)
        os << "char(" << chr(c) << ")";
    else
        os << chr(c);
}

string str(string s)
{
    string fmt = "\"";
    for (string::const_iterator i = s.begin(); i != s.end(); ++i)
    {
        if (*i == '\n')
            fmt += "\\n";
        else if (*i == '\t')
            fmt += "\\t";
        else if (*i == '\"')
            fmt += "\\\"";
        else if (*i == '\\')
            fmt += "\\\\";
        else
            fmt += *i;
    }
    return fmt + "\"";
}

void showString(ostream & os, string s, bool withConstructor)
{
    if (withConstructor)
        os << "string(" << str(s) << ")";
    else
        os << str(s);
}

string today()
{
    map<string, string> monthname;
    monthname["Jan"] = "January";    monthname["Feb"] = "February";
    monthname["Mar"] = "March";      monthname["Apr"] = "April";
    monthname["May"] = "May";        monthname["Jun"] = "June";
    monthname["Jul"] = "July";       monthname["Aug"] = "August";
    monthname["Sep"] = "September";  monthname["Oct"] = "October";
    monthname["Nov"] = "November";   monthname["Dec"] = "December";
    string month;
    int day;
    int year;
    istringstream dt(__DATE__);
    dt >> month >> day >> year;
    ostringstream os;
    os << year << ' ' << monthname[month] << ' ' << day << ' ' << __TIME__;
    return os.str();
}

