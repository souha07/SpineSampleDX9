#pragma once

// min/max�}�N�����g�p���Ȃ�
#define NOMINMAX


#include <spine/spine.h>
#include <spine/Extension.h>


// ���������[�N�΍�
#define _CRTDBG_MAP_ALLOC 
#include <cstdlib>
#include <crtdbg.h>

#ifndef NEW
	#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
		#define NEW  ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
	#else
		#define NEW  new
	#endif
#endif
