//
// recordset.cc - RecordSet (definition)
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
/// @brief RecordSet (definitions)
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

RecordSet::RecordSet(void)
    : ScrollableDataset(),
      m_records(),
      m_bm_records(),
      m_count(),
      m_cursorstate(DAL_CURSOR_CLOSED),
      m_cursor(),
      m_column_descriptors()
{
}

RecordSet::~RecordSet(void)
{
}

/*
Record&
RecordSet::allocRecord(const Bookmark &bm)
{

    assert(this->m_count > 0);
    std::shared_ptr<record_type> &ref = m_map_records[bm];
    assert(ref.get() == 0);
    ref.reset(new RecordSet::record_type(this->m_count));
    this->m_records.push_back(ref);
    //assert(m_records[bm] != 0);
    return *ref.get();
}
*/


void
RecordSet::open(void)
{
	DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_OPEN);
}

bool   
RecordSet::isBad(void) const
{
    return (this->m_cursorstate & DAL_CURSOR_BAD);
}

void  
RecordSet::first(void)
{
    if(this->isBad())
        throw EngineException("Resultset is in bad state.");

    //if(! this->isOpen())
    //    throw EngineException("Resultset is not open.");

    DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_POSITIONED);

    this->m_cursor = this->rows().begin();
    
    if(this->m_cursor == this->rows().end())
    	DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_EOF);
}

bool   
RecordSet::next(void)
{
    if(this->isBad())
        throw EngineException("Resultset is in bad state.");

    //  if(! this->isOpen())
    //    throw EngineException("Resultset is not open.");

    if(!eof())
    {
        ++this->m_cursor;
        if(this->m_cursor == this->rows().end())
            DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_EOF);
        return !eof();
    }
    else
    {
        return false;
    }
}



bool
RecordSet::last(void)
{
    this->m_cursor = this->end()-1;
    return true;
}


bool
RecordSet::setpos(rownum_t row)
{
    if(row > rowCount() || row < 1)
        return false;

    RecordSet::iterator i = this->begin();
    i += row-1;
    this->m_cursor = i;
	DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_POSITIONED);
    return true;
}

bool
RecordSet::prev(void)
{
    
    if(this->m_cursor != this->begin())
    {
        --this->m_cursor;
        return true;
    }
    else
        return false;
}



bool  
RecordSet::eof(void) const
{
    if(this->isBad())
        throw EngineException("Resultset is in bad state.");

    //if(! this->isOpen())
    //   throw EngineException("Resultset is not open.");

    if(! (this->m_cursorstate & DAL_CURSOR_POSITIONED))
    {
    	//return true;
        assert(!"bug");
        /*
          this->appendDiagRec(CREATE_DIAG(DAL_STATE_ERROR, 24000,
          String("Dataset is not positioned."),
          "Hint: Use the first() method to position the cursor "
          "until calling eof()."))
          .raiseException();
        */
    }

    return this->m_cursorstate & DAL_CURSOR_EOF;
}

bool  
RecordSet::isOpen(void) const
{
	return this->m_cursorstate & DAL_CURSOR_OPEN;
}

bool  
RecordSet::isPositioned(void) const
{
	return this->m_cursorstate & DAL_CURSOR_POSITIONED;
}

void  
RecordSet::close(void)
{
    //IS_UNIMPL();
}

rowcount_t     
RecordSet::rowCount(void) const
{
    return this->rows().size();
}

const IResult::value_type&     
RecordSet::column(colnum_t num)
{
    if(! this->isPositioned())
        throw EngineException("Resultset is not positioned.");

    if(this->eof())
        throw EngineException("Resultset is eof.");

    if(num < 1)
    	throw EngineException("RecordSet has no support for bookmark columns");

    assert(this->m_cursor != this->rows().end());
    return (*m_cursor)[num-1];
}

const IResult::value_type&   
RecordSet::column(String name)
{
    return this->column(this->columnID(name));
}


// column methods
size_t        
RecordSet::columnCount(void) const
{
    return this->m_count;
}

