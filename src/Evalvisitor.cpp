#include "Evalvisitor.h"
#include "Exception.h"
#include "utils.cc"
#include "Scope.h"
#include "utils.h"



antlrcpp::Any EvalVisitor::visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) {
    if(!ctx->augassign() && !ctx->ASSIGN(0)) {
        // std::cout << "NO ASSIGN CASE!\n";
        return visitChildren(ctx);
    } else {
        auto listVec = ctx->testlist();
        auto testVec = listVec.back()->test();
        std::vector <antlrcpp::Any> valVec;
        valVec.reserve(testVec.size());
        for(auto iter : testVec) {
            valVec.push_back(visitTest(iter));
        }
        if(ctx->augassign()) {
            auto op = visitAugassign(ctx->augassign()).as<AugAssignOP>();
            testVec = listVec[0]->test();
            for(int i = 0 ; i < testVec.size(); ++i) {
                auto &var = scope[testVec[i]->getText()];
                switch(op) {
                    case ADD_ASSIGN_OP:
                        var += valVec[i];
                        break;
                    case SUB_ASSIGN_OP:
                        var -= valVec[i];
                        break;
                    case MULT_ASSIGN_OP:
                        var *= valVec[i];
                        break;
                    case DIV_ASSIGN_OP:
                        var /= valVec[i];
                        break;
                    case IDIV_ASSIGN_OP:
                        var |= valVec[i];
                        break;
                    case MOD_ASSIGN_OP:
                        var %= valVec[i];
                }
            }
        } else {
            for(int i = listVec.size() - 2 ; i >= 0 ; --i) {
                testVec = listVec[i]->test();
                for(int j = 0 ; j < testVec.size() ; ++j) {
                    scope[testVec[j]->getText()] = valVec[j];
                }
            }
        }
    }
    return nullptr;
}

antlrcpp::Any EvalVisitor::visitTest(Python3Parser::TestContext *ctx) {
    return visitOr_test(ctx->or_test());
}

antlrcpp::Any EvalVisitor::visitStmt(Python3Parser::StmtContext *ctx) {
    if(ctx->simple_stmt()) {
        return visitSimple_stmt(ctx->simple_stmt());
    } else {
        return visitCompound_stmt(ctx->compound_stmt());
    }
}

antlrcpp::Any EvalVisitor::visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) {
    return visitSmall_stmt(ctx->small_stmt());
}

antlrcpp::Any EvalVisitor::visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) {
    if(ctx->expr_stmt()) {
        return visitExpr_stmt(ctx->expr_stmt());
    } else {
        return visitFlow_stmt(ctx->flow_stmt());
    }
}

antlrcpp::Any EvalVisitor::visitAugassign(Python3Parser::AugassignContext *ctx) {
    if(ctx->ADD_ASSIGN()) {
        return ADD_ASSIGN_OP;
    } else if(ctx->SUB_ASSIGN()) {
        return SUB_ASSIGN_OP;
    } else if(ctx->MULT_ASSIGN()) {
        return MULT_ASSIGN_OP;
    } else if(ctx->DIV_ASSIGN()) {
        return DIV_ASSIGN_OP;
    } else if(ctx->IDIV_ASSIGN()) {
        return IDIV_ASSIGN_OP;
    } else if(ctx->MOD_ASSIGN()) {
        return MOD_ASSIGN_OP;
    } else return {};
 }

antlrcpp::Any EvalVisitor::visitArith_expr(Python3Parser::Arith_exprContext *ctx) {
    auto termVec = ctx->term();
    auto ans = visitTerm(termVec[0]);
    if(termVec.size() == 1) {
        return ans;
    }
    else {
        auto opVec = ctx->addorsub_op();
        for(int i = 1 ; i < termVec.size() ; ++i) {
            auto ret = visitTerm(termVec[i]);
            if(opVec[i-1]->ADD()) {
                ans = ans + ret;
            } else if(opVec[i-1]->MINUS()) {
                ans = ans - ret;
            }
        }
    }
    return ans;
}

antlrcpp::Any EvalVisitor::visitTerm(Python3Parser::TermContext *ctx) {
    auto factorVec = ctx->factor();
    auto ans = visitFactor(factorVec[0]);
    if(factorVec.size() == 1) {
        return ans;
    }
    else {
        auto opVec = ctx->muldivmod_op();
        for(int i = 1 ; i < factorVec.size() ; ++i) {
            auto ret = visitFactor(factorVec[i]);
            if(opVec[i-1]->STAR()) {
                ans *= ret;
            } else if(opVec[i-1]->DIV()) {
                ans /= ret;
            } else if(opVec[i-1]->IDIV()) {
                ans |= ret;
            } else if(opVec[i-1]->MOD()) {
                ans %= ret;
            }
        }
    }
    return ans;
}


