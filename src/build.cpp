#include "ast.h"
#include "lts.h"

// Functions to construct a LTS from a protocol.

LTS *BaseNode::protocolGraph(set<Node> & fieldDecs)
{
    emergencyStop("BaseNode::build", ep);
}

LTS *DecNode::protocolGraph(set<Node> & fieldDecs)
{
    fieldDecs.insert(this);
    LTS *plts = new LTS();
    plts->insert(plts->getStart(), plts->getFinish(), name->getNameString());
    return plts;
}

LTS *UnopNode::protocolGraph(set<Node> & fieldDecs)
{
    LTS *plts;
    switch (op)
    {
        case UNOP_OPT: // ?
            {
                plts = new LTS();
                LTS *qlts = operand->protocolGraph(fieldDecs);
                plts->insert(qlts->getTrans());
                plts->insert(plts->getStart(), qlts->getStart());
                plts->insert(qlts->getFinish(), plts->getFinish());
                plts->insert(plts->getStart(), plts->getFinish(), "?");
            }
            break;

        case UNOP_MANY: // *
            {
                plts = operand->protocolGraph(fieldDecs);
                plts->insert(plts->getFinish(), plts->getStart());
            }
            break;

        case UNOP_MORE: // +
            {
                plts = new LTS();
                LTS *qlts = operand->protocolGraph(fieldDecs);
                LTS *rlts = operand->protocolGraph(fieldDecs);
                plts->insert(qlts->getTrans());
                plts->insert(rlts->getTrans());
                plts->insert(plts->getStart(), qlts->getStart());
                plts->insert(qlts->getFinish(), rlts->getStart());
                plts->insert(rlts->getFinish(), rlts->getStart());
                plts->insert(rlts->getFinish(), plts->getFinish());
            }
            break;

        default:
            plts = new LTS(true);
            break;
    }
    return plts;
}

LTS *ListopNode::protocolGraph(set<Node> & fieldDecs)
{
    LTS *plts;
    switch (op)
    {
        case LISTOP_SEQ: // ;
            {
                plts = new LTS();
                int state = plts->getStart();
                for (ListIter it = args.begin(); it != args.end(); ++it)
                {
                    LTS *qlts = (*it)->protocolGraph(fieldDecs);
                    plts->insert(qlts->getTrans());
                    plts->insert(state, qlts->getStart());
                    state = qlts->getFinish();
                }
                plts->insert(state, plts->getFinish());
            }
            break;

        case LISTOP_BAR: // |
            {
                plts = new LTS();
                for (ListIter it = args.begin(); it != args.end(); ++it)
                {
                    LTS *qlts = (*it)->protocolGraph(fieldDecs);
                    plts->insert(qlts->getTrans());
                    plts->insert(plts->getStart(), qlts->getStart());
                    plts->insert(qlts->getFinish(), plts->getFinish());
                }
            }
            break;

        default:
            plts = new LTS(true);
            break;
    }
    return plts;
}

// Functions to construct a LTS from a process body.

LTS *BaseNode::processGraph(Node portDec, int loopEnd)
{
    //cerr << nodeToString(kind()) << "::processGraph\n";
    return new LTS(true);
}

LTS *SequenceNode::processGraph(Node portDec, int loopEnd)
{
    //cerr << nodeToString(kind()) << "::processGraph\n";
    LTS *plts = new LTS();
    int state = plts->getStart();
    for (ListIter it = stmts.begin(); it != stmts.end(); ++it)
    {
        LTS *qlts = (*it)->processGraph(portDec, loopEnd);
        plts->insert(qlts->getTrans());
        plts->insert(state, qlts->getStart());
        state = qlts->getFinish();
    }
    plts->insert(state, plts->getFinish());
    return plts;
}

LTS *IfNode::processGraph(Node portDec, int loopEnd)
{
    //cerr << nodeToString(kind()) << "::processGraph\n";
    LTS *plts = new LTS();
    for (ListIter it = condPairs.begin(); it != condPairs.end(); ++it)
    {
        LTS *qlts = (*it)->processGraph(portDec, loopEnd);
        plts->insert(qlts->getTrans());
        plts->insert(plts->getStart(), qlts->getStart());
        plts->insert(qlts->getFinish(), plts->getFinish());
    }
    if (alt->isEmpty())
        plts->insert(plts->getStart(), plts->getFinish(), "?");
    else
    {
        LTS *qlts = alt->processGraph(portDec, loopEnd);
        plts->insert(qlts->getTrans());
        plts->insert(plts->getStart(), qlts->getStart());
        plts->insert(qlts->getFinish(), plts->getFinish());
    }
    return plts;
}

LTS *CondPairNode::processGraph(Node portDec, int loopEnd)
{
    //cerr << nodeToString(kind()) << "::processGraph\n";
    LTS *plts = new LTS();
    LTS *condLTS = cond->processGraph(portDec, loopEnd);
    LTS *seqLTS = seq->processGraph(portDec, loopEnd);
    plts->insert(condLTS->getTrans());
    plts->insert(seqLTS->getTrans());
    plts->insert(plts->getStart(), condLTS->getStart());
    plts->insert(condLTS->getFinish(), seqLTS->getStart());
    plts->insert(seqLTS->getFinish(), plts->getFinish());
    return plts;
}

