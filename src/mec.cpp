/** \file mec.cpp */

/** \todo 081129
 * check.cpp has been changed so that the type of a &= e is the type of a.
 * parser.cpp has a mod at line 592 to parse a chain of &=
 *            This doesn't work: perhaps AST has wrong structure
 */

/** \todo 090226
 *  Parser reads array dimensions as integers and stores them directly
 *  in the type node.  It should read expression(s) and let the checker
 *  ensure that they are literals or constants.
 */

/** \mainpage Datinguino Tests
 * - 090701
 *   -# Added rem operator, equivalent to mod and %
 *   -# Fixed test for file status
 */

/** \todo 090715
 * Byte to Unsigned Integer gives C++ errors.
 * Unsigned Byte to Unsigned Integer gives C++ errors (this is more surprising!)
 */

/** \todo 090715
 * decimal: Text -> Decimal is not working.
 */

/** \todo 090731
 *  Compiler gives misleading diagnostics if there are actions in a cell.
 */

/** \todo 090718
 * Assert is not working.
 * 1. The compiler generates assret_1 or assert_2, but generates code
 *    for a function, not a procedure.  (Put a 'proc' flag in class FuncDef?)
 * 2. prelude.cpp does not implement assert_1 or assert_2.
 * 090718 Fixed
 */

/** \todo 090715
 * No communication except sys.out in (loop)select branch does not give
 * a MEC error, but 'sys' is undefined in the C++ file.
 * 090718 Fixed
 */

#include "ast.h"
#include "basicblocks.h"
#include "error.h"
#include "functions.h"
#include "parser.h"
#include "scanner.h"
#include "types.h"
#include "utilities.h"

//#include "genassem.h" // Lightning

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <list>
#include <map>
#include <locale>

#include <glibmm.h>
//#include <sys/io.h> // patch (not needed)

using namespace std;

/** Expected version of prelude.cpp.
 *  This is a digit string that should be a substring of the first line
 *  of the prelude.  If the compiler is modified in a way that requires
 *  changes to the prelude, then both this value and prelude.cpp should
 *  be changed.
 */
const Glib::ustring PRELUDE_VERSION = "43";

/** Compiler option  "A": Write AST to a file. */
bool drawAST = false;

/** Compiler option  "B": Write basic blocks to log file. */
bool showBasicBlocks = false;

/** Compiler option  "F": Write function descriptors to log file. */
bool showFuncs = false;

/** Compiler option  "LP": write log files after parsing. */
bool logParse = false;

/** Compiler option  "LE": write log files after extracting. */
bool logExtract = false;

/** Compiler option  "LB": write log files after binding. */
bool logBind = false;

/** Compiler option  "LC": write log files after checking. */
bool logCheck = false;

/** Compiler option  "LG": write log files after code generation. */
bool logGen = false;

/** Compiler option "R": compile and run.
 *  If this is enabled, no C++ code is generated.
 */
bool comRun = true;

/** Compiler option  "T": Trace execution */
bool tracing = false;

/** Compiler option  "T": Number of cycles to trace. */
int maxCycles = 100;

/** Compiler option  "O": output file nane. */
Glib::ustring outfilename = "";

/** Compiler option  "W": show warning messages. */
bool showWarnings = false;

/** Default path to 'prelude.cpp'. */
Glib::ustring preludeFileName = "prelude.cpp";

/** Names of C++ files to be included in output. */
vector<Glib::ustring> cppfilenames;

/** Extract Erasmus source from a Latex file. */
void extract(istream & is, ostream & os)
{
    enum
    {
        SKIPPING, COPYING
    }
    mode = SKIPPING;
    string line;
    while (getline(is, line))
    {
        switch (mode)
        {
            case SKIPPING:
                if (line.find("\\begin{code}") == 0)
                    mode = COPYING;
                break;

            case COPYING:
                if (line.find("\\end{code}") == 0)
                    mode = SKIPPING;
                else
                    os << line << endl;
                break;
        }
    }
}


/** Read the prelude from "// *X" to // *Y".
 * The file \a prelude.cpp contains C++ code that is used by all
 * generated programs.  It is divided into "paragraphs" by comments
 * of the form "// *X", where "X" is a letter.
 * \param is is the stream to read from.
 * \param os is the stream to copy to.
 * \param start indicates where to start copying: it immediately follows "// *".
 */
