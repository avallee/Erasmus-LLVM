/** \file types.h */

#ifndef TYPES_H
#define TYPES_H

#include <list>
#include <set>
#include <string>
#include <vector>

class BaseNode;
typedef BaseNode* Node;
typedef std::vector<Node> List;
typedef List::const_iterator ListIter;

class BasicBlock;
typedef BasicBlock* Block;
typedef std::list<Block> BlockList;
typedef BlockList::iterator BlockIter;

class FuncDef;
typedef std::vector<FuncDef*>::const_iterator FuncIter;

typedef std::set<std::string> StringSet;

// Lightning stuff

typedef std::vector<void*> Patches;

#endif

