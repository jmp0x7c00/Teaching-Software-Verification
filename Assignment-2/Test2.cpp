//===- Software-Verification-Teaching Assignment 2-------------------------------------//
//
//     SVF: Static Value-Flow Analysis Framework for Source Code
//
// Copyright (C) <2013->
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===-----------------------------------------------------------------------===//

/*
 // Software-Verification-Teaching Assignment 2 : ICFG graph traversal (Control-flow reachability analysis)
 //
 // 
 */

#include "Assignment-2.h"
#include "WPA/Andersen.h"

using namespace SVF;
using namespace SVFUtil;


int main(int argc, char **argv)
{
    int arg_num = 0;
    int extraArgc = 1;
    char **arg_value = new char *[argc + extraArgc];
    std::vector<std::string> moduleNameVec;

    SVFUtil::processArguments(argc, argv, arg_num, arg_value, moduleNameVec);
    // add extra options
    int orgArgNum = arg_num;
    arg_value[arg_num++] = (char*) "-stat=false";
    assert(arg_num == (orgArgNum + extraArgc) && "more extra arguments? Change the value of extraArgc");

    llvm::cl::ParseCommandLineOptions(arg_num, arg_value,
                                "Whole Program Points-to Analysis\n");

    SVFModule *svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
    svfModule->buildSymbolTableInfo();
    LLVMModuleSet::getLLVMModuleSet()->dumpModulesToFile(".svf");

    SVFIRBuilder builder;
    SVFIR *svfir = builder.build(svfModule);

    PTACallGraph* callgraph = AndersenWaveDiff::createAndersenWaveDiff(svfir)->getPTACallGraph();
    builder.updateCallGraph(callgraph);

    /// ICFG
    ICFG *icfg = svfir->getICFG();
    icfg->updateCallGraph(callgraph);
    icfg->dump("icfg");

    ICFGTraversal *traversal = new ICFGTraversal(svfir, icfg);
    traversal->analyse();
    
    SVF::LLVMModuleSet::releaseLLVMModuleSet();
    SVF::SVFIR::releaseSVFIR();

    delete traversal;
    return 0;
}