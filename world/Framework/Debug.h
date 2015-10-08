#pragma once

#ifdef _DEBUG
#ifndef _CPPRTTI	
#error In debug you must enable RTTI!
#endif
#endif

#ifdef _DEBUG
#include <assert.h>
#endif

#ifdef _DEBUG
#define ASSERT_DOWNCAST( p, t ) { assert( dynamic_cast< t* >( p ) ); }
#else
#define ASSERT_DOWNCAST
#endif