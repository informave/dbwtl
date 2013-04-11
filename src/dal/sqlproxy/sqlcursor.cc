//
// sqlcursor.cc - Cursor wrapper (definition)
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
/// @brief Firebird Backend libfirebird
/// @author Daniel Vogelbacher
/// @since 0.0.1


#include "sqlcursor.hh"
#include "sqlparser.hh"

#include "../../utils.hh"
#include "../dal_debug.hh"

#include <cassert>
#include <iostream>


SQLPROXY_NAMESPACE_BEGIN


//..............................................................................
/////////////////////////////////////////////////////////////////////// safe_ptr
///
/// @since 0.0.1
/// @brief Safe pointer
template<typename T>
class safe_ptr
{
public:
    safe_ptr(T* p = 0) : m_pointee(p)
    {}

    safe_ptr& operator=(T *p)
    {
        this->m_pointee = p;
        return *this;
    }

    operator bool(void) const
    {
        return m_pointee != 0;
    }

    T* operator->(void)
    {
        assert(m_pointee);
        return m_pointee;
    }

    T& operator*(void)
    {
        return *this->m_pointee;
    }

    T* get(void)
    {
        return this->m_pointee;
    }

private:
    T *m_pointee;
};




//..............................................................................
///////////////////////////////////////////////////////////////////// ParamField
///
/// @since 0.0.1
/// @brief 
class ParamField : public Field
{
    friend class ParserAdaptor;

public:
    ParamField(size_t param_num, SqlCursor &cursor) : m_param_num(param_num),
                                                      m_cursor(cursor)
    {}

    virtual const Variant& getValue(void);

protected:
    size_t m_param_num;
    SqlCursor &m_cursor;
};


//..............................................................................
//////////////////////////////////////////////////////////////////// StaticField
///
/// @since 0.0.1
/// @brief 
class StaticField : public Field
{
    friend class ParserAdaptor;

public:
    StaticField(const Variant &var) : m_value(var)
    {}
    
    virtual const Variant& getValue(void)
    {
        return m_value;
    }

protected:
    Variant m_value;
};


//..............................................................................
////////////////////////////////////////////////////////////////// ComputedField
///
/// @since 0.0.1
/// @brief 
class ComputedField : public Field
{
    friend class ParserAdaptor;

public:
    ComputedField(SqlCursor &cursor, ExprNode *node)
        : m_cursor(cursor),
          m_node(node), 
          m_value()
    {}

    virtual const Variant& getValue(void);

protected:
    SqlCursor &m_cursor;
    ExprNode *m_node;
    Variant m_value;

private:
    ComputedField(const ComputedField&);
    ComputedField& operator=(const ComputedField&);
};



//..............................................................................
////////////////////////////////////////////////////////////////////// DataField
///
/// @since 0.0.1
/// @brief 
class DataField : public Field
{
    friend class ParserAdaptor;

public:
    DataField(colnum_t colnum, DBObject &obj)
        : m_colnum(colnum),
          m_obj(obj)
    {
        const IColumnDesc &desc = this->m_obj.prov->describeColumn(m_colnum);
        
        m_name = desc.getName();
        m_catalog_name = desc.getCatalogName();
        m_schema_name = desc.getSchemaName();
        m_base_column_name = desc.getBaseColumnName();
        m_type_name = desc.getTypeName();
        m_base_table_name = desc.getBaseTableName();
        m_size = desc.getSize();
        m_comment = desc.getComment();
        m_is_nullable = desc.getIsNullable();
        m_precision = desc.getPrecision();
        m_scale = desc.getScale();
        m_is_searchable = desc.getIsSearchable();
    }

    virtual daltype_t getDatatype(void) const
    {
        return this->m_obj.prov->describeColumn(m_colnum).getDatatype();
    }


    virtual const Variant& getValue(void)
    {
        return m_obj.prov->column(m_colnum);
    }

protected:
    colnum_t m_colnum;
    DBObject &m_obj;
};


//..............................................................................
////////////////////////////////////////////////////////////////// ParserAdaptor
///
/// @since 0.0.1
/// @brief 
class ParserAdaptor
{
public:
    ParserAdaptor(SqlCursor &cursor);

    void registerSourceObject(String alias, ObjectName name);
    
    void createProviders(void);

    void addAllColumns(void);

