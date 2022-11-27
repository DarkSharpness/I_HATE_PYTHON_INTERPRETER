#include "Evalvisitor.h"
#include "Exception.h"
#include "utils.cc"
#include "Scope.h"
#include "utils.h"

// antlrcpp::Any ReturnResult;
std::unordered_map <std::string,function> funcMap;


antlrcpp::Any EvalVisitor::visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) {
    if(!ctx->augassign() && !ctx->ASSIGN(0)) {
        return visitChildren(ctx);
    } else {
        auto listVec = ctx->testlist();
        auto testVec = listVec.back()->test();
        std::vector <antlrcpp::Any> valVec;
        valVec.reserve(testVec.size());
        for(auto iter : testVec) {
            auto tmp = visitTest(iter);
            if(tmp.is <Return_List> ()) {
                const auto &vec = tmp.as<Return_List>();
                for(auto it : vec) {
                    valVec.push_back(it);
                }
            }
            else valVec.push_back(tmp);
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
    } else return None_Type();
}

antlrcpp::Any EvalVisitor::visitArith_expr(Python3Parser::Arith_exprContext *ctx) {
    auto termVec = ctx->term();
    auto ans = visitTerm(termVec[0]);
    if(termVec.size() == 1) { return ans; }
    else {
        auto opVec = ctx->addorsub_op();
        for(int i = 1 ; i < termVec.size() ; ++i) {
            auto ret = visitTerm(termVec[i]);
            if(opVec[i - 1]->ADD()) {
                ans += ret;
            } else if(opVec[i-1]->MINUS()) {
                ans -= ret;
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
                               visitFactor(ctx->factor()) ;
    }
}

antlrcpp::Any EvalVisitor::visitAtom_expr(Python3Parser::Atom_exprContext *ctx) {
    if(!ctx->trailer()) {return visitAtom(ctx->atom());}

    auto tmp = visitTrailer(ctx->trailer());            // note down the t
    auto argVec = tmp.as<std::vector<Argument>>();      // get args
    std::string functionName = ctx->atom()->getText();  // Name of a function

    if(functionName == "print") {
        if(argVec.empty()) {
            std::cout << '\n';
            return None_Type();
        }
        std::cout << argVec[0].value;
        for(int i = 1 ; i < argVec.size() ; ++i) {
            std::cout << ' ' << argVec[i].value;
        }
        std::cout << "\n";
        return None_Type();
    } else if(functionName == "int") {
        return argVec.empty() ? int2048_t(0) : AnyToInt(argVec[0].value);
    } else if(functionName == "str") {
        return argVec.empty() ? ""    : AnyToString(argVec[0].value);
    } else if(functionName == "bool") {
        return argVec.empty() ? false : AnyToBool(argVec[0].value);
    } else if(functionName == "float") {
        return argVec.empty() ? 0.0   : AnyToDouble(argVec[0].value);
    } else {
        function func = funcMap[functionName];
        auto paraVec  = func.paraVec;
        auto &SCOPE   = scope.newSpace();

        for(int i = 0 ; i < paraVec.size() ; ++i) {
            SCOPE[paraVec[i].name] = paraVec[i].val;
        }

        for(int i = 0 ; i < argVec.size() ; ++i) {
            const Argument &arg = argVec[i];
            if(arg.keyword == "") {
                SCOPE[paraVec[i].name] = arg.value;
            } else {
                SCOPE[arg.keyword]     = arg.value;
            }
        }

        // std::cout << "IN SCOPE:";
        // for(auto it : SCOPE) {
        //     std::cout << it.first << ' ' << it.second; 
        // }
        // std::cout << '\n';

        auto ans = visitSuite(func.ptr);
        if(ans.is<Return_Type>()) {
            auto data = ans.as<Return_Type>().data;
            // std::cout << "RETURN_VALUE\n";
            if(data.empty()) {
                ans = None_Type();
            } else if(data.size() == 1) {
                ans = data[0];
            } else ans = data;
        } else ans = None_Type(); // No return value

        scope.deleteSpace();
        return ans;
    }

}

/// @return arglist in vector<Argument>
antlrcpp::Any EvalVisitor::visitTrailer(Python3Parser::TrailerContext *ctx) {
    auto argVec = ctx->arglist();
    if(!argVec) {
        return std::vector<Argument>();
    } else {
        return visitArglist(argVec);
    }
}

antlrcpp::Any EvalVisitor::visitAtom(Python3Parser::AtomContext *ctx) {
    if(ctx->NAME()) {
        return scope[ctx->getText()];
    } else if(ctx->NUMBER()) {
        std::string Name = ctx->getText();
        if(Name.find('.') == std::string::npos) {return StringToInt(Name);} 
        else {return StringToFloat(Name);}
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
            ans.append(tmp,1,tmp.length() - 2);
        }
        return ans;
    }
}

// Don't use it!!! Unfold this function it manually.
antlrcpp::Any EvalVisitor::visitTestlist(Python3Parser::TestlistContext *ctx) {
    // std::cout << "STUPID ERROR\n";
    return visitChildren(ctx);
}

/// @return arglist in vector <Argument>
antlrcpp::Any EvalVisitor::visitArglist(Python3Parser::ArglistContext *ctx) {
    auto argumentVec = ctx->argument();
    std::vector <Argument> argVec;
    for (auto arg : argumentVec) {
        argVec.push_back(visitArgument(arg).as<Argument>());
    }
    return argVec;
}


/// @brief Get arguments
antlrcpp::Any EvalVisitor::visitArgument(Python3Parser::ArgumentContext *ctx) {
    auto testVec = ctx->test();
    if(testVec.size() == 1) {
        return (Argument){"",visitTest(testVec[0])};
    } else {
        // std::cout << testVec[0]->getText() << ' ' << visitTest(testVec[1]) << '\n';
        return (Argument){testVec[0]->getText(),visitTest(testVec[1])};
    }
}


antlrcpp::Any EvalVisitor::visitFuncdef(Python3Parser::FuncdefContext *ctx) {
    auto paraVec = visitParameters(ctx->parameters()).as<std::vector <Para_Type>>();
    funcMap[ctx->NAME()->getText()] = function{ctx->suite(),paraVec};
    return None_Type();
}

// Must Return std::vector <Para_Type>()
antlrcpp::Any EvalVisitor::visitParameters(Python3Parser::ParametersContext *ctx) {
    if(!ctx->typedargslist()) {
        return std::vector <Para_Type>();
    }
    return visitTypedargslist(ctx->typedargslist());
}

// Must Return std::vector <Para_Type>()
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
    return visitChildren(ctx);
}
/// @brief Doing Nothing.Done.
antlrcpp::Any EvalVisitor::visitFile_input(Python3Parser::File_inputContext *ctx) {
    return visitChildren(ctx);
}





/// @brief No need to overwrite. It won't be reached 
antlrcpp::Any EvalVisitor::visitComp_op(Python3Parser::Comp_opContext *ctx){
    return visitChildren(ctx);
}


/// @brief No need to overwrite. It won't be reached 
antlrcpp::Any EvalVisitor::visitAddorsub_op(Python3Parser::Addorsub_opContext *ctx){
    return visitChildren(ctx);
}

/// @brief No need to overwrite. It won't be reached 
antlrcpp::Any EvalVisitor::visitMuldivmod_op(Python3Parser::Muldivmod_opContext *ctx){
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
        return visitIf_stmt(ctx->if_stmt());
    } else if(ctx->while_stmt()) {
        return visitWhile_stmt(ctx->while_stmt());
    } else if(ctx->funcdef()) {
        return visitFuncdef(ctx->funcdef());
    }
    return nullptr;
}


