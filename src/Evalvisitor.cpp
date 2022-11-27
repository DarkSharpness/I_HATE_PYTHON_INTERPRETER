#include "Evalvisitor.h"
#include "Exception.h"
#include "utils.cc"
#include "Scope.h"
#include "utils.h"
#include <unordered_map>


// Assign related
antlrcpp::Any EvalVisitor::visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) {
    auto listVec = ctx->testlist();
    auto testVec = listVec.back()->test();
    if(listVec.size() == 1) { // no assign
        for(auto it : testVec) visitTest(it);
        return No_Return{};
    }

    std::vector <antlrcpp::Any> valVec;
    valVec.reserve(testVec.size());
    for(auto iter : testVec) { valVec.push_back(visitTest(iter));}
    if(ctx->augassign()) {
        auto op = ctx->augassign();
        testVec = listVec[0]->test();
        for(int i = 0 ; i < testVec.size(); ++i) {
            auto &var = scope[testVec[i]->getText()];
            if(op->ADD_ASSIGN())       { var += valVec[i]; }
            else if(op->SUB_ASSIGN())  { var -= valVec[i]; }
            else if(op->MULT_ASSIGN()) { var *= valVec[i]; }
            else if(op->DIV_ASSIGN())  { var /= valVec[i]; }
            else if(op->IDIV_ASSIGN()) { var |= valVec[i]; }
            else  /* MOD_ASSIGN CASE*/ { var %= valVec[i]; }
        }
    } else {
        for(int i = listVec.size() - 2 ; i >= 0 ; --i) {
            testVec = listVec[i]->test();
            for(int j = 0 ; j < testVec.size() ; ++j) {
                scope[testVec[j]->getText()] = valVec[j];
            }
        }
    }
    return No_Return{}; // NO RETURN VALUE
}

// Middle vertex
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

// It won't be reached
antlrcpp::Any EvalVisitor::visitAugassign(Python3Parser::AugassignContext *ctx) {
    return No_Return{};
}

antlrcpp::Any EvalVisitor::visitArith_expr(Python3Parser::Arith_exprContext *ctx) {
    auto termVec = ctx->term();
    auto ans = visitTerm(termVec[0]);
    if(termVec.size() == 1) { return ans; }

    auto opVec = ctx->addorsub_op();
    for(int i = 1 ; i < termVec.size() ; ++i) {
        auto cur = visitTerm(termVec[i]);
        if(opVec[i - 1]->ADD()) {
            ans += cur;
        } else { /* Minus */
            ans -= cur;
        }
    }
    return ans;
}

