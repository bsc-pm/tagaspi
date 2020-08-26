/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2019-2020 Barcelona Supercomputing Center (BSC)
*/

#ifndef SYMBOL_RESOLVER_HPP
#define SYMBOL_RESOLVER_HPP

#include <cstdio>
#include <dlfcn.h>
#include <string>


//! Class that allows the dynamic loading of symbols at run-time
class SymbolResolver {
public:
	//! \brief Load a symbol from the subsequent libraries
	//!
	//! \param symbolName The name of the symbol to load
	//! \param mandatory Whether should abort the program if not found
	//!
	//! \returns An opaque pointer to the symbol or null if not found
	static inline void *load(const std::string &symbolName, bool mandatory = true)
	{
		void *symbol = dlsym(RTLD_NEXT, symbolName.c_str());
		if (symbol == nullptr && mandatory) {
			fprintf(stderr, "Error: Could not find symbol %s\n", symbolName.c_str());
			abort();
		}
		return symbol;
	}
};

#endif // SYMBOL_RESOLVER_HPP
