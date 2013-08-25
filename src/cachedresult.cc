//
// cachedresult.cc - CachedResult (definition)
//
// Copyright (C)         informave.org
//   2013,               Daniel Vogelbacher <daniel@vogelbacher.name>
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

/// @file
/// @brief CachedResult (definitions)
/// @author Daniel Vogelbacher
/// @since 0.0.1


#include "dbwtl/dal/dal_fwd.hh"
#include "dbwtl/db_objects.hh"
#include "dbwtl/exceptions.hh"
#include "dal/dal_debug.hh"
#include "utils.hh"

#include <ctime>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <typeinfo>
#include <locale>

#define IS_UNIMPL() assert(!"unimpl"); throw 1

DB_NAMESPACE_BEGIN


#define ROWSET_SIZE 100


CachedResultBase::CachedResultBase(void)
    : m_source(0),
      m_rscache(),
      m_current_row(0)
{
}

CachedResultBase::~CachedResultBase(void)
{
}

bool
CachedResultBase::isBad(void) const
{
	return this->m_rscache.isBad();
}


void
CachedResultBase::attach(IStmt &statement) 
{
    this->m_source = &statement.resultset();


}

bool
CachedResultBase::sourceAvail(void) const
{
	return this->m_source != 0;
}



void
CachedResultBase::open(void)
{
    if(!this->m_rscache.isOpen())
    {
        if(!this->sourceAvail())
            throw ex::engine_error("Could not open cached resultset because source "
                                   "resultset is not available.");
        this->m_rscache.open();

        if(!this->m_source->isPositioned())
            this->m_source->first();

        assert(this->m_source->isPositioned());

        this->copyMetadata();
    }
}

void
CachedResultBase::first(void)
{
	this->setpos(1);
}

bool
CachedResultBase::next(void)
{
    // if not positioned, throw

	return this->setpos(this->m_current_row+1);
}

bool
CachedResultBase::eof(void) const
{
	return this->m_rscache.eof();
}

bool
CachedResultBase::isOpen(void) const
{
	return this->m_rscache.isOpen();
}

bool
CachedResultBase::isPositioned(void) const
{
	return this->m_rscache.isPositioned();
}


void
CachedResultBase::close(void)
{
    this->resetInternal();
}


void
CachedResultBase::copyRecord(void)
{
    ShrRecord rec(this->m_source->columnCount());
    for(colnum_t i = 1; i <= this->m_source->columnCount(); ++i)
    {
	if(this->m_source->column(i).isnull())
	{
		rec[i-1] = Variant();
		continue;
	}

        if(this->m_source->describeColumn(i).getDatatype() == DAL_TYPE_MEMO)
        {
            rec[i-1] = String("MEMO-UNSUPP");
            continue;
        }
        if(this->m_source->describeColumn(i).getDatatype() == DAL_TYPE_BLOB)
        {
            rec[i-1] = String("BLOB-UNSUPP");
            continue;
        }
        try
        {
            rec[i-1] = this->m_source->column(i);
        }
        catch(...)
        {
            std::cout << "catch error X in copyRecord()" << std::endl;
            throw;
        }
    }
    this->m_rscache.insert(rec);
}



bool
CachedResultBase::last(void)
{
	if(this->sourceAvail())
	{
		while(!this->m_source->eof())
		{
			copyRecord();
			this->m_source->next();
		}
	}

	if(this->m_rscache.rowCount() > 0)
	{
		this->m_current_row = this->m_rscache.rowCount();
		this->m_rscache.setpos(this->m_current_row);
		return true;
	}
	return false;
}

bool
CachedResultBase::setpos(rownum_t row)
{
	if(this->sourceAvail())
	{
        while(row > this->m_rscache.rowCount() && !this->m_source->eof())
        {
            copyRecord();
            this->m_source->next();
        }
	}

	if(row > this->m_rscache.rowCount())
		return false;
	
	this->m_rscache.setpos(row);
	assert(!this->m_rscache.eof());
	this->m_current_row = row;

	return true;
}

bool
CachedResultBase::prev(void)
{
	if(this->m_current_row > 1)
	{
		this->m_rscache.setpos(this->m_current_row - 1);
		assert(!this->m_rscache.eof());
		--this->m_current_row;
		return true;
	}
	return false;
}


rowcount_t
CachedResultBase::rowCount(void) const
{
	return this->m_rscache.rowCount();
}


const CachedResultBase::value_type&
CachedResultBase::column(colnum_t num)
{
	return this->m_rscache.column(num);
}

const CachedResultBase::value_type&
CachedResultBase::column(String name)
{
	return this->m_rscache.column(name);
}

// column methods
size_t
CachedResultBase::columnCount(void) const
{
	return this->m_rscache.columnCount();
}

colnum_t
CachedResultBase::columnID(String name) const
{
	return this->m_rscache.columnID(name);
}

String
CachedResultBase::columnName(colnum_t num) const
{
	return this->m_rscache.columnName(num);
}

//virtual const ITypeInfo& datatype(colnum_t num) const;

/// @brief Returns the column descriptor for the given column number
const IColumnDesc&
CachedResultBase::describeColumn(colnum_t num) const
{
	return this->m_rscache.describeColumn(num);
}

/// @brief Returns the column descriptor for the given column name
const IColumnDesc&
CachedResultBase::describeColumn(String name) const
{
	return this->m_rscache.describeColumn(name);
}


void
CachedResultBase::resetInternal(void)
{
	this->m_source = 0;
	this->m_rscache.reset();
}


rowcount_t
CachedResultBase::fetchMore(void)
{
	rowcount_t c = 0;
	while(c < ROWSET_SIZE && this->sourceAvail() && !this->m_source->eof())
	{
		copyRecord();
		++c;
		this->m_source->next();
	}
	return c;
}

bool
CachedResultBase::moreAvail(void)
{
	return (this->sourceAvail() && !this->m_source->eof());
}


void
CachedResultBase::fetchAll(void)
{
	if(!this->sourceAvail())
		throw EngineException("fetchAll() failed: source is not available");
	while(!this->m_source->eof())
	{
		this->copyRecord();
		this->m_source->next();
	}

}


void
CachedResultBase::detach(void)
{
	this->m_source = 0;
}

void
CachedResultBase::copyMetadata(void)
{
	assert(this->m_source);

	if(!this->m_source->isPositioned())
		throw EngineException("Dataset is not positioned.");
	
	assert(this->m_source->columnCount() > 0);
	this->m_rscache.setColumnCount(this->m_source->columnCount());

	for(size_t i = 1; i <= this->m_source->columnCount(); ++i)
	{
		const IColumnDesc &desc = this->m_source->describeColumn(i);
		this->m_rscache.modifyColumnDesc(i, DBWTL_COLUMNDESC_NAME, desc.getName());
		this->m_rscache.setDatatype(i, desc.getDatatype());
	}
}




DB_NAMESPACE_END


//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
