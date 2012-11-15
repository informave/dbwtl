//
// ast.cc - Abstract Syntax Tree
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


/// @file
/// @brief Abstract Syntax Tree (nodes and tree classes) (definition)
/// @author Daniel Vogelbacher
/// @since 0.0.1


#include "sqlproxy_fwd.hh"
#include "ast.hh"
#include "token.hh"

#include "dbwtl/dal/dal_interface.hh"

#include <cstdlib>
#include <cstdio>
#include <memory>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <typeinfo>

#define PDEBUG(name) std::cout << name << std::endl


SQLPROXY_NAMESPACE_BEGIN


struct SyntaxError : std::exception
{
    std::string msg;

    SyntaxError(Token *t)
    {
        msg = t->str();
    }

    virtual const char* what(void) const throw()
    {
        return msg.c_str();
    }

    SyntaxError(void)
    {
    }

    virtual ~SyntaxError(void) throw()
    {}
};


void IdNode::accept(Visitor &visitor)           { visitor.visit(this); }
void TokenNode::accept(Visitor &visitor)           { visitor.visit(this); }
void ParseTree::accept(Visitor &visitor)          { visitor.visit(this); }
void SelectNode::accept(Visitor &visitor)            { visitor.visit(this); }
void AttrListNode::accept(Visitor &visitor)        { visitor.visit(this); }
void AllAttrNode::accept(Visitor &visitor)        { visitor.visit(this); }
void NumberNode::accept(Visitor &visitor)             { visitor.visit(this); }
void LiteralNode::accept(Visitor &visitor)       { visitor.visit(this); }
void NullNode::accept(Visitor &visitor)       { visitor.visit(this); }
void ExprNode::accept(Visitor &visitor)         { visitor.visit(this); }
void FromNode::accept(Visitor &visitor)        { visitor.visit(this); }
void WhereNode::accept(Visitor &visitor)          { visitor.visit(this); }
void AliasNode::accept(Visitor &visitor)          { visitor.visit(this); }
void FuncCallNode::accept(Visitor &visitor)          { visitor.visit(this); }
void ParamNode::accept(Visitor &visitor)          { visitor.visit(this); }
void IdGroupNode::accept(Visitor &visitor)          { visitor.visit(this); }



String IdNode::str(void) const              { return "ID"; /*this->id.str();*/ }
String TokenNode::str(void) const              { return "TokenNode"; }
String ParseTree::str(void) const              { return "ParseTree"; }
String SelectNode::str(void) const             { return "SelectNode"; }
String AttrListNode::str(void) const               { return "AttrListNode"; }
String LiteralNode::str(void) const                { return String("LiteralNode: ") + this->data(); }
String NullNode::str(void) const             { return "NullNode"; }
//String ExprNode::str(void) const          { return "ExprNode"; }
String FromNode::str(void) const            { return "FromNode"; }
String WhereNode::str(void) const             { return "WhereNode"; }
String AliasNode::str(void) const             { return "AliasNode"; }

String AllAttrNode::str(void) const             { return "AllAttrNode"; }
String FuncCallNode::str(void) const             { return "FuncCallNode"; }
String ParamNode::str(void) const             { return "ParamNode"; }
String IdGroupNode::str(void) const             { return "IdGroupNode"; }

String NumberNode::str(void) const
{
    std::stringstream ss;
    ss << "NumberNode " << this->data().str();
    return String(ss.str());
}


String IdNode::nodetype(void) const                { return IdNode::name();            }
String TokenNode::nodetype(void) const                { return TokenNode::name();            }
String NullNode::nodetype(void) const                { return NullNode::name();            }
String ParseTree::nodetype(void) const                { return ParseTree::name();            }
String SelectNode::nodetype(void) const               { return SelectNode::name();           }
String AttrListNode::nodetype(void) const                 { return AttrListNode::name();             }
String NumberNode::nodetype(void) const             { return NumberNode::name();         }
String LiteralNode::nodetype(void) const                  { return LiteralNode::name();              }
String ExprNode::nodetype(void) const            { return ExprNode::name();        }
String FromNode::nodetype(void) const              { return FromNode::name();          }
String WhereNode::nodetype(void) const               { return WhereNode::name();           }
String AliasNode::nodetype(void) const               { return AliasNode::name();           }

