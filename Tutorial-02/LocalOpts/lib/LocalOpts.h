#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>

class TransformPass final : public llvm::PassInfoMixin<TransformPass> 
{
	public:
  		llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
}; 

class AlgebraicIdentityPass final : public llvm::PassInfoMixin<TransformPass> 
{
	public:
  		llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
		void addAlgebraicIdentity(llvm::Instruction * Iter);
		void subAlgebraicIdentity(llvm::Instruction * Iter);
		void mulAlgebraicIdentity(llvm::Instruction * Iter);
		void divAlgebraicIdentity(llvm::Instruction * Iter);
};

class StrengthReductionPass final : public llvm::PassInfoMixin<TransformPass> 
{
	public:
  		llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
		void mulStrengthReduction(llvm::Instruction * Iter);
		void sdivStrengthReduction(llvm::Instruction * Iter);
};

class MultiInstructionOptimizationPass final : public llvm::PassInfoMixin<TransformPass> 
{
	public:
  		llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
		void addMultiInstructionOptimization(llvm::Instruction * Iter);
		void subMultiInstructionOptimization(llvm::Instruction * Iter);
		void mulMultiInstructionOptimization(llvm::Instruction * Iter);
		void sdivMultiInstructionOptimization(llvm::Instruction * Iter);
};

