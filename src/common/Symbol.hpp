/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <cassert>
#include <dlfcn.h>
#include <string>

#include "util/ErrorHandler.hpp"

namespace tagaspi {

//! \brief Utility class to store function prototypes
template <typename Ret = void, typename... Params>
class SymbolDecl {
public:
	//! The return type of the function
	typedef Ret ReturnTy;

	//! The complete function prototype
	typedef Ret SymbolTy(Params...);
};

//! Class that allows the dynamic loading of symbols at run-time
template <typename SymbolDecl>
class Symbol {
	using SymbolTy = typename SymbolDecl::SymbolTy;
	using ReturnTy = typename SymbolDecl::ReturnTy;

	//! The symbol name
	const char *_name;

	//! The loaded symbol or nullptr
	SymbolTy *_symbol;

public:
	Symbol(const char *name) :
		_name(name), _symbol(nullptr)
	{
	}

	//! \brief Load the symbol if not already loaded
	void load()
	{
		// Do nothing if it was already loaded
		if (_symbol != nullptr)
			return;

		_symbol = (SymbolTy *) dlsym(RTLD_DEFAULT, _name);
		if (_symbol == nullptr)
			ErrorHandler::fail("Could not find symbol ", _name);
	}

	//! \brief Indicate whether the symbol is loaded
	bool hasSymbol() const
	{
		return (_symbol != nullptr);
	}

	//! \brief Execute the function
	template <typename... Params>
	ReturnTy operator()(Params... params) const
	{
		assert(_symbol != nullptr);
		return (*_symbol)(params...);
	}
};

} // namespace tagaspi

#endif // SYMBOL_HPP