colnum_t      
RecordSet::columnID(String name) const
{
    if(this->isBad())
        throw EngineException("Resultset is in bad state.");

    if(! this->isOpen())
        throw EngineException("Resultset is not open.");

    for(colnum_t i = 1; i <= this->columnCount(); ++i)
    {
        if(name == this->columnName(i))
            return i;
    }
    throw NotFoundException(US("Column '") + String::Internal(name) + US("' not found."));
}

String        
RecordSet::columnName(colnum_t num) const
{
    DALTRACE("VISIT");

    return this->describeColumn(num).getName().asStr();

/*
    if(this->isBad())
        throw EngineException("Resultset is in bad state.");

    if(! this->isOpen())
        throw EngineException("Resultset is not open.");

    return this->describeColumn(num).getName().asStr();
*/
}

// const ITypeInfo& datatype(colnum_t num) const{}

/// @brief Returns the column descriptor for the given column number
const IColumnDesc&
RecordSet::describeColumn(colnum_t num) const
{
    if(this->isBad())
        throw EngineException("Resultset is in bad state.");

/*
if(! this->isPrepared())
        throw EngineException("Resultset is not open.");
*/

//std::vector<ColumnDesc>     m_column_descriptors

    if(num > 0 && num <= this->columnCount())
    	return this->m_column_descriptors[num-1];
    else
    {
    	 throw NotFoundException(US("Column '") + String::Internal(Variant(int(num)).asStr()) + US("' not found."));
    }
/*
    std::map<colnum_t, SDIColumnDesc_libsdi>::const_iterator i =
        this->m_column_desc.find(num);

    if(i == this->m_column_desc.end())
    {
        throw NotFoundException(US("Column '") + String::Internal(Variant(int(num)).asStr()) + US("' not found."));
    }
    else
        return i->second;
	*/
}

/// @brief Returns the column descriptor for the given column name
const IColumnDesc& 
RecordSet::describeColumn(String name) const
{
    colnum_t num = this->columnID(name);
    return this->describeColumn(num);
}


void
RecordSet::updateDescriptors(void)
{
	while(this->m_count > this->m_column_descriptors.size())
	{
		ColumnDesc cd;
		std::stringstream ss;
		ss << "column" << this->m_column_descriptors.size()+1;
		cd.changeEntry(DBWTL_COLUMNDESC_NAME, String(ss.str()));
		this->m_column_descriptors.push_back(cd);
	}
	// Remove dropped columns if any
	this->m_column_descriptors.resize(this->m_count);


}


void
RecordSet::clear(void)
{
	this->m_records.clear();
	//DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_CLOSED);
	this->m_cursorstate = DAL_CURSOR_CLOSED;
}

void
RecordSet::reset(void)
{
	this->clear();
	this->m_count = 0;
	this->m_column_descriptors.clear();
}



void
RecordSet::insert(const ShrRecord &rec)
{
    if(!this->isOpen())
        throw EngineException("Insert not possible because RecordSet is not open"); 


	if(!this->m_records.empty() || (this->m_records.empty() && this->m_count > 0))
	{
		if(this->m_count != rec.size())
			throw EngineException(FORMAT2("insert() failed, field count mismatch: %d vs. %d",
				this->m_count, rec.size()));
		assert(this->m_count == rec.size());
	        this->m_records.push_back(rec.clone());
	}
	else
	{
		this->m_count = rec.size();
		this->updateDescriptors();
		this->m_records.push_back(rec.clone());

	}
}

void
RecordSet::modifyColumnDesc(colnum_t num, ColumnDescEntry entry, const IColumnDesc::value_type &v)
{
	if(num <= 0)
		throw EngineException(FORMAT1("Column number out of range: %d", num));
	assert(num <= this->m_column_descriptors.size());
	this->m_column_descriptors[num-1].changeEntry(entry, v);
}

void
RecordSet::setDatatype(colnum_t num, daltype_t daltype)
{
	if(num <= 0)
		throw EngineException(FORMAT1("Column number out of range: %d", num));
	assert(num <= this->m_column_descriptors.size());
	this->m_column_descriptors[num-1].changeType(daltype);
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