void copyprelude(istream & is, ostream & os, Glib::ustring start)
{
    is.seekg(0);
    bool copying = false;
    string line;
    while (getline(is, line))
    {
        if (copying && line.find("//*", 0) != Glib::ustring::npos)
            break;
        if (line.find("//*" + start, 0) != Glib::ustring::npos)
        {
            copying = true;
            continue;
        }
        if (copying)
            os << line << endl;
    }
}

// Forward reference
void readFiles(Glib::ustring root, int & tfnum, vector<Glib::ustring> & filenames, ostream & log);

/** Check a file for import commands. */
void checkFile(Glib::ustring efn, int & tfnum, vector<Glib::ustring> & filenames, ostream & log)
{
    ifstream efs(efn.c_str());
    string buffer;
    while (getline(efs, buffer))
    {
        Glib::ustring ubuffer(buffer);
        Glib::ustring::size_type p1 = ubuffer.find_first_not_of(" \t");
        Glib::ustring::size_type p2 = ubuffer.find("import", p1);
        if (p2 == p1 && p2 != Glib::ustring::npos)
        {
            Glib::ustring::size_type pos = p1 + 7;
            int state = 1;
            bool scanning = true;
            Glib::ustring fn;
            while (scanning && pos < ubuffer.size())
            {
                char c = ubuffer[pos];
                switch (state)
                {
                    case 1:
                        if (c == ' ')
                            ++pos;
                        else
                        {
                            fn = "";
                            state = 2;
                        }
                        break;
                    case 2:
                        if (c == ' ' || c == ',' || c == ';')
                        {
                            if (fn.size() > 0)
                            {
                                readFiles(fn, tfnum, filenames, log);
                                fn = "";
                            }
                            state = 3;
                        }
                        else
                        {
                            fn += c;
                            ++pos;
                        }
                        break;
                    case 3:
                        if (c == ' ')
                            ++pos;
                        else if (c == ',')
                        {
                            ++pos;
                            state = 1;
                        }
                        else if (c == ';')
                            scanning = false;
                        break;
                }
            }
            if (fn.size() > 0)
                readFiles(fn, tfnum, filenames, log);
        }
    }
    efs.clear();
    efs.seekg(0);
    log << endl << "Input file '" << efn << "':\n";
    int line = 0;
    while (getline(efs, buffer))
        log << setw(4) << ++line << ' ' << buffer << endl;
    efs.close();
    filenames.push_back(efn);
}

/** Read a source file, recursively read imports. */
void readFiles(Glib::ustring root, int & tfnum, vector<Glib::ustring> & filenames, ostream & log)
{
    Glib::ustring efn = root + ".e";
    ifstream efs(efn.c_str());
    if (efs)
    {
        efs.close();
        checkFile(efn, tfnum, filenames, log);
    }
    else // No .e file, try .tex
    {
        Glib::ustring tfn = root + ".tex";
        ifstream tfs(tfn.c_str());
        if (tfs)
        {
            ostringstream oss;
            oss << "temp_" << ++tfnum << ".e";
            Glib::ustring tempfn = oss.str();
            ofstream ofs(tempfn.c_str());
            cout << "Extracting '" << tfn << "' -> '" << tempfn << "'\n";
            extract(tfs, ofs);
            tfs.close();
            ofs.close();
            checkFile(tempfn, tfnum, filenames, log);
        }
        else
            Error() << "Failed to open either '" << efn << "' or '" << tfn << "'\n" << THROW;
    }
}

/** Process one command-line argument.
 *  By default, the argument is a file name without extension.
 * If its first character is '+' or '-', it is processed
 * as a compiler option.
 * \param clArg is the command-line argument.
 * \return \a true if no errors, \a false if errors.
 */
