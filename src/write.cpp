/** \file write.cpp
* Functions for the final phase: writing the generated code.
*/

#include "ast.h"
#include "utilities.h"

#include <cassert>

using namespace std;

/** Write a line of the Erasmus source code in the C++ code. */
void writeSourceLine(ostream & code, Errpos ep)
{
   TAB3 << "sourceLine = " << str(ep.pLine) << ";\n";
}

/** Write source code. */
void BaseNode::write(ostream & code)
{}

/** Write process classes. */
void BaseNode::writeParts(ostream & code, WriteMode wm)
{}

void ProgramNode::write(ostream & code)
{
   for (ListIter it = nodes.begin(); it != nodes.end(); ++it)
      (*it)->write(code);
}

void ProgramNode::writeParts(ostream & code, WriteMode wm)
{
   for (ListIter it = nodes.begin(); it != nodes.end(); ++it)
      (*it)->writeParts(code, wm);
}

void writeArg(ostream & code, Node arg, bool & moreThanOne)
{
   if (moreThanOne)
      code << ", ";
   moreThanOne = true;
   if (arg->kind() == NAME_NODE)
   {
      if (arg->isPassByReference())
         code << "& ";
      code << arg->getFullName(true);
   }
   else
      arg->write(code);
}

void InstanceNode::write(ostream & code)
{
   if (!topLevel)
   {
      writeSourceLine(code, ep);
      bool moreThanOne = false;
      Node def = name->getDefinition();
      Node val = def->getValue();
      switch (val->kind())
      {
         case CELL_NODE:
            TAB3 << "new " << getNameString() << "(";
            for (ListIter it = args.begin(); it != args.end(); ++it)
               writeArg(code, *it, moreThanOne);
            code << ");\n";
            break;

         case PROCESS_NODE:
            TAB3 << "{\n";
            TAB4 << getNameString() << "* tempPtr = new " << getNameString() << "(";
            for (ListIter it = args.begin(); it != args.end(); ++it)
               writeArg(code, *it, moreThanOne);
            code << ");\n";
            TAB4 << "put(readyQueue, tempPtr);\n";
            TAB3 << "}\n";
            break;

         case CPP_NODE:
            {
               TAB3;
               code << name->getNameString() << '(';
               bool more = false;
               for (ListIter it = args.begin(); it != args.end(); ++it)
               {
                  if (more)
                     code << ", ";
                  more = true;
                  (*it)->write(code);
               }
               code << ");\n";
            }
            break;

         case PROCEDURE_NODE:
            TAB3 << getNameString() << '(';
            for (ListIter it = args.begin(); it != args.end(); ++it)
               writeArg(code, *it, moreThanOne);
            code << ");\n";
            break;
      }
   }
}

void InstanceNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case CELL_DECLARATION:
         {
            Node def = name->getDefinition();
            Node val = def->getValue();
            if (val->kind() == PROCESS_NODE)
            {
               string pn = name->getNameString();
               TAB2 << "put(readyQueue, new " << pn << "(";
               bool moreThanOne = false;
               for (ListIter it = args.begin(); it != args.end(); ++it)
                  writeArg(code, *it, moreThanOne);
               code << ")); // Instantiate process\n";
            }
            else if (val->kind() == CELL_NODE && !topLevel)
            {
               string pn = name->getNameString();
               TAB2 << "new " << pn << "(";
               bool moreThanOne = false;
               for (ListIter it = args.begin(); it != args.end(); ++it)
                  writeArg(code, *it, moreThanOne);
               code << "); // Instantiate cell\n";

            }
            break;
         }

      case CREATE_PROCESSES:
         TAB1 << getNameString() << ' ' << getNameString() <<
         "_inst; // InstanceNode\n";
         break;

      default:
         break;
   }
}

void ProcedureNode::write(ostream & code)
{
   for (ListIter it = params.begin(); it != params.end(); ++it)
      (*it)->write(code);
}

void ProcedureNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case CLOSURE_DECLARATION:
         bool moreThanOne = false;
         code << "void " << name << '(';
         for (ListIter it = params.begin(); it != params.end(); ++it)
         {
            if ((*it)->kind() == DEC_NODE)
            {
               if (moreThanOne)
                  code << ", ";
               moreThanOne = true;
               if ((*it)->isPort())
                  code << "Channel *" << (*it)->getFullName();
               else
               {
                  code << (*it)->getCTypeString() << ' ';
                  if ((*it)->isAlias())
                     code << '*';
                  code << (*it)->getFullName();
               }
            }
         }
         code << ")\n{\n";
         seq->writeParts(code, LOCAL_DECLARATION);
         TAB1 << "int pc = " << start << ";\n";
         TAB1 << "while (true)\n";
         TAB1 << "{\n";
         TAB2 << "switch (pc)\n";
         TAB2 << "{\n";
         for (BlockIter it = blocks.begin(); it != blocks.end(); ++it)
            code << *it;
         TAB2 << "}\n";
         TAB1 << "}\n}\n\n";
         break;
   }
}

void ProcessNode::write(ostream & code)
{
   for (ListIter it = params.begin(); it != params.end(); ++it)
      (*it)->write(code);
}

void ProcessNode::writeParts(ostream & code, WriteMode wm)
{
   bool moreThanOne;
   switch (wm)
   {
      case CLOSURE_DECLARATION:
         code << "struct " << name << " : public Process\n";
         code << "{\n";
         TAB1 << name << '(';

         // Constructor parameters
         moreThanOne = false;
         for (ListIter it = params.begin(); it != params.end(); ++it)
         {
            if ((*it)->kind() == DEC_NODE)
            {
               if ((*it)->isPort())
               {
                  if (moreThanOne)
                     code << ", ";
                  code << "Channel *" << (*it)->getFullName();
                  moreThanOne = true;
               }
               else
               {
                  if (moreThanOne)
                     code << ", ";
                  code << (*it)->getCTypeString() << ' ';
                  if ((*it)->isAlias())
                     code << '*';
                  code << (*it)->getFullName();
                  moreThanOne = true;
               }
            }
         }
         code << ')';

         // Initializers
         if (params.size() > 0)
         {
            code << " : ";
            moreThanOne = false;
            for (ListIter it = params.begin(); it != params.end(); ++it)
            {
               if ((*it)->kind() == DEC_NODE)
               {
                  if (moreThanOne)
                     code << ", ";
                  code << (*it)->getFullName(false) << '(' <<
                  (*it)->getFullName(false) << ')';
                  moreThanOne = true;
               }
            }
         }
         code << "\n";

         TAB1 << "{\n";
         TAB2 << "// Standard initialization\n";
         TAB2 << "name = \"" << name << "\";\n";
         TAB2 << "type = " << typeNum << ";\n";
         TAB2 << "pc = " << start << ";\n";
         TAB2 << "// Variable initialization\n";
         seq->writeParts(code, INITIALIZER);
         TAB1 <<  "}\n";

         TAB1 << "// Actions\n";
         TAB1 << "void do_actions();\n";

         TAB1 << "// Instance variable declarations\n";
         for (ListIter it = params.begin(); it != params.end(); ++it)
            (*it)->writeParts(code, wm);
         seq->writeParts(code, wm);
         code << "};\n\n";
         break;

      case ACTION_BODIES:
         code << "void " << name << "::do_actions()\n";
         code << "{\n";
         TAB1 << "while (true)\n";
         TAB1 << "{\n";
         TAB2 << "switch (pc)\n";
         TAB2 << "{\n";
         for (BlockIter it = blocks.begin(); it != blocks.end(); ++it)
            code << *it;
         TAB2 << "}\n";
         TAB1 << "}\n";
         code << "}\n\n";
         break;
   }
}