    void addStaticField(const String &alias, const Variant &var);
    void addDataField(colnum_t colnum, DBObject &obj, const String &alias = String());
    void addDataField(const String &name, const String &source, const String &alias = String());
    void addParamField(const String &alias, size_t param_num);
    void addMappingField(IdGroupNode *node, const String &name, const String &source);
    void addComputedField(ExprNode *node, const String &alias = String());

    inline String createAlias(void)
    {
        std::stringstream ss;
        ss << "expr" << ++m_alias_counter;
        return ss.str();
    }

    inline void registerWhereNode(WhereNode *node)
    {
        this->cursor().m_where_node = node;
    }

    inline SqlCursor& cursor(void)
    {
        return this->m_cursor;
    }


protected:
    SqlCursor &m_cursor;

    size_t m_alias_counter;
    
private:
    const Variant m_null_variant;
    ParserAdaptor(const ParserAdaptor&);
};




//..............................................................................
/////////////////////////////////////////////////////////////// PrintTreeVisitor
///
/// @since 0.0.1
/// @brief Pretty printer for AST
struct XPrintTreeVisitor : public Visitor
{
public:
    XPrintTreeVisitor(std::wostream &stream)
        : m_indent(L""),
          m_stream(stream)
    {}

    XPrintTreeVisitor(const XPrintTreeVisitor& pt, bool nextlevel = false)
        : Visitor(),
          m_indent(pt.m_indent),
          m_stream(pt.m_stream)
    {
        if(nextlevel)
        {
            std::replace( m_indent.begin(), m_indent.end(), '-', ' ');
            std::replace( m_indent.begin(), m_indent.end(), '+', '|');
            m_indent.append(L"+--- ");
        }
    }

    

    std::wstring m_indent;
    std::wostream &m_stream;
           
    virtual void fallback_action(Node *node)
    {
        m_stream << this->m_indent << node->str()  << std::endl;
        foreach_node(node->getChilds(), XPrintTreeVisitor(*this, true), 1);
    }
};



//..............................................................................
//////////////////////////////////////////////////////////////// ExprEvalVisitor
///
/// @since 0.0.1
/// @brief Evaluates an expression
struct ExprEvalVisitor : public Visitor
{
public:
    ExprEvalVisitor(SqlCursor &cursor, Variant &value)
        : Visitor(),
          m_cursor(cursor),
          m_value(value)
    {}



    virtual void visit(IdGroupNode *node)
    {
        Field& f = m_cursor.findField(node);
        m_value = f.getValue();
        //assert(0==1);
    }


    virtual void visit(NullNode *node)
    {
        m_value = Variant();
    }

    virtual void visit(ParamNode *node)
    {
        size_t i = this->m_cursor.findParam(node);
        assert(i > 0);
        m_value = this->m_cursor.getUserParam(i);
    }

    virtual void visit(LiteralNode *node)
    {
        m_value = node->data();
    }

    virtual void visit(NumberNode *node)
    {
        m_value = node->data();
    }