bool compile(Glib::ustring clArg)
{

    // Process compiler options
    if (clArg[0] == '+' || clArg[0] == '-')
    {
        if (clArg.size() < 2)
        {
            cerr << "Unknown option '" << clArg << "'.\n";
            return false;
        }
        switch (clArg[1])
        {

            // Write AST to output file
            case 'a':
            case 'A':
                drawAST = clArg[0] == '+';
                break;

                // Write basic blocks to logfile
            case 'b':
            case 'B':
                showBasicBlocks = clArg[0] == '+';
                break;

            case 'c':
            case 'C':
                if (clArg[2] == '"')
                    cppfilenames.push_back(clArg.substr(3, clArg.size() - 4));
                else
                    cppfilenames.push_back(clArg.substr(2));
                break;

                // Write function descriptors to log file.
            case 'f':
            case 'F':
                showFuncs =  clArg[0] == '+';
                break;

                // Logfile switch
            case 'l':
            case 'L':
                if (clArg[0] == '+')
                {
                    for (int i = 2; i < clArg.size(); ++i)
                    {
                        switch (clArg[i])
                        {
                            case 'p':
                            case 'P':
                                logParse = true;
                                break;

                            case 'e':
                            case 'E':
                                logExtract = true;
                                break;

                            case 'b':
                            case 'B':
                                logBind = true;
                                break;

                            case 'c':
                            case 'C':
                                logCheck = true;
                                break;

                            case 'g':
                            case 'G':
                                logGen = true;
                                break;
                        }
                    }
                }
                break;

                // Output file name
            case 'o':
            case 'O':
                if (clArg[2] == '"')
                    outfilename = clArg.substr(3, clArg.size() - 4);
                else
                    outfilename = clArg.substr(2);
                break;

                // Prelude file name
            case 'p':
            case 'P':
                if (clArg[0] == '+')
                    preludeFileName = clArg.substr(2) + "\\prelude.cpp";
                break;

                // Compile and run
            case 'r':
            case 'R':
                comRun = (clArg[0]=='+');
                break;

                // Tracing switch
            case 't':
            case 'T':
                if (clArg[0] == '+')
                {
                    tracing = true;
                    if (clArg.size() > 2)
                    {
                        maxCycles = 0;
                        for (size_t i = 2; i < clArg.size(); ++i)
                        {
                            char c = clArg[i];
                            if (isdigit(c))
                                maxCycles = 10 * maxCycles + c - '0';
                            else
                            {
                                cerr << "Unknown option '" << clArg << "'.\n";
                                return false;
                            }
                        }
                    }
                }
                else
                    tracing = false;
                break;

                // Show warning messages for incompatible protocols.
            case 'w':
            case 'W':
                showWarnings = clArg[0] == '+';
                break;

            default:
                cerr << "Unknown option '" << clArg << "'.\n";
                return false;
        }

        if (showFuncs)
        {
            showFuncDefs("coercions.tex", "functions.tex", funcDefs);
            cerr << "Function tables written.\n";
        }
        return true;
    }


    // The scanner must be kept in scope, because it contains pointers
    // to the source text that are used for error reporting.
    // The scanner appends tokens to the eponymous list.
    Scanner sc;

    try
    {
        // 'root' is the input file name without extension
        Glib::ustring root;
        if (clArg.rfind(".e") != Glib::ustring::npos)
            root = clArg.substr(0, clArg.size() - 2);
        else if (clArg.rfind(".tex") != Glib::ustring::npos)
            root = clArg.substr(0, clArg.size() - 4);
        else
            root = clArg;

        cerr << "Root = " << root << endl;

        Glib::ustring codefilename   = root + ".cpp";
        if (outfilename != "")
            codefilename = outfilename;
        Glib::ustring logfilename    = root + ".log";
        ofstream log(logfilename.c_str());
        log << "MEC " << today() << endl;

        // Build a list of files to scan.
        vector<Glib::ustring> filenames;
        //      filenames.push_back(sysProtName);

        int tfnum = 0;
        readFiles(root, tfnum, filenames, log);

        list<Token> tokens;
        // Phase 1: scan source files
        for (vector<Glib::ustring>::const_iterator it = filenames.begin();
             it != filenames.end();
             ++it)
        {
            cout << "Compiling   '" << *it << "'\n";
            sc.scanFile(*it, tokens);
        }
        tokens.push_back(Token(Errpos(), END_OF_FILE, "EOF"));

        // Construct AST nodes for standard input and output.
        List fds;
        fds.push_back(new DecNode(Errpos(),
                                  new NameNode(Errpos(), BASIO_INP, 0, true),
                                  BaseNode::theTextNode, 0, SERVER, QUERY,
                                  false, false, false, false));

        fds.push_back(new DecNode(Errpos(),
                                  new NameNode(Errpos(), BASIO_OUT, 0, true),
                                  BaseNode::theTextNode, 0, SERVER, REPLY,
                                  false, false, false, false));

        fds.push_back(new DecNode(Errpos(),
                                  new NameNode(Errpos(), BASIO_ERR, 0, true),
                                  BaseNode::theTextNode, 0, SERVER, REPLY,
                                  false, false, false, false));

        Node bar = new ListopNode(Errpos(), LISTOP_BAR, fds);
        Node star = new UnopNode(Errpos(), UNOP_MANY, bar);
        Node sysProtNode = new ProtocolNode(Errpos(), star);
        Node sysDefNode = new DefNode(Errpos(),
                                      new NameNode(Errpos(), "sysProtocol", 0, true),
                                      sysProtNode);
        Node sysPortNode = new DecNode(Errpos(),
                                       new NameNode(Errpos(), BASIO_SYS, 0, true),
                                       new NameNode(Errpos(), "sysProtocol",
                                                    sysDefNode, true), 0, SERVER, NO_FIELD,
                                       false, false, false, false);

        // Put nodes into definition chain
        sysPortNode->setDefChain(sysDefNode);
        Node defChain = sysPortNode;

        // Phase 2: build AST by parsing tokens.
        Parser pa(tokens);
        Node prog = pa.parseProgram();

        if (logParse)
            log << "\n\nAST after parsing:\n" << prog;

        if (errorCount() > 0)
            Error() << "Syntax errors." << THROW;

        // Phase 3: extract receive commands from expressions.
        int tempCounter = 0;
        prog->extract(tempCounter);

        if (logExtract)
            log << "\n\nAST after extracting:\n" << prog;

        // Display program text after modification by parser and extract.
        log << endl << "Modified source code:\n";
        prog->prettyPrint(log);

        // Phase 3: bind names to their definitions.
        prog->bind(defChain);

        if (logBind)
            log << "\n\nAST after binding:\n" << prog;

        // Phase 4: check semantics using a CheckData object to transfer
        // information around the ST.
        CheckData cd(showWarnings);
        prog->check(cd);
        if (logCheck)
            log << "\n\nAST after checking:\n" << prog;

        int numErrors = errorCount();
        if (numErrors > 0)
            cout << numErrors << " errors!  No code generated.\n";
        else
        {
            // Phase 5: decorate the AST with the information needed for
            // code generation.
            // Phases 5 and 6 could probably be combined.
            GenData gd;
            prog->gen(gd);

            if (comRun)
            {
                //AssemData aData;
                //prog->prepAssem(aData);
                //prog->genAssem();
            }
            else
            {

                // Phase 6: convert the tree-structured program (in the AST)
                // to a linear list of (not really) basic blocks.
                BlockList blocks;
                prog->genBlocks(blocks);
                if (showBasicBlocks)
                {
                    log << "\nBasic Blocks\n";
                    prog->showBB(log);
                }

                // Phase 7: interleave the generated code with pieces of the prelude.
                ifstream prelude(preludeFileName.c_str());
                if ( ! prelude)
                    Error() << "Failed to open '" << preludeFileName << "'\n" << THROW;

                string firstLine;
                getline(prelude, firstLine);
                Glib::ustring::size_type b = firstLine.find_first_of("01243456789");
                Glib::ustring::size_type e = firstLine.find_first_not_of("01243456789");
                if (b == Glib::ustring::npos && e == Glib::ustring::npos)
                    Error() << "No firstLine in prelude.cpp." << THROW;
                Glib::ustring version = firstLine.substr(b, e - b);
                if (version != PRELUDE_VERSION)
                {
                    cerr << "Incompatibility:\n";
                    cerr << "    Compiler version is    " << PRELUDE_VERSION << ".\n";
                    cerr << "    " << preludeFileName << " version is " << version << ".\n";
                    Error() << "Versions are incompatible." << THROW;
                }
                cout << "Reading '" << preludeFileName << "' version " << version << ".\n";

                // Open output file
                ofstream src(codefilename.c_str());
                copyprelude(prelude, src, "A");

                // Copy user declarations
                for (vector<Glib::ustring>::const_iterator it = cppfilenames.begin(); it != cppfilenames.end(); ++it)
                {
                    ifstream is(it->c_str());
                    if (!is)
                        Error() << "Failed to open '" << *it << "'\n" << THROW;
                    cerr << "Copying '" << *it << "'\n";
                    src << "// Function definitions from '" << *it << "'\n";
                    while (is)
                    {
                        string line;
                        getline(is, line);
                        src << line << endl;
                    }
                    is.close();
                }

                // Write declarations
                src << "// Pervasive constant declarations\n\n";
                //prog->writeParts(src, CONSTANT_DECLARATION);

                src << "\n// Process declarations\n\n";
                //prog->writeParts(src, CLOSURE_DECLARATION);

                src << "// Cell declarations\n\n";
                //prog->writeParts(src, CELL_DECLARATION);

                src << "// Action bodies\n\n";
                //prog->writeParts(src, ACTION_BODIES);

                copyprelude(prelude, src, "B");
                if (tracing)
                    src << "   int cycles = " << maxCycles << ";\n";

                // Create process classes
                //prog->writeParts(src, CREATE_PROCESSES);

                copyprelude(prelude, src, "C");
                if (tracing)
                    copyprelude(prelude, src, "E");
                copyprelude(prelude, src, "F");

                src.close();
                prelude.close();
            }
            if (logGen)
                log << "\n\nAST after code generation:\n" << prog;

            // Draw AST
            if (drawAST)
            {
                Glib::ustring astfilename = root + ".ast";
                set<int> nodeNums;
                ofstream ast(astfilename.c_str());
                prog->drawAST(ast, nodeNums, 0);
                ast << endl;
                ast.close();
                cerr << "AST written to " << astfilename << ".\n";
            }

            cout << "Done!\n";
        }
    }
    catch (const Glib::ustring & msg)
    {
        cerr << msg << "\nCompilation terminated.\n";
        return false;
    }
    return errorCount() == 0;
}

