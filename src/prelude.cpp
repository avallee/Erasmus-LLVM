// Version 43
//*A
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <time.h>
using namespace std;

//--------------------------------------------------------------------  to bool

inline bool string2bool(string s)
{
   if (s == "true")
      return true;
   if (s == "false")
      return false;
   // This error may occur during initialization, so we don't throw.
   cerr << "Text '" << s << "' cannot be converted to Bool.\n";
   return false;
}

//---------------------------------------------------------------------- to byte

inline char int2byte(int i)
{
   if (-128 <= i && i < 128)
      return static_cast<char>(i);
   cerr << "Integer '" << i << "' cannot be converted to Byte.\n";
   return 0;
}

inline char ubyte2byte(unsigned char ub)
{
   if (ub < 128)
      return static_cast<char>(ub);
   cerr << "Unsigned Byte '" << ub << "' cannot be converted to Byte.\n";
   return 0;
}

inline char uint2byte(unsigned int ui)
{
   if (ui < 128)
      return static_cast<char>(ui);
   cerr << "Unsigned Integer '" << ui << "' cannot be converted to Byte.\n";
   return 0;
}

//------------------------------------------------------------- to unsigned byte

inline unsigned char byte2ubyte(char c)
{
   if (c >= 0)
      return static_cast<unsigned char>(c);
   cerr << "Byte '" << c << "' cannot be converted to unsigned Byte.\n";
   return 0;
}

inline unsigned char int2ubyte(int i)
{
   if (0 <= i && i < 256)
      return static_cast<unsigned char>(i);
   cerr << "Integer '" << i << "' cannot be converted to unsigned Byte.\n";
   return 0;
}

inline unsigned char uint2ubyte(unsigned int ui)
{
   if (0 <= ui && ui < 256)
      return static_cast<unsigned char>(ui);
   cerr << "Unsigned Integer '" << ui << "' cannot be converted to unsigned Byte.\n";
   return 0;
}

//--------------------------------------------------------------------  to char

// Convert integer to ASCII character
inline char encode2char(int i)
{
   if (0 <= i && i < 256)
      return static_cast<char>(i);
   cerr << "There is no character code corresponding to Integer " << i << ".\n";
   return ' ';
}

inline char string2char(string s)
{
   if (s.length() == 1)
      return s[0];
   // This error may occur during initialization, so we don't throw.
   cerr << "Text '" << s << "' cannot be converted to Char.\n";
   return ' ';
}

//-------------------------------------------------------------------- to string

inline string bool2string1(bool b)
{
   return b ? "true" : "false";
}

inline string bool2string2(bool b, int width)
{
   ostringstream os;
   if (width < 0)
   {
      width =- width;
      os << left;
   }
   if (b)
      os << setw(width) << "true";
   else
      os << setw(width) << "false";
   return os.str();
}

inline string char2string1(char c)
{
   return string(1, c);
}

inline string char2string2(char c, int width)
{
   ostringstream os;
   if (width < 0)
   {
      width =- width;
      os << left;
   }
   os << setw(width) << c;
   return os.str();
}

inline string string2string2(string s, int width)
{
   ostringstream os;
   if (width < 0)
   {
      width =- width;
      os << left;
   }
   os << setw(width) << s;
   return os.str();
}

inline string int2string1(int i)
{
   ostringstream os;
   os << i;
   return os.str();
}

inline string uint2string1(unsigned int ui)
{
   ostringstream os;
   os << ui;
   return os.str();
}

inline string int2string2(int i, int width)
{
   ostringstream os;
   if (width < 0)
   {
      width =- width;
      os << left;
   }
   os << setw(width) << i;
   return os.str();
}

inline string uint2string2(unsigned int i, int width)
{
   ostringstream os;
   if (width < 0)
   {
      width =- width;
      os << left;
   }
   os << setw(width) << i;
   return os.str();
}

inline string byte2string1(char b)
{
   return int2string1(b);
}

inline string ubyte2string1(unsigned char ub)
{
   return uint2string1(ub);
}

inline string double2string1(double d)
{
   ostringstream os;
   os << d;
   return os.str();
}

inline string double2string2(double d, int width)
{
   ostringstream os;
   if (width < 0)
   {
      width =- width;
      os << left;
   }
   os << setw(width) << d;
   return os.str();
}

inline string double2string3(double d, int width, int prec)
{
   ostringstream os;
   if (width < 0)
   {
      width =- width;
      os << left;
   }
   os << fixed << setprecision(prec) << setw(width) << d;
   return os.str();
}

//----------------------------------------------------------------------- to int

// Return the integer in [0,9] corresponding to a character.
inline int char2int(char c)
{
   if ('0' <= c && c <= '9')
      return c - '0';
   cerr << "Char '" << c << "' is not a digit.\n";
   return 0;
}

// Return the ASCII code of a character.
inline int char2decode(char c)
{
   return c;
}

inline int string2int(string s)
{
   istringstream is(s);
   int i;
   is >> i;
   return i;
}

// Check enumeration value
inline int check_enum_val(int val, int max)
{
   if (0 <= val && val < max)
      return val;
   cerr << "Illegal enumeration value: " << val << endl;
}