antlrcpp::Any EvalVisitor::visitTerm(Python3Parser::TermContext *ctx) {
    auto factorVec = ctx->factor();
    auto ans = visitFactor(factorVec[0]);
    if(factorVec.size() == 1) { return ans; }

    auto opVec = ctx->muldivmod_op();
    for(int i = 1 ; i < factorVec.size() ; ++i) {
        auto cur = visitFactor(factorVec[i]);
        if(opVec[i - 1]->STAR()) {
            ans *= cur;
        } else if(opVec[i - 1]->DIV()) {
            ans /= cur;
        } else if(opVec[i - 1]->IDIV()) {
            ans |= cur;
        } else /* Mod Case */ {
            ans %= cur;
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
    if(!ctx->trailer()) return visitAtom(ctx->atom());

    std::string functionName = ctx->atom()->getText(); // Name of a function
    auto tmp = visitTrailer(ctx->trailer()); // note down the t
    auto &argVec = tmp.as<std::vector<antlrcpp::Any>>(); // get args

    if(functionName == "print") {
        if(argVec.empty()) {
            std::cout << '\n';
            return No_Return{};
        }
        std::cout << argVec[0];
        for(int i = 1 ; i < argVec.size(); ++i) {
            std::cout << ' ' << argVec[i];
        }
        std::cout << '\n';
        return No_Return{};
    } else if(functionName == "int") {
        return argVec.empty() ? 0 : AnyToInt(argVec[0]);
    } else if(functionName == "str") {
        return argVec.empty() ? "" : AnyToString(argVec[0]);
    } else if(functionName == "bool") {
        return argVec.empty() ? false : AnyToBool(argVec[0]);
    } else if(functionName == "float") {
        return argVec.empty() ? 0.0 : AnyToDouble(argVec[0]);
    } else {
        const function &func = funcScope[functionName];
        const auto &paraVec  = func.paraVec;
        auto &SCOPE = scope.newSpace();

        for(int i = 0 ; i < argVec.size() ; ++i) {
            if(argVec[i].is<Keyword_Position>()) {
                auto tmp = argVec[i].as <Keyword_Position>();
                SCOPE[tmp.keyword]     = tmp.value;
            } else {
                SCOPE[paraVec[i].name] = argVec[i];
            }
        }

        // Init value
        for(int i = 0 ; i < paraVec.size() ; ++i) {
            SCOPE.insert(std::make_pair(paraVec[i].name,
                                        paraVec[i].val));
        }

        auto ans = visitSuite(func.ptr);
        if(ans.is<Return_Type>()) {
            ans = ans.as<Return_Type>().data;
        }
        scope.deleteSpace();
        return ans;
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
        if(Name.find('.') == std::string::npos) {
               return StringToInt(Name);
        } else return StringToFloat(Name);
    } else if(ctx->TRUE()) {
        return true;
    } else if(ctx->FALSE()) {
        return false;
    } else if(ctx->NONE()) {
        return No_Return{};
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
antlrcpp::Any EvalVisitor::visitTestlist(Python3Parser::TestlistContext *ctx) {return visitChildren(ctx);}

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
    auto paraVec = visitParameters(ctx->parameters());
    funcScope[ctx->NAME()->getText()] = function {ctx->suite(),
                                                  paraVec.as<std::vector <Para_Type>>()};
    return {};
}

// TODO
antlrcpp::Any EvalVisitor::visitParameters(Python3Parser::ParametersContext *ctx) {
    if(!ctx->typedargslist()) { return std::vector <Para_Type>(); }
    else return visitTypedargslist(ctx->typedargslist());
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


// Nothing
antlrcpp::Any EvalVisitor::visitFile_input(Python3Parser::File_inputContext *ctx) {
    return visitChildren(ctx);
}





/// @brief No need to overwrite. It won't be reached 
antlrcpp::Any EvalVisitor::visitTfpdef(Python3Parser::TfpdefContext *ctx){return visitChildren(ctx);}
antlrcpp::Any EvalVisitor::visitAddorsub_op(Python3Parser::Addorsub_opContext *ctx){return visitChildren(ctx);}
antlrcpp::Any EvalVisitor::visitMuldivmod_op(Python3Parser::Muldivmod_opContext *ctx){return visitChildren(ctx);}
antlrcpp::Any EvalVisitor::visitComp_op(Python3Parser::Comp_opContext *ctx){return visitChildren(ctx);}


antlrcpp::Any EvalVisitor::visitOr_test(Python3Parser::Or_testContext *ctx) {
    auto andVec = ctx->and_test();
    if(andVec.size() == 1) { return visitAnd_test(andVec[0]); } 
    for(auto it : andVec) {
        if(AnyToBool(visitAnd_test(it)))  
            return true;
    }
    return false;
    
}

antlrcpp::Any EvalVisitor::visitAnd_test(Python3Parser::And_testContext *ctx) {
    auto notVec = ctx->not_test();
    if(notVec.size() == 1) { return visitNot_test(notVec[0]); } 
    for(auto it : notVec) {
        if(!AnyToBool(visitNot_test(it))) 
            return false;
    }
    return true;
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
            if(cmp->LESS_THAN()) { 
                ans = (cur < tmp);
            } else if(cmp->GREATER_THAN()) {
                ans = (cur > tmp);
            } else if(cmp->EQUALS()) {
                ans = (cur == tmp);
            } else if(cmp->LT_EQ()) {
                ans = (cur <= tmp);
            } else if(cmp->GT_EQ()) {
                ans = (cur >= tmp);
            } else if(cmp->NOT_EQ_2()) {
                ans = (cur != tmp);
            }
            if(!ans) return false;
            cur = tmp;
        }
        return true;
    } else {
        return visitArith_expr(arithVec[0]);
    }
}

antlrcpp::Any EvalVisitor::visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) {
    if(ctx->if_stmt()) {
        return visitIf_stmt(ctx->if_stmt());
    } else if(ctx->while_stmt()) {
        return visitWhile_stmt(ctx->while_stmt());
    } else if(ctx->funcdef()) {
        return visitFuncdef(ctx->funcdef());
    } else return No_Return{}; // ERROR
}


antlrcpp::Any EvalVisitor::visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) {
    if(ctx->break_stmt()) {
        return BREAK_CASE;
    } else if(ctx->continue_stmt()) {
        return CONTINUE_CASE;
    } else { // Return case
        Return_Type ans;
        if(!ctx->return_stmt()->testlist()) {
            ans.data = No_Return{}; // NO RETURN VALUE
        } else { /* At most one element. */
            ans.data = visitTest(ctx->return_stmt()->testlist()->test(0));
        }
        return ans;
    }
    
}


antlrcpp::Any EvalVisitor::visitSuite(Python3Parser::SuiteContext *ctx) {
    if(ctx->simple_stmt()) {
        return visitSimple_stmt(ctx->simple_stmt());
    }
    auto stmtVec = ctx->stmt();
    for(auto it : stmtVec) {
        auto ans = visitStmt(it);
        if(ans.is<Flow_Type>() || ans.is<Return_Type>()) {
            return ans;
        }
    }
    return No_Return{};
}


antlrcpp::Any EvalVisitor::visitIf_stmt(Python3Parser::If_stmtContext *ctx) {
    // if(!ctx->IF()) {std::cout << "NO IF EXCEPTION\n"; return {};}// Exception!!!
    auto testVec = ctx->test();
    int i = 0;
    for(int j = ctx->ELIF().size() ; i <= j ; ++i) {
        if(AnyToBool(visitTest(testVec[i]))) {
            return visitSuite(ctx->suite(i));
        }
    }
    // Last else (case)
    if(ctx->ELSE()) return visitSuite(ctx->suite(i));
    return No_Return{};
}

antlrcpp::Any EvalVisitor::visitWhile_stmt(Python3Parser::While_stmtContext *ctx) {
    // if(!ctx->WHILE()) return {}; // Exception!!!
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
    return No_Return{}; // No return.
}

// No need TO write
antlrcpp::Any EvalVisitor::visitBreak_stmt(Python3Parser::Break_stmtContext *ctx){return visitChildren(ctx);}
antlrcpp::Any EvalVisitor::visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx){return visitChildren(ctx);}
antlrcpp::Any EvalVisitor::visitReturn_stmt(Python3Parser::Return_stmtContext *ctx){return visitChildren(ctx);}



