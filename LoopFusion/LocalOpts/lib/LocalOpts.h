#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>


#include "llvm/Analysis/LoopInfo.h"
#include <llvm/IR/Dominators.h>
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/PostDominators.h"


class LoopFusionPass final : public llvm::PassInfoMixin<LoopFusionPass> 
{
	public:
  		llvm::PreservedAnalyses run([[maybe_unused]] llvm::Function &, llvm::FunctionAnalysisManager &);
	private:
		bool areLoopsAdjacent(llvm::BasicBlock * exitBlockL1, llvm::BasicBlock * preheaderL2, llvm::BasicBlock * headerL2);
		bool sameTripCount(llvm::Loop * L1, llvm::Loop * L2, llvm::ScalarEvolution &SE);
		bool sameBounds(llvm::Loop * L1, llvm::Loop * L2, llvm::ScalarEvolution &SE);
		bool areLoopsControlFlowEquivalent(llvm::Loop * L1, llvm::Loop * L2, llvm::DominatorTree * DT, llvm::PostDominatorTree *PDT);
};

