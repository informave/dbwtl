//
// sqlcursor.hh - Cursor wrapper
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
/// @brief SQL Cursor wrapper
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_SQLPROXY_SQLCURSOR_HH
#define INFORMAVE_DB_SQLPROXY_SQLCURSOR_HH

#include <cassert>
#include <map>
#include <memory>

#include "sqlproxy_fwd.hh" /// @bug required?
#include "sqlparser.hh"

#include "dbwtl/dal/dal_interface.hh"

SQLPROXY_NAMESPACE_BEGIN


#define DBWTL_SQLPROXY_OK                  0
#define DBWTL_SQLPROXY_SQL_ERROR          -1


class ParserAdaptor; // forward declaration


//..............................................................................
//////////////////////////////////////////////////////////////////////  DBObject
///
/// @since 0.0.1
/// @brief References a database object (table, view, etc.)
class DBObject
{
public:
    DBObject(const String &alias, const ObjectName &name);

    const String       alias;
    const ObjectName   name;
    IDataProvider     *prov;

private:
    DBObject(const DBObject&);
    DBObject& operator=(const DBObject&);
};


//..............................................................................
/////////////////////////////////////////////////////////////////////////  Field
///
/// @since 0.0.1
/// @brief Output field base class
class Field : public ColumnDescBase
{
public:
    Field(void) : ColumnDescBase()
    {}

    virtual ~Field(void)
    {}

    // We introduce a new method to get the value.
    virtual const Variant& getValue(void) = 0;
};



//..............................................................................
/////////////////////////////////////////////////////////////////////  SqlCursor
///
/// @since 0.0.1
/// @brief SQL Cursor
class SqlCursor : public ResultBase
{
    friend class ParserAdaptor;
    friend class PrepareSqlVisitor;
    friend class SourceVisitor;
    friend class AttrListVisitor;
    friend class ExprEvalVisitor;
    friend class ParamVisitor;

public:
    typedef Variant value_type;

    SqlCursor(IStmt &stmt);

    virtual void   first(void);
    virtual bool   next(void);

    virtual bool   eof(void) const;
    virtual void   close(void);

    virtual size_t paramCount(void) const;

    virtual rowcount_t       rowCount(void) const;

    virtual const value_type&      column(colnum_t num);
    virtual const value_type&      column(String name);

    // column methods
    virtual size_t           columnCount(void) const;
    virtual colnum_t         columnID(String name) const;
    virtual String           columnName(colnum_t num) const;

    virtual const IColumnDesc& describeColumn(colnum_t num) const;

    virtual const IColumnDesc& describeColumn(String name) const;

    virtual Variant          lastInsertRowId(void);

    virtual rowcount_t       affectedRows(void) const;

    virtual void prepare(String sql);

    virtual void execute(StmtBase::ParamMap& params);

    virtual IDALDriver* drv(void) const;


    // extensions
    const Variant& getUserParam(size_t param_num);


protected:
    Field& findField(IdGroupNode* nodekey);
    size_t findParam(ParamNode* nodekey);
    Field& findField(colnum_t num);
    const Field& findField(colnum_t num) const;

    bool matchCondition(void);
    bool internalNext(void);


    typedef std::vector<std::shared_ptr<DBObject> >         object_vector_type;
    typedef std::map<String, std::shared_ptr<DBObject> >    object_map_type;
    typedef std::map<colnum_t, std::shared_ptr<Field> >     field_map_type;

    typedef std::map<IdGroupNode*, std::shared_ptr<Field> > identifier_map_type;
    typedef std::map<ParamNode*, size_t>                    param_map_type;

    //typedef std::map<size_t, std::shared_ptr<Param> >  param_map_type;

    object_map_type               m_objects;
    object_vector_type            m_objects_vector;
    field_map_type                m_fields;
    identifier_map_type           m_identifiers;
    param_map_type                m_params;
    std::auto_ptr<ParseTree>      m_tree;
    IStmt                        &m_stmt;
    StmtBase::ParamMap           *m_user_params;

    // Some methods returns const references. In case we must return a null value,
    // we need an internal variable holding the null value.
    const Variant                 m_null_variant;

    // If the SQL statement contains a WHERE clause, this variable points
    // to the node in the AST.
    WhereNode                    *m_where_node;

private:
    SqlCursor(const SqlCursor&);
    SqlCursor& operator=(const SqlCursor&);   
};



SQLPROXY_NAMESPACE_END

#endif


//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