inline int stringlen(string s)
{
   return int(s.size());
}

// ------------------------------------------------------------- to unsigned int

inline unsigned int int2uint(int val)
{
   if (val >= 0)
      return static_cast<unsigned int>(val);
   cerr << "Integer '" << val << "' cannot be converted to unsigned Integer.\n";
}

// ------------------------------------------------ New conversions (2008/12/27)

inline bool isInteger(double d)
{
   return static_cast<double>(static_cast<int>(d)) == d;
}

inline int double2floor(double d)
{
   return d >= 0.0 || isInteger(d) ? static_cast<int>(d) : static_cast<int>(d - 1.0);
}

inline int double2round(double d)
{
   return d >= 0.0 ? static_cast<int>(d + 0.5) : static_cast<int>(d - 0.5);
}

inline int double2ceiling(double d)
{
   return d < 0.0 || isInteger(d) ? static_cast<int>(d) : static_cast<int>(d + 1.0);
}

//-------------------------------------------------------------------- to double

inline double string2double(string s)
{
   istringstream is(s);
   double d;
   is >> d;
   return d;
}

//-------------------------------------------------------------------- strings

char get_char(string s, int i)
{
   if (0 <= i && i < s.length())
      return s[i];
   // This error may occur during initialization, so we don't throw.
   cerr << "Range error:\n  string '" << s << "', index " << i << ".\n";
   exit(1);
}

string get_sub_string(string s, int i, int j)
{
   if (0 <= i && i < j && j <= s.length())
      return s.substr(i, j - i);
   else
      return string("");
}

//------------------------------------------------------------------ Assertions

void assert_1(bool assertion)
{
   if (!assertion)
      throw "assertion.\n";
}

void assert_2(bool assertion, string message)
{
   if (!assertion)
      throw "assertion. " + message + "\n";
}

//-------------------------------------------------------------- random numbers

unsigned int random(unsigned int max)
{
   static unsigned int seed = 12345678;
   unsigned int result;
   const unsigned int bucket = static_cast<unsigned int>(4294967296.0 / max);
   do
   {
      seed = 1664525 * seed + 1013904223;
      result = seed / bucket;
   }
   while (result >= max);
   return result;
}

//------------------------------------------------------------------------ time

void execTime()
{
   double time = clock();
   if (time == 0)
      cerr << endl;
   else if (time < static_cast<double>(CLOCKS_PER_SEC))
      cerr << endl << time / (0.001 * CLOCKS_PER_SEC) << " milliseconds.  ";
   else
      cerr << endl << time / CLOCKS_PER_SEC << " seconds.  ";
}

//---------------------------------------------------------------------- arrays

template<typename T>
class Array
{
public:

   // Default constructor creates an empty array.
   Array() : lo(0), hi(0), data(0), used(0), avail(0) {}

   // Erasmus code always calls this to initialize the array.
   void init(int ilo, int ihi)
   {
      lo = ilo;
      hi = ihi;
      int size = hi - lo;
      if (size > 0)
      {
         data = new T[size];
         used = data + size;
         avail = used;
         for (T* p = data; p != used; ++p)
            *p = T();
      }
   }

   // Add an element to the end of the array.
   Array<T> & extend(T e)
   {
      if (used == avail)
      {
         int size = hi - lo;
         int space = 2 * (hi - lo) + 1;
         T *newData = new T[space];
         T* q = newData;
         for (T* p = data; p != used;)
            *q++ = *p++;
         delete [] data;
         data = newData;
         used = data + size;
         avail = data + space;
      }
      *used++ = e;
      hi += 1;
      return *this;
   }

   // Read an element.
   T operator[] (int i) const
   {
      i -= lo;
      if (i >= 0 && i < hi)
         return data[i];
      throw "array subscript error (index = " + int2string1(i+lo) + ").";
   }

   // Write an element.
   T & operator[] (int i)
   {
      i -= lo;
      if (i >= 0 && i < hi)
         return data[i];
      throw "array subscript error (index = " + int2string1(i+lo) + ").";
   }

   // Return the size of the array.
   int size() const
   {
      return hi - lo;
   }

   // Return a pointer to the elements of the array.
   T* elems() const
   {
      return data;
   }

   // Assignment
   Array<T> & operator=(const Array<T> & rhs)
   {
      if (this == &rhs)
         return *this; // self-assignment
      delete data;
      lo = rhs.lo;
      hi = rhs.hi;
      int size = hi - lo;
      if (size > 0)
      {
         data = new T[size];
         used = data + size;
         avail = used;
         int i = lo;
         for (T* p = data; p != used; ++p)
            *p = rhs[i++];
      }
   }

   // Lower bound
   int getLo()
   {
      return lo;
   }

   // Upper bound
   int getHi()
   {
      return hi;
   }

private:
   int lo;   // Lower subscript bound
   int hi;   // Upper subscript bound (index of last element + 1)
   T *data;  // Pointer to data
   T *used;  // Pointer to first unused element
   T *avail; // Pointer to first unallocated element
};

