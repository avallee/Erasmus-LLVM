/** \file lts.h */

#ifndef LTS_H
#define LTS_H

#include <string>
#include <vector>
#include <set>
#include <map>

using namespace std;

class LTS;

/** Initialize the state counter to zero. */
void clearNextState();

/** Return a unique state number. */
int newState();

/** Check whether the server LTS satisfies the client LTS. */
bool satisfies(LTS *svrlts, LTS *clilts);


/**  Transition m from state s to state f with message msg.
 *   msg = "" indicates an empty transition.
 */
class Trans
{
    public:

        /** Construct a transition. */
        Trans(int start, int finish, string msg);

        /** Equality test for transitions. */
        bool operator==(const Trans & other) const;

        friend ostream & operator<<(ostream & os, Trans trs);
        friend ostream & operator<<(ostream & os, LTS *pl);
        friend class LTS;
        friend void advance(map<int,int> & pairs, LTS *plts, LTS *qlts);

    private:
        int start;
        int finish;
        string msg;
};

// Labelled state transition system
class LTS
{
    public:
        /** Default constructor.  The LTS has two new states, \a start and \a finish.
         * \param dummyLTS: if false, the default, the LTS has no transitions;
         * otherwise, the LTS has an empty transition \a start -> \a finish.
         */
        LTS(bool dummyLTS = false);

        /** Construct an LTS with given start and finish states and no transitions. */
        LTS(int start, int finish);

        /** Insert a transition into an LTS.
         * msg = 0 indicates an empty transition.
         */
        void insert(int s, int f, string msg = "");

        /** Insert a collection of transitions. */
        void insert(vector<Trans> t);

        /** Check for presence of a transition. */
        bool hasTrans(Trans t);

        /** Remove empty transitions by making their ends equivalent. */
        void collapse();

        int getStart() const { return start; }
        int getFinish() const { return finish; }
        set<int> getStates() const { return states; }
        vector<Trans> getTrans() const { return trs; }

        friend ostream & operator<<(ostream & os, LTS *pl);
        friend void advance(map<int,int> & pairs, LTS *plts, LTS *qlts);

    private:
        /** Initial state. */
        int start;

        /** Final state. */
        int finish;

        /** Transitions. */
        vector<Trans> trs;

        /** Set of states: created by collapse(). */
        set<int> states;
};

#endif
