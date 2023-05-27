#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

// opt-14 -load-pass-plugin=./libLocalOpts.so -passes=multiinstructionoptimization test/testMultiInstruction.ll -o test/test.multiinstructionoptimization.optimized.bc
// llvm-dis-14 test/test.multiinstructionoptimization.optimized.bc -o test/test.multiinstructionoptimization.optimized.ll


/* Come da specifica, si assume che la struttura delle istruzioni da ottimizzare sia del tipo:
    a = b + 1;      --->    a = b + 1;
    c = a - 1;              c = b;
*/

void addMultiInstructionOptimization(Instruction &Iter)
{
    outs()<<"----------------------------------------------------------------------------\nHo trovato una Add:"<<Iter<<"\n";
    ConstantInt * addConstant0 = dyn_cast<ConstantInt>(Iter.getOperand(0));
    ConstantInt * addConstant1 = dyn_cast<ConstantInt>(Iter.getOperand(1));

    if (addConstant0 && (!addConstant1)) // Se solo il primo operando della add è una costante
    {
        outs()<<"Ho trovato una costante in posizione 0 di valore: "<<(*addConstant0).getValue()<<"\n"; 
        
        // Assegna a NextInstruction il riferimento alla successiva istruzione
        auto tmpPtr = Iter.getNextNonDebugInstruction();
        auto &NextInstruction = *tmpPtr;
        
        if (NextInstruction.getOpcode() == Instruction::Sub) // Se l'istruzione successiva è una sub
        {
            outs()<<"Dopo la Add ho trovato l'istruzione sub "<<NextInstruction<<"\n";
            ConstantInt * nextIntstructionConstant0 = dyn_cast<ConstantInt>(NextInstruction.getOperand(0));
            ConstantInt * nextIntstructionConstant1 = dyn_cast<ConstantInt>(NextInstruction.getOperand(1));

            // Se solo il secondo operando della sub è una costante
            if ((!nextIntstructionConstant0) && nextIntstructionConstant1) 
            {
                outs()<<"Solo il secondo operando della sub è una costante che vale "<<(*nextIntstructionConstant1).getValue()<<"\n";
                
                // Se il valore del primo operando della add è uguale al valore del secondo operando della sub
                //  e se il primo operando della sub corrisponde all'istruzione precedente di Add
                if ((*addConstant0).getValue() == (*nextIntstructionConstant1).getValue() &&
                    (&Iter) == dyn_cast<Instruction>(NextInstruction.getOperand(0)))
                {
                    outs()<<"Il primo operando della add è uguale al secondo operando della sub e il primo operando della sub è uguale all'add precedente\n";

                    // Sostituisce tutte le occorenze del risultato dell'operazione di sub
                    //  con il secondo operando della add
                    NextInstruction.replaceAllUsesWith(Iter.getOperand(1)); 
                }
            }
        }
    
    }
    if ((!addConstant0) && addConstant1) // Se solo il secondo operando della add è una costante 
    {
        outs()<<"Ho trovato una costante in posizione 1 di valore: "<<(*addConstant1).getValue()<<"\n";
        
        // Assegna a NextInstruction il riferimento alla successiva istruzione
        auto &NextInstruction = *(Iter.getNextNonDebugInstruction());
        
        if (NextInstruction.getOpcode() == Instruction::Sub) // Se l'istruzione successiva è una sub
        {
            outs()<<"Dopo la Add ho trovato l'istruzione Sub: "<<NextInstruction<<"\n";
            ConstantInt * nextIntstructionConstant0 = dyn_cast<ConstantInt>(NextInstruction.getOperand(0));
            ConstantInt * nextIntstructionConstant1 = dyn_cast<ConstantInt>(NextInstruction.getOperand(1));

            // Se solo il secondo operando della sub è una costante
            if ((!nextIntstructionConstant0) && nextIntstructionConstant1) 
            {
                outs()<<"Solo il secondo operando della sub è una costante che vale "<<(*nextIntstructionConstant1).getValue()<<"\n";
                
                // Se il valore del secondo operando della add è uguale al valore del secondo operando della sub
                //  e se il primo operando della sub corrisponde all'istruzione precedente di Add
                if ((*addConstant1).getValue() == (*nextIntstructionConstant1).getValue() &&
                    (&Iter) == dyn_cast<Instruction>(NextInstruction.getOperand(0)))
                {
                    outs()<<"Il secondo operando della add è uguale al secondo operando della sub e il primo operando della sub è uguale all'add precedente\n";
                    
                    // Sostituisce tutte le occorenze del risultato dell'operazione
                    //  di sub con il primo operando della add
                    NextInstruction.replaceAllUsesWith(Iter.getOperand(0)); 
                }
            }
        }
    }
}

