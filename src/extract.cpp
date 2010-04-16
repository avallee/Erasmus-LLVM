/** \file extract.cpp */

#include "ast.h"
#include "utilities.h"

// Call extract for a statement.
// Call extractor for a node that might have to change
// as a result of extracting communications from it.
// In general, extract for statements, extractor for expressions.

/** Convert a node into a list of nodes that perform the function
 * of the original node.  Temporary variables and assignments
 * are introduced where necessary to accommodate commuunications.
 * If \a pn points to p.q, then the call returns
 *     temp := p.q
 * and this node is replaced by a pointer to \a temp.
 */
List extractor(Node & pn, int & tempNum)
{
    if (pn->kind() == DOT_NODE || pn->kind() == QUERY_NODE)
    {
        string varName = "ioTemp" + str(++tempNum);
        Node name = new NameNode(pn->getPos(), varName, 0, true);
        List subs;
        Node ass = new DecNode(pn->getPos(), name, 0, pn,
                               NO_ROLE, NO_FIELD, true, false, false, false);
        subs.push_back(ass);
        pn = new NameNode(pn->getPos(), varName, 0, false);
        return subs;
    }
    else
        return pn->extract(tempNum);
}

/**
 * This function does various things:
 * -# For a statement, return a list of statements.
 * -# For a sequence, replace each statement by the list above,
 *    unless the list is empty.
 * -# For nodes that may include sequences, call \a extract recursively.
 * -# For other nodes, do nothing.
 *
 * The list of statements for the first option consists of declarations of
 * temporary variables, with a receive operation as initializer, followed
 * by the original statement with the receive operation replaced by a
 * temporary.
 */
List BaseNode::extract(int & tempNum)
{
    return List();
}

List ProgramNode::extract(int & tempNum)
{
    for (ListIter it = nodes.begin(); it != nodes.end(); ++it)
        (*it)->extract(tempNum);
    return List();
}

List DefNode::extract(int & tempNum)
{
    return value->extract(tempNum);
}

List ProcedureNode::extract(int & tempNum)
{
    return seq->extract(tempNum);
}

List ProcessNode::extract(int & tempNum)
{
    return seq->extract(tempNum);
}

List CellNode::extract(int & tempNum)
{
    for (ListIter it = instances.begin(); it != instances.end(); ++it)
        (*it)->extract(tempNum);
    return List();
}

List IfNode::extract(int & tempNum)
{
    for (ListIter it = condPairs.begin(); it != condPairs.end(); ++it)
        (*it)->extract(tempNum);
    return alt->extract(tempNum);
}

List CondPairNode::extract(int & tempNum)
{
    decs = extractor(cond, tempNum); // 090227
    return seq->extract(tempNum);
}

List LoopNode::extract(int & tempNum)
{
    return seq->extract(tempNum);
}

List AnyNode::extract(int & tempNum)
{
    seq->extract(tempNum);
    return alt->extract(tempNum);
}

List ForNode::extract(int & tempNum)
{
    return seq->extract(tempNum);
}

List SelectNode::extract(int & tempNum)
{
    for (ListIter it = options.begin(); it != options.end(); ++it)
        (*it)->extract(tempNum);
    return List();
}

List OptionNode::extract(int & tempNum)
{
    return seq->extract(tempNum);
}

// Original version
List SequenceNode::extract(int & tempNum)
{
    List::iterator it = stmts.begin();
    while (it != stmts.end())
    {
        List ns = (*it)->extract(tempNum);
        //      for (ListIter jt = ns.begin(); jt != ns.end(); ++jt)
        for (List::reverse_iterator jt = ns.rbegin(); jt != ns.rend(); ++jt)
            it = stmts.insert(it, *jt);
        ++it;
    }
    return List();
}

// This version doesn't work
//List SequenceNode::extract(int & tempNum)
//{
//   List newSubs;
//   for (ListIter it = stmts.begin(); it != stmts.end(); ++it)
//   {
//      List subs = (*it)->extract(tempNum);
//      newSubs.insert(newSubs.end(), subs.begin(), subs.end());
//   }
//   stmts.insert(stmts.begin(), newSubs.begin(), newSubs.end());
//   return List();
//}

List DecNode::extract(int & tempNum)
{
    List typeSubs;
    if (type)
        typeSubs = type->extract(tempNum);

    List valSubs;
    if (value)
    {
        if (name->kind() == DOT_NODE && value->kind() == DOT_NODE)
            valSubs = extractor(value, tempNum);
        else
            valSubs = value->extract(tempNum);
    }

    typeSubs.insert(typeSubs.end(), valSubs.begin(), valSubs.end());
    return typeSubs;
}

List UnopNode::extract(int & tempNum)
{
    return extractor(operand, tempNum);
}

List SubscriptNode::extract(int & tempNum)
{
    return extractor(sub, tempNum);
}

List SubrangeNode::extract(int & tempNum)
{
    List lsubs = extractor(sub1, tempNum);
    List rsubs = extractor(sub2, tempNum);
    lsubs.insert(lsubs.end(), rsubs.begin(), rsubs.end());
    return lsubs;
}

List IteratorNode::extract(int & tempNum)
{
    return extractor(map, tempNum);
}

List BinopNode::extract(int & tempNum)
{
    List lsubs = extractor(lhs, tempNum);
    List rsubs = extractor(rhs, tempNum);
    lsubs.insert(lsubs.end(), rsubs.begin(), rsubs.end());
    return lsubs;
}

List ArrayTypeNode::extract(int & tempNum)
{
    List loSubs = extractor(lo, tempNum);
    List hiSubs = extractor(hi, tempNum);
    loSubs.insert(loSubs.end(), hiSubs.begin(), hiSubs.end());
    return loSubs;
}

List CondExprNode::extract(int & tempNum)
{
    List subs, allSubs;

    subs = extractor(lhs, tempNum);
    allSubs.insert(allSubs.end(), subs.begin(), subs.end());

    subs = extractor(rhs, tempNum);
    allSubs.insert(allSubs.end(), subs.begin(), subs.end());

    subs = extractor(pred, tempNum);
    allSubs.insert(allSubs.end(), subs.begin(), subs.end());

    return allSubs;
}

List StartNode::extract(int & tempNum)
{
    return seq->extract(tempNum);
}

List ThreadNode::extract(int & tempNum)
{
    return seq->extract(tempNum);
}

List FunctionNode::extract(int & tempNum)
{
    List allSubs;
    for (List::iterator it = args.begin(); it != args.end(); ++it)
    {
        List subs = extractor(*it, tempNum);
        allSubs.insert(allSubs.end(), subs.begin(), subs.end());
    }
    return allSubs;
}

List InstanceNode::extract(int & tempNum)
{
    List result;
    for (List::iterator it = args.begin(); it != args.end(); ++it)
    {
        List args = extractor(*it, tempNum);
        result.insert(result.end(), args.begin(), args.end());
    }
    return result;
}

List ThreadCallNode::extract(int & tempNum)
{
    List allSubs;
    for (List::iterator it = inputs.begin(); it != inputs.end(); ++it)
    {
        List sub = extractor(*it, tempNum);
        allSubs.insert(allSubs.end(), sub.begin(), sub.end());
    }
    for (List::iterator it = outputs.begin(); it != outputs.end(); ++it)
    {
        List sub = extractor(*it, tempNum);
        allSubs.insert(allSubs.end(), sub.begin(), sub.end());
    }
    return allSubs;
}

