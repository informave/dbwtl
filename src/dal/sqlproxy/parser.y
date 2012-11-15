//
// parser.y - Grammar definition
//
// Copyright (C)         informave.org
//   2012,               Daniel Vogelbacher <daniel@vogelbacher.name>
//
// BSD License
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the
// distribution.
//
// Neither the name of the copyright holders nor the names of its
// contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


%token_prefix DBWTL_TOK_

%token_type {informave::db::sqlproxy::Token*}

%default_type {Node*}

%extra_argument { informave::db::sqlproxy::ParseTree *tree }

%syntax_error { 
	//throw std::runtime_error("syntax error");
	tree->raiseSyntaxError();
}

%name SqlProxyParser

%include {
	#include "sqlproxy_fwd.hh"
	#include "ast.hh"
	#include "token.hh"
	#include <iostream>  

	using namespace informave::db;
	using namespace informave::db::sqlproxy;

	//#define YYNOERRORRECOVERY 1
     
	#define CREATE_NODE(type)  type *node = tree->newNode<type>()
   #define ADD_TOKEN(n, t)     n->addChild( tree->newTokenNode(t) )
   #define PRINTME(arg) std::cout << arg << std::endl;
}  



//..............................................................................
////////////////////////////////////////////////////////////////////////// START

%start_symbol start



start ::= selectExpr.
start ::= insertExpr.


selectExpr ::= SELECT attrList(B) FROM fromList(C) optWhere(D) cmdend. {
			  CREATE_NODE(SelectNode);
			  node->addChild(B);
			  node->addChild(C);
			  if(D)
					node->addChild(D);
			  tree->addChild(node);			  
}

optWhere(A) ::= . { A = 0; }
optWhere(A) ::= WHERE expression(B).{
				CREATE_NODE(WhereNode);
				A = node;
				A->addChild(B);
}


insertExpr ::= INSERT INTO id columnSpecList VALUES LP insertValueList RP cmdend. {
}

columnSpecList ::= .

insertValueList ::= expression.
insertValueList ::= insertValueList COMMA expression.


cmdend ::= SEP.
cmdend ::= .



//..............................................................................
///////////////////////////////////////////////////////////////////// expression

primaryExpr(A) ::= idGroup(B). { A = B; }
primaryExpr(A) ::= literal(B). { A = B; }
primaryExpr(A) ::= number(B). { A = B; }
primaryExpr(A) ::= null(B). { A = B; }
primaryExpr(A) ::= param(B). { A = B; }
primaryExpr(A) ::= LP expression(B) RP. { A = B; }


%type exprList { NodeList* }
exprList(A) ::= expression(B). {
				A = tree->newNodeList();
        		A->push_back(B);
}
exprList(A) ::= exprList(B) COMMA expression(C). {
				A = B;
				A->push_back(C);
}

postfixExpr(A) ::= primaryExpr(B). { A = B; }
postfixExpr(A) ::= id(B) LP exprList(C) RP. {
					CREATE_NODE(FuncCallNode);
					node->init(B->data());
					A = node;
					A->addChilds(C);
}
postfixExpr(A) ::= id(B) LP RP. {
					CREATE_NODE(FuncCallNode);
					node->init(B->data());
					A = node;
}

unaryExpr(A) ::= postfixExpr(B). { A = B; }
unaryExpr(A) ::= PLUS castExpr(B). {
				 CREATE_NODE(ExprNode);
				 node->init(ExprNode::plus_op);
				 node->addChild(B);
				 A = node;
}
unaryExpr(A) ::= MINUS castExpr(B). {
				 CREATE_NODE(ExprNode);
				 node->init(ExprNode::minus_op);
				 node->addChild(B);
				 A = node;
}
unaryExpr(A) ::= NOT castExpr(B). {
				 CREATE_NODE(ExprNode);
				 node->init(ExprNode::not_expr);
				 node->addChild(B);
				 A = node;
}