String AllAttrNode::nodetype(void) const               { return AllAttrNode::name();           }
String FuncCallNode::nodetype(void) const               { return FuncCallNode::name();           }
String ParamNode::nodetype(void) const               { return ParamNode::name();           }
String IdGroupNode::nodetype(void) const               { return IdGroupNode::name();           }

//..............................................................................
/////////////////////////////////////////////////////////////////////////// Node

/// @details
/// 
Node::Node(void)
    : m_childs(),
      m_sinfo()
{}


/// @details
/// 
Node::~Node(void)
{}


/// @details
/// 
SourceInfo
Node::getSourceInfo(void) const
{
    return this->m_sinfo;
}


/// @details
/// 
void
Node::addChilds(const NodeList *list)
{
    assert(list && "Nodelist is null");
    std::copy(list->begin(), list->end(), std::back_insert_iterator<NodeList>(this->m_childs));

    for(NodeList::const_iterator i = list->begin();
        i != list->end();
        ++i)
    {
        this->updateSourceInfo((*i)->getSourceInfo());
    }
}


/// @details
/// 
void 
Node::addChild(Node *child)
{
    assert(child);

    this->m_childs.push_back(child);
    this->updateSourceInfo(child->getSourceInfo());
}


/// @details
/// 
void
Node::updateSourceInfo(const SourceInfo &info)
{
    m_sinfo.expand(info);
}



//..............................................................................
//////////////////////////////////////////////////////////////////////// Visitor

/// @details
/// 
Visitor::Visitor(visitor_mode mode)
    : m_mode(mode)
{}


void
Visitor::fallback_action(Node *node)
{
    if(this->m_mode == ignore_none)
    {
        std::string  s(std::string("Visitor for ") +typeid(*node).name()+ " is not implemented in <");
        s.append(typeid(*this).name());
        s.append(">");
        throw std::runtime_error(s);
    }
}

#define DEFAULT_VISIT(type)                     \
    void                                        \
    Visitor::visit(type *node)                  \
    {                                           \
        this->fallback_action(node);            \
    }

DEFAULT_VISIT(IdNode)
DEFAULT_VISIT(TokenNode)
DEFAULT_VISIT(ParseTree)
DEFAULT_VISIT(SelectNode)
DEFAULT_VISIT(AttrListNode)
DEFAULT_VISIT(AllAttrNode)
DEFAULT_VISIT(NumberNode)
DEFAULT_VISIT(LiteralNode)
DEFAULT_VISIT(NullNode)
DEFAULT_VISIT(ExprNode)
DEFAULT_VISIT(FromNode)
DEFAULT_VISIT(WhereNode)
DEFAULT_VISIT(AliasNode)
    DEFAULT_VISIT(FuncCallNode)
    DEFAULT_VISIT(ParamNode)
    DEFAULT_VISIT(IdGroupNode)
/// @details
/// 
void
Visitor::operator()(Node *node)
{
    node->accept(*this);
}



//..............................................................................
////////////////////////////////////////////////////////////////////// TokenNode

/// @details
/// 
TokenNode::TokenNode(Token *tok)
    : m_token(tok)
{
    m_sinfo = tok->getSourceInfo();
}



//..............................................................................
///////////////////////////////////////////////////////////////////// SelectNode

/// @details
/// 
SelectNode::SelectNode(void)
    : Node()
{}


//..............................................................................
/////////////////////////////////////////////////////////////////// AttrListNode

/// @details
///
AttrListNode::AttrListNode(void)
    : Node()
{}




//..............................................................................
////////////////////////////////////////////////////////////////////// WhereNode

/// @details
///
WhereNode::WhereNode(void)
    : Node()
{}




//..............................................................................
/////////////////////////////////////////////////////////////////////// FromNode

/// @details
///
FromNode::FromNode(void)
    : Node()
{}


//..............................................................................
//////////////////////////////////////////////////////////////////// AllAttrNode

/// @details
///
AllAttrNode::AllAttrNode(void)
    : Node()
{}




//..............................................................................
/////////////////////////////////////////////////////////////////////// ExprNode


/// @details
/// 
ExprNode::ExprNode(void)
    : Node(),
      m_mode()
{}

/// @details
/// 
void
ExprNode::init(ExprNode::mode m)
{
    this->m_mode = m;
}


/// @details
/// 
ExprNode::mode
ExprNode::data(void) const
{
    return this->m_mode;
}


