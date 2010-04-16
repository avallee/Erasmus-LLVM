/** \file drawAST.cpp
 * Functions used to draw AST nodes for a back-end.
 */

#include <set>
#include <cstdlib>

using namespace std;

#include "ast.h"
#include "utilities.h"

string drawBool(string name, bool value)
{
    return value ? "[" + name + " true]" : "";
}

string drawInt(string name, int value)
{
    return value > 0 ? "[" + name + " " + str(value) + "]" : "";
}

string drawAttr(string name, string value)
{
    return value == "" ? "" : '[' + name + ' ' + value + ']';
}

string drawOp(Operator op)
{
    string result = "[op ";
    switch (op)
    {
        case LISTOP_BAR:
            result += "bar]";
            break;
        case LISTOP_SEQ:
            result += "seq]";
            break;
        case BINOP_OR:
            result += "or]";
            break;
        case BINOP_AND:
            result += "and]";
            break;
        case BINOP_PLUS:
            result += "plus]";
            break;
        case BINOP_MINUS:
            result += "minus]";
            break;
        case BINOP_MULTIPLY:
            result += "mul]";
            break;
        case BINOP_DIVIDE:
            result += "div]";
            break;
        case BINOP_MOD:
            result += "mod]";
            break;
        case BINOP_EXTEND:
            result += "extend]";
            break;
        case BINOP_LT:
            result += "lt]";
            break;
        case BINOP_LE:
            result += "le]";
            break;
        case BINOP_GT:
            result += "gt]";
            break;
        case BINOP_GE:
            result += "ge]";
            break;
        case BINOP_EQ:
            result += "eq]";
            break;
        case BINOP_NE:
            result += "ne]";
            break;
        case UNOP_MINUS:
            result += "minus]";
            break;
        case UNOP_NOT:
            result += "not]";
            break;
        case UNOP_MANY:
            result += "many]";
            break;
        case UNOP_MORE:
            result += "more]";
            break;
        case UNOP_OPT:
            result += "opt]";
            break;
        default:
            result += "unknown]";
            break;
    }
    return result;
}

void drawSubTree(ostream & os, Node pn, set<int> & nodeNums, int level)
{
    if (pn)
        pn->drawAST(os, nodeNums, level);
    else
        os << '\n' << setw(level) << ' ' << "  #0";
}

bool BaseNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    os << '\n' << setw(level) << ' ';
    /*
       if (find(nodeNums.begin(), nodeNums.end(), nodeNum) != nodeNums.end())
       {
       os << '#' << nodeNum;
       return false;
       }
       else
       {
       nodeNums.insert(nodeNum);
       os << '(' << nodeToString(kv) << ' ' << nodeNum << ' ';
               os << drawInt("line", ep.lineNum);
               os << drawInt("col", ep.charPos);
               return true;
               }
               */
    return false;
}

bool ProgramNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        for (ListIter it = nodes.begin(); it != nodes.end(); ++it)
            (*it)->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool InstanceNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawBool("toplevel", topLevel);
        name->drawAST(os, nodeNums, level + 2);
        for (ListIter it = args.begin(); it != args.end(); ++it)
            (*it)->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool RemoveNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool ProcedureNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    BaseNode::drawAST(os, nodeNums, level);
}

bool ProcessNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawAttr("name", name);
        os << drawInt("id", typeNum);
        for (ListIter it = params.begin(); it != params.end(); ++it)
            (*it)->drawAST(os, nodeNums, level + 2);
        seq->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool ProtocolNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        drawSubTree(os, expr, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool DefNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        name->drawAST(os, nodeNums, level + 2);
        value->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool CellNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawAttr("name", name);
        os << '\n' << setw(level+2) << ' ' << "(Parameters 0";
                                                for (ListIter it = params.begin(); it != params.end(); ++it)
                                                    (*it)->drawAST(os, nodeNums, level + 4);
                                                os << '\n' << setw(level+2) << ' ' << ')';
                                                os << '\n' << setw(level+2) << ' ' << "(Instances 0";
                                                                                        for (ListIter it = instances.begin(); it != instances.end(); ++it)
                                                                                            (*it)->drawAST(os, nodeNums, level + 4);
                                                                                        os << '\n' << setw(level+2) << ' ' << ')';
                                                                                        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool SequenceNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        for (ListIter it = stmts.begin(); it != stmts.end(); ++it)
            (*it)->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool SkipNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool ExitNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool IfNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        for (ListIter it = condPairs.begin(); it != condPairs.end(); ++it)
            (*it)->drawAST(os, nodeNums, level + 2);
        alt->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool CondPairNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        cond->drawAST(os, nodeNums, level + 2);
        seq->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool LoopNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        seq->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool ForNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        comp->drawAST(os, nodeNums, level + 2);
        seq->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool AnyNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        comp->drawAST(os, nodeNums, level + 2);
        seq->drawAST(os, nodeNums, level + 2);
        drawSubTree(os, alt, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool ComprehensionNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        drawSubTree(os, type, nodeNums, level + 2);
        var->drawAST(os, nodeNums, level + 2);
        collection->drawAST(os, nodeNums, level + 2);
        drawSubTree(os, pred, nodeNums, level + 2);
    }
}