//----------------------------------------------------------- execute byte codes

int exec_bytes(unsigned char *bytes, int size)
{
   // Testing code
   for (int i = 0; i < size; ++i)
      cout << int(bytes[i]) << ' ';
   cout << endl;
   return 0;

   // For real use
   // int (*p)() = (int(*)())bytes;
   // return p();
}

//------------------------------------------------------------------- processes

// Forward declarations for Channel
struct Channel;

// Base class for processes
struct Process
{
   Process() : type(0), pc(0), name(""), procNum(++procCounter), next(0)
   { }
   ~Process()
   {
      --procCounter;
   }
   friend ostream & operator<<(ostream & os, Process * pp)
   {
      return os << pp->procNum;
   }
   void report(string line)
   {
      cerr <<
      left << setw(20) << name <<
      right << setw(3) << type <<
      setw(5) << pc << "   " << line << endl;
   }
   virtual void do_actions() = 0;
   int type;     // Type id
   int pc;       // 'Program counter' used as switch case
   string name;
   int procNum;  // Unique id for this instance
   static int procCounter;

   // Pointers for communication
   bool *pBool;
   char *pByte;
   unsigned char *pUnsignedByte;
   int *pInt;
   unsigned int *pUnsignedInt;
   double *pDouble;
   char *pChar;
   string *pString;
   Channel **ppChannel;

   // Pointer to next process in queue
   Process * next;
};

int Process::procCounter = 0;

// Queue functions

// Insert process at end of queue
void put(Process * & queue, Process * proc)
{
   if (queue)
   {
      proc->next = queue->next;
      queue->next = proc;
   }
   else
      proc->next = proc;
   queue = proc;
}

// Return first process in queue without changing queue.
Process * first(Process * queue)
{
   if (queue)
      return queue->next;
   ostringstream os;
   os << "No processes to run: terminating.";
   throw os.str();
}

// Remove first process from front of queue
Process * get(Process * & queue)
{
   assert(queue);
   Process * result = queue->next;
   if (result == queue)
      queue = 0;
   else
      queue->next = queue->next->next;
   return result;
}

// Write a text representation of a queue to an output stream.
void showQueue(ostream & os, Process * queue)
{
   os << '<';
   if (queue)
   {
      Process * last = queue;
      Process * first = queue->next;
      while (true)
      {
         os << ' ' << first;
         if (first == last)
            break;
         first = first->next;
      }
   }
   os << " >";
}

// Remove the first process from A queue and delete it
void remove(Process * & queue)
{
   assert(queue);
   Process * pp = get(queue);
   delete pp;
}

// Global queue of processes ready to run
Process *readyQueue = 0;

// Return the length of the ready queue.
// We know that there is at least one entry.
long rql()
{
   Process *p = readyQueue;
   long len = 0;
   do
   {
      ++len;
      p = p->next;
   }
   while (p != readyQueue);
   return len;
}

// Average queue length
long aql = 0;

// Number of samples for averaging
long qs = 0;

// Maximum queue length
long mql = 0;

// Unique ID for channels
int channelNumber = 0;

struct Channel
{
   Channel() : wp(0), qp(0), fn(0) {}

   void setData(Process *w, int f)
   {
      wp = w;
      fn = f;
      if (qp)
      {
         put(readyQueue, qp);
         qp = 0;
      }
   }

   void resume()
   {
      put(readyQueue, wp);
      wp = 0;
   }

   bool idle()
   {
      return wp == 0;
   }

   bool check(int f)
   {
      return fn == f;
   }

   void setQuery(Process *q)
   {
      qp = q;
   }

   Process*getOther()
   {
      return wp;
   }

   Process *wp, *qp;
   int fn;
};

struct Select
{
   Select() : numBranches(0), branch(0), test(0)
   {}
   int numBranches;
   int branch;         // First branch to try next time
   int test;           // Branch currently being tested
   vector<int> states; // Index PC states for this branch
};

string sourceLine = "";

//*B

int main()
{
//*C
   Process *p;
   try
   {
      while (readyQueue)
      {
         // Queue statistics
         long ql = rql();
         aql += ql;
         ++qs;
         if (mql < ql)
            mql = ql;

         readyQueue = readyQueue->next;
         p = first(readyQueue);
//*E
         p->report(sourceLine);
         if (--cycles == 0)
            break;
//*F
         p->do_actions();
      }
   }
   catch (const char *msg)
   {
      cerr << "\nFailed: " << msg <<
      "\nProcess: " << p->name <<
      "\nLine: " << sourceLine << endl;
   }
   catch (string msg)
   {
      cerr << "\nFailed: " << msg <<
      "\nProcess: " << p->name <<
      "\nLine: " << sourceLine << endl;
   }
   execTime();
   if (Process::procCounter)
      cerr << Process::procCounter << " processes waiting.";
   else
      cerr << "All processes finished.";
   if (qs > 0)
      cerr <<
      "  Average: " << fixed << setprecision(1) <<
      double(aql)/double(qs) << ".  Maximum: " << mql;
   cerr << endl;
   return 0;
}

//*Z