castExpr(A) ::= unaryExpr(B). { A = B; }
castExpr ::= CAST LP castExpr AS id RP.

multiplicativeExpr(A) ::= castExpr(B). { A = B; }
multiplicativeExpr(A) ::= multiplicativeExpr(B) MUL castExpr(C). {
						CREATE_NODE(ExprNode);
						node->init(ExprNode::mul_expr);
						node->addChild(B);
						node->addChild(C);
						A = node;
}
multiplicativeExpr(A) ::= multiplicativeExpr(B) DIV castExpr(C). {
						CREATE_NODE(ExprNode);
						node->init(ExprNode::div_expr);
						node->addChild(B);
						node->addChild(C);
						A = node;
}
multiplicativeExpr(A) ::= multiplicativeExpr(B) MOD castExpr(C). {
						CREATE_NODE(ExprNode);
						node->init(ExprNode::mod_expr);
						node->addChild(B);
						node->addChild(C);
						A = node;
}

additiveExpr(A) ::= multiplicativeExpr(B). { A = B; }
additiveExpr(A) ::= additiveExpr(B) PLUS multiplicativeExpr(C). {
						CREATE_NODE(ExprNode);
						node->init(ExprNode::plus_expr);
						node->addChild(B);
						node->addChild(C);
						A = node;
}
additiveExpr(A) ::= additiveExpr(B) MINUS multiplicativeExpr(C). {
						CREATE_NODE(ExprNode);
						node->init(ExprNode::minus_expr);
						node->addChild(B);
						node->addChild(C);
						A = node;
}
additiveExpr(A) ::= additiveExpr(B) CONCAT multiplicativeExpr(C). {
						CREATE_NODE(ExprNode);
						node->init(ExprNode::concat_expr);
						node->addChild(B);
						node->addChild(C);
						A = node;}

%type relOp { ExprNode::mode }
relOp(A) ::= LOWER. { A = ExprNode::lower_expr; }
relOp(A) ::= GREATER.{ A = ExprNode::greater_expr; }
relOp(A) ::= LOWEREQUAL.{ A = ExprNode::lowerequal_expr; }
relOp(A) ::= GREATEREQUAL.{ A = ExprNode::greaterequal_expr; }

relationalExpr(A) ::= additiveExpr(B). { A = B; }
relationalExpr(A) ::= relationalExpr(B) relOp(C) additiveExpr(D). {
						CREATE_NODE(ExprNode);
						node->init(C);
						node->addChild(B);
						node->addChild(D);
						A = node;
}

equalityExpr(A) ::= relationalExpr(B). { A = B; }
equalityExpr(A) ::= equalityExpr(B) EQUAL relationalExpr(C). {
						CREATE_NODE(ExprNode);
						node->init(ExprNode::equal_expr);
						node->addChild(B);
						node->addChild(C);
						A = node;
}
equalityExpr(A) ::= equalityExpr(B) NOTEQUAL relationalExpr(C). {
						CREATE_NODE(ExprNode);
						node->init(ExprNode::notequal_expr);
						node->addChild(B);
						node->addChild(C);
						A = node;
}
equalityExpr(A) ::= equalityExpr(B) IS NULL. {
						CREATE_NODE(ExprNode);
						node->init(ExprNode::isnull_expr);
						node->addChild(B);
						A = node;
}
equalityExpr(A) ::= equalityExpr(B) IS NOT NULL. {
						CREATE_NODE(ExprNode);
						node->init(ExprNode::isnotnull_expr);
						node->addChild(B);
						A = node;
}

exclusiveOrExpr(A) ::= equalityExpr(B). { A = B; }
exclusiveOrExpr(A) ::= exclusiveOrExpr(B) XOR equalityExpr(C). {
						CREATE_NODE(ExprNode);
						node->init(ExprNode::logxor_expr);
						node->addChild(B);
						node->addChild(C);
						A = node;
}