void subMultiInstructionOptimization(Instruction &Iter)
{
    outs()<<"----------------------------------------------------------------------------\nHo trovato una Sub:"<<Iter<<"\n";
    ConstantInt * subConstant0 = dyn_cast<ConstantInt>(Iter.getOperand(0));
    ConstantInt * subConstant1 = dyn_cast<ConstantInt>(Iter.getOperand(1));

    if ((!subConstant0) && subConstant1) // Se solo il secondo operando è una costante 
    {
        outs()<<"Ho trovato una costante in posizione 1 di valore "<<(*subConstant1).getValue()<<"\n";

        // Assegna a NextInstruction il riferimento alla successiva istruzione
        auto tmpPtr = Iter.getNextNonDebugInstruction();
        auto &NextInstruction = *tmpPtr;             

        if (NextInstruction.getOpcode() == Instruction::Add) // Se l'istruzione successiva è una add
        {
            outs()<<"Dopo la Sub ho trovato l'istruzione Add: "<<NextInstruction<<"\n";
            ConstantInt * nextIntstructionConstant0 = dyn_cast<ConstantInt>(NextInstruction.getOperand(0));
            ConstantInt * nextIntstructionConstant1 = dyn_cast<ConstantInt>(NextInstruction.getOperand(1));

            // Se solo il primo operando della add è una costante
            if (nextIntstructionConstant0 && (!nextIntstructionConstant1)) 
            {
                outs()<<"Solo il primo operando della add è una costante che vale "<<(*nextIntstructionConstant0).getValue()<<"\n";

                // Se il valore del secondo operando della Sub è uguale al valore del primo operando della Add
                //  e se il secondo operando della Add corrisponde all'istruzione precedente di Sub
                if ((*subConstant1).getValue() == (*nextIntstructionConstant0).getValue() &&
                    (&Iter) == dyn_cast<Instruction>(NextInstruction.getOperand(1)))
                {
                    outs()<<"Il secondo operando della sub è uguale al primo operando della add e il secondo operando della Add è uguale all'istruzione precedente\n";

                    // Sostituisce tutte le occorenze del risultato dell'operazione di add
                    // con il primo operando della sub
                    NextInstruction.replaceAllUsesWith(Iter.getOperand(0)); 
                }
            }
            // Se solo il secondo operando della add è una costante
            if ((!nextIntstructionConstant0) && nextIntstructionConstant1) 
            {
                outs()<<"Solo il secondo operando della sub è una costante che vale "<<(*nextIntstructionConstant1).getValue()<<"\n";

                // Se il valore del secondo operando della sub è uguale al valore del secondo operando della add
                //  e se il primo operando della Add corrisponde all'istruzione precedente di Sub
                if ((*subConstant1).getValue() == (*nextIntstructionConstant1).getValue() &&
                    (&Iter) == dyn_cast<Instruction>(NextInstruction.getOperand(0)))
                {
                    outs()<<"Il secondo operando della sub è uguale al secondo operando della add e il primo operando della Add è uguale all'istruzione precedente\n";

                    // Sostituisce tutte le occorenze del risultato dell'operazione di add
                    // con il primo operando della sub
                    NextInstruction.replaceAllUsesWith(Iter.getOperand(0)); 
                }
            }
        }
    }    
}
void mulMultiInstructionOptimization(Instruction &Iter)
{
    outs()<<"----------------------------------------------------------------------------\nHo trovato una Mul:"<<Iter<<"\n";
    ConstantInt * mulConstant0 = dyn_cast<ConstantInt>(Iter.getOperand(0));
    ConstantInt * mulConstant1 = dyn_cast<ConstantInt>(Iter.getOperand(1));

    if (mulConstant0 && (!mulConstant1)) // Se solo il primo operando della Mul è una costante
    {
        outs()<<"Ho trovato una costante in posizione 0 di valore: "<<(*mulConstant0).getValue()<<"\n"; 
        
        // Assegna a NextInstruction il riferimento alla successiva istruzione
        auto tmpPtr = Iter.getNextNonDebugInstruction();
        auto &NextInstruction = *tmpPtr;
        
        if (NextInstruction.getOpcode() == Instruction::SDiv) // Se l'istruzione successiva è una SDiv
        {
            outs()<<"Dopo la Mul ho trovato l'istruzione SDiv "<<NextInstruction<<"\n";
            ConstantInt * nextIntstructionConstant0 = dyn_cast<ConstantInt>(NextInstruction.getOperand(0));
            ConstantInt * nextIntstructionConstant1 = dyn_cast<ConstantInt>(NextInstruction.getOperand(1));

            // Se solo il secondo operando della SDiv è una costante
            if ((!nextIntstructionConstant0) && nextIntstructionConstant1) 
            {
                outs()<<"Solo il secondo operando della SDiv è una costante che vale "<<(*nextIntstructionConstant1).getValue()<<"\n";
                
                // Se il valore del primo operando della Mul è uguale al valore del secondo operando della SDiv
                //  e se il primo operando della Sdiv corrisponde all'istruzione precedente di Mul
                if ((*mulConstant0).getValue() == (*nextIntstructionConstant1).getValue() &&
                    (&Iter) == dyn_cast<Instruction>(NextInstruction.getOperand(0)))
                {
                    outs()<<"Il primo operando della Mul è uguale al secondo operando della SDiv e il primo operando della SDiv è uguale alla Mul precedente\n";

                    // Sostituisce tutte le occorenze del risultato dell'operazione di SDiv
                    //  con il secondo operando della Mul
                    NextInstruction.replaceAllUsesWith(Iter.getOperand(1)); 
                }
            }
        }
    
    }
    if ((!mulConstant0) && mulConstant1) // Se solo il secondo operando della mul è una costante 
    {
        outs()<<"Ho trovato una costante in posizione 1 di valore: "<<(*mulConstant1).getValue()<<"\n";
        
        // Assegna a NextInstruction il riferimento alla successiva istruzione
        auto &NextInstruction = *(Iter.getNextNonDebugInstruction());
        
        if (NextInstruction.getOpcode() == Instruction::SDiv) // Se l'istruzione successiva è una SDiv
        {
            outs()<<"Dopo la Mul ho trovato l'istruzione SDiv: "<<NextInstruction<<"\n";
            ConstantInt * nextIntstructionConstant0 = dyn_cast<ConstantInt>(NextInstruction.getOperand(0));
            ConstantInt * nextIntstructionConstant1 = dyn_cast<ConstantInt>(NextInstruction.getOperand(1));

            // Se solo il secondo operando della SDiv è una costante
            if ((!nextIntstructionConstant0) && nextIntstructionConstant1) 
            {
                outs()<<"Solo il secondo operando della SDiv è una costante che vale "<<(*nextIntstructionConstant1).getValue()<<"\n";
                
                // Se il valore del secondo operando della Mul è uguale al valore del secondo operando della SDiv
                //  e se il primo operando della SDiv corrisponde all'istruzione precedente di Mul
                if ((*mulConstant1).getValue() == (*nextIntstructionConstant1).getValue() &&
                    (&Iter) == dyn_cast<Instruction>(NextInstruction.getOperand(0)))
                {
                    outs()<<"Il secondo operando della Mul è uguale al secondo operando della SDiv e il primo operando della SDiv è uguale alla mul precedente\n";
                    
                    // Sostituisce tutte le occorenze del risultato dell'operazione
                    //  di SDiv con il primo operando della Mul
                    NextInstruction.replaceAllUsesWith(Iter.getOperand(0)); 
                }
            }
        }
    }    
}
void sdivMultiInstructionOptimization(Instruction &Iter)
{
    outs()<<"----------------------------------------------------------------------------\nHo trovato una SDiv:"<<Iter<<"\n";
    ConstantInt * sdivConstant0 = dyn_cast<ConstantInt>(Iter.getOperand(0));
    ConstantInt * sdivConstant1 = dyn_cast<ConstantInt>(Iter.getOperand(1));

    if ((!sdivConstant0) && sdivConstant1) // Se solo il secondo operando è una costante 
    {
        outs()<<"Ho trovato una costante in posizione 1 di valore "<<(*sdivConstant1).getValue()<<"\n";

        // Assegna a NextInstruction il riferimento alla successiva istruzione
        auto tmpPtr = Iter.getNextNonDebugInstruction();
        auto &NextInstruction = *tmpPtr;             

        if (NextInstruction.getOpcode() == Instruction::Mul) // Se l'istruzione successiva è una Mul
        {
            outs()<<"Dopo la SDiv ho trovato l'istruzione Mul: "<<NextInstruction<<"\n";
            ConstantInt * nextIntstructionConstant0 = dyn_cast<ConstantInt>(NextInstruction.getOperand(0));
            ConstantInt * nextIntstructionConstant1 = dyn_cast<ConstantInt>(NextInstruction.getOperand(1));

            // Se solo il primo operando della Mul è una costante
            if (nextIntstructionConstant0 && (!nextIntstructionConstant1)) 
            {
                outs()<<"Solo il primo operando della Mul è una costante che vale "<<(*nextIntstructionConstant0).getValue()<<"\n";

                // Se il valore del secondo operando della SDiv è uguale al valore del primo operando della Mul
                //  e se il secondo operando della Mul corrisponde all'istruzione precedente SDiv
                if ((*sdivConstant1).getValue() == (*nextIntstructionConstant0).getValue() &&
                    (&Iter) == dyn_cast<Instruction>(NextInstruction.getOperand(1)))
                {
                    outs()<<"Il secondo operando della SDiv è uguale al primo operando della Mul e il secondo operando della Mul è uguale all'istruzione precedente\n";

                    // Sostituisce tutte le occorenze del risultato dell'operazione di Mul
                    //  con il primo operando della SDiv
                    NextInstruction.replaceAllUsesWith(Iter.getOperand(0)); 
                }
            }
            // Se solo il secondo operando della Mul è una costante
            if ((!nextIntstructionConstant0) && nextIntstructionConstant1) 
            {
                outs()<<"Solo il secondo operando della Mul è una costante che vale "<<(*nextIntstructionConstant1).getValue()<<"\n";

                // Se il valore del secondo operando della SDiv è uguale al valore del secondo operando della Mul
                //  e se il primo operando della Mul corrisponde all'istruzione precedente SDiv
                if ((*sdivConstant1).getValue() == (*nextIntstructionConstant1).getValue() &&
                    (&Iter) == dyn_cast<Instruction>(NextInstruction.getOperand(0)))
                {
                    outs()<<"Il secondo operando della SDiv è uguale al secondo operando della Mul e il primo operando della Mul è uguale all'istruzione precedente\n";

                    // Sostituisce tutte le occorenze del risultato dell'operazione di Mul
                    //  con il primo operando della SDiv
                    NextInstruction.replaceAllUsesWith(Iter.getOperand(0)); 
                }
            }
        }
    }    
}