bool RangeNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        drawSubTree(os, type, nodeNums, level + 2);
        start->drawAST(os, nodeNums, level + 2);
        finish->drawAST(os, nodeNums, level + 2);
        drawSubTree(os, step, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool EnumSetNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        drawSubTree(os, type, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool MapSetNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawAttr("setkind", mapSetKindToString(setKind));
        map->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool RangeInitNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        var->drawAST(os, nodeNums, level + 2);
        start->drawAST(os, nodeNums, level + 2);
        finish->drawAST(os, nodeNums, level + 2);
        drawSubTree(os, step, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool EnumInitNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        var->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool MapInitNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawAttr("kind", mapSetKindToString(setKind));
        map->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool RangeTermNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawBool("open", open);
        os << drawBool("ascending", ascending);
        var->drawAST(os, nodeNums, level + 2);
        if (step)
            step->drawAST(os, nodeNums, level + 2);
        finish->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool EnumTermNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        var->drawAST(os, nodeNums, level + 2);
        max->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool MapTermNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawAttr("kind", mapSetKindToString(setKind));
        map->drawAST(os, nodeNums, level + 2);
        var->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool MatchNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        pred->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool RangeStepNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawBool("ascending", ascending);
        var->drawAST(os, nodeNums, level + 2);
        if (step)
            step->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool EnumStepNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        var->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool MapStepNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawAttr("kind", mapSetKindToString(setKind));
        map->drawAST(os, nodeNums, level + 2);
        var->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool SelectNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawInt("id", selNum);
        os << drawAttr("policy", policyToString(policy));
        os << drawInt("branches", numBranches);
        for (ListIter it = options.begin(); it != options.end(); ++it)
            (*it)->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool OptionNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawInt("id", selNum);
        os << drawAttr("policy", policyToString(policy));
        drawSubTree(os, guard, nodeNums, level + 2);
        seq->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool SendNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << ' ' << tempName << ' ' << bufferName << ' ' << fieldNum;
        rhs->drawAST(os, nodeNums, level + 2);
        port->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool SendOptionNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << ' ' << tempName << ' ' << bufferName << ' ' << fieldNum;
        rhs->drawAST(os, nodeNums, level + 2);
        port->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool ReceiveNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        lhs->drawAST(os, nodeNums, level + 2);
        port->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool ReceiveOptionNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        lhs->drawAST(os, nodeNums, level + 2);
        port->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool ConstantNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawBool("global", pervasive);
        name->drawAST(os, nodeNums, level + 2);
        type->drawAST(os, nodeNums, level + 2);
        value->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool DecNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    os << '\n' << setw(level) << ' ';
    /*
       if (find(nodeNums.begin(), nodeNums.end(), nodeNum) != nodeNums.end())
       os << '#' << nodeNum;
       else
       {
       nodeNums.insert(nodeNum);
       if (type)
       {
    // has type
    if (value)
    {
    // type, value => declaration with initialization
    if (value->kind() == DOT_NODE)
    {
    // v: T := p.f
    os << "(DecReceive " << num() << ' ';
            os << drawInt("line", ep.lineNum);
            os << drawInt("col", ep.charPos);
            drawSubTree(os, name, nodeNums, level + 2);
            drawSubTree(os, type, nodeNums, level + 2);
            drawSubTree(os, value, nodeNums, level + 2);
            }
            else
            {
    // v: T := e
    os << "(DecInit " << num() << ' ';
            os << drawInt("line", ep.lineNum);
            os << drawInt("col", ep.charPos);
            os << drawBool("constant", constant);
            drawSubTree(os, name, nodeNums, level + 2);
            drawSubTree(os, type, nodeNums, level + 2);
            drawSubTree(os, value, nodeNums, level + 2);
            }
            }
            else
            {
    // type, no value => declaration
    os << "(Dec " << num() << ' ';
            os << drawInt("line", ep.lineNum);
            os << drawInt("col", ep.charPos);
            os << drawBool("ref", reference);
            os << drawAttr("port", portToString(portDir));
            drawSubTree(os, name, nodeNums, level + 2);
            drawSubTree(os, type, nodeNums, level + 2);
            }
            }
            else
            {
    // no type
    if (value)
    {
    // no type, value
    if (name->kind() == DOT_NODE)
    {
    // send
    os << "(Send " << num() << ' ';
            os << drawInt("line", ep.lineNum);
            os << drawInt("col", ep.charPos);
            drawSubTree(os, name, nodeNums, level + 2);
            drawSubTree(os, value, nodeNums, level + 2);
            }
            else if (value->kind() == DOT_NODE)
            {
    // receive
    os << "(Receive " << num() << ' ';
            os << drawInt("line", ep.lineNum);
            os << drawInt("col", ep.charPos);
            drawSubTree(os, name, nodeNums, level + 2);
            drawSubTree(os, value, nodeNums, level + 2);
            }
            else
    {
        // assign
        os << "(Assign " << num() << ' ';
                os << drawInt("line", ep.lineNum);
                os << drawInt("col", ep.charPos);
                drawSubTree(os, name, nodeNums, level + 2);
                drawSubTree(os, value, nodeNums, level + 2);
    }
}
else if (ck == SEND)
{
    // no type, no value => signal
    os << "(SendSignal " << num() << ' ';
            os << drawInt("line", ep.lineNum);
            os << drawInt("col", ep.charPos);
            drawSubTree(os, name, nodeNums, level + 2);
}
else if (ck == RECEIVE)
{
    // no type, no value => signal
    os << "(ReceiveSignal " << num() << ' ';
            os << drawInt("line", ep.lineNum);
            os << drawInt("col", ep.charPos);
            drawSubTree(os, name, nodeNums, level + 2);
}
else
{
    os << "(Unknown " << num() << ' ';
            drawSubTree(os, name, nodeNums, level + 2);
}
}
os << '\n' << setw(level) << ' ' << ')';
} */
return false;
}

bool DotNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        port->drawAST(os, nodeNums, level + 2);
        field->drawAST(os, nodeNums, level+ 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool QueryNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        port->drawAST(os, nodeNums, level + 2);
        field->drawAST(os, nodeNums, level+ 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool BoolNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawBool("val", value);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool CharNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawAttr("char", str(value));
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool TextNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawAttr("text", str(value));
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool NumNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawAttr("value", value);
        type->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool ListopNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawOp(op);
        for (ListIter it = args.begin(); it != args.end(); ++it)
            (*it)->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool BinopNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawOp(op);
        drawSubTree(os, type, nodeNums, level + 2);
        lhs->drawAST(os, nodeNums, level + 2);
        rhs->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool CondExprNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        drawSubTree(os, type, nodeNums, level + 2);
        pred->drawAST(os, nodeNums, level + 2);
        lhs->drawAST(os, nodeNums, level + 2);
        rhs->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool UnopNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawOp(op);
        drawSubTree(os, type, nodeNums, level + 2);
        operand->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool SubscriptNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawBool("lvalue", lvalue);
        base->drawAST(os, nodeNums, level+2);
        sub->drawAST(os, nodeNums, level+2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool SubrangeNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawBool("lvalue", lvalue);
        base->drawAST(os, nodeNums, level+2);
        sub1->drawAST(os, nodeNums, level+2);
        sub2->drawAST(os, nodeNums, level+2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool IteratorNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawAttr("fun", iterFunToString(fun));
        map->drawAST(os, nodeNums, level+2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool VoidTypeNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool BoolTypeNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool ByteTypeNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool FileTypeNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        switch (mode)
        {
            case FILE_INPUT:
                os << drawAttr("dir", "input");
                break;
            case FILE_OUTPUT:
                os << drawAttr("dir", "output");
                break;
            default:
                os << drawAttr("dir", str(mode));
                break;
        }
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool IntegerTypeNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool FloatTypeNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool DecimalTypeNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool CharTypeNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool TextTypeNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool EnumTypeNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        for (ListIter it = values.begin(); it != values.end(); ++it)
            (*it)->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool EnumValueNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawAttr("name", name);
        type->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool ArrayTypeNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        rangeType->drawAST(os, nodeNums, level+2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool MapTypeNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawAttr("dir", portToString(portDir));
        domainType->drawAST(os, nodeNums, level+2);
        rangeType->drawAST(os, nodeNums, level+2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool FunctionNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << name->getNameString();
        for (ListIter it = args.begin(); it != args.end(); ++it)
            (*it)->drawAST(os, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

bool NameNode::drawAST(ostream & os, set<int> & nodeNums, int level)
{
    if (BaseNode::drawAST(os, nodeNums, level))
    {
        os << drawAttr("value", value);
        os << drawBool("defocc", definingOccurrence);
        os << drawInt("uid", varNum);
        os << drawInt("field", fieldNum);
        drawSubTree(os, definition, nodeNums, level + 2);
        if (tie != this)
            drawSubTree(os, tie, nodeNums, level + 2);
        os << '\n' << setw(level) << ' ' << ')';
    }
}

