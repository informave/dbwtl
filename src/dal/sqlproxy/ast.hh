//
// ast.hh - Abstract Syntax Tree
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
//

///
/// @cond DEV_DOCS
/// @file
/// @brief Abstract Syntax Tree
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_SQLPROXY_AST_HH
#define INFORMAVE_DB_SQLPROXY_AST_HH


#include "sqlproxy_fwd.hh"
#include "token.hh"

#include <list>

SQLPROXY_NAMESPACE_BEGIN


struct Identifier;
struct SemanticCheck;
struct Processor;
class Visitor;
class ParseTree;

struct Node;
struct IdNode;
struct TokenNode;
struct SelectNode;
struct AttrListNode;
struct AllAttrNode;
struct NumberNode;
struct LiteralNode;
struct NullNode;
struct AliasNode;
struct ExprNode;
struct ParamNode;
struct IdGroupNode;
struct FromNode;
struct WhereNode;
struct FuncCallNode;


typedef std::deque<Node*> NodeList;


String gen_anonymous_id(void);


//..............................................................................
/////////////////////////////////////////////////////////////////////////// Node
///
/// @since 0.0.1
/// @brief Node base class
class Node
{
public:
    typedef NodeList nodelist_type;

    Node(void);

    virtual ~Node(void);

    /// @brief Get all child nodes
    inline nodelist_type& getChilds(void)
    {
        return this->m_childs; 
    }

    /// @brief Add a new child
    void addChild(Node *child);

    /// @brief Add new childs
    void addChilds(const NodeList *list);

    /// @brief Get Source information
    SourceInfo getSourceInfo(void) const;

    /// @brief Update information
    void updateSourceInfo(const SourceInfo &info);

    /// @brief String representation (used for debugging)
    virtual String str(void) const = 0;

    /// @brief Visitor function
    virtual void accept(Visitor &visitor) = 0;

    virtual String nodetype(void) const = 0;

    static String name(void) { return "node"; }

    /// @brief Called by the semantic checker
    /// @details
    /// Each node may provide their semantic checks.
    virtual void semanticCheck(SemanticCheck &sc) { }

protected:
    nodelist_type m_childs;
    SourceInfo m_sinfo;
};



//..............................................................................
//////////////////////////////////////////////////////////////////////// Visitor
///
/// @since 0.0.1
/// @brief Visitor base class
class Visitor
{
public:
    typedef enum {
        ignore_none = 0,
        ignore_other = 1
    } visitor_mode;

    Visitor(visitor_mode mode = ignore_none);

    virtual void visit(IdNode *node);
    virtual void visit(SelectNode *node);
    virtual void visit(ParseTree *node);
    virtual void visit(TokenNode *node);
    virtual void visit(AttrListNode *node);
    virtual void visit(AllAttrNode *node);
    virtual void visit(NumberNode *node);
    virtual void visit(LiteralNode *node);
    virtual void visit(NullNode *node);
    virtual void visit(ExprNode *node);
    virtual void visit(FromNode *node);
    virtual void visit(WhereNode *node);
    virtual void visit(AliasNode *node);
    virtual void visit(FuncCallNode *node);
    virtual void visit(ParamNode *node);
    virtual void visit(IdGroupNode *node);

    void operator()(Node *node);

protected:
    /// @brief This method is called by each visit method defined
    /// in this base class.
    /// It throws an exception, but derived visitor implementations
    /// can override this method to provide a new fallback action.
    virtual void fallback_action(Node *node);

    visitor_mode m_mode;
};



//..............................................................................
///////////////////////////////////////////////////////////////////// Identifier
///
/// @since 0.0.1
/// @brief Identifier
struct Identifier
{
    Identifier() : m_name() { }

    Identifier(const String& name) : m_name(name)
    { }

    bool operator==(const Identifier &o) const
    { return this->m_name == o.m_name; }

    bool operator!=(const Identifier &o) const
    { return ! operator==(o); }

    bool operator<(const Identifier &o) const
    { return this->m_name < o.m_name; }

    String name(void) const
    {
        return this->m_name;
    }

    String str(void) const
    {
        return this->m_name;
    }

protected:
    String m_name;
};


//..............................................................................
/////////////////////////////////////////////////////////////////// SimpleNode<>
///
/// @since 0.0.1
/// @brief Base class for nodes which only have one data attribute
template<class T>
class SimpleNode : public Node
{
public:
    virtual ~SimpleNode(void)
    {}