    virtual void visit(ExprNode *node)
    {
        Variant op1, op2;

        switch(node->data())
        {
        case ExprNode::plus_expr:
            assert(node->getChilds().size() == 2);
            apply_visitor(node->getChilds()[0], ExprEvalVisitor(m_cursor, op1));
            apply_visitor(node->getChilds()[1], ExprEvalVisitor(m_cursor, op2));
            m_value = op1.get<TNumeric>() + op2.get<TNumeric>();
            return;
        case ExprNode::minus_expr:
            assert(node->getChilds().size() == 2);
            apply_visitor(node->getChilds()[0], ExprEvalVisitor(m_cursor, op1));
            apply_visitor(node->getChilds()[1], ExprEvalVisitor(m_cursor, op2));
            m_value = op1.get<TNumeric>() - op2.get<TNumeric>();
            return;
        case ExprNode::mul_expr:
            assert(node->getChilds().size() == 2);
            apply_visitor(node->getChilds()[0], ExprEvalVisitor(m_cursor, op1));
            apply_visitor(node->getChilds()[1], ExprEvalVisitor(m_cursor, op2));
            m_value = op1.get<TNumeric>() * op2.get<TNumeric>();
            return;
        case ExprNode::div_expr:
            assert(node->getChilds().size() == 2);
            apply_visitor(node->getChilds()[0], ExprEvalVisitor(m_cursor, op1));
            apply_visitor(node->getChilds()[1], ExprEvalVisitor(m_cursor, op2));
            m_value = op1.get<TNumeric>() * op2.get<TNumeric>();
            return;
        case ExprNode::concat_expr:
            assert(node->getChilds().size() == 2);
            apply_visitor(node->getChilds()[0], ExprEvalVisitor(m_cursor, op1));
            apply_visitor(node->getChilds()[1], ExprEvalVisitor(m_cursor, op2));
            m_value = op1.get<String>() + op2.get<String>();
            return;
        case ExprNode::lower_expr:
            assert(node->getChilds().size() == 2);
            apply_visitor(node->getChilds()[0], ExprEvalVisitor(m_cursor, op1));
            apply_visitor(node->getChilds()[1], ExprEvalVisitor(m_cursor, op2));
            m_value = op1.get<TNumeric>() < op2.get<TNumeric>();
            return;
        case ExprNode::greater_expr:
            assert(node->getChilds().size() == 2);
            apply_visitor(node->getChilds()[0], ExprEvalVisitor(m_cursor, op1));
            apply_visitor(node->getChilds()[1], ExprEvalVisitor(m_cursor, op2));
            m_value = op1.get<TNumeric>() > op2.get<TNumeric>();
            return;
        case ExprNode::lowerequal_expr:
            assert(node->getChilds().size() == 2);
            apply_visitor(node->getChilds()[0], ExprEvalVisitor(m_cursor, op1));
            apply_visitor(node->getChilds()[1], ExprEvalVisitor(m_cursor, op2));
            m_value = op1.get<TNumeric>() <= op2.get<TNumeric>();
            return;
        case ExprNode::greaterequal_expr:
            assert(node->getChilds().size() == 2);
            apply_visitor(node->getChilds()[0], ExprEvalVisitor(m_cursor, op1));
            apply_visitor(node->getChilds()[1], ExprEvalVisitor(m_cursor, op2));
            m_value = op1.get<TNumeric>() >= op2.get<TNumeric>();
            return;
        case ExprNode::equal_expr:
            assert(node->getChilds().size() == 2);
            apply_visitor(node->getChilds()[0], ExprEvalVisitor(m_cursor, op1));
            apply_visitor(node->getChilds()[1], ExprEvalVisitor(m_cursor, op2));
            m_value = op1.get<TNumeric>() == op2.get<TNumeric>();
            return;
        case ExprNode::notequal_expr:
            assert(node->getChilds().size() == 2);
            apply_visitor(node->getChilds()[0], ExprEvalVisitor(m_cursor, op1));
            apply_visitor(node->getChilds()[1], ExprEvalVisitor(m_cursor, op2));
            m_value = op1.get<TNumeric>() != op2.get<TNumeric>();
            return;
        case ExprNode::isnull_expr:
            assert(node->getChilds().size() == 1);
            apply_visitor(node->getChilds()[0], ExprEvalVisitor(m_cursor, op1));
            m_value = op1.isnull();
            return;
        case ExprNode::isnotnull_expr:
            assert(node->getChilds().size() == 1);
            apply_visitor(node->getChilds()[0], ExprEvalVisitor(m_cursor, op1));
            m_value = ! op1.isnull();
            return;
        case ExprNode::logor_expr:
            assert(node->getChilds().size() == 2);
            apply_visitor(node->getChilds()[0], ExprEvalVisitor(m_cursor, op1));
            apply_visitor(node->getChilds()[1], ExprEvalVisitor(m_cursor, op2));
            m_value = op1.get<bool>() || op2.get<bool>();
            return;
        case ExprNode::logand_expr:
            assert(node->getChilds().size() == 2);
            apply_visitor(node->getChilds()[0], ExprEvalVisitor(m_cursor, op1));
            apply_visitor(node->getChilds()[1], ExprEvalVisitor(m_cursor, op2));
            m_value = op1.get<bool>() && op2.get<bool>();
            return;
        case ExprNode::logxor_expr:
            assert(node->getChilds().size() == 2);
            apply_visitor(node->getChilds()[0], ExprEvalVisitor(m_cursor, op1));
            apply_visitor(node->getChilds()[1], ExprEvalVisitor(m_cursor, op2));
            m_value = op1.get<bool>() ^ op2.get<bool>();
            return;
        case ExprNode::mod_expr:
            assert(node->getChilds().size() == 2);
            apply_visitor(node->getChilds()[0], ExprEvalVisitor(m_cursor, op1));
            apply_visitor(node->getChilds()[1], ExprEvalVisitor(m_cursor, op2));
            //m_value = op1.get<TNumeric>() % op2.get<TNumeric>();
            throw std::runtime_error("BUG"); /// @bug here
            return;
        case ExprNode::not_expr:
            assert(node->getChilds().size() == 1);
            apply_visitor(node->getChilds()[0], ExprEvalVisitor(m_cursor, op1));
            m_value = ! op1.get<bool>();
            return;
        case ExprNode::plus_op:
            assert(node->getChilds().size() == 1);
            apply_visitor(node->getChilds()[0], ExprEvalVisitor(m_cursor, op1));
            m_value = + op1.get<int>(); /// @bug type int
            return;
        case ExprNode::minus_op:
            assert(node->getChilds().size() == 1);
            apply_visitor(node->getChilds()[0], ExprEvalVisitor(m_cursor, op1));
            m_value = - op1.get<int>(); /// @bug
            return;
        }

        std::cout << "EXPR" << std::endl;
        m_value = Variant(1);
    }

protected:
    SqlCursor &m_cursor;
    Variant &m_value;
};




