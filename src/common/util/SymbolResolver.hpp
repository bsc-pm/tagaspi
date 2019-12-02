/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2019 Barcelona Supercomputing Center (BSC)
*/

#ifndef SYMBOL_RESOLVER_HPP
#define SYMBOL_RESOLVER_HPP

#include <cstdio>
#include <dlfcn.h>
#include <string>

namespace util {

class SymbolResolver {
public:
	static inline void *loadSymbol(const std::string &symbolName)
	{
		void *symbol = dlsym(RTLD_NEXT, symbolName.c_str());
		if (symbol == NULL) {
			fprintf(stderr, "Error: Symbol %s could not be resolved\n", symbolName.c_str());
			abort();
		}
		return symbol;
	}

	static inline void *tryLoadSymbol(const std::string &symbolName)
	{
		return dlsym(RTLD_NEXT, symbolName.c_str());
	}
};

} // namespace util

#endif // SYMBOL_RESOLVER_HPP