String
ExprNode::str(void) const
{
    String s("ExprNode ");
    switch(m_mode)
    {
    case ExprNode::plus_expr:
        s.append("+"); break;
    case ExprNode::minus_expr:
        s.append("-"); break;
    case ExprNode::mul_expr:
        s.append("*"); break;
    case ExprNode::div_expr:
        s.append("/"); break;
    case ExprNode::concat_expr:
        s.append("||"); break;
    case ExprNode::lower_expr:
        s.append("<"); break;
    case ExprNode::greater_expr:
        s.append(">"); break;
    case ExprNode::lowerequal_expr:
        s.append("<="); break;
    case ExprNode::greaterequal_expr:
        s.append(">="); break;
    case ExprNode::equal_expr:
        s.append("="); break;
    case ExprNode::notequal_expr:
        s.append("!="); break;
    case ExprNode::isnull_expr:
        s.append("IS NULL"); break;
    case ExprNode::isnotnull_expr:
        s.append("IS NOT NULL"); break;
    case ExprNode::logor_expr:
        s.append("OR"); break;
    case ExprNode::logand_expr:
        s.append("AND"); break;
    case ExprNode::logxor_expr:
        s.append("XOR"); break;
    case ExprNode::mod_expr:
        s.append("MOD"); break;
    };
    return s;
}


//..............................................................................
//////////////////////////////////////////////////////////////////// LiteralNode

/// @details
/// 
LiteralNode::LiteralNode(void)
    : SimpleNode<String>()
{}



//..............................................................................
///////////////////////////////////////////////////////////////////// NumberNode

/// @details
/// 
NumberNode::NumberNode(void)
    : Node(),
      m_data()
{}


/// @details
/// 
void
NumberNode::init(String data)
{
    assert(data.length() > 0);
    //db::Variant v(TNumeric(data));
    this->m_data = TNumeric(data);
}


//..............................................................................
///////////////////////////////////////////////////////////////////////// IdNode

/// @details
/// 
IdNode::IdNode(void)
    : SimpleNode<Identifier>()
{}




//..............................................................................
////////////////////////////////////////////////////////////////////// ParseTree


/// @details
/// 
Token*
ParseTree::newToken(const Token& t)
{
    Token* tok = new Token(t);
    this->m_tokens.push_back(tok);
    return tok;
}



struct delete_ptr
{
    template<class T>
    inline void operator()(T& ref)
    {
        delete ref;
    }
};


/// @details
/// 
ParseTree::~ParseTree(void)
{
    std::for_each(m_nodes.begin(), m_nodes.end(), delete_ptr());
    std::for_each(m_tokens.begin(), m_tokens.end(), delete_ptr());
    std::for_each(m_nodelists.begin(), m_nodelists.end(), delete_ptr());
}


/// @details
/// 
void
ParseTree::raiseSyntaxError(void)
{
    if(this->m_tokens.size())
    {
        Token *t = *this->m_tokens.rbegin();
        throw SyntaxError(t);
    }
    else
        throw SyntaxError(0);
}



String ParseTree::gen_anonymous_id(void)
{
    std::stringstream ss;
    ss << ++this->m_anonymous_id_counter;
    return "anonymous_obj" + ss.str();
}






//..............................................................................
/////////////////////////////////////////////////////////////// PrintTreeVisitor

/// @details
/// 
PrintTreeVisitor::PrintTreeVisitor(Processor &proc, std::wostream &stream)
    : Visitor(Visitor::ignore_none),
      m_proc(proc),
      m_indent(""),
      m_stream(stream)
{}


/// @details
/// 
PrintTreeVisitor::PrintTreeVisitor(const PrintTreeVisitor& pt)
    : Visitor(Visitor::ignore_none),
      m_proc(pt.m_proc),
      m_indent(pt.m_indent),
      m_stream(pt.m_stream)
{
    m_indent.append("   ");
}


/// @details
/// 
void 
PrintTreeVisitor::next(Node *node)
{
    foreach_node(node->getChilds(), PrintTreeVisitor(*this), 1);
}



/// @details
/// 
void
PrintTreeVisitor::fallback_action(Node *node)
{
    m_stream << this->m_indent << node->str()  << std::endl;
    next(node);
}



/// @details
/// 
std::ostream& operator<<(std::ostream &o, const Identifier &i)
{
    o << i.name();
    return o;
}




SQLPROXY_NAMESPACE_END


//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
