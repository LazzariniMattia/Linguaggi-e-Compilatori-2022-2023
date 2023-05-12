#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Analysis/LoopInfo.h"
#include <llvm/IR/Dominators.h>
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/PostDominators.h"

using namespace llvm;

// -----------------------------------------------------------------------------------
// Per generare il file .ll da Loop.c
//  clang -O0 -Xclang -disable-O0-optnone -emit-llvm -c Loop.c
//  opt -passes=mem2reg Loop.bc -o Loop.opt.bc
//  llvm-dis Loop.opt.bc -o Loop.opt.ll

// mem2reg - The mem2reg pass converts non-SSA form of LLVM IR into SSA form,
//  raising loads and stores to stack-allocated values to “registers” (SSA values).
// -----------------------------------------------------------------------------------

// opt -load-pass-plugin=./libLocalOpts.so -passes=loopfusion test/Loop.opt.ll -o test/test.loopfusion.optimized.bc
// llvm-dis test/test.loopfusion.optimized.bc -o test/test.loopfusion.optimized.ll
// -----------------------------------------------------------------------------------

// Ritorna true se i due loop analizzati sono adiacenti.
//  Due Loop sono adiacenti solo quando l'exit block del primo loop corrisponde al preheader del secondo loop
//   e l'exit block del primo Loop contiene solo un'istruzione di branch che porta all'header del secondo loop
bool LoopFusionPass::areLoopsAdjacent(BasicBlock * exitBlockL1, BasicBlock * preheaderL2, BasicBlock * headerL2)
{
	// Se l'exit Block del Loop1 coincide con il preheader del Loop2
	if (exitBlockL1 == preheaderL2)
	{
		// Se la PRIMA istruzione dell'exit Block del Loop1 è una branch, e se la branch porta al Loop2
		if (((*exitBlockL1).front()).getOpcode() == Instruction::Br 
			&& ((*exitBlockL1).front()).getOperand(0) == headerL2)
		{
			outs()<<"La prima istruzione dell'exit block del loop1 è una branch che porta al Loop2\n";
			return true;
		}
	}
	outs()<<"I due loop NON sono adiacenti\n";
	return false;
}

// Ritorna true se i trip count dei due loop sono uguali, false se sono diversi o non calcolabili
bool LoopFusionPass::sameTripCount(Loop * L1, Loop * L2, ScalarEvolution &SE)
{
	const SCEV* tripCountL1 = SE.getBackedgeTakenCount(L1);
	const SCEV* tripCountL2 = SE.getBackedgeTakenCount(L2);

	// Controlla che il trip count tra i due loop sia calcolabile e che sia uguale
	if ((!isa<SCEVCouldNotCompute>(tripCountL1)) && (!isa<SCEVCouldNotCompute>(tripCountL2)) && (tripCountL1 == tripCountL2))
		return true;

	return false;
}

// Ritorna true se i bound dei due loop sono uguali, false altrimenti
bool LoopFusionPass::sameBounds(Loop * L1, Loop * L2, ScalarEvolution &SE)
{
	Optional<Loop::LoopBounds> boundsL1 = L1->getBounds(SE);

	//TODO: Da rivedere

	//outs()<<"VALORE INIZIALE: "<<boundsL1.hasValue();
	//ConstantInt *initialValue = dyn_cast<ConstantInt>(&boundsL1->getInitialIVValue());
	//outs()<<*initialValue<<"\n";

	//if(ConstantInt *initialValue = dyn_cast<ConstantInt>(&boundsL1->getInitialIVValue()))
		//outs()<<"OK\n";
	//else
		//outs()<<"NO\n";

	//if (initialValue->isZero())
		//outs()<<"ZERO\n";
	//outs()<<"SAME BOUNDS: "<<(*bounds).getInitialIVValue();

	//outs()<<"SAME BOUNDS: "<<(*(*L1).LoopBounds).getInitialIVValue();
	//ConstantInt *InitialIVValue = dyn_cast<ConstantInt>(&bounds->getInitialIVValue());
	//outs()<<"SAME BOUNDS: "<<InitialIVValue;
	return true;
}