//..............................................................................
/////////////////////////////////////////////////////////////// IdMappingVisitor
///
/// @since 0.0.1
/// @brief 
struct IdMappingVisitor : public Visitor
{
public:
    IdMappingVisitor(ParserAdaptor &pa)
    : Visitor(ignore_other),
      adaptor(pa)
    {}


    virtual void visit(IdGroupNode *node)
    {
        String names[2];
        size_t i = 0;

        std::for_each(node->getChilds().rbegin(),
                      node->getChilds().rend(),
                      [&](Node* n)
                      {
                          if(i < 2)
                          {
                              names[i++] = node_cast<IdNode>(n)->data().str();
                          }
                          else
                          {
                              DBWTL_BUG(); /// @bug todo
                          }
                      });
        
        adaptor.addMappingField(node, names[0], names[1]);

    }

    ParserAdaptor &adaptor;
};


//..............................................................................
//////////////////////////////////////////////////////////////// AttrListVisitor
///
/// @since 0.0.1
/// @brief 
struct AttrListVisitor : public Visitor
{
public:
    AttrListVisitor(ParserAdaptor &pa, String alias = String()) 
        : Visitor(ignore_other),
          adaptor(pa),
          m_attr_alias(alias)
    {
    }

    virtual void visit(AliasNode *node)
    {
        std::cout << "AttrList" << std::endl;
        apply_visitor(node->getChilds()[0], AttrListVisitor(adaptor, node->data().str()));
    }


    virtual void visit(AllAttrNode *node)
    {
        adaptor.addAllColumns();
    }


    virtual void visit(LiteralNode *node)
    {
        adaptor.addStaticField(m_attr_alias.empty() ? adaptor.createAlias() : m_attr_alias,
                               Variant(node->data()));
    }


    virtual void visit(NumberNode *node)
    {
        adaptor.addStaticField(m_attr_alias.empty() ? adaptor.createAlias() : m_attr_alias,
                               Variant(node->data()));
    }

    virtual void visit(NullNode *node)
    {
        adaptor.addStaticField(m_attr_alias.empty() ? adaptor.createAlias() : m_attr_alias,
                               Variant());
    }


    virtual void visit(ParamNode *node)
    {
        adaptor.addParamField(m_attr_alias.empty() ? adaptor.createAlias() : m_attr_alias,
                              adaptor.cursor().findParam(node));
    }

    virtual void visit(IdGroupNode *node)
    {
        String names[2];
        size_t i = 0;

        std::for_each(node->getChilds().rbegin(),
                      node->getChilds().rend(),
                      [&](Node* n)
                      {
                          if(i < 2)
                          {
                              names[i++] = node_cast<IdNode>(n)->data().str();
                          }
                          else
                          {
                              DBWTL_BUG(); /// @bug todo
                          }
                      });
        
        adaptor.addDataField(names[0], names[1], m_attr_alias);
    }


    virtual void visit(ExprNode *node)
    {
        foreach_node(node->getChilds(), IdMappingVisitor(adaptor), -1);

        adaptor.addComputedField(node, m_attr_alias.empty() ? adaptor.createAlias() : m_attr_alias);
    }


