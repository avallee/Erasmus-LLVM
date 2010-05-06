#include "ast.h"
#include "llvmgen.h"

#include <iostream>

void BaseNode::genLLVM()
{std::cout << "InstanceNode,LLVM \n";}

void ProgramNode::genLLVM()
{   
   std::cout << "LLVM Code generation\n";
   //prolog();
   for (ListIter it = nodes.begin(); it != nodes.end(); ++it)
      (*it)->genLLVM();
   //epilog();
   //showCode();
}

void InstanceNode::genLLVM()
{std::cout << "InstanceNode,LLVM \n";}

void RemoveNode::genLLVM()
{std::cout << "Remove,LLVM \n";}

void ProcessNode::genLLVM()
{
 std::cout << "ProcessNode,LLVM \n";
 for (ListIter it = params.begin(); it != params.end(); ++it)
    (*it)->genLLVM();
}

void CppNode::genLLVM()  
{std::cout << "CppNode,LLVM \n";}

void ProcedureNode::genLLVM()    
{std::cout << "ProcedureNode,LLVM \n";}

void ProtocolNode::genLLVM()
{std::cout << "ProtocolNode,LLVM \n";}

void DefNode::genLLVM()
{
	std::cout << "DefNode,LLVM \n";
	name->genLLVM();
	value->genLLVM();
}

void CellNode::genLLVM()
{std::cout << "CellNode,LLVM \n";}

void SequenceNode::genLLVM()
{std::cout << "SequenceNode,LLVM \n";}

void SkipNode::genLLVM()
{std::cout << "SkipNode,LLVM \n";}

void ExitNode::genLLVM()
{std::cout << "ExitNode,LLVM \n";}

void IfNode::genLLVM()
{std::cout << "IfNode,LLVM \n";}

void CondPairNode::genLLVM()
{std::cout << "CondPairNode,LLVM \n";}

void LoopNode::genLLVM()
{std::cout << "LoopNode,LLVM \n";}

void ForNode::genLLVM()
{
	comp->genLLVM();
	seq->genLLVM();
	std::cout << "ForNode,LLVM \n";
	
}

void AnyNode::genLLVM()
{	
	comp->genLLVM();
	seq->genLLVM();
	std::cout << "AnyNode,LLVM \n";
}

void ComprehensionNode::genLLVM()
{
	  //drawSubTree(os, type, nodeNums, level + 2);
      var->genLLVM();
      collection->genLLVM();
      //drawSubTree(os, pred, nodeNums, level + 2);
	  std::cout << "ComprehensionNode,LLVM \n";
}

void RangeNode::genLLVM()
{std::cout << "RangeNode,LLVM \n";}

void RangeInitNode::genLLVM()
{      
	  var->genLLVM();
      start->genLLVM();
      finish->genLLVM();
	  std::cout << "RangeInitNode,LLVM \n";
}

void RangeTermNode::genLLVM()
{std::cout << "RangeTermNode,LLVM \n";}

void RangeStepNode::genLLVM()
{std::cout << "RangeStepNode,LLVM \n";}

void MapSetNode::genLLVM()
{std::cout << "MapSetNode,LLVM \n";}

void MapInitNode::genLLVM()
{std::cout << "MapInitNode,LLVM \n";}

void MapTermNode::genLLVM()
{std::cout << "MapTermNode,LLVM \n";}

void MapStepNode::genLLVM()
{std::cout << "MapStepNode,LLVM \n";}

void EnumSetNode::genLLVM()
{std::cout << "EnumSetNode,LLVM \n";}

void EnumInitNode::genLLVM()
{std::cout << "EnumInitNode,LLVM \n";}

void EnumTermNode::genLLVM()
{std::cout << "EnumTermNode,LLVM \n";}

void EnumStepNode::genLLVM()
{std::cout << "EnumStepNode,LLVM \n";}

void MatchNode::genLLVM()
{std::cout << "MatchNode,LLVM \n";}

void SelectNode::genLLVM()
{std::cout << "SelectNode,LLVM \n";}

void OptionNode::genLLVM()
{std::cout << "OptionNode,LLVM \n";}

void DotNode::genLLVM()
{std::cout << "DotNode,LLVM \n";}

void QueryNode::genLLVM()
{std::cout << "QueryNode,LLVM \n";}

void BoolNode::genLLVM()
{std::cout << "BoolNode,LLVM \n";}

void CharNode::genLLVM()
{std::cout << "CharNode,LLVM \n";}

void TextNode::genLLVM()
{std::cout << "TextNode,LLVM \n";}

void NumNode::genLLVM()
{std::cout << "NumNode,LLVM \n";}

void ListopNode::genLLVM()
{std::cout << "ListopNode,LLVM \n";}

void BinopNode::genLLVM()
{std::cout << "BinopNode,LLVM \n";}

void CondExprNode::genLLVM()
{std::cout << "CondExprNode,LLVM \n";}

void UnopNode::genLLVM()
{std::cout << "UnopNode,LLVM \n";}

void SubscriptNode::genLLVM()
{std::cout << "SubscriptNode,LLVM \n";}

void SubrangeNode::genLLVM()
{std::cout << "SubrangeNode,LLVM \n";}

void IteratorNode::genLLVM()
{std::cout << "IteratorNode,LLVM \n";}

void ConstantNode::genLLVM()
{
  std::cout << "ConstantNode,LLVM \n";
  //Not compiling
//  std::string Name = name->getValue();
//  Value *V = NamedValues[Name];
//  ConstantFP::get(getGlobalContext(), APFloat(value->getValue()));
//  Builder.CreateLoad(V, Name.c_str());

}

void DecNode::genLLVM()
{std::cout << "DecNode,LLVM \n";}

void EnumValueNode::genLLVM()
{std::cout << "EnumValueNode,LLVM \n";}

void ArrayTypeNode::genLLVM()
{std::cout << "ArrayTypeNode,LLVM \n";}

void MapTypeNode::genLLVM()
{std::cout << "MapTypeNode,LLVM \n";}

void IterTypeNode::genLLVM()
{std::cout << "IterTypeNode,LLVM \n";}

void FunctionNode::genLLVM()
{std::cout << "FunctionNode,LLVM \n";}

void NameNode::genLLVM()
{std::cout << "NameNode,LLVM \n";}

void SendNode::genLLVM()
{std::cout << "SendNode,LLVM \n";}

void SendOptionNode::genLLVM()
{std::cout << "SendOptionNode,LLVM \n";}

void ReceiveNode::genLLVM()
{std::cout << "ReceiveNode,LLVM \n";}

void ReceiveOptionNode::genLLVM()
{std::cout << "ReceiveOptionNode,LLVM \n";}

void ThreadNode::genLLVM()
{std::cout << "ThreadNode,LLVM \n";}

void ThreadParamNode::genLLVM()
{std::cout << "ThreadParamNode,LLVM \n";}

void StartNode::genLLVM()
{std::cout << "StartNode,LLVM \n";}

void ThreadCallNode::genLLVM()
{std::cout << "ThreadCallNode,LLVM \n";}

void ThreadStartNode::genLLVM()
{std::cout << "ThreadStartNode,LLVM \n";}

void ThreadStopNode::genLLVM()
{std::cout << "ThreadStopNode,LLVM \n";}