    virtual void init(const T &data)
    {
        this->m_data = data;
    }

    virtual const T& data(void) const
    {
        return this->m_data;
    }

    virtual T& data(void)
    {
        return this->m_data;
    }

protected:
    SimpleNode(void) : Node(), m_data()
    {}

    T m_data;
};


// Specialization for <void>
template<>
class SimpleNode<void> : public Node
{
public:
    virtual ~SimpleNode(void)
    {}

    virtual String dump(void) const
    {
        return "voidnode";
    }

protected:
    SimpleNode(void) : Node()
    {}
};




//..............................................................................
///////////////////////////////////////////////////////////////////// SelectNode
///
/// @since 0.0.1
/// @brief Node for SELECT
struct SelectNode : public Node
{
    SelectNode(void);

    virtual ~SelectNode(void) {}

    //void init(void);

    virtual void accept(Visitor &visitor);

    virtual String str(void) const;

    virtual String nodetype(void) const;

    static String name(void) { return "SELECT"; }

};


//..............................................................................
/////////////////////////////////////////////////////////////////// AttrListNode
///
/// @since 0.0.1
/// @brief 
struct AttrListNode : public Node
{
    AttrListNode(void);

    virtual ~AttrListNode(void) {}

    virtual void accept(Visitor &visitor);

    virtual String str(void) const;
    
    virtual String nodetype(void) const;

    static String name(void) { return "AttrList"; }
};


//..............................................................................
//////////////////////////////////////////////////////////////////// AllAttrNode
///
/// @since 0.0.1
/// @brief 
struct AllAttrNode : public Node
{
    AllAttrNode(void);

    virtual ~AllAttrNode(void) {}

    virtual void accept(Visitor &visitor);

    virtual String str(void) const;
    
    virtual String nodetype(void) const;

    static String name(void) { return "AllAttr"; }
};



//..............................................................................
///////////////////////////////////////////////////////////////////////// IdNode
///
/// @since 0.0.1
/// @brief Node for Identifiers
struct IdNode : public SimpleNode<Identifier>
{
    IdNode(void);

    virtual ~IdNode(void) {}

    virtual void accept(Visitor &visitor);

    virtual String str(void) const;

    virtual String nodetype(void) const;

    static String name(void) { return "Identifier"; }
};





//..............................................................................
////////////////////////////////////////////////////////////////////// AliasNode
///
/// @since 0.0.1
/// @brief 
struct AliasNode : public SimpleNode<Identifier>
{

    virtual ~AliasNode(void) {}

    virtual void accept(Visitor &visitor);

    virtual String str(void) const;

    virtual String nodetype(void) const;

    static String name(void) { return "ALIAS"; }
};



//..............................................................................
/////////////////////////////////////////////////////////////////////// NullNode
///
/// @since 0.0.1
/// @brief Node for NULL
struct NullNode : public SimpleNode<void>
{
    virtual ~NullNode(void) {}

    virtual void accept(Visitor &visitor);

    virtual String str(void) const;

    virtual String nodetype(void) const;

    static String name(void) { return "NULL"; }
};


//..............................................................................
//////////////////////////////////////////////////////////////////// IdGroupNode
///
/// @since 0.0.1
/// @brief Node for ID groups (like db.dbo.mytable)
struct IdGroupNode : public SimpleNode<void>
{
    virtual ~IdGroupNode(void) {}

    virtual void accept(Visitor &visitor);

    virtual String str(void) const;

    virtual String nodetype(void) const;

    static String name(void) { return "NULL"; }
};



//..............................................................................
/////////////////////////////////////////////////////////////////// FuncCallNode
///
/// @since 0.0.1
/// @brief Node for function calls
struct FuncCallNode : public SimpleNode<Identifier>
{
    virtual ~FuncCallNode(void) {}

    virtual void accept(Visitor &visitor);

    virtual String str(void) const;

    virtual String nodetype(void) const;

    static String name(void) { return "FuncCall"; }
};


//..............................................................................
////////////////////////////////////////////////////////////////////// ParamNode
///
/// @since 0.0.1
/// @brief Node for Params (? Placeholders)
struct ParamNode : public SimpleNode<size_t>
{
    virtual ~ParamNode(void) {}

    virtual void accept(Visitor &visitor);

    virtual String str(void) const;

    virtual String nodetype(void) const;

    static String name(void) { return "Param"; }
};










