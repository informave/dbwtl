//
// library_support.hh - Support for so/dll loading
//
// Copyright (C)         informave.org
//   2010,               Daniel Vogelbacher <daniel@vogelbacher.name>
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
/// @brief Support for so/dll loading
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DBWTL_DAL_DALEXCEPTIONS_HH
#define INFORMAVE_DBWTL_DAL_DALEXCEPTIONS_HH

#include "dbwtl/dal/dal_fwd.hh"
#include "dbwtl/dal/dal_interface.hh"

#include <iostream>
#include <cassert>
#ifdef DBWTL_ON_UNIX
#include <dlfcn.h> /// @bug check for dlfcn.h
#endif
#include <sqlite3.h>


#ifdef DBWTL_ON_WIN32
#include "../include/targetver.h"
#define NOMINMAX
#include <windows.h>
#endif

DAL_NAMESPACE_BEGIN



//------------------------------------------------------------------------------
///
/// @brief Base class for low level library symbol handling
class DynamicLibraryBase : public IDALDriver
{
protected:
    typedef void (*funcptr_t)(void);


    ///
    /// This union is a workaround for POSIX systems.
    /// The function dlsym() must return a void* to conform to the POSIX standard.
    /// But C++ standard doesn't allows casting from void* to a function pointer.
    /// So we just save the void* via ptr and access it with func.
    ///
    /// @note This doesn't work on systems sizeof(void*) != sizeof(void(*func)(void))
    union dlsym_workaround
    {
        void* ptr;
        funcptr_t func;
    };

    // handle to the library
#ifdef DBWTL_ON_UNIX
    void *m_handle;
#else
	HMODULE m_handle;
#endif


public:

    DynamicLibraryBase(void) : m_handle(0)
        { }


    virtual ~DynamicLibraryBase(void)
        {
        }

    funcptr_t addrofsymbol(const char *const sym) const
        {
#ifdef DBWTL_ON_UNIX
            ::dlerror();
            void* ptr = ::dlsym(this->m_handle, sym);
            const char *dlsym_error = ::dlerror();
            if (dlsym_error)
            {
                std::cerr << "Cannot load symbol: " << dlsym_error << '\n';
            }
            dlsym_workaround p;
            p.ptr = ptr;
            return p.func;
#else
            FARPROC ptr = GetProcAddress(const_cast<HMODULE>(this->m_handle), sym);
            if (!ptr)
            {
                std::cerr << "Cannot load symbol: " << sym << '\n';
            }
			return reinterpret_cast<funcptr_t>(ptr);
#endif



        }
private:
    DynamicLibraryBase(const DynamicLibraryBase&);
    DynamicLibraryBase& operator=(const DynamicLibraryBase&);
};





//------------------------------------------------------------------------------
///
/// @brief Dynamic library support
class DynamicLibrary : public DynamicLibraryBase
{
public:
    DynamicLibrary(const char* const name) : DynamicLibraryBase()
        {
#ifdef DBWTL_ON_UNIX
            this->m_handle = ::dlopen(name, RTLD_LAZY);
            if (!this->m_handle) {
                std::cerr << "Cannot load library: " << ::dlerror() << '\n';
            }
#else
			this->m_handle = LoadLibrary(name);
            if (!this->m_handle) {
				  LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 
				    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );


                std::cerr << "Cannot load library: \"" << name << "\", " << (const char*)lpMsgBuf << '\n';
            }
#endif
        }

    virtual ~DynamicLibrary(void)
        {
            if(this->m_handle)
            {
#ifdef DBWTL_ON_UNIX
                ::dlclose(this->m_handle);
#else
                FreeLibrary(this->m_handle);
#endif
            }
        }


    template<typename T>
    inline void getproc(T& func, const char *const fname)
        {
            func = reinterpret_cast<T>(this->addrofsymbol(fname));
        }    
};


DAL_NAMESPACE_END


#endif