void CellNode::write(ostream & code)
{
}

void CellNode::writeParts(ostream & code, WriteMode wm)
{
   bool moreThanOne;
   switch (wm)
   {
      case CELL_DECLARATION:
         code << "struct " << name;
         code << "\n{\n";
         TAB1 << name << "(";

         // Constructor parameters
         moreThanOne = false;
         for (ListIter it = params.begin(); it != params.end(); ++it)
         {
            if ((*it)->kind() == DEC_NODE)
            {
               if (moreThanOne)
                  code << ", ";
               if ((*it)->isPort())
                  code << "Channel *" << (*it)->getFullName();
               else
               {
                  code << (*it)->getCTypeString() << ' ';
                  if ((*it)->isAlias())
                     code << '*';
                  code << (*it)->getFullName();
               }
               moreThanOne = true;
            }
         }
         code << ")";

         // Initializers
         if (params.size() > 0)
         {
            code << " : ";
            moreThanOne = false;
            for (ListIter it = params.begin(); it != params.end(); ++it)
            {
               if ((*it)->kind() == DEC_NODE)
               {
                  if (moreThanOne)
                     code << ", ";
                  code << (*it)->getFullName(false) <<
                  '(' << (*it)->getFullName(false) << ')';
                  moreThanOne = true;
               }
            }
         }
         code << "\n";
         TAB1 << "{\n";

         // Body of constructor
         for (ListIter it = instances.begin(); it != instances.end(); ++it)
         {
            if ((*it)->kind() == DEC_NODE)
            {
//               if ((*it)->isPort())
//                  (*it)->writeParts(code, wm);
//               else
               (*it)->writeParts(code, VARDEC_ASS);
            }
            else if ((*it)->kind() == INSTANCE_NODE)
               (*it)->writeParts(code, wm);
         }

         TAB1 << "}\n";

         // Instance variable declarations
         for (ListIter it = params.begin(); it != params.end(); ++it)
         {
            if ((*it)->kind() == DEC_NODE)
            {
               if ((*it)->isPort())
               {
                  TAB1 << "Channel *" << (*it)->getFullName(false) <<
                  "; // CellNode: port parameter declaration\n";
               }
               else
               {
                  TAB1 << (*it)->getCTypeString() << ' ';
                  if (isAlias())
                     code << '*';
                  code << (*it)->getFullName(false) <<
                  "; // CellNode: parameter declaration\n";
               }
            }
         }


         for (ListIter it = instances.begin(); it != instances.end(); ++it)
         {
            if ((*it)->kind() == DEC_NODE)
            {
               if ((*it)->isPort())
               {
                  TAB1 << "Channel *" << (*it)->getFullName(false) <<
                  "; // CellNode: port instance declaration\n";
               }
               else
               {
                  TAB1 << (*it)->getCTypeString() << ' ' <<
                  (*it)->getFullName(false) <<
                  "; // CellNode: instance declaration\n";
               }
            }
         }
         code << "};\n\n";
         break;

      default:
         break;
   }
}

void DefNode::write(ostream & code)
{
   value->write(code);
}

void DefNode::writeParts(ostream & code, WriteMode wm)
{
   value->writeParts(code, wm);
}

void ConstantNode::writeParts(ostream & code, WriteMode wm)
{
   if (wm == CONSTANT_DECLARATION)
   {
      code << "const " << type->getCTypeString() << ' ' << name->getFullName(false) << " = ";
      value->write(code);
      code << "; // pervasive constant\n";
   }
}

void DecNode::writeParts(ostream & code, WriteMode wm)
{
   if (type)
   {
      switch (wm)
      {

         case INITIALIZER:
            if (isPort() && ! type->isMap())
               TAB2 << getFullName(false) << " = 0; // Dec channel initializer\n";
            else if ( ! ( type->isArrayType() || type->isMap() || type->kind() == FILE_TYPE_NODE) )
            {
               TAB2 << getFullName(false) << " = ";
               type->writeParts(code, wm);
               code << "; // Dec initializer\n";
            }
            break;

         case LOCAL_DECLARATION:
            if (isPort() && ! type->isMap())
               TAB1 << "Channel *" << getFullName(false) << " = 0; // Dec channel initializer\n";
            else if ( ! ( type->isMap() || type->kind() == FILE_TYPE_NODE) )
               TAB1;
            type->write(code);
            code << ' ' << getFullName(false) << "; // Local declaration\n";
            break;

         case CLOSURE_DECLARATION:
            if (isPort() && ! type->isMap())
               TAB1 << "Channel *" << getFullName(false) <<
               "; // Dec channel declaration\n";
            else
            {
               TAB1 << getCTypeString() << ' ';
               if (alias)
                  code << '*';
               code << getFullName(false) << "; // Dec closure declaration\n";
               if (isMap())
                  TAB1 <<getCTypeString() << "::iterator " <<
                  getFullName(false) << "_iter;\n";
            }
            break;

         case CELL_DECLARATION:
            if (isPort())
            {
               TAB2 << "Channel *" << getFullName(false) <<
               " = new Channel(); // Dec port cell\n";
            }
            else
            {
               TAB2 << getCTypeString() << (reference ? " *" : " ") <<
               getFullName(false) << "; // Not a port\n";
            }
            break;

         case VARDEC_ASS:
            if (value && ! reference)
            {
               TAB2 << getFullName(false) << " = ";
               value->write(code);
               code << "; // CellNode: variable initialization\n";
            }
            else if (isPort())
            {
               // Communication: channel constructor has no arguments
               TAB2 << "Channel *" << getFullName(false) <<
               " = new Channel(); // CellNode: constructing '" <<
               type->getNameString() << "'\n";
            }
            else if (type && type->isArrayType())
            {
               TAB2 << getFullName(false) << ".init(" <<
               TAB2 << getFullName(false) << ".init(";
               type->getLo()->writeParts(code, ARRAY_INITIALIZER);
               code << ", ";
               type->getHi()->writeParts(code, ARRAY_INITIALIZER);
               code << "); // CellNode: array initialization\n";
            }
            else if (type->isMapType())
            {
               // Nothing to do
            }
            else
            {
               TAB2 << getFullName(false) << " = ";
               type->writeParts(code, INITIALIZER);
               code << "; // CellNode: default value\n";
            }
            break;

         default:
            break;
      }
   }
}