antlrcpp::Any EvalVisitor::visitFactor(Python3Parser::FactorContext *ctx) {
    if(ctx->atom_expr()) {
        return visitAtom_expr(ctx->atom_expr());
    } else {
        return ctx->MINUS() ? -visitFactor(ctx->factor()) : 
                               visitFactor(ctx->factor());
    }
}

antlrcpp::Any EvalVisitor::visitAtom_expr(Python3Parser::Atom_exprContext *ctx) {
    if(ctx->trailer()) {
        const auto &functionName = ctx->atom()->getText(); // Name of a function
        auto tmp = visitTrailer(ctx->trailer()); // note down the t
        auto &argVec = tmp.as<std::vector<antlrcpp::Any>>(); // get args
        if(functionName == "print") {
            for(const auto &arg : argVec) {
                std::cout << arg << ' ';
            }
            std::cout << "\n";
            return {};
        } else if(functionName == "int") {
            return AnyToInt(argVec[0]);
        } else if(functionName == "str") {
            return AnyToString(argVec[0]);
        } else if(functionName == "bool") {
            return AnyToBool(argVec[0]);
        } else if(functionName == "float") {
            return AnyToDouble(argVec[0]);
        } else {
            auto func = scope[functionName].as<function>();
            auto paraVec = func.paraVec;
            scope.newSpace();
            int i = 0;
            for(; i != argVec.size() ; ++i) {
                if(argVec[i].is<Keyword_Position>()) break;
                scope.insert(paraVec[i].name,argVec[i]);
            }
            // std::cout << "OK_IN_FUNC\n";
            std::unordered_map <std::string,int> keymap;
            for(int j = i; j < argVec.size() ; ++j) {
                keymap[argVec[j].as<Keyword_Position>().keyword] = j;
            }
            for(; i < paraVec.size() ; ++i) {
                auto iter = keymap.find(paraVec[i].name);
                if(iter == keymap.end()) { // Not found.
                    scope.insert(paraVec[i].name,paraVec[i].val);
                } else { // Found.
                    scope.insert(iter->first,
                                    argVec[iter->second].as<Keyword_Position>().value);
                }
            }
            // std::cout << "GO INTO FUNCTION!!\n";
            auto ans = visitSuite(func.ptr);
            if(ans.is<Return_Type>()) {
                // std::cout << "Return!\n";
                ans = ans.as<Return_Type>().data;
                // std::cout << "Test out:" << ans << '\n';
            } else {} // No return value
            //{std::cout << "Fatal Error of Return!!!\n";}
            // std::cout << "RemoveSpace:" << scope.scope.size() << '\n';
            scope.deleteSpace();
            return ans;
        }
        // return nullptr;
    } else {
        return visitAtom(ctx->atom());
    }
}

/// @return arglist in vector<any>
antlrcpp::Any EvalVisitor::visitTrailer(Python3Parser::TrailerContext *ctx) {
    auto argVec = ctx->arglist();
    if(argVec == nullptr) {
        return std::vector<antlrcpp::Any>();
    } else {
        return visitArglist(argVec);
    }
}

antlrcpp::Any EvalVisitor::visitAtom(Python3Parser::AtomContext *ctx) {
    if(ctx->NAME()) {
        return scope[ctx->getText()];
    } else if(ctx->NUMBER()) {
        std::string Name = ctx->getText();
        if(Name.find('.') == -1uLL) {
            // std::cout << "INT!!!!\n";
            // std::cout << Name << '\n';
            return StringToInt(Name);
        } else return StringToFloat(Name);
    } else if(ctx->TRUE()) {
        return true;
    } else if(ctx->FALSE()) {
        return false;
    } else if(ctx->NONE()) {
        return None_Type();
    } else if(ctx->test()) {
        return visitTest(ctx->test());
    } else {
        auto stringVec = ctx->STRING();
        std::string ans;
        for(auto iter : stringVec) {
            const std::string tmp = iter->getText();
            ans.append(tmp,1,tmp.length()-2);
        }
        return ans;
    }
}

// Don't use it!!! Unfold this function it manually.
antlrcpp::Any EvalVisitor::visitTestlist(Python3Parser::TestlistContext *ctx) {
    // std::cout << "STUPID ERROR\n";
    return visitChildren(ctx);
}

