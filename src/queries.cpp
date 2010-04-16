/** \file queries.cpp
 * Predicates used to query AST nodes.
 */

#include "ast.h"

//-------------------------------------------------------- isBoolType

bool BaseNode::isBoolType() const
{
    return false;
}

bool BoolTypeNode::isBoolType() const
{
    return true;
}

//-------------------------------------------------------- isArrayType

bool BaseNode::isArrayType() const
{
    return false;
}

bool DefNode::isArrayType() const
{
    return value->isArrayType();
}

bool ArrayTypeNode::isArrayType() const
{
    return true;
}

bool NameNode::isArrayType() const
{
    return definition && definition->isArrayType();
}
//-------------------------------------------------------- isMapType

bool BaseNode::isMapType() const
{
    return false;
}

bool DefNode::isMapType() const
{
    return value->isMapType();
}

bool MapTypeNode::isMapType() const
{
    return true;
}

bool NameNode::isMapType() const
{
    return definition && definition->isMapType();
}

//-------------------------------------------------------- isEnumType

bool BaseNode::isEnumType() const
{
    return false;
}

bool DefNode::isEnumType() const
{
    return value->isEnumType();
}

bool EnumTypeNode::isEnumType() const
{
    return true;
}

bool NameNode::isEnumType() const
{
    return definition ? definition->isEnumType() : false;
}

//-------------------------------------------------------- isIterType

bool BaseNode::isIterType() const
{
    return false;
}

bool IterTypeNode::isIterType() const
{
    return true;
}

//-------------------------------------------------------- isNumericType

bool BaseNode::isNumericType() const
{
    return false;
}

bool ByteTypeNode::isNumericType() const
{
    return false;
}

bool IntegerTypeNode::isNumericType() const
{
    return true;
}

bool FloatTypeNode::isNumericType() const
{
    return true;
}

bool DecimalTypeNode::isNumericType() const
{
    return true;
}

//-------------------------------------------------------- isEmpty

bool BaseNode::isEmpty() const
{
    return false;
}

bool SequenceNode::isEmpty() const
{
    return stmts.size() == 0;
}

//-------------------------------------------------------- isReply

bool BaseNode::isReply() const
{
    return false;
}

bool DecNode::isReply() const
{
    return mk == REPLY;
}

bool NameNode::isReply() const
{
    return definition->isReply();
}

//-------------------------------------------------------- isReference

bool BaseNode::isReference() const
{
    return false;
}

bool DecNode::isReference() const
{
    return reference;
}

bool NameNode::isReference() const
{
    return definition ? definition->isReference() : false;
}

//-------------------------------------------------------- checkSysIO

FileMode BaseNode::checkSysIO() const
{
    return SYS_NULL;
}

FileMode DotNode::checkSysIO() const
{
    if (port->getNameString() == BASIO_SYS)
    {
        string fieldName = field->getNameString();
        if (fieldName == BASIO_INP)
            return SYS_IN;
        else if (fieldName == BASIO_OUT)
            return SYS_OUT;
        else if (fieldName == BASIO_ERR)
            return SYS_ERR;
        else
            return SYS_NULL;
    }
    else
        return SYS_NULL;
}

FileMode QueryNode::checkSysIO() const
{
    if (port->getNameString() == BASIO_SYS)
    {
        string fieldName = field->getNameString();
        if (fieldName == BASIO_INP)
            return SYS_IN;
        else if (fieldName == BASIO_OUT)
            return SYS_OUT;
        else if (fieldName == BASIO_ERR)
            return SYS_ERR;
        else
            return SYS_NULL;
    }
    else
        return SYS_NULL;
}

//-------------------------------------------------------- isMap

bool BaseNode::isMap() const
{
    return false;
}

bool DefNode::isMap() const
{
    return value->isMap();
}

bool NameNode::isMap() const
{
    return definition && definition->isMap();
}

bool MapTypeNode::isMap() const
{
    return true;
}

bool DecNode::isMap() const
{
    return type && type->isMap();
}

//-------------------------------------------------------- isPort

bool BaseNode::isPort() const
{
    return false;
}

bool DefNode::isPort() const
{
    return value->isPort();
}

bool DecNode::isPort() const
{
    return type ? type->isPort() : false;
}

bool ProtocolNode::isPort() const
{
    return true;
}

bool SubscriptNode::isPort() const
{
    return base->isPort();
}

bool MapTypeNode::isPort() const
{
    return rangeType->isPort();
}

bool NameNode::isPort() const
{
    return definition && definition->isPort();
}

bool ThreadParamNode::isPort() const
{
    return type->isPort();
}

//-------------------------------------------------------- isConstant

bool BaseNode::isConstant() const
{
    emergencyStop("isConstant", ep);
    return false;
}

bool DefNode::isConstant() const
{
    Error() << "Name used in invalid context." << name->getPos() << REPORT;
    return false;
}

bool ConstantNode::isConstant() const
{
    return true;
}

bool DecNode::isConstant() const
{
    return constant;
}

bool DotNode::isConstant() const
{
    return false;
}

bool QueryNode::isConstant() const
{
    return false;
}

bool ComprehensionNode::isConstant() const
{
    return false;
}

bool SubscriptNode::isConstant() const
{
    return false;
}

bool ThreadParamNode::isConstant() const
{
    return false;
}

bool NameNode::isConstant() const
{
    return definition && definition->isConstant();
}

//-------------------------------------------------------- assignable

bool BaseNode::assignable() const
{
    return true;
}

bool ComprehensionNode::assignable() const
{
    return collection->assignable();
}

bool MapSetNode::assignable() const
{
    return setKind == MAPSET_RANGE;
}

bool RangeNode::assignable() const
{
    return false;
}

bool EnumSetNode::assignable() const
{
    return false;
}

bool NameNode::assignable() const
{
    return definition ? definition->assignable() : true;
}

//-------------------------------------------------------- isPervasive

bool BaseNode::isPervasive() const
{
    return false;
}

bool ConstantNode::isPervasive() const
{
    return true;
}

bool NameNode::isPervasive() const
{
    return definition && definition->isPervasive();
}

//-------------------------------------------------------- isEnumVal

bool BaseNode::isEnumVal() const
{
    emergencyStop("isEnumVal", ep);
    return false;
}

bool DecNode::isEnumVal() const
{
    return type &&  type->isEnumVal();
}

bool NameNode::isEnumVal() const
{
    return definition && (definition->kind() == ENUM_VALUE_NODE);
}

//-------------------------------------------------------- isIterator

bool BaseNode::isIterator() const
{
    return false;
}

bool ComprehensionNode::isIterator() const
{
    return collection->isIterator();
}

bool MapSetNode::isIterator() const
{
    return true;
}

bool NameNode::isIterator() const
{
    return definition && definition->isIterator();
}

//-------------------------------------------------------- isAlias

bool BaseNode::isAlias() const
{
    return false;
}

bool DecNode::isAlias() const
{
    return alias;
}

bool NameNode::isAlias() const
{
    return definition ? definition->isAlias() : false;
    //   return passByReference;             changed 081206
}


//-------------------------------------------------------- isPassByReference

bool BaseNode::isPassByReference() const
{
    return false;
}

bool NameNode::isPassByReference() const
{
    return passByReference;
}

/** \todo Check whether both of these functions are necessary. */