logicalAndExpr(A) ::= exclusiveOrExpr(B). { A = B; }
logicalAndExpr(A) ::= logicalAndExpr(B) AND exclusiveOrExpr(C). {
						CREATE_NODE(ExprNode);
						node->init(ExprNode::logand_expr);
						node->addChild(B);
						node->addChild(C);
						A = node;
}

logicalOrExpr(A) ::= logicalAndExpr(B). { A = B; }
logicalOrExpr(A) ::= logicalOrExpr(B) OR logicalAndExpr(C). {
						CREATE_NODE(ExprNode);
						node->init(ExprNode::logor_expr);
						node->addChild(B);
						node->addChild(C);
						A = node;
}

condExpr(A) ::= logicalOrExpr(B). { A = B; }

expression(A) ::= condExpr(B). { A = B; }


//..............................................................................
/////////////////////////////////////////////////////////////////////////// FROM



fromList(A) ::= fromListItem(B). {
				CREATE_NODE(FromNode);
				node->addChild(B);
				A = node;
}

fromList(A) ::= fromList(B) COMMA fromListItem(C). {
				A = B;
				A->addChild(C);
}


fromListItem(A) ::= idGroup(B). {
					 A = B;
}

fromListItem(A) ::= idGroup(B) id(C). {
					 CREATE_NODE(AliasNode);
					 node->init(C->data());
					 A = node;
					 A->addChild(B);
}


//..............................................................................
///////////////////////////////////////////////////////////////////// Attributes

%type attrList { AttrListNode* }


attrList(A) ::= attrListItem(B). {
				CREATE_NODE(AttrListNode);
				node->addChild(B);
				A = node;
}

attrList(A) ::= attrList(B) COMMA attrListItem(C). {
				A = B;
				A->addChild(C);
}



attrListItem(A) ::= all(B). {
	A = B;
}

attrListItem(A) ::= expression(B). {
					 /*
					 CREATE_NODE(AliasNode);
					 node->init(Identifier(""));
					 A = node;
					 A->addChild(B);
					 */
					 A = B;
}

attrAsKeyword ::= AS.
attrAsKeyword ::= .

attrListItem(A) ::= expression(B) attrAsKeyword id(C). {
					 CREATE_NODE(AliasNode);
					 node->init(C->data());
					 A = node;
					 A->addChild(B);
}





all(A) ::= MUL. {
	CREATE_NODE(AllAttrNode);
	A = node;
}



%type idGroup { IdGroupNode* }
idGroup(A) ::= id(B). {
			  CREATE_NODE(IdGroupNode);
			  node->addChild(B);
			  A = node;
}

idGroup(A) ::= idGroup(B) DOT id(C). {
			  A = B;
			  A->addChild(C);
}



//..............................................................................
///////////////////////////////////////////////////////////////////// Identifier

%type id { IdNode* }
id(A) ::= ID(B). {
               CREATE_NODE(IdNode);
               node->init(Identifier(B->data()));
					ADD_TOKEN(node, B);
               A = node;
}




//..............................................................................
//////////////////////////////////////////////////////////////////////// Literal

%type literal { LiteralNode* }
literal(A) ::= LITERAL(B). {
               CREATE_NODE(LiteralNode);
               node->init(B->data());
					ADD_TOKEN(node, B);
               A = node;
}


//..............................................................................
///////////////////////////////////////////////////////////////////////// Number

%type number { NumberNode* }
number(A) ::= NUMBER(B). {
	CREATE_NODE(NumberNode);
	node->init(B->data());
	ADD_TOKEN(node, B);
	A = node;
}


//..............................................................................
/////////////////////////////////////////////////////////////////////////// NULL
null(A) ::= NULL. {
	  CREATE_NODE(NullNode);
	  A = node;
}

//..............................................................................
////////////////////////////////////////////////////////////////////// Parameter
param(A) ::= QMARK. {
	  CREATE_NODE(ParamNode);
	  node->init(tree->nextParamNum());
	  A = node;
}