/// @return arglist in vector <any>  
antlrcpp::Any EvalVisitor::visitArglist(Python3Parser::ArglistContext *ctx) {
    auto argumentVec = ctx->argument();
    std::vector <antlrcpp::Any> argVec;
    for (auto arg : argumentVec) {
        argVec.push_back(visitArgument(arg));
    }
    return argVec;
}


/// @brief Get arguments
antlrcpp::Any EvalVisitor::visitArgument(Python3Parser::ArgumentContext *ctx) {
    auto testVec = ctx->test();
    if(testVec.size() == 1) {
        return visitTest(testVec[0]);
    } else {
        return (Keyword_Position){testVec[0]->getText(),visitTest(testVec[1])};
    }
}


antlrcpp::Any EvalVisitor::visitFuncdef(Python3Parser::FuncdefContext *ctx) {
    scope[ctx->NAME()->getText()] = function{ctx->suite(),visitParameters(ctx->parameters())};
    return {};
}

// TODO
antlrcpp::Any EvalVisitor::visitParameters(Python3Parser::ParametersContext *ctx) {
    if(!ctx->typedargslist()) {
        // std::cout << "NO Parameter list\n";
        return std::vector <Para_Type>();
    }
    return visitTypedargslist(ctx->typedargslist());
}
antlrcpp::Any EvalVisitor::visitTypedargslist(Python3Parser::TypedargslistContext *ctx){
    auto tfpdefVec = ctx->tfpdef();
    auto testVec   = ctx->test();
    // Basic Function type.
    std::vector <Para_Type> ans;
    ans.reserve(tfpdefVec.size());
    const int d = tfpdefVec.size() - testVec.size();
    for(int i = 0 ; i < d  ; ++i) {
        ans.emplace_back(tfpdefVec[i]->NAME()->getText(),nullptr);
    }
    for(int i = 0 ; i < testVec.size() ; ++i) {
        ans.emplace_back(tfpdefVec[i + d]->NAME()->getText(),visitTest(testVec[i]));
    }
    return ans;
}

/// @brief No need to overwrite. It won't be reached
antlrcpp::Any EvalVisitor::visitTfpdef(Python3Parser::TfpdefContext *ctx){
    // std::cout << "Error:visitTfdef\n";
    return visitChildren(ctx);
}
/// @brief Doing Nothing.Done.
antlrcpp::Any EvalVisitor::visitFile_input(Python3Parser::File_inputContext *ctx) {
    // std::cout << "FileInput\n";
    return visitChildren(ctx);
}





/// @brief No need to overwrite. It won't be reached 
antlrcpp::Any EvalVisitor::visitComp_op(Python3Parser::Comp_opContext *ctx){
    // std::cout << "Error: Comp\n";
    return visitChildren(ctx);
}


/// @brief No need to overwrite. It won't be reached 
antlrcpp::Any EvalVisitor::visitAddorsub_op(Python3Parser::Addorsub_opContext *ctx){
    // std::cout << "Error: ADD\n";
    return visitChildren(ctx);
}

/// @brief No need to overwrite. It won't be reached 
antlrcpp::Any EvalVisitor::visitMuldivmod_op(Python3Parser::Muldivmod_opContext *ctx){
    // std::cout << "Error: MULT\n";
    return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitOr_test(Python3Parser::Or_testContext *ctx) {
    auto andVec = ctx->and_test();
    if(!ctx->OR(0)) {
        return visitAnd_test(andVec[0]);
    } else {
        for(auto iter : andVec) {
            if(AnyToBool(visitAnd_test(iter)) == true) {
                return true;
            }
        }
        return false;
    }
}

antlrcpp::Any EvalVisitor::visitAnd_test(Python3Parser::And_testContext *ctx) {
    auto notVec = ctx->not_test();
    if(!ctx->AND(0)) {
        return visitNot_test(notVec[0]);
    } else {
        for(auto iter : notVec) {
            if(AnyToBool(visitNot_test(iter)) == false) {
                return false;
            }
        }
        return true;
    }
}

antlrcpp::Any EvalVisitor::visitNot_test(Python3Parser::Not_testContext *ctx) {
    if(!ctx->NOT()) {
        return visitComparison(ctx->comparison());
    } else {
        return !AnyToBool(visitNot_test(ctx->not_test()));        
    }
}


antlrcpp::Any EvalVisitor::visitComparison(Python3Parser::ComparisonContext *ctx){
    auto arithVec = ctx->arith_expr();
    if(arithVec.size() > 1) {
        auto cmpVec = ctx->comp_op();
        auto cur    = visitArith_expr(arithVec[0]);
        for(int i = 1 ; i < arithVec.size() ; ++i) {
            auto tmp = visitArith_expr(arithVec[i]);
            auto cmp = cmpVec[i - 1];
            bool ans = true;
            if(cmp->LESS_THAN()) { // <
                ans = cur < tmp; // Need to be written!!
            } else if(cmp->GREATER_THAN()) {
                ans = cur > tmp;
            } else if(cmp->EQUALS()) {
                ans = cur == tmp;
            } else if(cmp->LT_EQ()) {
                ans = cur <= tmp;
            } else if(cmp->GT_EQ()) {
                ans = cur >= tmp;
            } else if(cmp->NOT_EQ_2()) {
                ans = cur != tmp;
            }
            if(!ans) return false;
            cur = std::move(tmp);
        }
        return true;
    } else {
        return visitArith_expr(arithVec[0]);
    }
}

antlrcpp::Any EvalVisitor::visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) {
    if(ctx->if_stmt()) {
        // std::cout << "IF\n";
        return visitIf_stmt(ctx->if_stmt());
    } else if(ctx->while_stmt()) {
        // std::cout << "WHILE\n";
        return visitWhile_stmt(ctx->while_stmt());
    } else if(ctx->funcdef()) {
        // std::cout << "FUNC\n";
        return visitFuncdef(ctx->funcdef());
    }
    return nullptr;
}


