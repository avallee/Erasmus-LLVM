/** \file lts.cpp */

#include <iostream>
#include <iomanip>
#include <map>

#include "lts.h"

// Set utilities

bool member(int n, set<int> s)
{
   return s.find(n) != s.end();
}

void printset(ostream & os, string name, set<int> s)
{
   os << name << ":";
   for (set<int>::const_iterator i = s.begin(); i != s.end(); ++i)
      os << setw(3) << *i;
   os << endl;
}

// Map utilities

set<int> dom(map<int,int> m)
{
   set<int> s;
   for (map<int,int>::const_iterator i = m.begin(); i != m.end(); ++i)
   {
      s.insert(i->first);
   }
   return s;
}

void printmap(ostream & os, string name, map<int,int> m)
{
   os << name << ": ";
   for (map<int,int>::const_iterator i = m.begin(); i != m.end(); ++i)
      os << setw(3) << i->first << "->" << i->second;
   os << endl;
}

// eqs[i] is the equivalence class of state i
// Initially, eqs[i] = i
void add(int i, int j, vector<int> & eqs)
{
   //   cout <<
   //      "eqs[" << setw(2) << i << "] = " << setw(2) << eqs[i] << "  " <<
   //      "eqs[" << setw(2) << j << "] = " << setw(2) << eqs[j] << "  ";

   if (eqs[i] != eqs[j])
   {
      int ei = eqs[i];
      int ej = eqs[j];
      int m = min(ei, ej);
      for (vector<int>::iterator it = eqs.begin(); it != eqs.end(); ++it)
         if (*it == ei || *it == ej)
            *it = m;
   }

   //   for (vector<int>::iterator it = eqs.begin(); it != eqs.end(); ++it)
   //      cout << setw(3) << *it;
   //   cout << endl;

}

int nextState = 0;

void clearNextState()
{
   nextState = 0;
}

int newState()
{
   return nextState++;
}

// Implementation of class Trans

Trans::Trans(int start, int finish, string msg) :
      start(start), finish(finish), msg(msg)
{}

bool Trans::operator==(const Trans & other) const
{
   return start == other.start && finish == other.finish && msg == other.msg;
}

ostream & operator<<(ostream & os, Trans trs)
{
   return os << trs.start << "->" << trs.msg << "->" << trs.finish;
}

// Implementation of class LTS

LTS::LTS(bool dummyLTS)
{
   start = newState();
   finish = newState();
   if (dummyLTS)
      insert(start, finish);
}

LTS::LTS(int start, int finish) : start(start), finish(finish)
{}

// Insert a single transition
void LTS::insert(int s, int f, string msg)
{
   trs.push_back(Trans(s, f, msg));
}

// Insert a collection of transitions
void LTS::insert(vector<Trans> t)
{
   trs.insert(trs.end(), t.begin(), t.end());
}

bool LTS::hasTrans(Trans t)
{
   for (vector<Trans>::const_iterator ti = trs.begin(); ti != trs.end(); ti++)
      if (*ti == t)
         return true;
   return false;
}

// Reduce the LTS by collapsing empty transitions into single nodes
void LTS::collapse()
{
   // Short-circuit "?" messages.
//   for (vector<Trans>::iterator i = trs.begin(); i != trs.end(); ++i)
//      for (vector<Trans>::iterator j = trs.begin(); j != trs.end(); ++j)
//         if (i != j)
//         {
//            if (i->msg == "?" && i->finish == j->start)
//               j->start = i->start;
//            if (j->msg == "?" && i->finish == j->start)
//               i->finish = j->finish;
//         }



   // Initialize equivalence classes of nodes
   vector<int> eqs;
   for (int i = 0; i < nextState; ++i)
      eqs.push_back(i);

   // Process and remove empty transitions
   vector<Trans>::iterator it = trs.begin();
   while (it != trs.end())
   {
      if (it->msg == "")
      {
         add(it->start, it->finish, eqs);
         it = trs.erase(it);
      }
      else
         ++it;
   }

   // Replace each state by its equivalence class
   start = eqs[start];
   finish = eqs[finish];
   for (vector<Trans>::iterator it = trs.begin(); it != trs.end(); ++it)
   {
      it->start = eqs[it->start];
      it->finish = eqs[it->finish];
   }

   // Construct the set of states.
   for (vector<Trans>::const_iterator it = trs.begin(); it != trs.end(); ++it)
   {
      states.insert(it->start);
      states.insert(it->finish);
   }
}

ostream & operator<<(ostream & os, LTS *pl)
{
   os << "s=" << pl->start << " f=" << pl->finish << " {";
   bool moreThanOne = false;
   for (set<int>::const_iterator it = pl->states.begin(); it != pl->states.end(); ++it)
   {
      if (moreThanOne)
         os << ", ";
      moreThanOne = true;
      os << *it;
   }

   moreThanOne = false;
   os << "} <";
   for (vector<Trans>::const_iterator it = pl->trs.begin(); it != pl->trs.end(); ++it)
   {
      //if (it->msg != "")
      {
         if (moreThanOne)
            os << ", ";
         moreThanOne = true;
         os << *it;
      }
   }
   return os << '>';
}

// Recursively extend a mapping from states of plts to states of qlts
void advance(map<int,int> & pairs, LTS *plts, LTS *qlts)
{
   // unused = p states \ dom pairs
   set<int> unused = plts->states;
   for (map<int,int>::const_iterator i = pairs.begin(); i != pairs.end(); ++i)
      unused.erase(i->first);

//   printmap(cerr, "Pairs", pairs);
//   printset(cerr, "Unused", unused);
//   cerr << "----------------------\n";

   if (unused.empty())
      // Success - we've matched all states.
      return;

   int p = *unused.begin();

   // Try each state of qlts as an image for p
   for (set<int>::const_iterator qi = qlts->states.begin(); qi != qlts->states.end(); ++qi)
   {
      int q = *qi;
      pairs.insert(pair<int,int>(p,q));
      set<int> mapped = dom(pairs);

      // The map is 'good' if:
      // start +> start' and finish +> finish';
      // for each transition <s,f,m> with m != "?", there is a transition <s',f',m'>
      bool good = true;
      if (    member(plts->start, mapped) && pairs[plts->start] != qlts->start ||
              member(plts->finish, mapped) && pairs[plts->finish] != qlts->finish)
         good = false;
      else
      {
         for (vector<Trans>::const_iterator i = plts->trs.begin(); i != plts->trs.end(); ++i)
         {
            if (
               i->msg != "?" &&
               member(i->start, mapped) && member(i->finish, mapped) &&
               ! qlts->hasTrans(Trans(pairs[i->start], pairs[i->finish], i->msg)))
            {
               good = false;
               break;
            }
         }
      }

      // If the map is good, try to extend it.
      // Otherwise remove the pair (p,q) and try another q.
      if (good)
         advance(pairs, plts, qlts);
      else
         pairs.erase(p);
   }
}

bool satisfies(LTS *svrLTS, LTS *cliLTS)
{
   map<int,int> pairs;
   advance(pairs, cliLTS, svrLTS);
   //printset(cerr, "dom(pairs)", dom(pairs));
   bool res = dom(pairs) == cliLTS->getStates();
   return res;
}