    virtual void fallback_action(Node *node)
    {
        //std::cout << "AttrList" << std::endl;
        throw std::runtime_error("expressions not supported");
    }

    ParserAdaptor &adaptor;
    String m_attr_alias;
};


//..............................................................................
/////////////////////////////////////////////////////////////////// ParamVisitor
///
/// @since 0.0.1
/// @brief 
struct ParamVisitor : public Visitor
{
public:
    ParamVisitor(ParserAdaptor &pa) 
    : Visitor(ignore_other),
      adaptor(pa)
    {
    }

    virtual void visit(ParamNode *node)
    {
        size_t i = adaptor.cursor().m_params.size();
        adaptor.cursor().m_params[node] = i +1;
    }

    ParserAdaptor &adaptor;
};



//..............................................................................
////////////////////////////////////////////////////////////////// SourceVisitor
///
/// @since 0.0.1
/// @brief 
struct SourceVisitor : public Visitor
{
public:
    SourceVisitor(ParserAdaptor &pa) 
    : Visitor(),
      adaptor(pa)
    {
    }

    virtual void visit(IdGroupNode *node)
    {
        String names[3];
        size_t i = 0;

        std::for_each(node->getChilds().rbegin(),
                      node->getChilds().rend(),
                      [&](Node* n)
                      {
                          if(i < 3)
                          {
                              names[i++] = node_cast<IdNode>(n)->data().str();
                          }
                          else
                          {
                              DBWTL_BUG(); /// @bug todo
                          }
                      });
        
        ObjectName x(names[0], names[1], names[2]);
        adaptor.registerSourceObject(x.table(), x);
        std::cout << "Source: " << x.table() << std::endl;
    }


    virtual void visit(AliasNode *node)
    {
        String names[3];
        size_t i = 0;

        IdGroupNode *idgroup = find_node<IdGroupNode>(node);

        std::for_each(idgroup->getChilds().rbegin(),
                      idgroup->getChilds().rend(),
                      [&](Node* n)
                      {
                          if(i < 3)
                          {
                              names[i++] = node_cast<IdNode>(n)->data().str();
                          }
                          else
                          {
                              DBWTL_BUG(); /// @bug todo
                          }
                      });
        
        ObjectName x(names[0], names[1], names[2]);
        adaptor.registerSourceObject(node->data().str(), x);
        std::cout << "Source: " << node->data().str() << std::endl;
    }

    ParserAdaptor &adaptor;
};




//..............................................................................
////////////////////////////////////////////////////////////// PrepareSqlVisitor
///
/// @since 0.0.1
/// @brief 
struct PrepareSqlVisitor : public Visitor
{
public:
    PrepareSqlVisitor(ParserAdaptor &pa) 
    : Visitor(),
      adaptor(pa)
    {}


    virtual void visit(SelectNode *node)
    {
        // search for all source objects
        safe_ptr<FromNode> from = find_node<FromNode>(node);
        foreach_node(from->getChilds(), SourceVisitor(adaptor), 1);
        adaptor.createProviders();

        // search for all params
        foreach_node(node, ParamVisitor(adaptor), -1);

        // collect attributes
        safe_ptr<AttrListNode> attrs = find_node<AttrListNode>(node);
        foreach_node(attrs->getChilds(), AttrListVisitor(adaptor), 1);
        //cursor.createMetadata(); 


        WhereNode *where = find_node<WhereNode>(node);
	if(where)
	{
        	adaptor.registerWhereNode(where);
        	foreach_node(where->getChilds(), IdMappingVisitor(adaptor), -1);
	}

        std::cout << "END SQL" << std::endl;
    }

    ParserAdaptor &adaptor;
};



//..............................................................................
////////////////////////////////////////////////////////////////// ComputedField

/// @details
///
const Variant&
ComputedField::getValue(void)
{
    apply_visitor(m_node, ExprEvalVisitor(m_cursor, m_value));
    return m_value;
}



//..............................................................................
///////////////////////////////////////////////////////////////////// ParamField

/// @details
/// 
const Variant&
ParamField::getValue(void)
{
    return this->m_cursor.getUserParam(m_param_num);
    //return *new Variant(String("param"));
}



//..............................................................................
/////////////////////////////////////////////////////////////////////// DBObject

