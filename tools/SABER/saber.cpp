/* SVF - Static Value-Flow Analysis Framework 
Copyright (C) 2015 Yulei Sui
Copyright (C) 2015 Jingling Xue

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
 // Saber: Software Bug Check.
 //
 // Author: Yulei Sui,
 */

#include "SABER/LeakChecker.h"
#include "SABER/FileChecker.h"
#include "SABER/DoubleFreeChecker.h"

#include <llvm/Support/CommandLine.h>	// for cl
#include <llvm/Bitcode/BitcodeWriterPass.h>  // for bitcode write
#include <llvm/PassManager.h>		// pass manager
#include <llvm/Support/Signals.h>	// singal for command line
#include <llvm/IRReader/IRReader.h>	// IR reader for bit file
#include <llvm/Support/ToolOutputFile.h> // for tool output file
#include <llvm/Support/PrettyStackTrace.h> // for pass list
#include <llvm/IR/LLVMContext.h>		// for llvm LLVMContext
#include <llvm/Support/SourceMgr.h> // for SMDiagnostic
#include <llvm/Bitcode/ReaderWriter.h>		// for createBitcodeWriterPass


using namespace llvm;

static cl::opt<std::string> InputFilename(cl::Positional,
		cl::desc("<input bitcode>"), cl::init("-"));

static cl::opt<bool> LEAKCHECKER("leak", cl::init(false),
		cl::desc("Memory Leak Detection"));

static cl::opt<bool> FILECHECKER("fileck", cl::init(false),
		cl::desc("File Open/Close Detection"));

static cl::opt<bool> DFREECHECKER("dfree", cl::init(false),
		cl::desc("Double Free Detection"));

int main(int argc, char ** argv) {

	sys::PrintStackTraceOnErrorSignal();
	llvm::PrettyStackTraceProgram X(argc, argv);

	LLVMContext &Context = getGlobalContext();

	std::string OutputFilename;

	cl::ParseCommandLineOptions(argc, argv, "Software Bug Check\n");
	sys::PrintStackTraceOnErrorSignal();

	PassRegistry &Registry = *PassRegistry::getPassRegistry();

	initializeCore(Registry);
	initializeScalarOpts(Registry);
	initializeIPO(Registry);
	initializeAnalysis(Registry);
	initializeIPA(Registry);
	initializeTransformUtils(Registry);
	initializeInstCombine(Registry);
	initializeInstrumentation(Registry);
	initializeTarget(Registry);

	PassManager Passes;

	SMDiagnostic Err;
	std::auto_ptr<Module> M1;

	M1.reset(ParseIRFile(InputFilename, Err, Context));
	if (M1.get() == 0) {
		Err.print(argv[0], errs());
		return 1;
	}

	std::unique_ptr<tool_output_file> Out;
	std::string ErrorInfo;

	StringRef str(InputFilename);
	InputFilename = str.rsplit('.').first;
	OutputFilename = InputFilename + ".saber";

	Out.reset(
			new tool_output_file(OutputFilename.c_str(), ErrorInfo,
					sys::fs::F_None));

	if (!ErrorInfo.empty()) {
		errs() << ErrorInfo << '\n';
		return 1;
	}

	if(LEAKCHECKER)
		Passes.add(new LeakChecker());
	else if(FILECHECKER)
		Passes.add(new FileChecker());
	else if(DFREECHECKER)
		Passes.add(new DoubleFreeChecker());

	Passes.add(createBitcodeWriterPass(Out->os()));

	Passes.run(*M1.get());
	Out->keep();

	return 0;

}