void DecNode::write(ostream & code)
{
   writeSourceLine(code, ep);
   if (value)
   {
      if (  value->getFun() == "file_open_read" ||
            value->getFun() == "file_open_write" )
      {
         TAB3;
         name->write(code);
         code << ".open(";
         value->write(code);
         code << ");\n";
      }
      else if (value->getFun() == "file_read")
      {
         TAB3 << "getline(";
         value->write(code);
         code << ", ";
         name->write(code);
         code << ");\n";
      }
      else
      {
         // Assignment (lhs := rhs) or move (lhs <- rhs)
         TAB3;
         name->write(code);
         code << " = ";
         value->write(code);
         code << ";\n";
         if (move)
         {
            TAB3;
            string result = "// makeNull -- no type";
            Node type = value->getType();
            if (type)
            {
               if (type->kind() == NAME_NODE)
                  type = type->getValue();
               result = value->getFullName();
               if (type->isEnumType())
                  result += " = 0";
               else
                  result += " = " + type->getDefaultValue();
            }
            code << result + ";\n";
         }
      }
   }
   else if (! parameter && type && isPort() && ! type->isMap())
   {
      // Local port declaration: construct the channel.
      TAB3;
      name->write(code);
      code << " = new Channel(); // Channel for local port.\n";
   }
   else if (type->isArrayType())
   {
//      type->getLo()->showDebug("lo");
//      type->getHi()->showDebug("hi");
      TAB3 << getFullName(false) << ".init(";
      type->getLo()->writeParts(code, ARRAY_INITIALIZER);
      code << ", ";
      type->getHi()->writeParts(code, ARRAY_INITIALIZER);
      code << "); // Array initializer\n";
   }
}

void SequenceNode::write(ostream & code)
{
   for (ListIter it = stmts.begin(); it != stmts.end(); ++it)
      (*it)->write(code);
}

//   {
//      showDebug("binop");
//      if ((*it)->kind() == BINOP_NODE)
//      {
//         showDebug("binop");
//         TAB3;
//         (*it)->write(code);
//         code << ";\n";
//      }
//      else
//         (*it)->write(code);
//   }
//}

void SequenceNode::writeParts(ostream & code, WriteMode wm)
{
   for (ListIter it = stmts.begin(); it != stmts.end(); ++it)
      (*it)->writeParts(code, wm);
}

void IfNode::write(ostream & code)
{
   for (ListIter it = condPairs.begin(); it != condPairs.end(); ++it)
      (*it)->write(code);
   alt->write(code);
}

void IfNode::writeParts(ostream & code, WriteMode wm)
{
   for (ListIter it = condPairs.begin(); it != condPairs.end(); ++it)
      (*it)->writeParts(code, wm);
   alt->writeParts(code, wm);
}

void CondPairNode::write(ostream & code)
{
   for (ListIter it = decs.begin(); it != decs.end(); ++it)
      (*it)->write(code);
   cond->write(code);
   seq->write(code);
}

void CondPairNode::writeParts(ostream & code, WriteMode wm)
{
   for (ListIter it = decs.begin(); it != decs.end(); ++it)
      (*it)->writeParts(code, wm);
   cond->writeParts(code, wm);
   seq->writeParts(code, wm);
}

void LoopNode::write(ostream & code)
{
   writeSourceLine(code, ep);
   seq->write(code);
}

void LoopNode::writeParts(ostream & code, WriteMode wm)
{
   seq->writeParts(code, wm);
}

void ForNode::writeParts(ostream & code, WriteMode wm)
{
   comp->writeParts(code, wm);
   seq->writeParts(code, wm);
}

void AnyNode::writeParts(ostream & code, WriteMode wm)
{
   comp->writeParts(code, wm);
   seq->writeParts(code, wm);
   if (alt)
      alt->writeParts(code, wm);
}

void ComprehensionNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case CLOSURE_DECLARATION:
         {
            MapKind mk = collection->getMapKind();
            MapSetKind msk = collection->getMapSetKind();

            string smk = mapKindToString(mk);
            string smsk = mapSetKindToString(msk);

            // This code breaks some for loops.
//            cerr << "\nComprehension Node" <<
//            "\nmapKind = " << smk <<
//            "\nsetKind = " << smsk <<
//            "\nIter = " << iter->getFullName() <<
//            " : " << iterType->getCTypeString() <<
//            "\nVar = " << var->getFullName() <<
//            " : " << type->getCTypeString() << endl;

            TAB1;
            switch (mk)
            {
               case NO_MAP_KIND:
                  if (type)
                     code << type->getCTypeString();
                  else
                     code << "int";
                  break;
               case INDEXED:
                  code << iterType->getCTypeString() << "::iterator";
                  break;
               case ARRAY:
                  code << (msk == MAPSET_DOMAIN ? "int" : iterType->getRangeType()->getCTypeString());
                  break;
               case TEXT:
                  code << (msk == MAPSET_DOMAIN ? "int" : "char");
                  break;
            }
            code << ' ' << var->getFullName(false) << "; // loop variable\n";
            collection->writeParts(code, wm);
         }
         break;
   }
}

void MapSetNode::writeParts(ostream & code, WriteMode wm)
{
   if (indexName != "")
      TAB1 << "int " << indexName << "; // loop index\n";
}

void RangeNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case CLOSURE_DECLARATION:
         TAB1;
         if (type)
            type->write(code);
         else
            code << "int";
         code << " finish_" << finishNum << "; // loop end\n";
         TAB1;
         if (type)
            type->write(code);
         else
            code << "int";
         code << " step_" << stepNum << "; // loop step\n";
         break;
   }
}

void EnumSetNode::writeParts(ostream & code, WriteMode wm)
{
}

void RangeInitNode::write(ostream & code)
{
   writeSourceLine(code, ep);
   TAB3;
   var->write(code);
   code << " = " ;
   start->write(code);
   code << "; // loop initialization for interval\n";

   TAB3 << "finish_" << finishNum << " = ";
   finish->write(code);
   code << ";\n";

   TAB3 << "step_" << stepNum << " = ";
   if (step)
      step->write(code);
   else
      code << '1';
   code << ";\n";
}