/// @details
/// 
DBObject::DBObject(const String &alias, const ObjectName &name)
    : alias(alias), name(name), prov(0)
{
}




//..............................................................................
////////////////////////////////////////////////////////////////////// SqlCursor


/// @details
/// 
bool
SqlCursor::matchCondition(void)
{
    if(!m_where_node)
        return true;
    else
    {
        Variant value;
        apply_visitor(m_where_node->getChilds()[0], ExprEvalVisitor(*this, value));
        return value.get<bool>();
    }


/*
  safe_ptr<FromNode> from = find_node<FromNode>(this->m_tree->);
  foreach_node(from->getChilds(), SourceVisitor(adaptor), 1);
  adaptor.createProviders();
*/

}


/// @details
/// 
bool
SqlCursor::next(void)
{
    bool res;
    do
    {
        res = internalNext();
    }
    while(res && !matchCondition());
    return res;
}


/// @details
/// 
bool
SqlCursor::internalNext(void)
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    // Start with the last source and iterate to the first source
    // until we found a source which is not EOF after next().
    // If the first source is EOF, all data is retrieved.
    for(typename object_vector_type::reverse_iterator p = this->m_objects_vector.rbegin();
        p != this->m_objects_vector.rend();
        ++p)
    {
        (*p)->prov->next();
        if((*p)->prov->eof())
        {
            if(*p == *this->m_objects_vector.begin()) // first source is EOF
            {
                DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_EOF);   
                return false;                                        
            }
            else // reopen current source and move to previous source
            {
                (*p)->prov->close();
                (*p)->prov->open(IDataProvider::MODE_READ,
                                 (*p)->name.table(),
                                 (*p)->name.schema(),
                                 (*p)->name.catalog());
                (*p)->prov->first();
                continue;
            }
        }
        else // found record
        {
            return true;
        }
    }

    DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_EOF);   
    return false;               
}


/// @details
/// 
const Variant&
SqlCursor::getUserParam(size_t param_num)
{
    if(this->m_user_params)
    {
        StmtBase::ParamMap &p = *this->m_user_params;
        StmtBase::ParamMap::iterator i = p.find(param_num);
        if(i != p.end() && i->second)
        {
            return *i->second;
        }
        else
            return m_null_variant;
    }
    else
        return m_null_variant;
}


/// @details
/// 
bool
SqlCursor::eof(void) const
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    if(! (this->m_cursorstate & DAL_CURSOR_POSITIONED))
    {
/*
  this->appendDiagRec(CREATE_DIAG(DAL_STATE_ERROR, 24000,
  String("Dataset is not positioned."),
  "Hint: Use the first() method to position the cursor "
  "until calling eof()."))
  .raiseException();
*/
        //assert(0 == 1); /// @bug fixme
    }

    return this->m_cursorstate & DAL_CURSOR_EOF;
    //return this->m_last_row_status != 0; // a valid row returns status 0
}


/// @details
/// 
void
SqlCursor::close(void)
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    this->m_fields.clear();
    //this->m_params.clear();

    this->m_objects.clear();
    this->m_objects_vector.clear();
}


/// @details
/// 
size_t
SqlCursor::paramCount(void) const
{
    return this->m_params.size();
}


/// @details
/// 
rowcount_t
SqlCursor::rowCount(void) const
{
    return -1;
}


/// @details
/// 
const SqlCursor::value_type&
SqlCursor::column(colnum_t num)
{
    return this->findField(num).getValue();
}


/// @details
/// 
const SqlCursor::value_type&
SqlCursor::column(String name)
{
    return this->findField(this->columnID(name)).getValue();
}


/// @details
/// 
size_t
SqlCursor::columnCount(void) const
{
    return this->m_fields.size();
}


/// @details
/// 
colnum_t
SqlCursor::columnID(String name) const
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    for(colnum_t i = 1; i <= this->columnCount(); ++i)
    {
        if(name == this->columnName(i))
            return i;
    }
    throw ex::not_found(US("Column '") + String::Internal(name) + US("' not found."));
}


/// @details
/// 
String
SqlCursor::columnName(colnum_t num) const
{
    return this->findField(num).getName();
}


/// @details
/// 
const IColumnDesc&
SqlCursor::describeColumn(colnum_t num) const
{
    return this->findField(num);
}