// Due Loop sono Control-Flow Equivalent se l'esecuzione di uno assicura l'esecuzione dell'altro.
//  Per verificare che due loop siano Control-Flow Equivalent si usa l'analisi dei dominatori e
//   dei post-dominatori:
//    Se il Loop j domina il Loop k, e il Loop k post-domina il loop j
//     ---> i due Loop sono Control-Flow Equivalent

// Dominator: un nodo x domina un nodo y se ogni percorso dall'entry block a y contiene x
// Post-dominator: un nodo y post-domina un nodo x se ogni percorso da x all'uscita contiene y
// ----------------------------------------------------------------------------------------------
// Ritorna true se i loop sono Control-Flow Equicalent, false altrimenti
bool LoopFusionPass::areLoopsControlFlowEquivalent(Loop * L1, Loop * L2, DominatorTree * DT, PostDominatorTree *PDT)
{
	// Se IterLoop1 domina IterLoop2 e se IterLoop2 post-domina IterLoop1 sono Control-Flow Equivalent
	if ((*DT).dominates((*L1).getLoopPreheader(), (*L2).getLoopPreheader()) 
		&& (*PDT).dominates((*L2).getLoopPreheader(), (*L1).getLoopPreheader())) 
		return true;

	return false;
}


llvm::PreservedAnalyses LoopFusionPass::run([[maybe_unused]] Function &F, FunctionAnalysisManager &FAM) 
{
	auto &LI = FAM.getResult<LoopAnalysis>(F);
	ScalarEvolution &SE = FAM.getResult<ScalarEvolutionAnalysis>(F);
	DominatorTree * DT = &FAM.getResult<DominatorTreeAnalysis>(F);
	PostDominatorTree *PDT = &FAM.getResult<PostDominatorTreeAnalysis>(F);

	SmallVector<Loop *> loops = LI.getLoopsInPreorder(); // SmallVector contenente tutti i loop
	
	// Itera su tutti i loop
	for (auto &IterLoop1 : loops)
	{
		for (auto &IterLoop2 : loops)
		{
			if (IterLoop1 != IterLoop2)
			{
				BasicBlock * exitBlockL1 = (*IterLoop1).getExitBlock();
				BasicBlock * preheaderL2 = (*IterLoop2).getLoopPreheader();
				BasicBlock * headerL2 = (*IterLoop2).getHeader();
		
				if (areLoopsAdjacent(exitBlockL1, preheaderL2, headerL2))
				{
					outs()<<"I Loop:\n"<<*IterLoop1<<*IterLoop2<<" sono adiacenti\n----------------------\n";
					
					if (sameTripCount(IterLoop1, IterLoop2, SE) && sameBounds(IterLoop1, IterLoop2, SE))
					{
						outs()<<"I Loop:\n"<<*IterLoop1<<*IterLoop2<<" hanno lo stesso trip count\n----------------------\n";

						// TODO : CONTROLLARE CHE I BOUND DEI DUE LOOP SIANO UGUALI (MAGARI ANCHE LE VARIABILI D'INDUZIONE?)

						// Se i due loop sono Control-Flow Equivalent
						if (areLoopsControlFlowEquivalent(IterLoop1, IterLoop2, DT, PDT))
						{
							outs()<<"Ho trovato due loop Control-Flow Equivalent:\n";
							outs()<<"Il Loop: "<<*IterLoop1<<" domina ---> "<<*IterLoop2<<"\n";
							outs()<<"Il Loop: "<<*IterLoop2<<" post-domina ---> "<<*IterLoop1<<"----------------------\n";
						}		
					}
				}			
			}
		}
	}

	loops.clear();
  	return PreservedAnalyses::none();
}