int main(int argc, char *argv[])
{

//    locale user_locale = locale("");
//    locale::global(user_locale);
    Glib::init();

    cerr << "MEC (" << today() << ").\n\n";

    buildFuncDefs(funcDefs);

    if (argc <= 1)
    {
        cerr <<
            "Usage:\n"
            "   mec <options> <file name>\n\n"
            "Option = ( '+' | '-' ) <letter> <parameter>\n\n"
            "   '+' enables the option, '-' disables it.\n\n"
            "   Letter  Effect\n"
            "   -------------------------------------------------------------\n"
            "      A    Write AST to .ast file\n"
            "      B    Write basic blocks to log file\n"
            "      Cf   Read C++ function definitions from file 'f'\n"
            "      F    Display built-in functions\n"
            "      LP   Write AST to log file after parsing\n"
            "      LE   Write AST to log file after extracting\n"
            "      LB   Write AST to log file after binding\n"
            "      LC   Write AST to log file after checking\n"
            "      LG   Write AST to log file after generating code\n"
            "      Of   Write C++ code to file 'f'\n"
            "      P<path>  Read 'prelude.cpp' from the given path\n"
            "      R    Compile and run (suppresses .cpp output)\n"
            "      T    Trace execution until program terminates\n"
            "      Tn   Trace execution for n context switches\n"
            "      W    Show warnings about incompatible protocols\n"
            "   -------------------------------------------------------------\n"
            " Default settings: ";
        cerr << (drawAST         ? "+A"   : "-A")  << ' ';
        cerr << (showBasicBlocks ? "+B"   : "-B")  << ' ';
        cerr << (showFuncs       ? "+F"   : "-F")  << ' ';
        cerr << (logParse        ? "+LP" : "-LP")  << ' ';
        cerr << (logExtract      ? "+LE" : "-LE")  << ' ';
        cerr << (logBind         ? "+LB" : "-LB")  << ' ';
        cerr << (logCheck        ? "+LC" : "-LC")  << ' ';
        cerr << (logGen          ? "+LG" : "-LG")  << ' ';
        cerr << "+P" << preludeFileName            << ' ';
        cerr << (comRun          ? "+R"   : "-R")  << ' ';
        cerr << (tracing         ? "+T"   : "-T")  << ' ';
        cerr << (showWarnings    ? "+W"   : "-W")  << ' ';
        cerr << endl;
        return 0;
    }

    bool success;
    for (int a = 1; a != argc; ++a)
    {
        success = compile(argv[a]);
        if (!success)
            return 1;
    }
    return 0;
}