/// @details
/// 
const IColumnDesc&
SqlCursor::describeColumn(String name) const
{
    return this->describeColumn(this->columnID(name));
}


/// @details
/// 
Variant
SqlCursor::lastInsertRowId(void)
{
    return Variant();
}


/// @details
/// 
rowcount_t
SqlCursor::affectedRows(void) const
{
    return -1;
}


/// @details
/// 
Field&
SqlCursor::findField(colnum_t num)
{
    field_map_type::iterator i = this->m_fields.find(num);
    if(i != this->m_fields.end())
        return *i->second;
    else
        throw ex::engine_error("field not found");
}


/// @details
/// 
const Field&
SqlCursor::findField(colnum_t num) const
{
    field_map_type::const_iterator i = this->m_fields.find(num);
    if(i != this->m_fields.end())
        return *i->second;
    else
        throw ex::engine_error("field not found");
}



/// @details
/// 
SqlCursor::SqlCursor(IStmt &stmt)
    : 
    m_objects(),
    m_objects_vector(),
    m_fields(),
    m_identifiers(),
    m_params(),
    m_tree(),
    m_stmt(stmt),
    m_user_params(0),
    m_null_variant(),
    m_where_node(0)
{
}


/// @details
/// 
void
SqlCursor::first(void)
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

/*
  if(!(this->m_cursorstate & DAL_CURSOR_POSITIONED))
  this->next();
  else if(this->eof() || this->m_current_tuple != 1)
  {
  throw ex::engine_error("can't scroll to first record");
  }
*/
    std::for_each(this->m_objects_vector.begin(),
                  this->m_objects_vector.end(),
                  [](typename object_vector_type::value_type i)
                  {
                      i->prov->first();
                  });
    typename object_vector_type::iterator i = this->m_objects_vector.begin();
    
    DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_POSITIONED);

    while((*i)->prov->eof())
    {
        if(++i == this->m_objects_vector.end())
        {
            DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_EOF);   
            break;
        }
    }


    if(!matchCondition())
        next(); // checks matchCondition, too.
}


/// @details
/// 
Field&
SqlCursor::findField(IdGroupNode* nodekey)
{
    identifier_map_type::iterator i = this->m_identifiers.find(nodekey);
    if(i != this->m_identifiers.end())
    {
        assert(i->second);
        return *i->second;
    }
    else
        throw ex::engine_error("nodekey not found");
}


/// @details
/// 
size_t
SqlCursor::findParam(ParamNode *nodekey)
{
    param_map_type::iterator i = this->m_params.find(nodekey);
    if(i != this->m_params.end())
    {
        assert(i->second > 0);
        return i->second;
    }
    else
        throw ex::engine_error("nodekey not found");
}


/// @details
/// 
void
SqlCursor::prepare(String sql)
{

    SqlParser sp;
    ParserAdaptor pa(*this);
            
    //m_columns.reset(new OutputColumns());

    this->m_tree.reset(sp.parse(sql));


    foreach_node(this->m_tree->getChilds(), XPrintTreeVisitor(std::wcerr), 1);
        

    Node *node = this->m_tree->getChilds()[0];
        

    apply_visitor(node, PrepareSqlVisitor(pa));


    //m_cursor.reset(new Cursor(*m_columns.get(), m_tree.get(), m_stmt)); // cursor class newProvider()

    DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_PREPARED);
}


/// @details
/// 
void
SqlCursor::execute(StmtBase::ParamMap& params)
{
    this->m_user_params = &params;
    DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_OPEN);
}


/// @details
/// 
IDALDriver*
SqlCursor::drv(void) const
{
    throw ex::engine_error("no driver");
}


//..............................................................................
////////////////////////////////////////////////////////////////// ParserAdaptor


/// @details
/// 
ParserAdaptor::ParserAdaptor(SqlCursor &cursor)
    : m_cursor(cursor),
      m_alias_counter(0),
      m_null_variant()
{}


/// @details
/// 
void
ParserAdaptor::addComputedField(ExprNode *node, const String &alias)
{
    std::shared_ptr<ComputedField> f(new ComputedField(cursor(), node));
    //f->m_daltype = var.datatype();
    assert(alias.length());
    f->m_name = alias;
    this->cursor().m_fields[this->cursor().m_fields.size() + 1] = f;
}