antlrcpp::Any EvalVisitor::visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) {
    if(ctx->break_stmt()) {
        return BREAK_CASE;
    } else if(ctx->continue_stmt()) {
        return CONTINUE_CASE;
    } else {
        Return_Type ans;
        ans.data.clear();
        if(!ctx->return_stmt()->testlist()) {return ans;}
        auto testVec = ctx->return_stmt()->testlist()->test();
        for(auto it : testVec) {
            ans.data.push_back(visit(it));
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
        if(ans.is <Flow_Type> () || ans.is <Return_Type> ()) {
            return ans;
        }
    }
    return None_Type(); // EXCEPTION
}


antlrcpp::Any EvalVisitor::visitIf_stmt(Python3Parser::If_stmtContext *ctx) {
    auto testVec = ctx->test();
    for(int i = 0 ; i < testVec.size() ; ++i) {
        if(AnyToBool(visitTest(testVec[i]))) {
            return visitSuite(ctx->suite(i));
        }
   }
   // Last else (case)
   if(ctx->suite(testVec.size())) return visitSuite(ctx->suite(testVec.size()));
   return None_Type();
}

antlrcpp::Any EvalVisitor::visitWhile_stmt(Python3Parser::While_stmtContext *ctx) {
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
    return None_Type(); // No return.
}

// No need TO write
antlrcpp::Any EvalVisitor::visitBreak_stmt(Python3Parser::Break_stmtContext *ctx){
    return visitChildren(ctx);
}
antlrcpp::Any EvalVisitor::visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx){
    return visitChildren(ctx);
}
antlrcpp::Any EvalVisitor::visitReturn_stmt(Python3Parser::Return_stmtContext *ctx){
    return visitChildren(ctx);
}