//..............................................................................
//////////////////////////////////////////////////////////////////// LiteralNode
///
/// @since 0.0.1
/// @brief Node for Literals
struct LiteralNode : public SimpleNode<String>
{
    LiteralNode(void);

    virtual ~LiteralNode(void) {}

    virtual void accept(Visitor &visitor);

    virtual String str(void) const;

    virtual String nodetype(void) const;

    static String name(void) { return "Literal"; }
};


//..............................................................................
///////////////////////////////////////////////////////////////////// NumberNode
///
/// @since 0.0.1
/// @brief Node for Numbers
struct NumberNode : public Node
{
    NumberNode(void);

    virtual ~NumberNode(void) {}

    void init(String data);

    virtual void accept(Visitor &visitor);

    virtual String str(void) const;

    inline TNumeric data(void) const { return m_data; }

    virtual String nodetype(void) const;

    static String name(void) { return "Number"; }

protected:
    TNumeric m_data;
};





//..............................................................................
////////////////////////////////////////////////////////////////////// WhereNode
///
/// @since 0.0.1
/// @brief 
struct WhereNode : public Node
{
    WhereNode(void);

    virtual ~WhereNode(void)
    {}

    virtual void accept(Visitor &visitor);

    virtual String str(void) const;

    virtual String nodetype(void) const;

    static String name(void) { return "WHERE"; }
};



//..............................................................................
/////////////////////////////////////////////////////////////////////// FromNode
///
/// @since 0.0.1
/// @brief 
struct FromNode : public Node
{
    FromNode(void);

    virtual ~FromNode(void)
    {}

    //void init(Identifier _id, String type);

    virtual void accept(Visitor &visitor);

    virtual String str(void) const;

    virtual String nodetype(void) const;

    static String name(void) { return "FROM"; }

    //virtual void semanticCheck(SemanticCheck &sc);
};


//..............................................................................
////////////////////////////////////////////////////////////////////// TokenNode
///
/// @since 0.0.1
/// @brief Node for Tokens
struct TokenNode : public Node
{
    TokenNode(Token *tok);

    virtual ~TokenNode(void) {}

    virtual void accept(Visitor &visitor);

    virtual String str(void) const;
    
    virtual String nodetype(void) const;

    static String name(void) { return "TOKEN"; }

protected:
    Token *m_token;

private:
    TokenNode(const TokenNode&);
    TokenNode& operator=(const TokenNode&);
};



//..............................................................................
/////////////////////////////////////////////////////////////////////// ExprNode
///
/// @since 0.0.1
/// @brief Node for Expressions
struct ExprNode : public Node
{
    enum mode { plus_expr, minus_expr, mul_expr, div_expr, concat_expr, lower_expr,
                greater_expr, lowerequal_expr, greaterequal_expr, equal_expr, notequal_expr,
                isnull_expr, isnotnull_expr, logor_expr, logand_expr, logxor_expr, mod_expr,
                not_expr, plus_op, minus_op
    };

    ExprNode(void);

    virtual ~ExprNode(void)
    {}

    void init(enum mode m);

    virtual void accept(Visitor &visitor);

    virtual String str(void) const;

    enum mode data(void) const;

    virtual String nodetype(void) const;

    static String name(void) { return "Expr"; }

protected:
    enum mode m_mode;
};



//..............................................................................
////////////////////////////////////////////////////////////////////// ParseTree
///
/// @since 0.0.1
/// @brief Prase Tree
class ParseTree : public Node
{
public:
    ParseTree(void)
        : m_nodelists(),
          m_nodes(),
          m_tokens(),
          m_anonymous_id_counter(0),
          m_next_param(1)
    {}

    virtual ~ParseTree(void);

    virtual void accept(Visitor &visitor);

    virtual String str(void) const;

    virtual String nodetype(void) const;
    static String name(void) { return "Parsetree"; }

    String gen_anonymous_id(void);

    virtual void raiseSyntaxError(void);

    virtual Token* newToken(const Token &t);

    inline NodeList* newNodeList(void)
    {
        NodeList* n = new NodeList();
        m_nodelists.push_back(n);
        return n;
    }

    /// @brief Create a new token node
    inline TokenNode* newTokenNode(Token *t)
    {
        TokenNode *node = new TokenNode(t);
        m_nodes.push_back(node);
        return node;
    }

    inline size_t nextParamNum(void)
        {
            return this->m_next_param++;
        }
   