LTS *LoopNode::processGraph(Node portDec, int loopEnd)
{
    //cerr << nodeToString(kind()) << "::processGraph\n";
    LTS *plts = new LTS();
    LTS *seqLTS = seq->processGraph(portDec, plts->getFinish());
    plts->insert(seqLTS->getTrans());
    plts->insert(plts->getStart(), seqLTS->getStart());
    plts->insert(seqLTS->getFinish(), seqLTS->getStart());
    return plts;
}

LTS *ExitNode::processGraph(Node portDec, int loopEnd)
{
    //cerr << nodeToString(kind()) << "::processGraph\n";
    LTS *plts = new LTS();
    plts->insert(plts->getStart(), loopEnd);
    return plts;
}

LTS *SelectNode::processGraph(Node portDec, int loopEnd)
{
    //cerr << nodeToString(kind()) << "::processGraph\n";
    LTS *plts = new LTS();
    for (ListIter it = options.begin(); it != options.end(); ++it)
    {
        LTS *qlts = (*it)->processGraph(portDec, loopEnd);
        plts->insert(qlts->getTrans());
        plts->insert(plts->getStart(), qlts->getStart());
        plts->insert(qlts->getFinish(), plts->getFinish());
    }
    return plts;
}

LTS *OptionNode::processGraph(Node portDec, int loopEnd)
{
    //cerr << nodeToString(kind()) << "::processGraph\n";
    return seq->processGraph(portDec, loopEnd);
}

LTS *DotNode::processGraph(Node portDec, int loopEnd)
{
    //cerr << nodeToString(kind()) << "::processGraph\n";
    LTS *plts;
    Node myPortDec = port->getDefinition();
    if (myPortDec == portDec)
    {
        string fieldName = field->getNameString();
        plts = new LTS();
        plts->insert(plts->getStart(), plts->getFinish(), fieldName);
    }
    else
        plts = new LTS(true);
    return plts;
}

LTS *QueryNode::processGraph(Node portDec, int loopEnd)
{
    //cerr << nodeToString(kind()) << "::processGraph\n";
    LTS *plts;
    Node myPortDec = port->getDefinition();
    if (myPortDec == portDec)
    {
        string fieldName = field->getNameString();
        plts = new LTS();
        plts->insert(plts->getStart(), plts->getFinish(), fieldName);
    }
    else
        plts = new LTS(true);
    return plts;
}

LTS *BinopNode::processGraph(Node portDec, int loopEnd)
{
    //cerr << nodeToString(kind()) << "::processGraph\n";
    LTS *plts = new LTS();
    LTS *lhsLTS = lhs->processGraph(portDec, loopEnd);
    LTS *rhsLTS = rhs->processGraph(portDec, loopEnd);
    plts->insert(lhsLTS->getTrans());
    plts->insert(rhsLTS->getTrans());
    plts->insert(plts->getStart(), lhsLTS->getStart());
    plts->insert(lhsLTS->getFinish(), rhsLTS->getStart());
    plts->insert(rhsLTS->getFinish(), plts->getFinish());
    return plts;
}

LTS *CondExprNode::processGraph(Node portDec, int loopEnd)
{
    LTS *plts = new LTS();
    // \todo: complete implementation
    return plts;
}

LTS *UnopNode::processGraph(Node portDec, int loopEnd)
{
    //cerr << nodeToString(kind()) << "::processGraph\n";
    return operand->processGraph(portDec, loopEnd);
}

LTS *SubscriptNode::processGraph(Node portDec, int loopEnd)
{
    //cerr << nodeToString(kind()) << "::processGraph\n";
    return sub->processGraph(portDec, loopEnd);
}

LTS *FunctionNode::processGraph(Node portDec, int loopEnd)
{
    //cerr << nodeToString(kind()) << "::processGraph\n";
    LTS *plts = new LTS();
    int state = plts->getStart();
    for (ListIter it = args.begin(); it != args.end(); ++it)
    {
        LTS *qlts = (*it)->processGraph(portDec, loopEnd);
        plts->insert(qlts->getTrans());
        plts->insert(state, qlts->getStart());
        state = qlts->getFinish();
    }
    plts->insert(state, plts->getFinish());
    return plts;
}

LTS *DecNode::processGraph(Node portDec, int loopEnd)
{
    //cerr << nodeToString(kind()) << "::processGraph\n";
    if (type)
    {
        if (value)
            // v:T := e - must check e
            return value->processGraph(portDec, loopEnd);
        else
        {
            ; // v:T - nothing to check
            return new LTS(true);
        }
    }
    else if (value)
    {
        // v := e - must check v and e
        LTS *plts = new LTS();
        LTS *nameLTS = name->processGraph(portDec, loopEnd);
        LTS *valueLTS = value->processGraph(portDec, loopEnd);
        plts->insert(nameLTS->getTrans());
        plts->insert(valueLTS->getTrans());
        plts->insert(plts->getStart(), nameLTS->getStart());
        plts->insert(nameLTS->getFinish(), valueLTS->getStart());
        plts->insert(valueLTS->getFinish(), plts->getFinish());
        return plts;
    }
    else
        // p.s - must check p.s
        return name->processGraph(portDec, loopEnd);
}

LTS *NameNode::processGraph(Node portDec, int loopEnd)
{
    //cerr << nodeToString(kind()) << "::processGraph\n";
    return new LTS(true);
}

