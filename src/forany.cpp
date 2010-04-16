/** \file forany.cpp
 *
 * This file contains functions that create special nodes
 * for 'for' and 'any' statements.
 *
 * A ComprehensionNode has a field, \a collection, which points to either a
 * RangeNode or a MapSetNode.  Each of these nodes describes a set.
 * A RangeNode has start, step, and finish values.
 * A MapSetNode has a map variable; the elements of the set are the
 * elements of the map.
 */

#include "ast.h"

//----------------------------------------------------------------------makeInit

Node BaseNode::makeInit(Node, Node)
{
    emergencyStop("makeInit", ep);
}

Node ComprehensionNode::makeInit(Node, Node) // Arguments ignored
{
    return collection->makeInit(var, iter);
}

Node RangeNode::makeInit(Node var, Node) // Second argument ignored
{
    return new RangeInitNode(ep, owner, type, var, start,
                             finish, step, finishNum, stepNum);
}

Node MapSetNode::makeInit(Node var, Node loopIter)
{
    iter = loopIter;
    return new MapInitNode(ep, owner, type, var, loopIter, mapKind, setKind, map, indexName);
}

Node EnumSetNode::makeInit(Node var, Node)
{
    return new EnumInitNode(ep, owner, var);
}

//------------------------------------------------------------------makeTermTest

Node BaseNode::makeTermTest(Block, Node, Node)
{
    emergencyStop("makeTermTest", ep);
}

Node ComprehensionNode::makeTermTest(Block bb, Node, Node) // Two arguments ignored
{
    return collection->makeTermTest(bb, var, iter);
}

Node RangeNode::makeTermTest(Block bb, Node var, Node) // One argument ignored
{
    return new RangeTermNode(ep, owner, bb, type, var, step, stepNum,
                             finish, finishNum, open, ascending);
}

Node MapSetNode::makeTermTest(Block bb, Node var, Node iter)
{
    return new MapTermNode(ep, owner, bb, type, var, iter, mapKind, setKind, map, indexName);
}

Node EnumSetNode::makeTermTest(Block bb, Node var, Node)
{
    return new EnumTermNode(ep, owner, bb, var, max);
}



//----------------------------------------------------------------------makeStep

Node BaseNode::makeStep(Node, Node)
{
    emergencyStop("makeStep", ep);
}

Node ComprehensionNode::makeStep(Node, Node)
{
    return collection->makeStep(var, iter);
}

Node RangeNode::makeStep(Node var, Node)
{
    return new RangeStepNode(ep, owner, type, var, step, stepNum, ascending);
}

Node MapSetNode::makeStep(Node var, Node iter)
{
    return new MapStepNode(ep, owner, type, var, iter, mapKind, setKind, map, indexName);
}

Node EnumSetNode::makeStep(Node var, Node)
{
    return new EnumStepNode(ep, owner, var);
}



//-----------------------------------------------------------------makeMatchTest

Node BaseNode::makeMatchTest(Block)
{
    emergencyStop("makeMatchtest", ep);
}

Node ComprehensionNode::makeMatchTest(Block bb)
{
    return new MatchNode(ep, bb, pred);
}



