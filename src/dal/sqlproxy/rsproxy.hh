//
// rsproxy.hh - Resultset Proxy
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
/// @brief Resultset Proxy
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_SQLPROXY_RSPROXY_HH
#define INFORMAVE_DB_SQLPROXY_RSPROXY_HH

#include "sqlproxy_fwd.hh"

#include <cassert>
#include <map>
#include <memory>


SQLPROXY_NAMESPACE_BEGIN


template<typename ResultsetInterface, typename InternalResultset, typename StmtType>
class ResultsetProxy : public ResultsetInterface
{
public:
    typedef typename ResultsetInterface::value_type      value_type;

    virtual ~ResultsetProxy(void)
    {}

    ResultsetProxy(StmtType &type)
    : m_internal_rs(),
      m_stmt(type)
    {}


    virtual void prepare(String sql)
    {
        this->getImpl().prepare(sql);
    }

    virtual void execute(StmtBase::ParamMap& params)
    {
        this->getImpl().execute(params);
    }

    virtual bool isBad(void) const
    {
        return this->getImpl().isBad();
    }

    virtual void first(void)
    {
        this->getImpl().first();
    }

    virtual bool   next(void)
    {
        return this->getImpl().next();
    }

    virtual bool   eof(void) const
    {
        return this->getImpl().eof();
    }

	virtual bool   isPrepared(void) const
    {
        return this->getImpl().isPrepared();
    }

    virtual bool   isOpen(void) const
    {
        return this->getImpl().isOpen();
    }

    virtual void   close(void)
    {
        this->getImpl().close();
    }

    virtual rowcount_t       rowCount(void) const
    {
        return this->getImpl().rowCount();
    }

    virtual const value_type&      column(colnum_t num)
    {
        return this->getImpl().column(num);
    }

    virtual const value_type&      column(String name)
    {
        return this->getImpl().column(name);
    }

    virtual size_t           columnCount(void) const
    {
        return this->getImpl().columnCount();
    }


    virtual colnum_t         columnID(String name) const
    {
        return this->getImpl().columnID(name);
    }

    virtual String           columnName(colnum_t num) const
    {
        return this->getImpl().columnName(num);
    }


    virtual const IColumnDesc& describeColumn(colnum_t num) const
    {
        return this->getImpl().describeColumn(num);
    }


    virtual const IColumnDesc& describeColumn(String name) const
    {
        return this->getImpl().describeColumn(name);
    }


	virtual size_t paramCount(void) const
    {
        return this->getImpl().paramCount();
    }


	virtual Variant          lastInsertRowId(void)
    {
        return this->getImpl().lastInsertRowId();
    }

	virtual rowcount_t       affectedRows(void) const
    {
        return this->getImpl().affectedRows();
    }



protected:
    inline InternalResultset& getImpl(void)
    {
        return *this->m_internal_rs.get();
    }

    inline const InternalResultset& getImpl(void) const
    {
        return *this->m_internal_rs.get();
    }

    virtual void setInternalResultset(InternalResultset *rs)
    {
        this->m_internal_rs.reset(rs);
    }

    std::auto_ptr<InternalResultset> m_internal_rs;

    StmtType     &m_stmt;
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
