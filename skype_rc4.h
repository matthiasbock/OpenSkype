
/*\
|*|
|*| Skype Library RC4 v1.108 by Sean O'Neil.
|*| All rights reserved.
|*| Not for commercial use.
|*|
|*| We are reverse engineers.
|*| We can prove if you have used this code in your product.
|*| We will find you.
|*| We will prosecute for copyright infringement.
|*| This code is quite unique and is easily identifiable.
|*| Result may match Skype's 100%, but this code is ours.
|*| The computation is significantly different from Skype's.
|*|
|*| For academic research and educational purposes only.
|*| If you require Skype compatibility in your products,
|*| feel free to contact Sean O'Neil on www.enrupt.com
|*|
|*| Last changes: 01.12.2009
|*| Published: 07.07.2010
|*| More will be published at 27C3, December 2010 (http://www.ccc.de/en/calendar)
|*|
\*/

#ifndef _skype_rc4_
#define _skype_rc4_

#include <stdio.h>
#include <stdlib.h>

typedef unsigned char	u8;
typedef unsigned long	u32;

typedef struct _RC4_context
{
	u8			i, j, s[256];
} RC4_context;

void RC4_crypt (u8 * buffer, u32 bytes, RC4_context * const rc4, const u32 test); // RC4 encrypt/decrypt (test=1 leaves rc4 context unaltered)
void Skype_RC4_Expand_IV (const u32 iv, const void *iv2, RC4_context * const rc4, const u32 flags, const u32 iv2_bytes);	// Main RC4 IV expansion function, matching Skype parameters, with optional IV2 for the latest DH-384 based protocol

#endif