void RangeTermNode::write(ostream & code)
{
   TAB3 << "pc = (";
   var->write(code);
   if (open)
      code << (ascending ? " < " : " > ");
   else
      code << (ascending ? " <= " : " >= ");
   code << "finish_" << finishNum <<
   ") ? " << bb->altTransfer << " : " << bb->transfer << "; // loop test for interval\n";
}

void RangeStepNode::write(ostream & code)
{
   if (ascending)
   {
      TAB3;
      var->write(code);
      code << " += " << "step_" << stepNum << "; // loop step for interval\n";
   }
   else
   {
      TAB3;
      var->write(code);
      code << " -= " << "step_" << stepNum << "; // loop step for interval\n";
   }
}

void MapInitNode::write(ostream & code)
{
   writeSourceLine(code, ep);
   TAB3;
   switch (mapKind)
   {
      case INDEXED:
         {
            var->write(code);
            code << " = ";
            map->write(code);
            code << ".begin(); // loop initialization for map\n";
         }
         break;

      case ARRAY:
         if (setKind == MAPSET_DOMAIN)
            var->write(code);
         else
            code << indexName;
         code << " = ";
         map->write(code);
         code << ".getLo()";
         code << "; // loop initializetion for array\n";
         break;

      case TEXT:
         if (setKind == MAPSET_DOMAIN)
         {
            var->write(code);
            code << " = 0; // loop initialization for Text domain\n";
         }
         else
            code << indexName << " = 0; // loop initialization for Text domain\n";
         break;
   }
}

void MapTermNode::write(ostream & code)
{
   TAB3 << "pc = (";
   switch (mapKind)
   {
      case INDEXED:
         var->write(code);
         code << " == ";
         map->write(code);
         code << ".end()";
         break;

      case ARRAY:
         if (setKind == MAPSET_DOMAIN)
            var->write(code);
         else
            code << indexName;
         code << " == ";
         map->write(code);
         code << ".getHi()";
         break;

      case TEXT:
         if (setKind == MAPSET_DOMAIN)
            var->write(code);
         else
            code << indexName;
         code << " == ";
         map->write(code);
         code << ".length()";
         break;
   }
   code << ") ? " << bb->transfer << " : " <<
   bb->altTransfer << "; // loop test for maps and arrays\n";
}

void MapStepNode::write(ostream & code)
{
   TAB3 << "++";
   if ( (mapKind == ARRAY || mapKind == TEXT) && setKind == MAPSET_RANGE)
      code << indexName;
   else
      var->write(code);
   code << "; // loop step\n";
}

void EnumInitNode::write(ostream & code)
{
   writeSourceLine(code, ep);
   TAB3;
   var->write(code);
   code << " = 0; // loop initialization for enumeration\n";
}

void EnumStepNode::write(ostream & code)
{
   TAB3 << "++";
   var->write(code);
   code << "; // loop step for enumeration\n";
}

void EnumTermNode::write(ostream & code)
{
   TAB3 << "pc = (";
   var->write(code);
   code << " < ";
   max->write(code);
   code << ") ? " << bb->altTransfer << " : " <<
   bb->transfer << "; // loop test for enumeration\n";
}

void MatchNode::write(ostream & code)
{
   if (pred)
   {
      TAB3 << "// select eligible elements\n";
      TAB3 << "pc = (";
      pred->write(code);
      code << ") ? " << bb->altTransfer << " : " << bb->transfer << ";\n";
   }
   else
      TAB3 << "pc = " << bb->altTransfer << ";\n";
}

void SelectNode::write(ostream & code)
{
   writeSourceLine(code, ep);
   string selName = "sel_" + str(selNum);

   TAB3 << "// select\n";
   TAB3 << selName << ".test = " << selName << ".branch;\n";
   TAB3 << "pc = " << selName << ".states[" << selName << ".test];\n";
}

void SelectNode::writeParts(ostream & code, WriteMode wm)
{
   string selName = "sel_" + str(selNum);
   switch (wm)
   {
      case CLOSURE_DECLARATION:
         TAB1 << "Select " << selName << "; // Select closure declaration\n";
         break;
      case INITIALIZER:
         TAB2 << selName << ".numBranches = " << numBranches <<
         "; // Select initializer\n";
         if (policy == RANDOM)
            TAB2 << selName << ".branch = random(" << numBranches <<
            "); // Select initializer\n";
         break;
      default:
         break;
   }
   for (ListIter it = options.begin(); it != options.end(); ++it)
      (*it)->writeParts(code, wm);
}

void OptionNode::write(ostream & code)
{
   string selName = "sel_" + str(selNum);
   string test = selName + ".test";
   string branch = selName + ".branch";
   string states = selName + ".states";

   writeSourceLine(code, ep);
   if (guard)
   {
      TAB3 << "if (";
      guard->write(code);
      code << " && " << seq->getQueueTest() << ") // OptionNode\n";
   }
   else
      TAB3 << "if (" << seq->getQueueTest() << ") // OptionNode\n";
   TAB3 << "{\n";
   switch (policy)
   {
      case DEFAULT_POLICY:
      case ORDERED:
         TAB4 << "// policy = ordered\n";
         TAB4 << branch << " = 0;\n";
         break;
      case FAIR:
         TAB4 << "// policy = fair\n";
         TAB4 << branch << " = (" << branch << " + 1) % " <<
         selName << ".numBranches;\n";
         break;
      case RANDOM:
         TAB4 << "// policy = random\n";
         TAB4 << branch << " = random(" << selName << ".numBranches);\n";
         break;
   }
   TAB4 << "pc = " << execBranch << ";\n";
   TAB3 << "}\n";
   TAB3 << "else\n";
   TAB3 << "{\n";
   TAB4 << "// Choose next branch to test\n";
   TAB4 << test << " = (" << test << " + 1) % " << selName << ".numBranches;\n";
   TAB4 << "if (" << test << " == " << branch << ")\n";
   TAB4 << "{\n";
   TAB5 << "pc = " << selectStart << ";\n";
   TAB5 << "return;\n";
   TAB4 << "}\n";
   TAB4 << "else\n";
   TAB5 << "pc = " << states << "[" << test << "];\n";
   TAB3 << "}\n";
}

void OptionNode::writeParts(ostream & code, WriteMode wm)
{
   string selName = "sel_" + str(selNum);
   switch (wm)
   {
      case INITIALIZER:
         TAB2 << selName << ".states.push_back(" << testGuard <<
         "); // Option initializer\n";
         break;
      default:
         break;
   }
   seq->writeParts(code, wm);
}

void BoolNode::write(ostream & code)
{
   if (bb)
   {
      // Decision code for end of block
      writeSourceLine(code, ep);
      TAB3 << "pc = " <<
      (value ? bb->transfer : bb->altTransfer) <<
      ";\n";
   }
   else
      code << (value ? "true" : "false");
}

void CharNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case CELL_DECLARATION:
         showChar(code, value, true);
         break;
   }
}

void CharNode::write(ostream & code)
{
   showChar(code, value, true);
}

void TextNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case CELL_DECLARATION:
         showString(code, value, true);
         break;
   }
}

void TextNode::write(ostream & code)
{
   showString(code, value, true);
}

void NumNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case ARRAY_INITIALIZER:
      case CELL_DECLARATION:
         code << value;
         break;
   }
}

void NumNode::write(ostream & code)
{
   code << value;
}

void UnopNode::write(ostream & code)
{
   if (bb)
   {
      // Decision code for end of block
      writeSourceLine(code, ep);
      TAB3 << "pc = (";
      operand->write(code);
      code << ") ? " << bb->altTransfer << " : " << bb->transfer << ";\n";
   }
   else if (op == UNOP_EXECUTE)
   {
      code << "exec_bytes(";
      operand->write(code);
      code << ".elems(), ";
      operand->write(code);
      code << ".size())";
   }
   else
   {
      code << opToString(op);
      code << "(";
      operand->write(code);
      code << ")";
   }
}

void SubscriptNode::write(ostream & code)
{
   base->write(code);
   code << '[';
   sub->write(code);
   code << ']';
}

void SubrangeNode::write(ostream & code)
{
   code << "get_sub_string(";
   base->write(code);
   code << ", ";
   sub1->write(code);
   code << ", ";
   sub2->write(code);
   code << ')';
}

void IteratorNode::write(ostream & code)
{
   string var = map->getFullName(false);
   string iter = var + "_iter";
   if (map->isReference())
      var = "(*(" + map->getFullName(false) + "))";
   switch (fun)
   {
      case ITER_START:
         writeSourceLine(code, ep);
         TAB3 << iter << " = " << var << ".begin();\n";
         break;
      case ITER_FINISH:
         if (bb)
         {
            // Decision code for end of block
            writeSourceLine(code, ep);
            TAB3 << "pc = (" <<
            iter << " == " << var << ".end()" << ") ? " <<
            bb->transfer << " : " << bb->altTransfer << ";\n";
         }
         else
            code << iter << " == " << var << ".end()";
         break;
      case ITER_VALUE:
         code << iter << "->second";
         break;
      case ITER_KEY:
         code << iter << "->first";
         break;
      case ITER_STEP:
         writeSourceLine(code, ep);
         TAB5 << "++" << iter << ";\n";
         break;
   }
}

void BinopNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case CELL_DECLARATION:
         if (lhs->kind() == BINOP_NODE)
            code << '(';
         lhs->writeParts(code, wm);
         if (lhs->kind() == BINOP_NODE)
            code << ')';
         code << opToString(op);
         if (rhs->kind() == BINOP_NODE)
            code << '(';
         rhs->writeParts(code, wm);
         if (rhs->kind() == BINOP_NODE)
            code << ')';

      case ARRAY_INITIALIZER:
         if (lhs->kind() == BINOP_NODE)
            code << '(';
         lhs->writeParts(code, ARRAY_INITIALIZER);
         if (lhs->kind() == BINOP_NODE)
            code << ')';
         code << opToString(op);
         if (rhs->kind() == BINOP_NODE)
            code << '(';
         rhs->writeParts(code, ARRAY_INITIALIZER);
         if (rhs->kind() == BINOP_NODE)
            code << ')';

   }
}

void BinopNode::write(ostream & code)
{
   if (op == BINOP_EXTEND)
   {
      lhs->write(code);
      code << ".extend(";
      rhs->write(code);
      code << ')';
   }
   else if (bb)
   {
      // Decision code for end of block
      writeSourceLine(code, ep);
      TAB3 << "pc = (";
      if (lhs->kind() == BINOP_NODE)
      {
         code << '(';
         lhs->write(code);
         code << ')';
      }
      else
         lhs->write(code);
      code << opToString(op);
      if (rhs->kind() == BINOP_NODE)
      {
         code << '(';
         rhs->write(code);
         code << ')';
      }
      else
         rhs->write(code);
      code << ") ? " << bb->transfer << " : " << bb->altTransfer << ";\n";
   }
   else
   {
      // Normal binary operator expression
      if (lhs->kind() == BINOP_NODE)
         code << '(';
      lhs->write(code);
      if (lhs->kind() == BINOP_NODE)
         code << ')';
      code << opToString(op);
      if (rhs->kind() == BINOP_NODE)
         code << '(';
      rhs->write(code);
      if (rhs->kind() == BINOP_NODE)
         code << ')';
   }
}

void CondExprNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case CELL_DECLARATION:
         if (pred->kind() == BINOP_NODE)
            code << '(';
         pred->writeParts(code, wm);
         if (pred->kind() == BINOP_NODE)
            code << ')';
         code << " ? ";
         if (lhs->kind() == BINOP_NODE)
            code << '(';
         lhs->writeParts(code, wm);
         if (lhs->kind() == BINOP_NODE)
            code << ')';
         code << " : ";
         if (rhs->kind() == BINOP_NODE)
            code << '(';
         rhs->writeParts(code, wm);
         if (rhs->kind() == BINOP_NODE)
            code << ')';
   }
}

void CondExprNode::write(ostream & code)
{
   if (bb)
   {
      // Decision code for end of block
      writeSourceLine(code, ep);
      TAB4 << "pc = (";
      if (pred->kind() == BINOP_NODE)
      {
         code << '(';
         pred->write(code);
         code << ')';
      }
      else
         pred->write(code);
      code << " ? ";
      if (lhs->kind() == BINOP_NODE)
      {
         code << '(';
         lhs->write(code);
         code << ')';
      }
      else
         lhs->write(code);
      code << " : ";
      if (rhs->kind() == BINOP_NODE)
      {
         code << '(';
         rhs->write(code);
         code << ')';
      }
      else
         rhs->write(code);
      code << ") ? " << bb->transfer << " : " << bb->altTransfer << ";\n";
   }
   else
   {
      // Normal conditional expression
      if (pred->kind() == BINOP_NODE)
         code << '(';
      pred->write(code);
      if (pred->kind() == BINOP_NODE)
         code << ')';
      code << " ? ";
      if (lhs->kind() == BINOP_NODE)
         code << '(';
      lhs->write(code);
      if (lhs->kind() == BINOP_NODE)
         code << ')';
      code << " : ";
      if (rhs->kind() == BINOP_NODE)
         code << '(';
      rhs->write(code);
      if (rhs->kind() == BINOP_NODE)
         code << ')';
   }
}

void VoidTypeNode::write(ostream & code)
{
   code << "void";
}

void BoolTypeNode::write(ostream & code)
{
   code << "bool";
}

void ByteTypeNode::write(ostream & code)
{
   code << getCTypeString();
}