bool runOnBasicBlockMultiInstructionOptimization(BasicBlock &B) 
{
    for (auto &Iter : B) // Itera sulle istruzioni del Basic Block
    {
        switch(Iter.getOpcode())
        {
            case Instruction::Add: // Se la prima operazione è una Add
            {
                addMultiInstructionOptimization(Iter);
                break;
            }
            case Instruction::Sub: // Se la prima operazione è una Sub
            {
                subMultiInstructionOptimization(Iter);
                break;
            }
            case Instruction::Mul: // Se l'operazione è una Mul
            {
                mulMultiInstructionOptimization(Iter);
                break;
            }
            case Instruction::SDiv: // Se l'operazione è una SDiv
            {
                sdivMultiInstructionOptimization(Iter);
                break;
            }   
            default:
            {
                break;
            }           
        }
    }

    return true;    
}


bool runOnFunctionMultiInstructionOptimization(Function &F) 
{
    bool Transformed = false;

    for (auto Iter = F.begin(); Iter != F.end(); ++Iter) 
    {
        if (runOnBasicBlockMultiInstructionOptimization(*Iter)) 
        {
            Transformed = true;
        }
    }

    return Transformed;
}


PreservedAnalyses MultiInstructionOptimizationPass::run([[maybe_unused]] Module &M, ModuleAnalysisManager &) 
{
    // Un semplice passo di esempio di manipolazione della IR
    for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
        if (runOnFunctionMultiInstructionOptimization(*Iter)) 
        {
            return PreservedAnalyses::none();
        }
    }

  return PreservedAnalyses::none();
}