/// @details
/// 
void
ParserAdaptor::addStaticField(const String &alias, const Variant &var)
{
    std::shared_ptr<StaticField> f(new StaticField(var));
    f->m_daltype = var.datatype();
    assert(alias.length());
    f->m_name = alias;
    this->cursor().m_fields[this->cursor().m_fields.size() + 1] = f;
}


/// @details
/// 
void
ParserAdaptor::addMappingField(IdGroupNode *node, const String &name, const String &source)
{
    if(source.empty())
    {
        assert(cursor().m_objects.size() == 1);
        std::shared_ptr<DBObject> p = cursor().m_objects.begin()->second;
        colnum_t num = p->prov->columnID(name);

        std::shared_ptr<DataField> f(new DataField(num, *p));

        cursor().m_identifiers[node] = f;

        //addDataField(num, *p, alias);
    }
    else
    {
        SqlCursor::object_map_type::iterator i = cursor().m_objects.find(source);
        if(i != cursor().m_objects.end())
        {
            std::shared_ptr<DBObject> p = i->second;
            colnum_t num = p->prov->columnID(name);

            std::shared_ptr<DataField> f(new DataField(num, *p));

            cursor().m_identifiers[node] = f;

            //addDataField(num, *p, alias);
        }
        else
        {
            assert(!"not found");
        }
    }
}


/// @details
/// 
void
ParserAdaptor::addDataField(colnum_t colnum, DBObject &obj, const String &alias)
{
    std::shared_ptr<DataField> f(new DataField(colnum, obj));
    if(!alias.empty())
        f->m_name = alias;
    this->cursor().m_fields[this->cursor().m_fields.size() + 1] = f;
}


/// @details
/// 
void
ParserAdaptor::addParamField( const String &alias, size_t param_num)
{
    std::shared_ptr<ParamField> f(new ParamField(param_num, this->cursor()));
    assert(alias.length());
    f->m_name = alias;
    this->cursor().m_fields[this->cursor().m_fields.size() + 1] = f;
}


/// @details
/// 
void
ParserAdaptor::addDataField(const String &name, const String &source, const String &alias)
{
    if(source.empty())
    {
        assert(cursor().m_objects.size() == 1);
        std::shared_ptr<DBObject> p = cursor().m_objects.begin()->second;
        colnum_t num = p->prov->columnID(name);
        addDataField(num, *p, alias);
    }
    else
    {
        SqlCursor::object_map_type::iterator i = cursor().m_objects.find(source);
        if(i != cursor().m_objects.end())
        {
            std::shared_ptr<DBObject> p = i->second;
            colnum_t num = p->prov->columnID(name);
            addDataField(num, *p, alias);
        }
        else
        {
            assert(!"not found");
        }
    }
}



/// @details
/// 
void
ParserAdaptor::addAllColumns(void)
{
    assert(cursor().m_objects.size() > 0);


    std::for_each(cursor().m_objects_vector.begin(),
                  cursor().m_objects_vector.end(),
                  [this](typename SqlCursor::object_vector_type::value_type p)
                  {
                      IDataProvider *prov = p->prov;
                      assert(prov);
                      for(colnum_t i = 1;
                          i <= prov->columnCount();
                          ++i)
                      {
                          //std::cout << prov->describeColumn(i).getName().asStr() << std::endl;
                          this->addDataField(i, *p);
                      }
                  });

}



/// @details
/// 
void
ParserAdaptor::registerSourceObject(String alias, ObjectName name)
{
    if(this->cursor().m_objects.find(alias) == this->cursor().m_objects.end())
    {
        std::shared_ptr<DBObject> p(new DBObject(alias, name));

        this->cursor().m_objects[alias] = p;
        this->cursor().m_objects_vector.push_back(p);
    }
    else
    {
        DBWTL_BUG_EX("dup source"); /// @bug fixme
    }
}


/// @details
/// 
void
ParserAdaptor::createProviders(void)
{
    std::for_each(this->cursor().m_objects_vector.begin(),
                  this->cursor().m_objects_vector.end(),
                  [&](typename SqlCursor::object_vector_type::value_type elem)
                  {
                      IDataProvider *prov = this->cursor().m_stmt.newProvider();
                      prov->open(IDataProvider::MODE_READ,
                                 elem->name.table(),
                                 elem->name.schema(),
                                 elem->name.catalog());
                      elem->prov = prov;
                  });    
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