void IntegerTypeNode::write(ostream & code)
{
   code << getCTypeString();
}

void FileTypeNode::write(ostream & code)
{
   switch (mode)
   {
      case FILE_INPUT:
         code << "ifstream";
         break;
      case FILE_OUTPUT:
         code << "ofstream";
         break;
      default:
         code << "file?";
         break;
   }
}

void FloatTypeNode::write(ostream & code)
{
   code << "double";
}

void DecimalTypeNode::write(ostream & code)
{
   code << "double";
}

void CharTypeNode::write(ostream & code)
{
   code << "char";
}

void TextTypeNode::write(ostream & code)
{
   code << "string";
}

void VoidTypeNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case INITIALIZER:
         code << getDefaultValue();
         break;
   }
}

void BoolTypeNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case INITIALIZER:
         code << getDefaultValue();
         break;
   }
}

void ByteTypeNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case INITIALIZER:
         code << getDefaultValue();
         break;
   }
}

void FileTypeNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case INITIALIZER:
         code << getDefaultValue();
         break;
   }
}

void IntegerTypeNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case INITIALIZER:
         code << getDefaultValue();
         break;
   }
}

void FloatTypeNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case INITIALIZER:
         code << getDefaultValue();
         break;
   }
}

void DecimalTypeNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case INITIALIZER:
         code << getDefaultValue();
         break;
   }
}

void CharTypeNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case INITIALIZER:
         code << getDefaultValue();
         break;
   }
}

void TextTypeNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case INITIALIZER:
         code << getDefaultValue();
         break;
   }
}

void EnumTypeNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case CONSTANT_DECLARATION:
         {
            code << "const string " << name << "[" << values.size() << "] =\n{\n";
            bool prev = false;
            for (ListIter it = values.begin(); it != values.end(); ++it)
            {
               if (prev)
                  code << ",\n";
               code << "  \"" << (*it)->getNameString() << "\"";
               prev = true;
            }
            code << "\n};\n";
         }
         break;

      case INITIALIZER:
         code << getDefaultValue();
         break;
   }
}

void SubscriptNode::writeExistCheck(ostream & code)
{
   if (base->kind() == SUBSCRIPT_NODE)
   {
      base->writeExistCheck(code);
      code << " && ";
   }
   base->write(code);
   code << ".count(";
   sub->write(code);
   code << ") > 0";
}

void FunctionNode::write(ostream & code)
{
   // C++ functions
   if (name)
   {
      Node def = name->getDefinition();
      if (def)
      {
         Node val = def->getValue();
         if (val->kind() == CPP_NODE)
         {
            code << name->getNameString() << '(';
            bool more = false;
            for (ListIter it = args.begin(); it != args.end(); ++it)
            {
               if (more)
                  code << ", ";
               more = true;
               (*it)->write(code);
            }
            code << ')';
         }
         return;
      }
   }

   if (desc)
   {
      string fun = getFun();
      if (fun == "file_open_read" || fun == "file_open_write")
      {
         args[0]->write(code);
         code << ".c_str()";
      }
      else if (fun == "file_read")
      {
         args[0]->write(code);
      }
      else if (fun == "file_write")
      {
         writeSourceLine(code, ep);
         TAB3;
         args[0]->write(code);
         code << " << (";
         args[1]->write(code);
         code << ");\n";
      }
      else if (fun == "file_ok")
      {
         if (bb)
         {
            // Decision code for end of block
            writeSourceLine(code, ep);
            TAB3 << "pc = ";
            args[0]->write(code);
            code << " ? " << bb->transfer << " : " << bb->altTransfer << ";\n";
         }
         else
            args[0]->write(code);
      }
      else if (fun == "file_eof")
      {
         if (bb)
         {
            // Decision code for end of block
            writeSourceLine(code, ep);
            TAB3 << "pc = (";
            args[0]->write(code);
            code << ".eof()) ? " << bb->transfer << " : " << bb->altTransfer << ";\n";
         }
         else
         {
            args[0]->write(code);
            code << ".eof()";
         }
      }
      else if (fun == "file_close")
      {
         writeSourceLine(code, ep);
         TAB3;
         args[0]->write(code);
         code << ".close();\n";
      }
      else if (fun == "exists")
      {
         if (bb)
         {
            TAB3 << "pc = (";
            args[0]->writeExistCheck(code);
            code << ") ? " << bb->transfer << " : " << bb->altTransfer << ";\n";
         }
         else
            args[0]->writeExistCheck(code);
      }
      else if (desc == funEnumText)
      {
         Node argType = args[0]->getType();
         if (argType->kind() == NAME_NODE)
            argType = argType->getDefinition();
         assert(argType->isEnumType());
         code << argType->getNameString() << '[';
         args[0]->write(code);
         code << ']';
      }
      else if (desc == funIntegerEnum)
      {
//         args[0]->showDebug("type");
//         args[1]->showDebug("expr");
         code << "check_enum_val(";
         args[1]->write(code);
         code << ", " << args[0]->getIntVal() << ")";
      }
      else if (desc->getCppName() == "")
         args[0]->write(code);
      else if (desc->getUserName() == "#" && args[0]->getType()->isArrayType())
      {
         args[0]->write(code);
         code << ".size()";
      }
      else if (desc->resultType() == BaseNode::theVoidNode)
      {
         // Procedure call

         cerr << "Generating procedure call\n";

         writeSourceLine(code, ep);
         TAB3;
         code << desc->getCppName() << '(';
         bool more = false;
         for (ListIter it = args.begin(); it != args.end(); ++it)
         {
            if (more)
               code << ", ";
            more = true;
            (*it)->write(code);
         }
         code << ");\n";
      }
      else
      {
         // Function call
         code << desc->getCppName() << '(';
         bool more = false;
         for (ListIter it = args.begin(); it != args.end(); ++it)
         {
            if (more)
               code << ", ";
            more = true;
            (*it)->write(code);
         }
         code << ')';
      }
      return;
   }

   if (name)
   {
      string fun = name->getNameString();
      if (fun == "assert")
      {
         TAB3 << "if (!(";
         args[0]->write(code);
         code << "))\n";
         TAB3 << "{\n";
         TAB4 << "cerr << \"*** Assertion failed ***\\nLine " <<
         getPos().lineNum << ": \" << ";
         if (args.size() > 1)
         {
            args[1]->write(code);
            code << " << ";
         }
         code << "endl;\n";
         TAB4 << "exit(1);\n";
         TAB3 << "}\n";
      }
      else if (fun == "#")
      {
         showDebug("fun #");
         Node argType = args[0]->getType();
         if (argType->isArrayType())
         {
            args[0]->write(code);
            code << ".size()";
         }
         else
         {
            code << desc->getCppName() << '(';
            args[0]->write(code);
            code << ')';
         }
      }
      else
         Error() << "Compiler error: function '" << fun << "'." << THROW;
   }
}

void NameNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case INITIALIZER:
         {
            Node def = getDefinition();
            if (def)
            {
               Node val = def->getValue();
               if (val)
               {
                  if (val->kind() == ENUM_TYPE_NODE || val->kind() == PROTOCOL_NODE)
                     code << '0';
                  else
                     val->writeParts(code, wm);
               }
            }
         }
         break;

      case ARRAY_INITIALIZER:
      case CELL_DECLARATION:
         code << getFullName(false);
         break;

   }
}

void NameNode::write(ostream & code)
{
   // If this name is the controlled variable of a loop, use its loop name.
   string loopName = getLoopVar();
   if (loopName != "")
   {
      code << loopName;
      return;
   }

   // Normal code for writing names.
   if (bb)
   {
      // Decision code for end of block
      writeSourceLine(code, ep);
      TAB3 << "pc = (";
      if (isReference())
         code << "*(" << getFullName(true) << ")";
      else
         code << getFullName(true);
      code << ") ? " << bb->transfer << " : " << bb->altTransfer << ";\n";
   }
   else if (isReference()  && ! isPort()) // 080629
      code << "(*(" << getFullName(true) << "))";
   else if (isPervasive())
      code << getFullName(false);
   else if (isEnumVal())
      code << definition->getVarNum();
   else
      code << getFullName(true);
}

void RemoveNode::write(ostream & code)
{
   TAB3 << "remove(readyQueue);\n";
}

void SendNode::write(ostream & code)
{
   writeSourceLine(code, ep);
   if (rhs)
   {
      if (mode == SYS_OUT)
      {
         TAB3 << "cout << ";
         rhs->write(code);
         code << ";\n";
         return;
      }
      if (mode == SYS_ERR)
      {
         TAB3 << "cerr << ";
         rhs->write(code);
         code << ";\n";
         return;
      }
   }

   string portname = port->getFullName(true);
   if (rhs)
      TAB3 << "// New code for sending a message\n";
   else
      TAB3 << "// New code for sending a signal\n";
   TAB3 << "if (" << portname << "->idle())\n";
   TAB3 << "{\n";
   TAB4 << portname << "->setData(get(readyQueue), " << fieldNum << ");\n";
   if (rhs)
   {
      string type = rhs->getCTypeString();
      if (rhs->kind() == NAME_NODE)
      {
         if ( rhs->isEnumVal() || rhs->isPervasive() || rhs->isIterator() )
         {
            // The name is not addressable: create a new node.
            TAB4 << bufferName << " = new " << type << "(";
            rhs->write(code);
            code << ");\n";
         }
         else
         {
            // The name is addressable.
            TAB4 << bufferName << " = &(";
            rhs->write(code);
            code << ");\n";
         }
      }
      else if (rhs->kind() == SUBSCRIPT_NODE || rhs->kind() == THREAD_PARAM_NODE)
      {
         // The name is addressable.
         TAB4 << bufferName << " = &(";
         rhs->write(code);
         code << ");\n";
      }
      else
      {
         if (rhs->getFun() == "float" && type == "double")
         {
            TAB4 << bufferName << " = new double(";
            rhs->write(code);
            code << ");\n";
         }
         else
         {
            TAB4 << bufferName << " = new " << type << "(";
            rhs->write(code);
            code << ");\n";
         }
      }
   }
   TAB3 << "}\n";
   TAB3 << "else if (" << portname << "->check(" << fieldNum << "))\n";
   TAB3 << "{\n";
   if (rhs)
   {
      TAB4 << "*(" << portname << "->getOther()->" << bufferName << ") = ";
      rhs->write(code);
      code << ";\n";
   }
   TAB4 << portname << "->resume();\n";
   TAB3 << "}\n";
   TAB3 << "else\n";
   TAB4 << "throw \"Bad field number in sender\\n\";\n";

//   string rdrs = portname + "->readers[" + str(fieldNum) + "]";
//   string wrts = portname + "->writers[" + str(fieldNum) + "]";
//
//   TAB3 << "if (" << rdrs << ") // Send Node\n";
//   TAB3 << "{\n";
//   TAB4 << "Process *pReader = get(" << rdrs << ");\n";
//   if (rhs)
//   {
//      TAB4 << "*(pReader->" << bufferName << ") = ";
//      rhs->write(code);
//      code << ";\n";
//   }
//   TAB4 << "put(readyQueue, pReader); // signal\n";
//   TAB3 << "}\n";
//   TAB3 << "else\n";
//   TAB3 << "{\n";
//   if (rhs)
//   {
//      string type = rhs->getCTypeString();
//      if (rhs->kind() == NAME_NODE)
//      {
//         if ( rhs->isEnumVal() || rhs->isPervasive() || rhs->isIterator() )
//         {
//            // The name is not addressable: create a new node.
//            TAB4 << bufferName << " = new " << type << "(";
//            rhs->write(code);
//            code << ");\n";
//         }
//         else
//         {
//            // The name is addressable.
//            TAB4 << bufferName << " = &(";
//            rhs->write(code);
//            code << ");\n";
//         }
//      }
//      else if (rhs->kind() == SUBSCRIPT_NODE || rhs->kind() == THREAD_PARAM_NODE)
//      {
//         // The name is addressable.
//         TAB4 << bufferName << " = &(";
//         rhs->write(code);
//         code << ");\n";
//      }
//      else
//      {
//         if (rhs->getFun() == "float" && type == "double")
//         {
//            TAB4 << bufferName << " = new double(";
//            rhs->write(code);
//            code << ");\n";
//         }
//         else
//         {
//            TAB4 << bufferName << " = new " << type << "(";
//            rhs->write(code);
//            code << ");\n";
//         }
//      }
//   }
//   TAB4 << "put(" << wrts << ", get(readyQueue)); // wait\n";
//   TAB3 << "}\n";
}

void SendOptionNode::write(ostream & code)
{
   string portname = port->getFullName(true);

   TAB3 << "// New code for send option\n";
   TAB3 << "if (!" << portname << "->idle() && " << portname << "->check(" << fieldNum << "))\n";
   if (rhs)
   {
      TAB4 << "*(" << portname << "->getOther()->" << bufferName << ") = ";
      rhs->write(code);
      code << ";\n";
   }
   TAB3 << portname << "->resume();\n";

//   string rdrs = portname + "->readers[" + str(fieldNum) + "]";
//
//   writeSourceLine(code, ep);
//   TAB3 << "Process *pReader = get(" << rdrs << "); // SendOptionNode\n";
//   if (rhs)
//   {
//      TAB3 << "*(pReader->" << bufferName << ") = ";
//      rhs->write(code);
//      code << ";\n";
//   }
//   TAB3 << "put(readyQueue, pReader); // signal\n";
}

