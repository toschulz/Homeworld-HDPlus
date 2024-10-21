// =============================================================================
//  Types.c
// =============================================================================

#include "Types.h"

float SwapFloat32( float val )
{
	union
	{
		float  f;
		Uint32 i;
	} swap;

	swap.f = val;
	swap.i = SDL_Swap32( swap.i );
    
	return swap.f;
}

udword Real32ToUdword(real32 a)
{
 //return (*(udword*)(&a));
	udword tmp;
    memcpy(&tmp, &a, sizeof(udword));
    return tmp;
}

udword Real32ToSdword(real32 a)
{
 //return (*(sdword*)(&a));
	udword tmp;
    memcpy(&tmp, &a, sizeof(udword));
    return tmp;
}

real32 UdwordToReal32(udword a)
{
 //return (*(real32*)(&a));
	real32 tmp;
    memcpy(&tmp, &a, sizeof(real32));
    return tmp;
}

real32 SdwordToReal32(sdword a)
{
 //return (*(real32*)(&a));
	real32 tmp;
    memcpy(&tmp, &a, sizeof(real32));
    return tmp;
}

real32 MemsizeToReal32(memsize a)
{
    //return (*(real32 *)(&a));
	real32 tmp;
    memcpy(&tmp, &a, sizeof(real32));
    return tmp;
}