    /// @brief Create new node and register it in the node list
    template<typename T>
    inline T* newNode(void)
    {
        T *tmp = new T();
        m_nodes.push_back(tmp); 
        return tmp;
    }

protected:
    /// @brief All allocated nodelists, cleaned up by the destructor
    std::list<NodeList*>  m_nodelists;

    /// @brief All allocated nodes, cleaned up by the destructor
    NodeList              m_nodes;

    /// @brief All allocated tokens, cleaned up by the destructor
    std::list<Token*>     m_tokens;

    size_t                m_anonymous_id_counter;

    size_t                m_next_param;

private:
    ParseTree(const ParseTree&);
    ParseTree& operator=(const ParseTree&);
};



/// @details
/// 
template<typename Op>
inline void foreach_node(Node::nodelist_type &nodes, Op op, int deep = -1)
{
    if(deep != 0)
    {
        Node::nodelist_type childs = nodes;
        for(Node::nodelist_type::iterator i = childs.begin();
            i != childs.end();
            ++i)
        {
            foreach_node(*i, op, deep);
        }
    }
}


/// @details
/// 
template<typename Op>
inline void foreach_node(Node *node, Op op, int deep = -1)
{
    if(deep != 0)
    {
        op(node);
        if(deep > 0)
            --deep;

        if(deep != 0)
        {
            Node::nodelist_type childs = node->getChilds();
            
            for(Node::nodelist_type::iterator i = childs.begin();
                i != childs.end();
                ++i)
            {
                foreach_node(*i, op, deep);
            }
        }
    }
}


/// @details
/// 
template<class Op>
inline void apply_visitor(Node *node, Op op)
{
    foreach_node(node, op, 1);
}



/// @details
/// 
template<class T>
std::list<T*> find_nodes(Node *node, int deep = -1)
{
    std::list<T*> nlist;

    for(Node::nodelist_type::iterator i = node->getChilds().begin();
        i != node->getChilds().end();
        ++i)
    {
        T* tmp = dynamic_cast<T*>(*i);
        if(tmp)
            nlist.push_back(tmp);
    }

    return nlist;
}


/// @details
/// 
template<class T>
T* find_node(Node *node, int deep = -1)
{
    for(Node::nodelist_type::iterator i = node->getChilds().begin();
        i != node->getChilds().end();
        ++i)
    {
        T* tmp = dynamic_cast<T*>(*i);
        if(tmp)
            return tmp;
    }

    return 0;
}


/// @details
/// 
template<typename T>
T* node_cast(Node *node)
{
    T* tmp = dynamic_cast<T*>(node);
    if(!tmp)
	{
        //throw std::runtime_error("invalid node cast");
        String s = "Type error: expecting `";
		s.append(T::name());
		s.append("` but got `");
        s.append(node->nodetype());
		s.append("` at position ");
		s.append(node->getSourceInfo().str());
		throw std::runtime_error(s);
	}
    return tmp;
}




/// @details
/// 
template<typename T>
inline T*
find_node_byid(Node* node, Identifier id, int deep = -1)
{
    for(Node::nodelist_type::iterator i = node->getChilds().begin();
        i != node->getChilds().end();
        ++i)
    {
        T* tmp = dynamic_cast<T*>(*i);
        if(tmp)
        {
            if(tmp->id == id)
                return tmp;
        }
    }
    return 0;
}




/// @details
///
template<typename T>
inline bool is_nodetype(Node *node)
{
    return dynamic_cast<T>(node) != 0 ? true : false;
}



//..............................................................................
/////////////////////////////////////////////////////////////// PrintTreeVisitor
///
/// @since 0.0.1
/// @brief Pretty printer for AST
struct PrintTreeVisitor : public Visitor
{
public:
    PrintTreeVisitor(Processor &proc, std::wostream &stream);

    PrintTreeVisitor(const PrintTreeVisitor& pt);

    mutable Processor &m_proc;
    String m_indent;
    mutable std::wostream &m_stream;
    
    void next(Node *node);
       
    virtual void fallback_action(Node *node);
};



std::ostream& operator<<(std::ostream &o, const Identifier &i);

/*

class ParseTree
{
public:
Token*
newToken(const Token& t)
{
    Token* tok = new Token(t);
    this->m_tokens.push_back(tok);
    return tok;
}

std::list<Token*>     m_tokens;

};
*/

SQLPROXY_NAMESPACE_END




#endif