void ReceiveNode::write(ostream & code)
{
   writeSourceLine(code, ep);
   if (mode == SYS_IN)
   {
      TAB3 << "getline(cin, ";
      lhs->write(code);
      code << ");\n";
      return;
   }

   string portname = port->getFullName(true);

   if (signal)
      TAB3 << "// New code for receiving a signal\n";
   else
      TAB3 << "// New code for receiving a message\n";
   TAB3 << "if (" << portname << "->idle())\n";
   TAB3 << "{\n";
   TAB4 << portname << "->setData(get(readyQueue), " << fieldNum << ");\n";
   if (!signal)
   {
      TAB4 << bufferName << " = &(";
      lhs->write(code);
      code << ");\n";
   }
   TAB3 << "}\n";
   TAB3 << "else if (" << portname << "->check(" << fieldNum << "))\n";
   TAB3 << "{\n";
   if (!signal)
   {
      TAB4;
      lhs->write(code);
      code << " = *(" << portname << "->getOther()->" << bufferName << ");\n";
   }
   TAB4 << portname << "->resume();\n";
   TAB3 << "}\n";
   TAB3 << "else\n";
   TAB4 << "throw \"Bad field number in receiver\\n\";\n";

//   string rdrs = portname + "->readers[" + str(fieldNum) + "]";
//   string wrts = portname + "->writers[" + str(fieldNum) + "]";
//
//   TAB3 << "if (" << wrts << ")\n";
//   TAB3 << "{\n";
//   TAB4 << "Process *pWriter = get(" << wrts << "); // ReceiveNode\n";
//   if (!signal)
//   {
//      TAB4;
//      lhs->write(code);
//      code << " = *(pWriter->" << bufferName << ");\n";
//   }
//   TAB4 << "put(readyQueue, pWriter); // signal\n";
//   TAB3 << "}\n";
//   TAB3 << "else\n";
//   TAB3 << "{\n";
//   if (!signal)
//   {
//      TAB4 << bufferName << " = &(";
//      lhs->write(code);
//      code << ");\n";
//   }
//   TAB4 << "put(" << rdrs << ", get(readyQueue)); // wait\n";
//   TAB3 << "}\n";
}

void ReceiveOptionNode::write(ostream & code)
{
   string portname = port->getFullName(true);

   TAB3 << "// New communication code\n";
   if (!signal)
   {
      TAB3;
      lhs->write(code);
      code << " = *(" << portname << "->getOther()->" << bufferName << ");\n";
   }
   TAB3 << portname << "->resume();\n";

//   string wrts = portname + "->writers[" + str(fieldNum) + "]";
//   writeSourceLine(code, ep);
//   TAB3 << "Process *pWriter = get(" << wrts << "); // ReceiveOptionNode\n";
//   if (!signal)
//   {
//      TAB3;
//      lhs->write(code);
//      code << " = *(pWriter->" << bufferName << ");\n";
//   }
//   TAB3 << "put(readyQueue, pWriter); // signal\n";
}

void ThreadNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case CLOSURE_DECLARATION:
         {
            code << "struct " << name << " : public Process\n";
            code << "{\n";
            TAB1 << name << "(Channel *" << port->getFullName() << ") : " <<
            port->getFullName() << "(" << port->getFullName() << ")\n";
            TAB1 << "{\n";
            TAB2 << "pc = " << start << ";\n";
            seq->writeParts(code, INITIALIZER);
            for (ListIter it = inputs.begin(); it != inputs.end(); ++it)
               (*it)->writeParts(code, INITIALIZER);
            for (ListIter it = outputs.begin(); it != outputs.end(); ++it)
               (*it)->writeParts(code, INITIALIZER);
            TAB1 << "}\n";

            TAB1 << "// Actions\n";
            TAB1 << "void do_actions();\n";

            TAB1 << "// Data members\n";
            TAB1 << "Channel *" << port->getFullName() << ";\n";
            for (ListIter it = inputs.begin(); it != inputs.end(); ++it)
               (*it)->writeParts(code, wm);
            for (ListIter it = outputs.begin(); it != outputs.end(); ++it)
               (*it)->writeParts(code, wm);
            seq->writeParts(code, wm);
            code << "};\n\n";
            break;
         }

      case ACTION_BODIES:
         code << "void " << name << "::do_actions()\n";
         code << "{\n";
         TAB1 << "while (true)\n";
         TAB1 << "{\n";
         TAB2 << "switch (pc)\n";
         TAB2 << "{\n";
         for (BlockIter it = blocks.begin(); it != blocks.end(); ++it)
            code << *it;
         TAB2 << "}\n";
         TAB1 << "}\n";
         code << "}\n\n";
         break;
   }
}

void ThreadNode::write(ostream & code)
{
}

void ThreadParamNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case INITIALIZER:
         TAB2 << name->getFullName(false) << " = ";
         type->writeParts(code, wm);
         code << "; // Parameter initialization\n";
         break;

      case CLOSURE_DECLARATION:
         TAB1 << type->getCTypeString() << ' ' << name->getFullName(false) <<
         "; // Parameter\n";
         break;
   }
}

void ThreadParamNode::write(ostream & code)
{
   code << name->getFullName();
}

void StartNode::writeParts(ostream & code, WriteMode wm)
{
   for (ListIter it = calls.begin(); it != calls.end(); ++it)
      (*it)->writeParts(code, wm);
   seq->writeParts(code, wm);
}

void StartNode::write(ostream & code)
{
}

void ThreadCallNode::writeParts(ostream & code, WriteMode wm)
{
   switch (wm)
   {
      case INITIALIZER:
         break;

      case CLOSURE_DECLARATION:
         TAB1 << "Channel *" << chName->getFullName(false) <<
         "; // Channel for thread call\n";
         break;
   }
}

void ThreadCallNode::write(ostream & code)
{
}

void ThreadStartNode::write(ostream & code)
{
   TAB3 << chName->getFullName() << " = new Channel();\n";
   TAB3 << "put(readyQueue, new " << name->getNameString() << "(" << chName->getFullName() << "));\n";
}

void ThreadStopNode::write(ostream & code)
{
   TAB3 << "delete " << chName->getFullName() << ";\n";
}

void QueryNode::write(ostream & code)
{
   string portname = port->getFullName(true);
   switch (phase)
   {
      case 1:
         TAB3 << "if (" << portname << "->idle())  // Query 1\n";
         TAB4 << portname << "->setQuery(get(readyQueue));\n";
         break;

      case 2:
         TAB3 << name->getFullName(false) << " = " << portname << "->check(" << getFieldNum() << ");  // Query 2\n";
         break;
   }
}