antlrcpp::Any EvalVisitor::visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) {
    if(ctx->break_stmt()) {
        return BREAK_CASE;
    } else if(ctx->continue_stmt()) {
        return CONTINUE_CASE;
    } else { // Return case
        // std::cout << "Return_Type!\n";
        Return_Type ans;
        
        if(!ctx->return_stmt()->testlist()) {
            ans.data = (void*)0;
            return ans;
        }

        auto testVec = ctx->return_stmt()->testlist()->test();
        // std::cout << "testVec:" << testVec.size() << '\n';

        ans.data = visitTest(testVec[0]);
        /* At most one element...
        ans.data.reserve(testVec.size());
        for(auto it : testVec) {
            auto tmp = visitTest(it);
            // std::cout << tmp << "value\n";
            ans.data.emplace_back(tmp);
        }
        */
        return ans;
    }
    
}


antlrcpp::Any EvalVisitor::visitSuite(Python3Parser::SuiteContext *ctx) {
    if(ctx->simple_stmt()) {
        auto ans = visitSimple_stmt(ctx->simple_stmt());
        if(ans.is<Flow_Type>() || ans.is<Return_Type>()) {
            // std::cout << "Here\n";
            return ans;
        }
    }
    auto stmtVec = ctx->stmt();
    for(auto it : stmtVec) {
        auto ans = visitStmt(it);
        if(ans.is<Flow_Type>() || ans.is<Return_Type>()) {
            // std::cout << "Here2\n";
            return ans;
        }
    }
    return {}; // EXCEPTION
}


antlrcpp::Any EvalVisitor::visitIf_stmt(Python3Parser::If_stmtContext *ctx) {
    // if(!ctx->IF()) {std::cout << "NO IF EXCEPTION\n"; return {};}// Exception!!!
    auto testVec = ctx->test();
    for(int i = 0 ; i < testVec.size() ; ++i) {
        if(AnyToBool(visitTest(testVec[i]))) {
            return visitSuite(ctx->suite(i));
        }
   }
   // Last else (case)
   if(ctx->suite(testVec.size())) return visitSuite(ctx->suite(testVec.size()));
   return {};
}

antlrcpp::Any EvalVisitor::visitWhile_stmt(Python3Parser::While_stmtContext *ctx) {
    if(!ctx->WHILE()) return {}; // Exception!!!
    while(AnyToBool(visitTest(ctx->test()))) {
        auto ans = visitSuite(ctx->suite());
        if(ans.is<Flow_Type>()) {
            if(ans.as<Flow_Type>() == BREAK_CASE) {
                break;
            } else { // CONTINUE CASE:
                continue;
            }
        } else if(ans.is<Return_Type>()) {
            return ans;
        }
    }
    return {}; // No return.
}

// No need TO write
antlrcpp::Any EvalVisitor::visitBreak_stmt(Python3Parser::Break_stmtContext *ctx){
    // std::cout << "Error: Break\n";
    return visitChildren(ctx);
}
antlrcpp::Any EvalVisitor::visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx){
    // std::cout << "Error: Continue\n";
    return visitChildren(ctx);
}
antlrcpp::Any EvalVisitor::visitReturn_stmt(Python3Parser::Return_stmtContext *ctx){
    // std::cout << "Error: Return\n";
    return visitChildren(ctx);
}



