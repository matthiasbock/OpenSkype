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

#include <string.h>
#include "skype_rc4.h"

#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
	#ifndef __inline__
		#define __inline__	__forceinline
	#endif
	#ifndef rotl32
		#define rotl32(x,r)	_lrotl(x,r)
	#endif
	#ifndef rotr32
		#define rotr32(x,r)	_lrotr(x,r)
	#endif
#else
	#ifndef shr32
		#define shr32(x,r)	(((x)&0xFFFFFFFF)>>(r))
	#endif
	#ifndef rotl32
		#define rotl32(x,r)	(((x)<<((r)&31))|shr32(x,(0-(r))&31))
	#endif
	#ifndef rotr32
		#define rotr32(x,r)	(shr32(x,(r)&31)|((x)<<((0-(r))&31)))
	#endif
#endif

#ifndef __min
	static __inline__ u32 __min (u32 a, u32 b) { return a<b?a:b; }
#endif

#define byte(x,n)			(*(u8*)(((u8*)(x))+(n)))
#define dword(x,n)			(*(u32*)(((u8*)(x))+(n)))

/*
 * array from 0 to 255
 *
 * u8 u8sqrt(x)
 * {
 *   return u8(sqrt(x+1))
 * }
 */
static const u8				u8sqrt[256] =
{
	 1,  1,  1,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  4,
	 4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,
	 5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
	 7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,
	 8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
	 9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
	 9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
	10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11,
	11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12,
	12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13,
	13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16
};

static const u8				u8fcos[256] =
{
	0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,
	0,1,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,
	0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,
	1,1,1,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,1,0,0,0,
	1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,
	1,1,0,0,0,1,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,1,
	1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,1,0,0,0,1,1,1,0,0,0,1,1,
	1,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1
};

static const u8				u8fsin[256] =
{
	0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,1,0,0,0,1,1,1,
	0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,
	0,0,1,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,1,0,
	0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,
	0,1,1,1,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,1,0,0,
	0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,
	1,1,1,0,0,0,1,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,
	1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,1,0,0,0,1,1,1,0,0,0,1
};

#define u32root(n)			(u8sqrt[(n)&0xFF])
#define u32cos(n)			(u8fcos[(n)&0xFF])
#define u32sin(n)			(u8fsin[(n)&0xFF])

#define SKYPE_RC4_IV1()		(key[10] ^= key[7] - 0x354C1FF2)
#define SKYPE_RC4_IV2()		(key[17] += key[13] - 0x292C1156)
#define SKYPE_RC4_IV3n(n)	(key[13] |= u32cos(n) ? 0x1510A109 : key[14])
#define SKYPE_RC4_IV4()		(key[15] ^= (key[14] < 0x291B9650) ? key[14] : key[2])
#define SKYPE_RC4_IV5()		(key[ 3] ^= key[0] + 0x4376FF7)
#define SKYPE_RC4_IV6()		{key[ 9]  = rotl32 (key[9], rotl32 (key[1], 14)); if (key[9] & 1) return 1;}
#define SKYPE_RC4_IV7n(n)	(key[13] ^= (n < 0x2E0AF4F7) ? n : key[15])
#define SKYPE_RC4_IV8()		(key[ 6] *= 0x1304694A * key[11])
#define SKYPE_RC4_IV9()		(key[ 6] ^= u32cos(key[7]) ? 0x1AB1E599 : key[18])
#define SKYPE_RC4_IV10()	(key[ 5] += key[11] | 0xEA02A83)
#define SKYPE_RC4_IV11()	(key[ 6]  = rotl32 (key[6], key[13] - 18))
#define SKYPE_RC4_IV12()	(key[11] ^= key[15] | 0x11273409)
#define SKYPE_RC4_IV13()	(key[ 2] += 0xEA2D3D5D * key[7])
#define SKYPE_RC4_IV14()	{key[ 3] -= key[17] | 0x2433636; if (key[3] & 1) return 1;}
#define SKYPE_RC4_IV15()	(key[ 3] += key[9] + 0x48210C78)
#define SKYPE_RC4_IV16n(n)	(key[ 0]  = rotl32 (key[0], (n>>17)&0x1F))
#define SKYPE_RC4_IV17()	(key[ 9]  = rotr32 (key[9], u32cos(key[9]) ? 20 : key[0]))
#define SKYPE_RC4_IV18n(n)	(key[ 5] *= rotl32 (n, 3))
#define SKYPE_RC4_IV19()	(key[16] &= (key[11] < 0x5578A05) ? key[11] : key[16])
#define SKYPE_RC4_IV20n(n)	(key[17] ^= n)
#define SKYPE_RC4_IV21()	(key[ 4] ^= 17 * key[0])
#define SKYPE_RC4_IV22()	(key[ 2] ^= u32sin(key[17]) ? 0x1C0E70BF : key[5])
#define SKYPE_RC4_IV23()	(key[16]  = rotr32 (key[16], key[10] - 11))
#define SKYPE_RC4_IV24()	(key[ 6] += 0x975C61BA - key[8])
#define SKYPE_RC4_IV25()	{key[ 7] += rotr32 (key[7], 21); if (key[7] & 1) return 1;}
#define SKYPE_RC4_IV26()	{key[ 1] ^= u32cos(key[3]) ? 0x7C23395 : key[18]; if (key[1] & 1) return 1;}
#define SKYPE_RC4_IV27()	(key[ 9] += 0x3A82007 - key[14])
#define SKYPE_RC4_IV28()	{key[ 0] *= 33 * key[0]; if (key[0] & 1) return 1;}
#define SKYPE_RC4_IV29n(n)	{key[10]  = rotl32 (key[10], n-6); if (key[10] & 1) return 1;}
#define SKYPE_RC4_IV30n(n)	(key[ 2] -= u32sin(n) ? 0x73423C3 : key[7])
#define SKYPE_RC4_IV31()	(key[ 2] ^= key[15] + 0x57CE331)
#define SKYPE_RC4_IV32()	(key[ 9]  = rotr32 (key[9], key[17]*18))
#define SKYPE_RC4_IV33n(n)	(key[ 7] += n)
#define SKYPE_RC4_IV34()	(key[18] ^= key[10] + 0x1EE65B0C)
#define SKYPE_RC4_IV35()	(key[14] ^= u32cos(key[9]) ? 0x73CD560C : key[4])
#define SKYPE_RC4_IV36n(n)	(key[ 7] -= n)
#define SKYPE_RC4_IV37()	(key[ 7] ^= key[10] - 0x3035E544)
#define SKYPE_RC4_IV38()	{if (key[5] & 1) return 1;}
#define SKYPE_RC4_IV39n(n)	(key[9 ] *= u32sin(n) ? 0x28D781D2 : key[10])
#define SKYPE_RC4_IV40()	(key[11] -= key[12] << 5)
#define SKYPE_RC4_IV41()	(key[8 ] ^= u32cos(key[17]) ? 0x3544CA5E : key[8])
#define SKYPE_RC4_IV42()	(key[ 1] -= key[16] | 0x59C1677)
#define SKYPE_RC4_IV43()	{key[11] += 0xF6B10986 - key[14]; if (key[11] & 1) return 1;}
#define SKYPE_RC4_IV44()	(key[ 4] ^= key[19] - 0x303D46FE)
#define SKYPE_RC4_IV45()	(key[ 9] ^= u32cos(key[11]) ? 0xEEB638B : key[6])
#define SKYPE_RC4_IV46()	(key[16] ^= (key[18] < 0xE87F32) ? key[18] : key[11])
#define SKYPE_RC4_IV47n(n)	(key[12] *= u32cos(n) ? 0x1734D89C : key[5])
#define SKYPE_RC4_IV48()	(key[11] |= key[4] - 0x224114CD)
#define SKYPE_RC4_IV49n(n)	(key[11] &= n)
#define SKYPE_RC4_IV50()	(key[ 2] &= key[18] - 0x37CF1A3F)
#define SKYPE_RC4_IV51n(n)	(key[19] &= n)
#define SKYPE_RC4_IV52n(n)	(key[ 9]  = rotl32 (key[9], n))
#define SKYPE_RC4_IV53()	(key[18] -= 122 * key[6])
#define SKYPE_RC4_IV54()	(key[11]  = rotl32 (key[11], u32cos(key[5]) ? 19 : key[11]))
#define SKYPE_RC4_IV55()	(key[11] += 0x29CC7F53 - key[5])
#define SKYPE_RC4_IV56()	(key[12] -= 66 * key[2])
#define SKYPE_RC4_IV57()	{key[ 7] += key[2] ^ 0x376E1538; if (key[7] & 1) return 1;}
#define SKYPE_RC4_IV58n(n)	(key[15] -= u32cos(n) ? 0x344432F : key[18])
#define SKYPE_RC4_IV59()	(key[ 7] ^= u32root (key[15]))
#define SKYPE_RC4_IV60()	(key[10]  = rotr32 (key[10], key[14] + 6))
#define SKYPE_RC4_IV61n(n)	(key[ 6] += (n < 0x61F0BAA) ? n : key[16])
#define SKYPE_RC4_IV62n(n)	(key[ 1] ^= rotl32 (n, 8))
#define SKYPE_RC4_IV63n(n)	(key[12]  = rotr32 (key[12], key[18] ^ 9))
#define SKYPE_RC4_IV64()	(key[ 0]  = rotl32 (key[0], 8 * key[18]))
#define SKYPE_RC4_IV65()	(key[17] ^= 0x2F961 * key[4])
#define SKYPE_RC4_IV66()	(key[ 6] ^= rotr32 (key[14], 28))
#define SKYPE_RC4_IV67n(n)	(key[ 2] &= rotr32 (n, 17))
#define SKYPE_RC4_IV68n(n)	{key[16] &= (key[12] < 0x28165E7B) ? key[12] : n; if (key[16] & 1) return 1;}
#define SKYPE_RC4_IV69()	(key[ 9] -= rotr32 (key[16], 25))
#define SKYPE_RC4_IV70()	{key[ 1] ^= (key[4] < 0x196D816A) ? key[4] : key[17]; if (key[1] & 1) return 1;}
#define SKYPE_RC4_IV71n(n)	{u32 jv   = n + (u32sin (key[7]) ? 0xCC95AFBF : key[9]);\
							 key[ 2] += jv + 0xE6ECDA3,\
							 key[ 9] &= u32sin(key[7]) ? 0x13D68223 : jv,\
							 key[15] += 0x38245913 - key[12],\
							 key[16]  = rotr32 (key[16], 30 * key[17]),\
							 key[11] += 0x36F87E5B - key[5],\
							 key[ 2] += 102 * key[3],\
							 jv      *= rotl32 (key[5], 30),\
							 key[ 9] += 123 * jv,\
							 key[ 5]  = rotl32 (key[5], key[16] - 11),\
							 key[10] |= u32sin(key[4]) ? 0x84EDC63 : key[4];}
#define SKYPE_RC4_IV72n(n)	{u32 jv   = n & (u32cos(key[10]) ? 0xF998E196 : key[10]);\
							 key[ 6] ^= rotl32 (jv, 7),\
							 key[ 1] ^= jv - 0x4B327DA,\
							 key[ 7] += jv ^ 0x672E5A7,\
							 jv      ^= u32sin(jv) ? 0xBC91B04 : key[8],\
							 key[11] ^= key[6] & 0xBE53718,\
							 jv      ^= u32cos(key[2]) ? 0x9DADA8A4 : jv,\
							 key[ 0] -= 0x9DADA8A4 & key[6],\
							 key[13] += key[1] - 0x7B284744,\
							 key[ 3] ^= 20 * key[18],\
							 key[ 2] |= jv - 0x313BB22;}

static u32 skype_rc4_iv1 (u32 * const key, u32 n);
static u32 skype_rc4_iv2 (u32 * const key, u32 n);
static u32 skype_rc4_iv3 (u32 * const key, u32 n);
static u32 skype_rc4_iv4 (u32 * const key, u32 n);
static u32 skype_rc4_iv5 (u32 * const key, u32 n);
static u32 skype_rc4_iv6 (u32 * const key, u32 n);
static u32 skype_rc4_iv7 (u32 * const key, u32 n);
static u32 skype_rc4_iv8 (u32 * const key, u32 n);
static u32 skype_rc4_iv9 (u32 * const key, u32 n);
static u32 skype_rc4_iv10 (u32 * const key, u32 n);
static u32 skype_rc4_iv11 (u32 * const key, u32 n);
static u32 skype_rc4_iv12 (u32 * const key, u32 n);
static u32 skype_rc4_iv13 (u32 * const key, u32 n);
static u32 skype_rc4_iv14 (u32 * const key, u32 n);
static u32 skype_rc4_iv15 (u32 * const key, u32 n);
static u32 skype_rc4_iv16 (u32 * const key, u32 n);
static u32 skype_rc4_iv17 (u32 * const key, u32 n);
static u32 skype_rc4_iv18 (u32 * const key, u32 n);
static u32 skype_rc4_iv19 (u32 * const key, u32 n);
static u32 skype_rc4_iv20 (u32 * const key, u32 n);
static u32 skype_rc4_iv21 (u32 * const key, u32 n);
static u32 skype_rc4_iv22 (u32 * const key, u32 n);
static u32 skype_rc4_iv23 (u32 * const key, u32 n);
static u32 skype_rc4_iv24 (u32 * const key, u32 n);

u32 skype_rc4_iva (u32 * const key, u32 iv)
{
	u32			k = iv & 15;
	
	if (k == 4)
	{
		SKYPE_RC4_IV1();
		SKYPE_RC4_IV2();
		skype_rc4_iv1 (key, key[2]);
	}
	if (k == 8)
	{
		SKYPE_RC4_IV3n (0x767255F0);
		SKYPE_RC4_IV4();
		skype_rc4_iv2 (key, iv);
	}
	key[12] -= 28 * key[19];
	if (k == 1)
	{
		SKYPE_RC4_IV5();
		SKYPE_RC4_IV2();
		skype_rc4_iv3 (key, key[16]);
	}
	if (k == 6)
	{
		SKYPE_RC4_IV6();
		SKYPE_RC4_IV7n (0x258A329D);
		skype_rc4_iv4 (key, key[16]);
	}
	if (k == 14)
	{
		SKYPE_RC4_IV8();
		SKYPE_RC4_IV9();
		skype_rc4_iv5 (key, iv);
	}
	key[15] ^= 45 * key[9];
	if (k == 0)
	{
		SKYPE_RC4_IV10();
		SKYPE_RC4_IV11();
		skype_rc4_iv6 (key, key[12]);
	}
	if (k == 9)
	{
		SKYPE_RC4_IV12();
		SKYPE_RC4_IV13();
		skype_rc4_iv7 (key, key[19]);
	}
	key[18] ^= 0x327BAFFB * key[2];
	if (k == 7)
	{
		SKYPE_RC4_IV2();
		SKYPE_RC4_IV14();
		skype_rc4_iv8 (key, key[8]);
	}
	if (k == 13)
	{
		SKYPE_RC4_IV15();
		SKYPE_RC4_IV16n (0xA1C70157);
		skype_rc4_iv9 (key, iv);
	}
	if (k == 3)
	{
		SKYPE_RC4_IV3n (0x5947B4C0);
		SKYPE_RC4_IV17();
		skype_rc4_iv10 (key, key[12]);
	}
	key[13] *= u32root (iv);
	if (k == 15)
	{
		SKYPE_RC4_IV18n (0x59BBBCF2);
		SKYPE_RC4_IV19();
		skype_rc4_iv11 (key, key[8]);
	}
	if (k == 2)
	{
		SKYPE_RC4_IV20n (0xB00FB3F3);
		SKYPE_RC4_IV21();
		skype_rc4_iv12 (key, key[3]);
	}
	iv &= key[19] ^ 0x22BD05B7;
	if (k == 0)
	{
		SKYPE_RC4_IV22();
		SKYPE_RC4_IV23();
		skype_rc4_iv1 (key, iv);
	}
	if (k == 6)
	{
		SKYPE_RC4_IV24();
		SKYPE_RC4_IV25();
		skype_rc4_iv2 (key, key[6]);
	}
	if (k == 3)
	{
		SKYPE_RC4_IV26();
		SKYPE_RC4_IV27();
		skype_rc4_iv3 (key, key[0]);
	}
	key[1] ^= key[15] & 0x1ED68333;
	if (k == 11)
	{
		SKYPE_RC4_IV28();
		SKYPE_RC4_IV29n (0x8FD14B43);
		skype_rc4_iv4 (key, iv);
	}
	if (k == 7)
	{
		SKYPE_RC4_IV30n (0x173A48D4);
		SKYPE_RC4_IV1 ();
		skype_rc4_iv5 (key, key[6]);
	}
	if (k == 9)
	{
		SKYPE_RC4_IV31();
		SKYPE_RC4_IV32();
		skype_rc4_iv6 (key, iv);
	}
	key[12] ^= iv - 0x7F670F2F;
	if (k == 12)
	{
		SKYPE_RC4_IV2();
		SKYPE_RC4_IV19();
		skype_rc4_iv7 (key, iv);
	}
	if (k == 10)
	{
		SKYPE_RC4_IV32();
		SKYPE_RC4_IV33n (0x62E34BC8);
		skype_rc4_iv8 (key, key[4]);
	}
	key[3] = rotr32 (key[3], key[10]*4);
	if (k == 5)
	{
		SKYPE_RC4_IV34();
		SKYPE_RC4_IV35();
		skype_rc4_iv9 (key, key[4]);
	}
	if (k == 10)
	{
		SKYPE_RC4_IV7n (0x224B5A8A);
		SKYPE_RC4_IV36n (0xE729AFE3);
		skype_rc4_iv10 (key, key[5]);
	}
	if (k == 8)
	{
		SKYPE_RC4_IV29n (0x7B403A3E);
		SKYPE_RC4_IV7n (0x2606748C);
		skype_rc4_iv11 (key, iv);
	}
	iv *= (0x72E979C7 ^ key[7]);
	if (k == 1)
	{
		SKYPE_RC4_IV7n (0x3F541FC9);
		SKYPE_RC4_IV13();
		skype_rc4_iv12 (key, key[6]);
	}
	if (k == 4)
	{
		SKYPE_RC4_IV20n (0xDB079C63);
		SKYPE_RC4_IV1();
		skype_rc4_iv1 (key, key[13]);
	}
	key[15] ^= u32root (key[15]);
	if (k == 5)
	{
		SKYPE_RC4_IV37();
		SKYPE_RC4_IV27();
		skype_rc4_iv2 (key, key[1]);
	}
	if (k == 2)
	{
		SKYPE_RC4_IV23();
		SKYPE_RC4_IV37();
		skype_rc4_iv3 (key, key[17]);
	}
	key[9] -= rotl32 (key[18], 15);
	return 0;
}

u32 skype_rc4_iv1 (u32 * const key, u32 iv)
{
	u32			k = (key[13] ^ key[12] ^ key[8]) % 14;
	
	if (k == 0)
	{
		SKYPE_RC4_IV38();
		SKYPE_RC4_IV4();
		skype_rc4_iv13 (key, key[10]);
	}
	key[10] *= 87 * key[9];
	if (k == 13)
	{
		SKYPE_RC4_IV39n (0x2F378459);
		SKYPE_RC4_IV8();
		skype_rc4_iv14 (key, key[15]);
	}
	if (k == 11)
	{
		SKYPE_RC4_IV40();
		SKYPE_RC4_IV41();
		skype_rc4_iv15 (key, key[7]);
	}
	iv *= key[13] - 0x1B6664FC;
	if (k == 10)
	{
		SKYPE_RC4_IV42();
		SKYPE_RC4_IV1();
		skype_rc4_iv16 (key, key[7]);
	}
	key[19] = rotr32 (key[19], u32root (key[18]));
	if (k == 4)
	{
		SKYPE_RC4_IV39n (0x55CE2E77);
		SKYPE_RC4_IV43();
		skype_rc4_iv17 (key, iv);
	}
	if (k == 2)
	{
		SKYPE_RC4_IV44();
		SKYPE_RC4_IV3n (0xEEDD2781);
		skype_rc4_iv18 (key, key[19]);
	}
	key[17] += u32root (key[3]);
	if (k == 12)
	{
		SKYPE_RC4_IV8();
		SKYPE_RC4_IV45();
		skype_rc4_iv19 (key, key[4]);
	}
	key[2] *= iv + 0x3D4CFA4F;
	if (k == 9)
	{
		SKYPE_RC4_IV46();
		SKYPE_RC4_IV19();
		skype_rc4_iv20 (key, iv);
	}
	if (k == 1)
	{
		SKYPE_RC4_IV33n (0x505E90D8);
		SKYPE_RC4_IV47n (0x13951A5E);
		skype_rc4_iv21 (key, key[6]);
	}
	key[12] *= 79 * key[6];
	if (k == 6)
	{
		SKYPE_RC4_IV11();
		SKYPE_RC4_IV3n (0x6F001987);
		skype_rc4_iv22 (key, key[17]);
	}
	key[3] &= u32sin(key[17]) ? 0x4C35C59A : iv;
	if (k == 3)
	{
		SKYPE_RC4_IV48();
		SKYPE_RC4_IV14();
		skype_rc4_iv23 (key, key[11]);
	}
	if (k == 5)
	{
		SKYPE_RC4_IV47n (0xE2AEC47F);
		SKYPE_RC4_IV35();
		skype_rc4_iv24 (key, iv);
	}
	key[10] = rotl32 (key[10], 25 * key[16]);
	if (k == 0)
	{
		SKYPE_RC4_IV28();
		SKYPE_RC4_IV4();
		skype_rc4_iv13 (key, iv);
	}
	key[15] += 0xC7308059 - key[10];
	if (k == 8)
	{
		SKYPE_RC4_IV49n (0x45AE0A86);
		SKYPE_RC4_IV37();
		skype_rc4_iv14 (key, iv);
	}
	if (k == 7)
	{
		SKYPE_RC4_IV46();
		SKYPE_RC4_IV50();
		skype_rc4_iv15 (key, key[12]);
	}
	key[1] ^= iv & 0xF42F3BCF;
	return iv;
}

u32 skype_rc4_iv2 (u32 * const key, u32 iv)
{
	u32			k = key[4] % 15;
	
	if (k == 5)
	{
		SKYPE_RC4_IV45();
		SKYPE_RC4_IV32();
		skype_rc4_iv14 (key, key[9]);
	}
	key[1] += rotl32 (iv, 4);
	if (k == 9)
	{
		SKYPE_RC4_IV51n (0xB0AB0E55);
		SKYPE_RC4_IV3n (0x1A8398B1);
		skype_rc4_iv15 (key, key[19]);
	}
	key[5] ^= 0x39C770C4 & key[2];
	if (k == 0)
	{
		SKYPE_RC4_IV52n (11);
		SKYPE_RC4_IV42();
		skype_rc4_iv16 (key, key[14]);
	}
	if (k == 1)
	{
		SKYPE_RC4_IV34();
		SKYPE_RC4_IV17();
		skype_rc4_iv17 (key, key[7]);
	}
	iv -= rotl32 (iv, 1);
	if (k == 13)
	{
		SKYPE_RC4_IV53();
		SKYPE_RC4_IV54();
		skype_rc4_iv18 (key, key[9]);
	}
	if (k == 8)
	{
		SKYPE_RC4_IV8();
		SKYPE_RC4_IV15();
		skype_rc4_iv19 (key, key[17]);
	}
	iv += key[17] + 0x25FB77C1;
	if (k == 14)
	{
		SKYPE_RC4_IV49n (0x094909EA);
		SKYPE_RC4_IV55();
		skype_rc4_iv20 (key, key[7]);
	}
	key[4] *= u32cos(key[19]) ? 0x336C9268 : key[4];
	if (k == 11)
	{
		SKYPE_RC4_IV8();
		SKYPE_RC4_IV6();
		skype_rc4_iv21 (key, iv);
	}
	key[17] *= u32root (iv);
	if (k == 2)
	{
		SKYPE_RC4_IV52n (17);
		SKYPE_RC4_IV56();
		skype_rc4_iv22 (key, key[12]);
	}
	if (k == 7)
	{
		SKYPE_RC4_IV57();
		SKYPE_RC4_IV17();
		skype_rc4_iv23 (key, key[14]);
	}
	key[5] += u32root (key[0]);
	if (k == 4)
	{
		SKYPE_RC4_IV32();
		SKYPE_RC4_IV27();
		skype_rc4_iv24 (key, key[13]);
	}
	key[14] ^= rotr32 (key[18], 20);
	if (k == 3)
	{
		SKYPE_RC4_IV40();
		SKYPE_RC4_IV54();
		skype_rc4_iv1 (key, key[15]);
	}
	if (k == 0)
	{
		SKYPE_RC4_IV34();
		SKYPE_RC4_IV51n (0xA14E4231);
		skype_rc4_iv14 (key, key[5]);
	}
	key[10] *= 34 * key[19];
	if (k == 6)
	{
		SKYPE_RC4_IV58n (0xA7E8E811);
		SKYPE_RC4_IV26();
		skype_rc4_iv15 (key, iv);
	}
	if (k == 10)
	{
		SKYPE_RC4_IV5();
		SKYPE_RC4_IV7n (0x27546CBA);
		skype_rc4_iv16 (key, key[16]);
	}
	iv |= 21 * key[10];
	if (k == 12)
	{
		SKYPE_RC4_IV59();
		SKYPE_RC4_IV29n (0xE8357BC6);
		skype_rc4_iv17 (key, key[8]);
	}
	key[17] &= u32sin(iv) ? 0x24D1E601 : key[17];
	return iv;
}

u32 skype_rc4_iv3 (u32 * const key, u32 iv)
{
	u32			k = (key[2] ^ key[11] ^ key[19]) & 15;
	
	if (k == 7)
	{
		SKYPE_RC4_IV45();
		SKYPE_RC4_IV55();
		skype_rc4_iv15 (key, key[7]);
	}
	iv = rotl32 (iv, 20 * key[15]);
	if (k == 2)
	{
		SKYPE_RC4_IV49n (0x159A2134);
		SKYPE_RC4_IV3n (0x855DE620);
		skype_rc4_iv16 (key, key[2]);
	}
	if (k == 13)
	{
		SKYPE_RC4_IV3n (0xC4146E71);
		SKYPE_RC4_IV36n (0x3F3F6FFB);
		skype_rc4_iv17 (key, iv);
	}
	key[13] += key[18] & 0x2581026D;
	if (k == 11)
	{
		SKYPE_RC4_IV27();
		SKYPE_RC4_IV37();
		skype_rc4_iv18 (key, iv);
	}
	key[1] &= iv & 0x64BB010;
	if (k == 5)
	{
		SKYPE_RC4_IV60();
		SKYPE_RC4_IV23();
		skype_rc4_iv19 (key, key[13]);
	}
	if (k == 3)
	{
		SKYPE_RC4_IV2();
		SKYPE_RC4_IV51n (0x7EC9C31F);
		skype_rc4_iv20 (key, iv);
	}
	key[10] |= 67 * key[13];
	if (k == 8)
	{
		SKYPE_RC4_IV61n (0xDFDC4A68);
		SKYPE_RC4_IV44();
		skype_rc4_iv21 (key, key[5]);
	}
	key[9] += 0xAA213313 * iv;
	if (k == 0)
	{
		SKYPE_RC4_IV37();
		SKYPE_RC4_IV58n (0x16E7FB2A);
		skype_rc4_iv22 (key, iv);
	}
	if (k == 15)
	{
		SKYPE_RC4_IV1();
		SKYPE_RC4_IV62n (0x6D43A2A8);
		skype_rc4_iv23 (key, iv);
	}
	key[14] |= u32root (key[12]);
	if (k == 12)
	{
		SKYPE_RC4_IV47n (0xE5FB063D);
		SKYPE_RC4_IV42();
		skype_rc4_iv24 (key, iv);
	}
	if (k == 10)
	{
		SKYPE_RC4_IV10();
		SKYPE_RC4_IV21();
		skype_rc4_iv1 (key, key[4]);
	}
	iv = 107 * key[1] ^ iv;
	if (k == 1)
	{
		SKYPE_RC4_IV33n (0x357B0AC2);
		SKYPE_RC4_IV5();
		skype_rc4_iv2 (key, key[1]);
	}
	key[9] = rotr32 (key[9], u32sin(key[16]) ? 26 : key[12]);
	if (k == 4)
	{
		SKYPE_RC4_IV37();
		SKYPE_RC4_IV37();
		skype_rc4_iv15 (key, key[7]);
	}
	if (k == 0)
	{
		SKYPE_RC4_IV63n (0x60298842);
		SKYPE_RC4_IV22();
		skype_rc4_iv16 (key, iv);
	}
	key[13] *= key[18] ^ 0x1D347B67;
	if (k == 6)
	{
		SKYPE_RC4_IV30n (0xAC2CA8C4);
		SKYPE_RC4_IV61n (0xFF8CF458);
		skype_rc4_iv17 (key, key[6]);
	}
	key[2] = u32sin(key[0]) ? key[2] : rotr32 (key[2], key[0]);
	if (k == 14)
	{
		SKYPE_RC4_IV21();
		SKYPE_RC4_IV29n (0x52041287);
		skype_rc4_iv18 (key, key[7]);
	}
	if (k == 9)
	{
		SKYPE_RC4_IV36n (0xBF896FA3);
		SKYPE_RC4_IV64();
		skype_rc4_iv19 (key, key[3]);
	}
	return 50 * iv * iv;
}

u32 skype_rc4_iv4 (u32 * const key, u32 iv)
{
	u32			k = iv & 15;
	
	if (k == 10)
	{
		SKYPE_RC4_IV13();
		SKYPE_RC4_IV51n (0x590CCB85);
		skype_rc4_iv16 (key, key[7]);
	}
	key[10] ^= (iv < 0x3D18A13) ? iv : key[9];
	if (k == 6)
	{
		SKYPE_RC4_IV11();
		SKYPE_RC4_IV17();
		skype_rc4_iv17 (key, key[17]);
	}
	if (k == 14)
	{
		SKYPE_RC4_IV58n (0xA6F86E90);
		SKYPE_RC4_IV50();
		skype_rc4_iv18 (key, key[17]);
	}
	iv -= key[10] & 0x16926664;
	if (k == 0)
	{
		SKYPE_RC4_IV18n (0x95AAFBC8);
		SKYPE_RC4_IV12();
		skype_rc4_iv19 (key, key[7]);
	}
	if (k == 11)
	{
		SKYPE_RC4_IV13();
		SKYPE_RC4_IV22();
		skype_rc4_iv20 (key, key[6]);
	}
	key[6] ^= key[7] - 0xD669F4D;
	if (k == 1)
	{
		SKYPE_RC4_IV11();
		SKYPE_RC4_IV24();
		skype_rc4_iv21 (key, key[7]);
	}
	key[1] ^= rotr32 (key[15], 14);
	if (k == 5)
	{
		SKYPE_RC4_IV21();
		SKYPE_RC4_IV65();
		skype_rc4_iv22 (key, iv);
	}
	if (k == 4)
	{
		SKYPE_RC4_IV3n (0x8EEA1FE3);
		SKYPE_RC4_IV22();
		skype_rc4_iv23 (key, key[13]);
	}
	iv = rotr32 (iv, 0x19 & key[5]);
	if (k == 2)
	{
		SKYPE_RC4_IV41();
		SKYPE_RC4_IV20n (0x34C48CA3);
		skype_rc4_iv24 (key, key[9]);
	}
	if (k == 3)
	{
		SKYPE_RC4_IV42();
		SKYPE_RC4_IV58n (0x104BE7AE);
		skype_rc4_iv1 (key, key[9]);
	}
	key[3] *= u32root (iv);
	if (k == 0)
	{
		SKYPE_RC4_IV54();
		SKYPE_RC4_IV62n (0x507DA30D);
		skype_rc4_iv2(key, key[2]);
	}
	key[16] -= u32root (iv);
	if (k == 13)
	{
		SKYPE_RC4_IV36n (0x3FCB3FA3);
		SKYPE_RC4_IV47n (0xD0D9B11D);
		skype_rc4_iv3 (key, key[4]);
	}
	if (k == 7)
	{
		SKYPE_RC4_IV45();
		SKYPE_RC4_IV33n (0x6E4473BD);
		skype_rc4_iv16 (key, key[19]);
	}
	iv += 0x720E12F5 + key[5];
	if (k == 15)
	{
		SKYPE_RC4_IV4();
		SKYPE_RC4_IV55();
		skype_rc4_iv17 (key, key[1]);
	}
	if (k == 9)
	{
		SKYPE_RC4_IV43();
		SKYPE_RC4_IV8();
		skype_rc4_iv18 (key, key[10]);
	}
	key[1] ^= (iv < 0x585C6D88) ? iv : key[13];
	if (k == 12)
	{
		SKYPE_RC4_IV48();
		SKYPE_RC4_IV38();
		skype_rc4_iv19 (key, key[14]);
	}
	if (k == 8)
	{
		SKYPE_RC4_IV44();
		SKYPE_RC4_IV50();
		skype_rc4_iv20 (key, key[11]);
	}
	iv |= key[13] + 0x38D39E93;
	if (k == 1)
	{
		SKYPE_RC4_IV6();
		SKYPE_RC4_IV9();
		skype_rc4_iv21 (key, key[8]);
	}
	key[19] ^= key[9] + 0x23280350;
	return iv;
}

u32 skype_rc4_iv5 (u32 * const key, u32 iv)
{
	u32			k = key[13] & 15;
	
	if (k == 1)
	{
		SKYPE_RC4_IV19();
		SKYPE_RC4_IV42();
		skype_rc4_iv17 (key, iv);
	}
	iv += 0xCB72BB0E + key[10];
	if (k == 0)
	{
		SKYPE_RC4_IV54();
		SKYPE_RC4_IV32();
		skype_rc4_iv18 (key, key[0]);
	}
	if (k == 5)
	{
		SKYPE_RC4_IV2();
		SKYPE_RC4_IV20n (0xEFE4F823);
		skype_rc4_iv19 (key, key[3]);
	}
	key[7] += 0x72A1B49 - key[6];
	if (k == 1)
	{
		SKYPE_RC4_IV26();
		SKYPE_RC4_IV65();
		skype_rc4_iv20 (key, key[15]);
	}
	if (k == 4)
	{
		SKYPE_RC4_IV9();
		SKYPE_RC4_IV47n (0x391FA13E);
		skype_rc4_iv21 (key, iv);
	}
	key[17] += u32cos(key[16]) ? 0xD1C5DCA : key[10];
	if (k == 11)
	{
		SKYPE_RC4_IV5();
		SKYPE_RC4_IV43();
		skype_rc4_iv22 (key, key[5]);
	}
	if (k == 14)
	{
		SKYPE_RC4_IV52n (28);
		SKYPE_RC4_IV33n (0x5BAAA0D8);
		skype_rc4_iv23 (key, key[3]);
	}
	iv += 0xA4631CA4 & key[0];
	if (k == 10)
	{
		SKYPE_RC4_IV45();
		SKYPE_RC4_IV25();
		skype_rc4_iv24 (key, key[6]);
	}
	if (k == 3)
	{
		SKYPE_RC4_IV55();
		SKYPE_RC4_IV59();
		skype_rc4_iv1 (key, key[11]);
	}
	key[18] = rotl32 (key[18], u32root (key[13]));
	if (k == 2)
	{
		SKYPE_RC4_IV29n (0x255DB12E);
		SKYPE_RC4_IV63n (0x8570A62A);
		skype_rc4_iv2 (key, key[19]);
	}
	iv += key[12] & 0x3EB958F;
	if (k == 13)
	{
		SKYPE_RC4_IV2();
		SKYPE_RC4_IV50();
		skype_rc4_iv3 (key, key[19]);
	}
	if (k == 2)
	{
		SKYPE_RC4_IV12();
		SKYPE_RC4_IV30n (0xFD54B81D);
		skype_rc4_iv4 (key, iv);
	}
	key[10] *= 61 * iv;
	if (k == 15)
	{
		SKYPE_RC4_IV65();
		SKYPE_RC4_IV43();
		skype_rc4_iv17 (key, key[9]);
	}
	if (k == 0)
	{
		SKYPE_RC4_IV48();
		SKYPE_RC4_IV12();
		skype_rc4_iv18 (key, key[17]);
	}
	iv = rotl32 (iv, key[16]);
	if (k == 9)
	{
		SKYPE_RC4_IV3n (0xD59BE521);
		SKYPE_RC4_IV39n (0x59621E0F);
		skype_rc4_iv19 (key, key[6]);
	}
	if (k == 7)
	{
		SKYPE_RC4_IV55();
		SKYPE_RC4_IV51n (0x935A86BD);
		skype_rc4_iv20 (key, key[4]);
	}
	key[16] -= (key[5] < 0x4DEA623B) ? key[5] : key[2];
	if (k == 8)
	{
		SKYPE_RC4_IV20n (0xAC1EA487);
		SKYPE_RC4_IV3n (0x99B3E5F0);
		skype_rc4_iv21(key, key[10]);
	}
	if (k == 6)
	{
		SKYPE_RC4_IV45();
		SKYPE_RC4_IV53();
		skype_rc4_iv22 (key, key[11]);
	}
	key[14] = rotr32 (key[14], u32sin(key[19]) ? 27 : key[5]);
	if (k == 12)
	{
		SKYPE_RC4_IV6();
		SKYPE_RC4_IV54();
		skype_rc4_iv23 (key, key[12]);
	}
	return iv * (iv - 0x497488D1);
}

u32 skype_rc4_iv6 (u32 * const key, u32 iv)
{
	u32			k = (key[11] ^ key[6] ^ key[4]) & 15;
	
	if (k == 2)
	{
		SKYPE_RC4_IV11();
		SKYPE_RC4_IV5();
		skype_rc4_iv18 (key, iv);
	}
	key[10] |= rotr32 (iv, 6);
	if (k == 14)
	{
		SKYPE_RC4_IV37();
		SKYPE_RC4_IV31();
		skype_rc4_iv19 (key, key[18]);
	}
	if (k == 9)
	{
		SKYPE_RC4_IV43();
		SKYPE_RC4_IV58n (0x13BC76D5);
		skype_rc4_iv20 (key, key[1]);
	}
	iv &= u32sin(key[11]) ? 0xDA03B206 : key[15];
	if (k == 12)
	{
		SKYPE_RC4_IV37();
		SKYPE_RC4_IV2();
		skype_rc4_iv21 (key, key[0]);
	}
	if (k == 3)
	{
		SKYPE_RC4_IV41();
		SKYPE_RC4_IV59();
		skype_rc4_iv22 (key, iv);
	}
	key[14] ^= 83 * key[14];
	if (k == 7)
	{
		SKYPE_RC4_IV41();
		SKYPE_RC4_IV59();
		skype_rc4_iv23 (key, key[12]);
	}
	if (k == 11)
	{
		SKYPE_RC4_IV6();
		SKYPE_RC4_IV40();
		skype_rc4_iv24 (key, key[10]);
	}
	key[11] -= u32sin(iv) ? 0xCA758BFB : key[12];
	if (k == 10)
	{
		SKYPE_RC4_IV66();
		SKYPE_RC4_IV38();
		skype_rc4_iv1 (key, iv);
	}
	if (k == 8)
	{
		SKYPE_RC4_IV26();
		SKYPE_RC4_IV67n (0x26B5176D);
		skype_rc4_iv2 (key, key[14]);
	}
	iv *= 0xACA6EF1E ^ key[1];
	if (k == 13)
	{
		SKYPE_RC4_IV44();
		SKYPE_RC4_IV24();
		skype_rc4_iv3 (key, iv);
	}
	if (k == 6)
	{
		SKYPE_RC4_IV44();
		SKYPE_RC4_IV48();
		skype_rc4_iv4 (key, key[13]);
	}
	iv += 41 * key[11];
	if (k == 3)
	{
		SKYPE_RC4_IV17();
		SKYPE_RC4_IV46();
		skype_rc4_iv5 (key, key[17]);
	}
	if (k == 1)
	{
		SKYPE_RC4_IV54();
		SKYPE_RC4_IV10();
		skype_rc4_iv18 (key, iv);
	}
	key[8] += 35 * key[6];
	if (k == 4)
	{
		SKYPE_RC4_IV50();
		SKYPE_RC4_IV48();
		skype_rc4_iv19 (key, key[1]);
	}
	if (k == 1)
	{
		SKYPE_RC4_IV47n (0x909F);
		SKYPE_RC4_IV44();
		skype_rc4_iv20 (key, key[2]);
	}
	key[11] *= key[12] - 0x48766E9C;
	if (k == 0)
	{
		SKYPE_RC4_IV7n (0x29B6BEC0);
		SKYPE_RC4_IV68n (0x5233F757);
		skype_rc4_iv21 (key, key[9]);
	}
	if (k == 15)
	{
		SKYPE_RC4_IV59();
		SKYPE_RC4_IV28();
		skype_rc4_iv22 (key, iv);
	}
	key[3] += u32root (key[15]);
	if (k == 5)
	{
		SKYPE_RC4_IV4();
		SKYPE_RC4_IV28();
		skype_rc4_iv23 (key, key[1]);
	}
	key[4] = rotl32 (key[4], u32sin(key[12]) ? 13 : key[14]);
	if (k == 2)
	{
		SKYPE_RC4_IV4();
		SKYPE_RC4_IV65();
		skype_rc4_iv24 (key, key[4]);
	}
	if (k == 0)
	{
		SKYPE_RC4_IV40();
		SKYPE_RC4_IV21();
		skype_rc4_iv1 (key, key[5]);
	}
	key[5] &= 97 * key[16];
	return iv;
}

u32 skype_rc4_iv7 (u32 * const key, u32 iv)
{
	u32			k = (key[8] ^ key[7] ^ key[16]) & 15;
	
	if (k == 8)
	{
		SKYPE_RC4_IV62n (0xCDBFC92);
		SKYPE_RC4_IV6();
		skype_rc4_iv19 (key, key[13]);
	}
	key[3] -= key[15] + 0xFE7A752E;
	if (k == 3)
	{
		SKYPE_RC4_IV18n (0xB706A577);
		SKYPE_RC4_IV1();
		skype_rc4_iv20 (key, key[8]);
	}
	if (k == 7)
	{
		SKYPE_RC4_IV69();
		SKYPE_RC4_IV70();
		skype_rc4_iv21 (key, iv);
	}
	key[0] ^= 25 * key[14];
	if (k == 2)
	{
		SKYPE_RC4_IV49n (0x06839B1C);
		SKYPE_RC4_IV54();
		skype_rc4_iv22 (key, key[6]);
	}
	if (k == 12)
	{
		SKYPE_RC4_IV38();
		SKYPE_RC4_IV20n (0x2D742F43);
		skype_rc4_iv23 (key, key[0]);
	}
	key[0] ^= (key[18] < 0xD339212E) ? key[18] : iv;
	if (k == 11)
	{
		SKYPE_RC4_IV62n (0x5C1A35DA);
		SKYPE_RC4_IV14();
		skype_rc4_iv24 (key, key[15]);
	}
	if (k == 0)
	{
		SKYPE_RC4_IV39n (0x24FE0880);
		SKYPE_RC4_IV3n (0x58FC9D30);
		skype_rc4_iv1 (key, key[12]);
	}
	key[19] ^= key[2] ^ 0xACF3864;
	if (k == 15)
	{
		SKYPE_RC4_IV35();
		SKYPE_RC4_IV8();
		skype_rc4_iv2 (key, iv);
	}
	if (k == 2)
	{
		SKYPE_RC4_IV62n (0xAAB1F08);
		SKYPE_RC4_IV2();
		skype_rc4_iv3 (key, iv);
	}
	iv -= (key[7] < 0xF2D2F7) ? key[7] : iv;
	if (k == 10)
	{
		SKYPE_RC4_IV25();
		SKYPE_RC4_IV34();
		skype_rc4_iv4 (key, key[3]);
	}
	key[12] = rotr32 (key[12], key[19] + 3);
	if (k == 6)
	{
		SKYPE_RC4_IV43();
		SKYPE_RC4_IV68n (0x4ADA50AE);
		skype_rc4_iv5 (key, key[19]);
	}
	if (k == 9)
	{
		SKYPE_RC4_IV42();
		SKYPE_RC4_IV28();
		skype_rc4_iv6 (key, key[9]);
	}
	iv ^= key[0] + 0xEC0FD36;
	if (k == 1)
	{
		SKYPE_RC4_IV7n (0x2AB43EDB);
		SKYPE_RC4_IV28();
		skype_rc4_iv19 (key, iv);
	}
	if (k == 0)
	{
		SKYPE_RC4_IV61n (0xF7FB2E31);
		SKYPE_RC4_IV2();
		skype_rc4_iv20 (key, key[6]);
	}
	iv += 93 * key[5];
	if (k == 13)
	{
		SKYPE_RC4_IV22();
		SKYPE_RC4_IV9();
		skype_rc4_iv21 (key, key[4]);
	}
	if (k == 5)
	{
		SKYPE_RC4_IV59();
		SKYPE_RC4_IV32();
		skype_rc4_iv22 (key, key[5]);
	}
	iv &= iv - 0x6718263;
	if (k == 1)
	{
		SKYPE_RC4_IV39n (0x30A63C23);
		SKYPE_RC4_IV12();
		skype_rc4_iv23 (key, key[1]);
	}
	if (k == 14)
	{
		SKYPE_RC4_IV18n (0x98A2A62E);
		SKYPE_RC4_IV1();
		skype_rc4_iv24 (key, key[18]);
	}
	if (k == 3)
	{
		SKYPE_RC4_IV69();
		SKYPE_RC4_IV68n (0x87E63075);
		skype_rc4_iv1 (key, key[14]);
	}
	iv = 0xEC076923 - key[10];
	if (k == 4)
	{
		SKYPE_RC4_IV56();
		SKYPE_RC4_IV32();
		skype_rc4_iv2 (key, key[10]);
		SKYPE_RC4_IV57();
		SKYPE_RC4_IV9();
		skype_rc4_iv3 (key, key[9]);
	}
	return 0xD271A9AD - key[8] + iv;
}

u32 skype_rc4_iv8 (u32 * const key, u32 iv)
{
	u32			k = (key[14] ^ key[1] ^ key[8]) & 15;
	
	if (k == 3)
	{
		SKYPE_RC4_IV8();
		SKYPE_RC4_IV7n (0x23841ACB);
		skype_rc4_iv20 (key, key[18]);
	}
	if (k == 0)
	{
		SKYPE_RC4_IV15();
		SKYPE_RC4_IV61n (0x1656F2C);
		skype_rc4_iv21 (key, iv);
	}
	key[18] ^= u32root (key[1]);
	if (k == 8)
	{
		SKYPE_RC4_IV52n (12);
		SKYPE_RC4_IV51n (0xC98D4040);
		skype_rc4_iv22 (key, key[10]);
	}
	if (k == 13)
	{
		SKYPE_RC4_IV6();
		SKYPE_RC4_IV20n (0xC53796B3);
		skype_rc4_iv23 (key, key[18]);
	}
	key[2] += rotr32 (key[14], 12);
	if (k == 3)
	{
		SKYPE_RC4_IV49n (0x29D9DEBC);
		SKYPE_RC4_IV64();
		skype_rc4_iv24 (key, iv);
	}
	if (k == 9)
	{
		SKYPE_RC4_IV26();
		SKYPE_RC4_IV70();
		skype_rc4_iv1 (key, key[7]);
	}
	if (k == 11)
	{
		SKYPE_RC4_IV19();
		SKYPE_RC4_IV42();
		skype_rc4_iv2 (key, key[6]);
	}
	key[18] ^= u32root (key[17]);
	if (k == 10)
	{
		SKYPE_RC4_IV31();
		SKYPE_RC4_IV35();
		skype_rc4_iv3(key, iv);
	}
	if (k == 4)
	{
		SKYPE_RC4_IV16n (0x41528AC);
		SKYPE_RC4_IV56();
		skype_rc4_iv4(key, iv);
	}
	key[16] += 73 * key[2];
	if (k == 14)
	{
		SKYPE_RC4_IV15();
		SKYPE_RC4_IV47n (0xA5DCFAFF);
		skype_rc4_iv5(key, key[10]);
	}
	if (k == 5)
	{
		SKYPE_RC4_IV14();
		SKYPE_RC4_IV69();
		skype_rc4_iv6(key, iv);
	}
	iv += rotr32 (iv, 19);
	if (k == 2)
	{
		SKYPE_RC4_IV7n (0x504F395C);
		SKYPE_RC4_IV61n (0xB6E979A8);
		skype_rc4_iv7 (key, iv);
		SKYPE_RC4_IV41();
		SKYPE_RC4_IV13();
		skype_rc4_iv20 (key, iv);
	}
	key[9] ^= 110 * key[1];
	if (k == 1)
	{
		SKYPE_RC4_IV56();
		SKYPE_RC4_IV47n (0x6F09E141);
		skype_rc4_iv21 (key, key[7]);
	}
	if (k == 4)
	{
		SKYPE_RC4_IV37();
		SKYPE_RC4_IV31();
		skype_rc4_iv22(key, key[7]);
	}
	if (k == 15)
	{
		SKYPE_RC4_IV29n (0xA09F88F1);
		SKYPE_RC4_IV59();
		skype_rc4_iv23(key, key[14]);
	}
	key[16] -= (key[0] < 0x22C8CF2) ? key[0] : key[5];
	if (k == 0)
	{
		SKYPE_RC4_IV57();
		SKYPE_RC4_IV45();
		skype_rc4_iv24(key, key[2]);
	}
	if (k == 5)
	{
		SKYPE_RC4_IV53();
		SKYPE_RC4_IV14();
		skype_rc4_iv1(key, key[4]);
	}
	key[9] *= (iv < 0x5FD809D2) ? iv : key[4];
	if (k == 7)
	{
		SKYPE_RC4_IV69();
		SKYPE_RC4_IV49n (0x332DA0B6);
		skype_rc4_iv2(key, key[5]);
	}
	if (k == 1)
	{
		SKYPE_RC4_IV12();
		SKYPE_RC4_IV51n (0x86D5035B);
		skype_rc4_iv3 (key, key[0]);
	}
	iv += iv - 0x5790C685;
	if (k == 12)
	{
		SKYPE_RC4_IV13();
		SKYPE_RC4_IV58n (0x121738E5);
		skype_rc4_iv4 (key, iv);
	}
	if (k == 6)
	{
		SKYPE_RC4_IV27();
		SKYPE_RC4_IV16n (0x60E885A6);
		skype_rc4_iv5 (key, key[4]);
	}
	key[18] *= 0x5790C685 ^ key[13];
	return iv;
}

u32 skype_rc4_iv9 (u32 * const key, u32 iv)
{
	u32			k = (key[18] ^ key[5] ^ key[2]) & 15;
	
	if (k == 10)
	{
		SKYPE_RC4_IV36n (0xA7EF6FAB);
		SKYPE_RC4_IV31();
		skype_rc4_iv21 (key, iv);
	}
	if (k == 6)
	{
		SKYPE_RC4_IV28();
		SKYPE_RC4_IV24();
		skype_rc4_iv22(key, key[19]);
	}
	key[5] |= u32root (key[11]);
	if (k == 5)
	{
		SKYPE_RC4_IV15();
		SKYPE_RC4_IV55();
		skype_rc4_iv23(key, key[5]);
	}
	if (k == 4)
	{
		SKYPE_RC4_IV27();
		SKYPE_RC4_IV8();
		skype_rc4_iv24(key, key[9]);
	}
	key[15] &= 88 * key[12];
	if (k == 0)
	{
		SKYPE_RC4_IV47n (0xF5D726FF);
		SKYPE_RC4_IV32();
		skype_rc4_iv1 (key, key[0]);
	}
	if (k == 7)
	{
		SKYPE_RC4_IV56();
		SKYPE_RC4_IV62n (0x6C0A34BC);
		skype_rc4_iv2 (key, key[10]);
	}
	if (k == 4)
	{
		SKYPE_RC4_IV54();
		SKYPE_RC4_IV38();
		skype_rc4_iv3 (key, key[2]);
	}
	iv += 0xFDFB78FE * key[11];
	if (k == 5)
	{
		SKYPE_RC4_IV40();
		SKYPE_RC4_IV34();
		skype_rc4_iv4 (key, key[17]);
	}
	if (k == 12)
	{
		SKYPE_RC4_IV4();
		SKYPE_RC4_IV47n (0x4EFAD4C);
		skype_rc4_iv5 (key, key[9]);
	}
	key[12] += iv - 0x4C7E1193;
	if (k == 3)
	{
		SKYPE_RC4_IV62n (0x6FFD6E50);
		SKYPE_RC4_IV9();
		skype_rc4_iv6 (key, key[17]);
	}
	if (k == 2)
	{
		SKYPE_RC4_IV59();
		SKYPE_RC4_IV13();
		skype_rc4_iv7 (key, key[15]);
	}
	if (k == 3)
	{
		SKYPE_RC4_IV9();
		SKYPE_RC4_IV64();
		skype_rc4_iv8 (key, iv);
	}
	key[16] += u32root (key[2]);
	if (k == 6)
	{
		SKYPE_RC4_IV19();
		SKYPE_RC4_IV47n (0x28288B07);
		skype_rc4_iv21 (key, key[15]);
	}
	if (k == 2)
	{
		SKYPE_RC4_IV30n (0xA623C3F4);
		SKYPE_RC4_IV65();
		skype_rc4_iv22 (key, key[5]);
	}
	key[0] *= 123 * key[19];
	if (k == 9)
	{
		SKYPE_RC4_IV41();
		SKYPE_RC4_IV35();
		skype_rc4_iv23 (key, key[0]);
	}
	if (k == 15)
	{
		SKYPE_RC4_IV1();
		SKYPE_RC4_IV52n (5);
		skype_rc4_iv24 (key, iv);
	}
	if (k == 14)
	{
		SKYPE_RC4_IV34();
		SKYPE_RC4_IV6();
		skype_rc4_iv1 (key, key[8]);
	}
	key[4] |= 0x89C7C4D3 ^ key[13];
	if (k == 13)
	{
		SKYPE_RC4_IV6();
		SKYPE_RC4_IV19();
		skype_rc4_iv2 (key, key[3]);
	}
	if (k == 8)
	{
		SKYPE_RC4_IV13();
		SKYPE_RC4_IV27();
		skype_rc4_iv3 (key, key[5]);
	}
	iv -= u32cos(key[14]) ? 0x111DCA68 : key[18];
	if (k == 11)
	{
		SKYPE_RC4_IV52n (29);
		SKYPE_RC4_IV58n (0xA35F29BB);
		skype_rc4_iv4 (key, key[3]);
	}
	if (k == 0)
	{
		SKYPE_RC4_IV69();
		SKYPE_RC4_IV18n (0x20F609B2);
		skype_rc4_iv5 (key, key[11]);
	}
	if (k == 1)
	{
		SKYPE_RC4_IV42();
		SKYPE_RC4_IV34();
		skype_rc4_iv6 (key, key[4]);
	}
	key[11] *= u32root (iv);
	if (k == 1)
	{
		SKYPE_RC4_IV18n (0x8FE3C3F3);
		SKYPE_RC4_IV1();
		skype_rc4_iv7 (key, iv);
	}
	key[16] ^= u32sin(iv) ? 0x16283882 : key[12];
	return iv;
}

u32 skype_rc4_iv10 (u32 * const key, u32 iv)
{
	u32			k = key[14] & 15;
	
	if (k == 5)
	{
		SKYPE_RC4_IV17();
		SKYPE_RC4_IV59();
		skype_rc4_iv22 (key, iv);
	}
	if (k == 9)
	{
		SKYPE_RC4_IV67n (0xBD460637);
		SKYPE_RC4_IV67n (0x23D5BA3F);
		skype_rc4_iv23 (key, key[19]);
	}
	key[19] *= u32root (iv);
	if (k == 10)
	{
		SKYPE_RC4_IV55();
		SKYPE_RC4_IV36n (0x7789BFF3);
		skype_rc4_iv24 (key, key[12]);
	}
	if (k == 11)
	{
		SKYPE_RC4_IV65();
		SKYPE_RC4_IV52n (1);
		skype_rc4_iv1 (key, iv);
	}
	key[12] ^= u32cos(key[11]) ? 0x1594E0E0 : key[7];
	if (k == 15)
	{
		SKYPE_RC4_IV57();
		SKYPE_RC4_IV23();
		skype_rc4_iv2 (key, iv);
	}
	if (k == 4)
	{
		SKYPE_RC4_IV29n (0x7D76C942);
		SKYPE_RC4_IV53();
		skype_rc4_iv3 (key, iv);
	}
	key[18] &= rotr32 (key[17], 29);
	if (k == 12)
	{
		SKYPE_RC4_IV24();
		SKYPE_RC4_IV16n (0xB82B0DAB);
		skype_rc4_iv4 (key, key[3]);
	}
	if (k == 1)
	{
		SKYPE_RC4_IV36n (0x3F7FFFE3);
		SKYPE_RC4_IV33n (0x6AEC3B22);
		skype_rc4_iv5 (key, key[5]);
	}
	key[1] -= 92 * key[19];
	if (k == 14)
	{
		SKYPE_RC4_IV30n (0x752B3066);
		SKYPE_RC4_IV2();
		skype_rc4_iv6 (key, key[0]);
	}
	if (k == 3)
	{
		SKYPE_RC4_IV65();
		SKYPE_RC4_IV39n (0x2CE8A84E);
		skype_rc4_iv7 (key, iv);
	}
	key[14] += 0x1579CA5 - iv;
	if (k == 1)
	{
		SKYPE_RC4_IV35();
		SKYPE_RC4_IV64();
		skype_rc4_iv8 (key, iv);
	}
	if (k == 6)
	{
		SKYPE_RC4_IV20n (0x2EFE59F4);
		SKYPE_RC4_IV66();
		skype_rc4_iv9 (key, iv);
	}
	key[8] -= 78 * key[1];
	if (k == 13)
	{
		SKYPE_RC4_IV20n (0x18A96853);
		SKYPE_RC4_IV47n (0x0F6AF31E);
		skype_rc4_iv22 (key, key[14]);
	}
	if (k == 2)
	{
		SKYPE_RC4_IV48();
		SKYPE_RC4_IV52n (4);
		skype_rc4_iv23 (key, key[13]);
	}
	key[16] = rotr32 (key[16], key[15] | 30);
	if (k == 0)
	{
		SKYPE_RC4_IV36n (0xA7E9B7A3);
		SKYPE_RC4_IV44();
		skype_rc4_iv24 (key, key[7]);
		SKYPE_RC4_IV49n (0xF7849C32);
		SKYPE_RC4_IV32();
		skype_rc4_iv1 (key, key[6]);
	}
	key[8] -= 12 * key[18];
	if (k == 4)
	{
		SKYPE_RC4_IV69();
		SKYPE_RC4_IV70();
		skype_rc4_iv2 (key, iv);
	}
	if (k == 3)
	{
		SKYPE_RC4_IV33n (0x351E5020);
		SKYPE_RC4_IV40();
		skype_rc4_iv3 (key, key[1]);
	}
	key[14] ^= u32cos(key[5]) ? 0x579B4B85 : key[6];
	if (k == 2)
	{
		SKYPE_RC4_IV64();
		SKYPE_RC4_IV57();
		skype_rc4_iv4 (key, key[10]);
	}
	if (k == 5)
	{
		SKYPE_RC4_IV38();
		SKYPE_RC4_IV10();
		skype_rc4_iv5 (key, iv);
	}
	key[10] += 0x3575DC9 * key[13];
	if (k == 7)
	{
		SKYPE_RC4_IV50();
		SKYPE_RC4_IV16n (0xB8AD3ED9);
		skype_rc4_iv6 (key, key[16]);
		SKYPE_RC4_IV29n (0xC279F292);
		SKYPE_RC4_IV48();
		skype_rc4_iv7 (key, key[16]);
	}
	if (k == 8)
	{
		SKYPE_RC4_IV2();
		SKYPE_RC4_IV45();
		skype_rc4_iv8 (key, key[14]);
	}
	key[4] ^= key[5] & 0x136008B9;
	if (k == 6)
	{
		SKYPE_RC4_IV66();
		SKYPE_RC4_IV10();
		skype_rc4_iv9 (key, key[4]);
	}
	key[3] ^= 0x29909382 + key[13];
	return iv;
}

u32 skype_rc4_iv11 (u32 * const key, u32 iv)
{
	u32			k = key[3] & 15;
	
	if (k == 5)
	{
		SKYPE_RC4_IV45();
		SKYPE_RC4_IV14();
		skype_rc4_iv23 (key, key[4]);
	}
	if (k == 7)
	{
		SKYPE_RC4_IV20n (0xBEF59983);
		SKYPE_RC4_IV34();
		skype_rc4_iv24 (key, iv);
	}
	key[4] *= u32sin(iv) ? 0x23D1B75F : key[11];
	if (k == 10)
	{
		SKYPE_RC4_IV44();
		SKYPE_RC4_IV15();
		skype_rc4_iv1 (key, iv);
	}
	if (k == 8)
	{
		SKYPE_RC4_IV15();
		SKYPE_RC4_IV13();
		skype_rc4_iv2 (key, key[11]);
	}
	iv *= key[13] - 0x21175CB;
	if (k == 4)
	{
		SKYPE_RC4_IV63n (0x60AB10C9);
		SKYPE_RC4_IV12();
		skype_rc4_iv3 (key, key[11]);
	}
	if (k == 2)
	{
		SKYPE_RC4_IV34();
		SKYPE_RC4_IV32();
		skype_rc4_iv4 (key, key[15]);
		SKYPE_RC4_IV21();
		SKYPE_RC4_IV1();
		skype_rc4_iv5 (key, key[8]);
	}
	iv = rotl32 (iv, iv + 5);
	if (k == 4)
	{
		SKYPE_RC4_IV67n (0xF2F0F729);
		SKYPE_RC4_IV9();
		skype_rc4_iv6 (key, key[7]);
	}
	if (k == 0)
	{
		SKYPE_RC4_IV59();
		SKYPE_RC4_IV52n (12);
		skype_rc4_iv7 (key, key[17]);
	}
	key[3] = rotl32 (key[3], (key[8] < 0x354AE538) ? key[8] : iv);
	if (k == 0)
	{
		SKYPE_RC4_IV28();
		SKYPE_RC4_IV8();
		skype_rc4_iv8 (key, key[7]);
	}
	if (k == 11)
	{
		SKYPE_RC4_IV61n (0xD083A1C7);
		SKYPE_RC4_IV23();
		skype_rc4_iv9 (key, key[3]);
	}
	key[7] -= key[18] ^ 0xF1CDC7D;
	if (k == 8)
	{
		SKYPE_RC4_IV9();
		SKYPE_RC4_IV47n (0xC67A38DE);
		skype_rc4_iv10 (key, key[16]);
	}
	if (k == 1)
	{
		SKYPE_RC4_IV14();
		SKYPE_RC4_IV34();
		skype_rc4_iv23 (key, iv);
	}
	if (k == 15)
	{
		SKYPE_RC4_IV44();
		SKYPE_RC4_IV3n (0xE6E035C2);
		skype_rc4_iv24 (key, key[3]);
	}
	iv += (key[13] < 0x32081481) ? key[13] : iv;
	if (k == 9)
	{
		SKYPE_RC4_IV51n (0xBFAF8559);
		SKYPE_RC4_IV30n (0x8494F082);
		skype_rc4_iv1 (key, key[10]);
	}
	if (k == 3)
	{
		SKYPE_RC4_IV24();
		SKYPE_RC4_IV20n (0x0D4CA763);
		skype_rc4_iv2 (key, key[3]);
	}
	key[18] += 0xD3BE6F0E - key[17];
	if (k == 1)
	{
		SKYPE_RC4_IV51n (0xAEB00548);
		SKYPE_RC4_IV63n (0x91E7D2DC);
		skype_rc4_iv3 (key, key[13]);
	}
	if (k == 5)
	{
		SKYPE_RC4_IV5();
		SKYPE_RC4_IV36n (0x379DEFB3);
		skype_rc4_iv4 (key, iv);
	}
	if (k == 6)
	{
		SKYPE_RC4_IV19();
		SKYPE_RC4_IV4();
		skype_rc4_iv5 (key, key[7]);
	}
	key[7] -= (iv < 0xD28B93FE) ? iv : key[18];
	if (k == 14)
	{
		SKYPE_RC4_IV4();
		SKYPE_RC4_IV67n (0xD0D89937);
		skype_rc4_iv6 (key, iv);
	}
	if (k == 3)
	{
		SKYPE_RC4_IV3n (0x91403FF1);
		SKYPE_RC4_IV54();
		skype_rc4_iv7 (key, key[10]);
	}
	key[2] = rotl32 (key[2], -80 * key[17]);
	if (k == 13)
	{
		SKYPE_RC4_IV63n (0x313B46D8);
		SKYPE_RC4_IV47n (0xD9FEC55F);
		skype_rc4_iv8 (key, key[10]);
	}
	if (k == 12)
	{
		SKYPE_RC4_IV20n (0xDFBCF197);
		SKYPE_RC4_IV36n (0x3F4F37BB);
		skype_rc4_iv9 (key, iv);
	}
	key[17] -= key[3] & 0x5C40869;
	if (k == 7)
	{
		SKYPE_RC4_IV8();
		SKYPE_RC4_IV47n (0xE8B5F17D);
		skype_rc4_iv10 (key, key[4]);
	}
	if (k == 6)
	{
		SKYPE_RC4_IV63n (0x4321C7C4);
		SKYPE_RC4_IV2();
		skype_rc4_iv23 (key, key[7]);
	}
	key[6] += iv - 0xAD58C16C;
	return iv;
}

u32 skype_rc4_iv12 (u32 * const key, u32 iv)
{
	u32			k = (key[19] ^ iv ^ key[0]) & 15;
	
	if (k == 2)
	{
		SKYPE_RC4_IV32();
		SKYPE_RC4_IV38();
		skype_rc4_iv24 (key, key[6]);
	}
	if (k == 3)
	{
		SKYPE_RC4_IV17();
		SKYPE_RC4_IV69();
		skype_rc4_iv1 (key, key[0]);
	}
	iv -= rotr32 (key[7], 10);
	if (k == 8)
	{
		SKYPE_RC4_IV27();
		SKYPE_RC4_IV39n (0x454B4CB0);
		skype_rc4_iv2 (key, key[5]);
	}
	if (k == 13)
	{
		SKYPE_RC4_IV37();
		SKYPE_RC4_IV14();
		skype_rc4_iv3 (key, key[4]);
	}
	key[5] ^= key[13] - 0x05648E1C;
	if (k == 1)
	{
		SKYPE_RC4_IV63n (0x5F7C048F);
		SKYPE_RC4_IV4();
		skype_rc4_iv4 (key, key[5]);
	}
	if (k == 15)
	{
		SKYPE_RC4_IV57();
		SKYPE_RC4_IV61n (0xDBC32E55);
		skype_rc4_iv5 (key, iv);
	}
	if (k == 7)
	{
		SKYPE_RC4_IV56();
		SKYPE_RC4_IV69();
		skype_rc4_iv6 (key, key[16]);
	}
	key[0] ^= 0xC8350BCD * key[17];
	if (k == 1)
	{
		SKYPE_RC4_IV6();
		SKYPE_RC4_IV61n (0xF14A0809);
		skype_rc4_iv7 (key, key[12]);
	}
	if (k == 10)
	{
		SKYPE_RC4_IV42();
		SKYPE_RC4_IV36n (0xEF4BAFA3);
		skype_rc4_iv8 (key, key[16]);
	}
	if (k == 4)
	{
		SKYPE_RC4_IV57();
		SKYPE_RC4_IV4();
		skype_rc4_iv9 (key, key[18]);
	}
	iv &= (0xC8350BCD + key[13]);
	if (k == 5)
	{
		SKYPE_RC4_IV16n (0x75B10262);
		SKYPE_RC4_IV21();
		skype_rc4_iv10 (key, key[7]);
	}
	if (k == 6)
	{
		SKYPE_RC4_IV56();
		SKYPE_RC4_IV1();
		skype_rc4_iv11 (key, key[2]);
	}
	key[2] = rotr32 (key[2], key[17] & 8);
	if (k == 5)
	{
		SKYPE_RC4_IV22();
		SKYPE_RC4_IV24();
		skype_rc4_iv24 (key, iv);
	}
	if (k == 0)
	{
		SKYPE_RC4_IV23();
		SKYPE_RC4_IV57();
		skype_rc4_iv1 (key, key[8]);
	}
	iv = rotr32 (iv, iv - 16);
	if (k == 9)
	{
		SKYPE_RC4_IV1();
		SKYPE_RC4_IV62n (0x7C854F50);
		skype_rc4_iv2 (key, key[2]);
	}
	if (k == 3)
	{
		SKYPE_RC4_IV38();
		SKYPE_RC4_IV25();
		skype_rc4_iv3 (key, key[16]);
	}
	if (k == 6)
	{
		SKYPE_RC4_IV37();
		SKYPE_RC4_IV50();
		skype_rc4_iv4 (key, key[12]);
	}
	key[17] ^= rotl32 (key[3], 5);
	if (k == 4)
	{
		SKYPE_RC4_IV37();
		SKYPE_RC4_IV54();
		skype_rc4_iv5 (key, key[19]);
	}
	if (k == 11)
	{
		SKYPE_RC4_IV10();
		SKYPE_RC4_IV30n (0xBDDD6945);
		skype_rc4_iv6 (key, iv);
	}
	key[15] += 0x531BF4BD - key[5];
	if (k == 0)
	{
		SKYPE_RC4_IV24();
		SKYPE_RC4_IV70();
		skype_rc4_iv7 (key, iv);
	}
	if (k == 9)
	{
		SKYPE_RC4_IV27();
		SKYPE_RC4_IV22();
		skype_rc4_iv8 (key, iv);
	}
	if (k == 14)
	{
		SKYPE_RC4_IV21();
		SKYPE_RC4_IV8();
		skype_rc4_iv9 (key, key[7]);
	}
	key[5] &= (key[9] < 0x802D5786) ? key[9] : key[16];
	if (k == 12)
	{
		SKYPE_RC4_IV5();
		SKYPE_RC4_IV48();
		skype_rc4_iv10 (key, iv);
	}
	if (k == 7)
	{
		SKYPE_RC4_IV62n (0x75382920);
		SKYPE_RC4_IV64();
		skype_rc4_iv11 (key, key[10]);
	}
	key[1] *= 104 * key[12];
	if (k == 8)
	{
		SKYPE_RC4_IV24();
		SKYPE_RC4_IV3n (0xCCB27D90);
		skype_rc4_iv24 (key, key[19]);
	}
	if (k == 2)
	{
		SKYPE_RC4_IV13();
		SKYPE_RC4_IV54();
		skype_rc4_iv1 (key, key[4]);
	}
	key[17] ^= (iv < 0x136FA01) ? iv : key[18];
	return iv;
}

u32 skype_rc4_iv13 (u32 * const key, u32 iv)
{
	u32			k = (iv ^ key[0] ^ key[18]) & 1;
	
	key[11] ^= key[5] | 0x16AE2234;
	key[12] += (key[8] & 0xAE32B8E);
	key[0] -= 12 * key[16];
	key[16] ^= iv + 0xA8BDFE15;
	key[9] -= 0x8181F7E7 + key[10];
	if (k == 0)
	{
		SKYPE_RC4_IV47n (0x24B5D4C1);
		SKYPE_RC4_IV69();
		SKYPE_RC4_IV71n (iv);
	}
	iv = rotr32 (iv, 30 * key[16]);
	iv ^= rotr32 (key[15], 20);
	key[13] += key[13] ^ 0x486B19AC;
	key[2] |= key[9] + 0x1086C59F;
	key[14] |= u32cos(iv) ? 0x5BADEF93 : key[10];
	if (k == 1)
	{
		SKYPE_RC4_IV9();
		SKYPE_RC4_IV29n (0xB0DAD1C3);
		SKYPE_RC4_IV72n (key[17]);
	}
	key[12] += (key[10] < 0x2EE7C8C3) ? key[10] : iv;
	return iv;
}

u32 skype_rc4_iv14 (u32 * const key, u32 iv)
{
	u32			k = (iv ^ key[0] ^ key[16]) % 3;
	
	key[12] += (iv < 0x3FEC317) ? iv : key[12];
	iv = rotr32 (iv, iv - 8);
	key[10] *= key[10] - 0x54940E;
	if (k == 2)
	{
		SKYPE_RC4_IV52n (29);
		SKYPE_RC4_IV1();
		SKYPE_RC4_IV71n (key[19]);
	}
	key[2] = rotr32 (key[2], rotr32 (key[6], 22));
	key[12] = rotr32 (key[12], key[1] - 0x2206F590);
	key[15] = rotl32 (key[15], key[8] ^ 150);
	if (k == 0)
	{
		SKYPE_RC4_IV32();
		SKYPE_RC4_IV43();
		SKYPE_RC4_IV72n (key[5]);
	}
	iv += u32root (key[9]);
	key[10] -= rotl32 (iv, 0x209B4CB8);
	key[1] += 41 * iv;
	key[9] *= 105 * key[3];
	if (k == 1)
	{
		SKYPE_RC4_IV24();
		SKYPE_RC4_IV55();
		skype_rc4_iv13 (key, key[0]);
	}
	iv -= 0xFCCEF753 | key[10];
	return iv;
}

u32 skype_rc4_iv15 (u32 * const key, u32 iv)
{
	u32			k = key[16] & 3;
	
	iv += rotl32 (key[10], 4);
	iv = rotr32 (iv, u32cos(key[18]) ? 14 : key[15]);
	if (k == 3)
	{
		SKYPE_RC4_IV27();
		SKYPE_RC4_IV53();
		SKYPE_RC4_IV71n (key[2]);
	}
	iv = rotl32 (iv, rotr32 (key[6], 21));
	key[17] |= key[11] - 0xBF85104;
	key[17] -= 78 * iv;
	if (k == 1)
	{
		SKYPE_RC4_IV70();
		SKYPE_RC4_IV9();
		SKYPE_RC4_IV72n (iv);
	}
	key[15] += u32cos(iv) ? 0xA82B365 : key[7];
	key[17] -= u32cos(key[11]) ? 0x9AA8540 : iv;
	iv ^= u32sin(key[12]) ? 0x3B018731 : key[12];
	if (k == 2)
	{
		SKYPE_RC4_IV69();
		SKYPE_RC4_IV70();
		skype_rc4_iv13 (key, iv);
	}
	key[7] ^= u32sin(key[17]) ? 0x836B3E03 : key[6];
	key[2] ^= key[6] + 0x1E0BBA53;
	key[15] += 0x812EAA9D - key[1];
	if (k == 0)
	{
		SKYPE_RC4_IV42();
		SKYPE_RC4_IV37();
		skype_rc4_iv14 (key, key[2]);
	}
	key[1] ^= (key[14] < 0x812EAA9D) ? key[14] : iv;
	return iv;
}

u32 skype_rc4_iv16 (u32 * const key, u32 iv)
{
	u32			k = (key[12] ^ iv ^ key[5]) % 5;
	
	key[17] = rotl32 (key[17], u32cos(iv) ? 2 : key[19]);
	iv -= iv | 0x16E2A7D1;
	if (k == 2)
	{
		SKYPE_RC4_IV8();
		SKYPE_RC4_IV8();
		SKYPE_RC4_IV71n (key[0]);
	}
	key[15] *= 118 * iv;
	key[12] = rotl32 (key[12], key[10] - 7);
	if (k == 1)
	{
		SKYPE_RC4_IV4();
		SKYPE_RC4_IV19();
		SKYPE_RC4_IV72n (key[3]);
	}
	key[2] += (key[13] < 0x588BA7CA) ? key[13] : iv;
	key[19] -= (key[12] < 0x3F5E1B0) ? key[12] : key[9];
	if (k == 3)
	{
		SKYPE_RC4_IV12();
		SKYPE_RC4_IV36n (0xBFA93FA3);
		skype_rc4_iv13 (key, key[14]);
	}
	iv *= -11;
	key[3] += 76 * key[5];
	if (k == 4)
	{
		SKYPE_RC4_IV48();
		SKYPE_RC4_IV55();
		skype_rc4_iv14 (key, key[17]);
	}
	key[11] *= key[3] | 0xA760EE25;
	key[19] += key[12] + 0xA760EE25;
	if (k == 0)
	{
		SKYPE_RC4_IV32();
		SKYPE_RC4_IV11();
		skype_rc4_iv15 (key, key[6]);
	}
	key[15] = rotr32 (key[15], (key[14] < 0x9161D683) ? key[14] : key[9]);
	return iv;
}

u32 skype_rc4_iv17 (u32 * const key, u32 iv)
{
	u32			k = (key[14] ^ key[2] ^ key[6]) % 6;
	
	key[1] += u32sin(key[1]) ? 0x1FAE4F0 : key[17];
	if (k == 4)
	{
		SKYPE_RC4_IV67n (0xF37F8F83);
		SKYPE_RC4_IV52n (31);
		SKYPE_RC4_IV71n (key[7]);
	}
	key[3] -= rotl32 (key[9], 5);
	key[12] *= rotr32 (key[4], 19);
	if (k == 3)
	{
		SKYPE_RC4_IV28();
		SKYPE_RC4_IV56();
		SKYPE_RC4_IV72n (iv);
	}
	iv -= rotl32 (iv, 18);
	key[19] ^= iv + 0x222E1310;
	if (k == 2)
	{
		SKYPE_RC4_IV38();
		SKYPE_RC4_IV52n (6);
		skype_rc4_iv13 (key, key[18]);
	}
	key[17] ^= iv - 0x7FAB735B;
	if (k == 0)
	{
		SKYPE_RC4_IV3n (0xD2430E50);
		SKYPE_RC4_IV25();
		skype_rc4_iv14 (key, iv);
	}
	key[14] |= key[1] ^ 0x71B103BD;
	iv = rotr32 (iv, 6 * key[16]);
	if (k == 1)
	{
		SKYPE_RC4_IV48();
		SKYPE_RC4_IV60();
		skype_rc4_iv15 (key, key[17]);
	}
	key[0] ^= u32root (key[15]);
	key[14] = rotr32 (key[14], u32cos(key[15]) ? 8 : key[1]);
	if (k == 0)
	{
		SKYPE_RC4_IV56();
		SKYPE_RC4_IV50();
		skype_rc4_iv16 (key, key[9]);
	}
	key[10] -= u32cos(key[13]) ? 0xF83DD71A : key[10];
	return iv;
}

u32 skype_rc4_iv18 (u32 * const key, u32 iv)
{
	u32			k = key[14] % 7;
	
	iv -= 31 * key[6];
	if (k == 5)
	{
		SKYPE_RC4_IV6();
		SKYPE_RC4_IV7n (0x3DB5CDC8);
		SKYPE_RC4_IV71n (key[0]);
	}
	key[14] ^= key[15] - 0x274BB11;
	if (k == 4)
	{
		SKYPE_RC4_IV11();
		SKYPE_RC4_IV13();
		SKYPE_RC4_IV72n (key[12]);
	}
	iv ^= 8 * key[1];
	if (k == 2)
	{
		SKYPE_RC4_IV47n (0x1D06A87F);
		SKYPE_RC4_IV48();
		skype_rc4_iv13 (key, key[13]);
	}
	key[16] = rotl32 (key[16], u32root (key[15]));
	key[19] *= u32root (key[10]);
	if (k == 3)
	{
		SKYPE_RC4_IV5();
		SKYPE_RC4_IV32();
		skype_rc4_iv14 (key, key[4]);
	}
	iv += key[16] | 0x1DF5D7AD;
	if (k == 0)
	{
		SKYPE_RC4_IV35();
		SKYPE_RC4_IV12();
		skype_rc4_iv15 (key, key[4]);
	}
	iv *= key[2] + 0xC70282A8;
	if (k == 0)
	{
		SKYPE_RC4_IV5();
		SKYPE_RC4_IV54();
		skype_rc4_iv16 (key, key[9]);
	}
	key[6] &= iv - 0x9798A83;
	iv = rotr32 (iv, 2 * key[11]);
	if (k == 1)
	{
		SKYPE_RC4_IV45();
		SKYPE_RC4_IV17();
		skype_rc4_iv17 (key, key[7]);
	}
	key[2] ^= (key[9] < 0x2EEDED9) ? key[9] : key[12];
	return iv;
}

u32 skype_rc4_iv19 (u32 * const key, u32 iv)
{
	u32			k = (key[13] ^ iv ^ key[4]) & 7;
	
	key[16] *= key[15];
	if (k == 0)
	{
		SKYPE_RC4_IV27();
		SKYPE_RC4_IV40();
		SKYPE_RC4_IV71n (iv);
	}
	key[14] = rotr32 (key[14], 87 * key[11]);
	if (k == 2)
	{
		SKYPE_RC4_IV12();
		SKYPE_RC4_IV68n (0x73492C37);
		SKYPE_RC4_IV72n (key[16]);
	}
	iv &= iv ^ 0xBFEECFFE;
	if (k == 6)
	{
		SKYPE_RC4_IV13();
		SKYPE_RC4_IV68n (0x7FE25F6B);
		skype_rc4_iv13 (key, key[7]);
	}
	iv += u32cos(iv) ? 0xD68DC95 : key[5];
	key[2] ^= u32sin(key[7]) ? 0x2124BD5 : iv;
	if (k == 4)
	{
		SKYPE_RC4_IV21();
		SKYPE_RC4_IV26();
		skype_rc4_iv14 (key, key[5]);
	}
	key[2] ^= (iv < 0xA427B60) ? iv : key[19];
	if (k == 0)
	{
		SKYPE_RC4_IV1();
		SKYPE_RC4_IV66();
		skype_rc4_iv15 (key, iv);
	}
	key[12] -= u32root (iv);
	if (k == 1)
	{
		SKYPE_RC4_IV48();
		SKYPE_RC4_IV65();
		skype_rc4_iv16 (key, key[15]);
	}
	key[16] -= 0x61A83016 | key[4];
	if (k == 3)
	{
		SKYPE_RC4_IV1();
		SKYPE_RC4_IV48();
		skype_rc4_iv17 (key, iv);
	}
	iv *= u32root (iv);
	key[13] *= 0x59F1E662 + key[16];
	if (k == 7)
	{
		SKYPE_RC4_IV26();
		SKYPE_RC4_IV15();
		skype_rc4_iv18 (key, key[19]);
	}
	key[3] ^= 0x699B23 * key[12];
	return iv;
}

u32 skype_rc4_iv20 (u32 * const key, u32 iv)
{
	u32			k = (iv ^ key[7] ^ key[12]) % 9;
	
	iv = rotr32 (iv, u32root (iv));
	if (k == 8)
	{
		SKYPE_RC4_IV46();
		SKYPE_RC4_IV29n (0x141586A);
		SKYPE_RC4_IV71n (key[15]);
	}
	key[4] += 15 * key[16];
	if (k == 0)
	{
		SKYPE_RC4_IV16n (0x1CB835FD);
		SKYPE_RC4_IV63n (0x835400E0);
		SKYPE_RC4_IV72n (key[16]);
	}
	key[10] ^= rotr32 (key[15], 30);
	if (k == 1)
	{
		SKYPE_RC4_IV13();
		SKYPE_RC4_IV42();
		skype_rc4_iv13 (key, iv);
	}
	key[7] -= u32sin(iv) ? 0x0D93E92C : key[16];
	if (k == 2)
	{
		SKYPE_RC4_IV68n (0x66E0FF5C);
		SKYPE_RC4_IV59();
		skype_rc4_iv14 (key, key[17]);
	}
	key[6] *= rotr32 (key[10], 7);
	if (k == 6)
	{
		SKYPE_RC4_IV15();
		SKYPE_RC4_IV55();
		skype_rc4_iv15 (key, key[14]);
	}
	iv -= 90 * key[16];
	if (k == 4)
	{
		SKYPE_RC4_IV6();
		SKYPE_RC4_IV39n (0x7C7D7E42);
		skype_rc4_iv16 (key, key[4]);
	}
	key[3] -= key[16] & 0xEFF9DB02;
	if (k == 0)
	{
		SKYPE_RC4_IV15();
		SKYPE_RC4_IV66();
		skype_rc4_iv17 (key, key[13]);
	}
	iv += u32cos(key[19]) ? 0x39AEBDF : key[5];
	if (k == 7)
	{
		SKYPE_RC4_IV64();
		SKYPE_RC4_IV59();
		skype_rc4_iv18 (key, key[6]);
	}
	key[13] |= u32root (key[10]);
	if (k == 5)
	{
		SKYPE_RC4_IV25();
		SKYPE_RC4_IV68n (0x54AE62D0);
		skype_rc4_iv19 (key, key[14]);
	}
	iv = rotl32 (iv, 26 * iv);
	key[14] -= u32sin(key[1]) ? 0xB95DC2D3 : key[18];
	return iv;
}

u32 skype_rc4_iv21 (u32 * const key, u32 iv)
{
	u32			k = (key[17] ^ key[16] ^ key[7]) % 10;
	
	key[15] -= 13 * key[7];
	if (k == 2)
	{
		SKYPE_RC4_IV28();
		SKYPE_RC4_IV54();
		SKYPE_RC4_IV71n (key[14]);
	}
	key[7] += key[5] ^ 0x28CA4358;
	if (k == 1)
	{
		SKYPE_RC4_IV17();
		SKYPE_RC4_IV30n (0xC14393E2);
		SKYPE_RC4_IV72n (key[10]);
	}
	key[10] += iv & 0xE9F1006;
	if (k == 9)
	{
		SKYPE_RC4_IV67n (0x3C50D16);
		SKYPE_RC4_IV2();
		skype_rc4_iv13 (key, iv);
	}
	key[19] *= (key[0] < 0xB71D9592) ? key[0] : key[14];
	if (k == 5)
	{
		SKYPE_RC4_IV16n (0x6481D348);
		SKYPE_RC4_IV54();
		skype_rc4_iv14 (key, iv);
	}
	key[16] += 0x7C476077 - key[0];
	if (k == 4)
	{
		SKYPE_RC4_IV70();
		SKYPE_RC4_IV35();
		skype_rc4_iv15 (key, key[10]);
	}
	iv ^= key[10] << 6;
	if (k == 0)
	{
		SKYPE_RC4_IV25();
		SKYPE_RC4_IV42();
		skype_rc4_iv16 (key, key[4]);
	}
	key[3] += u32sin(key[12]) ? 0x7B0CA6E0 : iv;
	if (k == 6)
	{
		SKYPE_RC4_IV61n (0xFCD29227);
		SKYPE_RC4_IV47n (0x3FFF3F1E);
		skype_rc4_iv17 (key, key[2]);
	}
	iv |= 0x2FF561CA * iv;
	if (k == 8)
	{
		SKYPE_RC4_IV62n (0x5BCC4FBE);
		SKYPE_RC4_IV47n (0x8328D41F);
		skype_rc4_iv18 (key, key[3]);
	}
	key[14] += rotr32 (key[0], 26);
	if (k == 3)
	{
		SKYPE_RC4_IV68n (0x7F1F0A41);
		SKYPE_RC4_IV37();
		skype_rc4_iv19 (key, iv);
	}
	key[14] -= (key[6] < 0x2C23ABF1) ? key[6] : key[19];
	if (k == 7)
	{
		SKYPE_RC4_IV63n (0x3C12C053);
		SKYPE_RC4_IV7n (0x302EE99C);
		skype_rc4_iv20 (key, key[17]);
	}
	key[4] &= 14 * key[11];
	return iv;
}

u32 skype_rc4_iv22 (u32 * const key, u32 iv)
{
	u32			k = (iv ^ key[2] ^ key[6]) % 11;
	
	key[17] = rotr32 (key[17], iv - 7);
	if (k == 8)
	{
		SKYPE_RC4_IV39n (0x74FA76D5);
		SKYPE_RC4_IV62n (0x6D3C052C);
		SKYPE_RC4_IV71n (key[19]);
	}
	key[16] -= 0xF9263500 & key[8];
	if (k == 0)
	{
		SKYPE_RC4_IV66();
		SKYPE_RC4_IV55();
		SKYPE_RC4_IV72n (key[14]);
	}
	iv += (key[9] ^ 0x32C5F54);
	if (k == 1)
	{
		SKYPE_RC4_IV47n (0x187AA563);
		SKYPE_RC4_IV44();
		skype_rc4_iv13 (key, iv);
	}
	key[15] += (key[4] < 0x763BB14) ? key[4] : iv;
	if (k == 6)
	{
		SKYPE_RC4_IV28();
		SKYPE_RC4_IV63n (0x290286CE);
		skype_rc4_iv14 (key, key[18]);
	}
	key[4] += key[11] + 0x1B2FADB0;
	if (k == 10)
	{
		SKYPE_RC4_IV37();
		SKYPE_RC4_IV26();
		skype_rc4_iv15 (key, key[14]);
	}
	iv &= key[3] & 0x4E081541;
	if (k == 9)
	{
		SKYPE_RC4_IV55();
		SKYPE_RC4_IV3n (0xD89C09D3);
		skype_rc4_iv16 (key, key[14]);
	}
	key[18] ^= (iv < 0x22E75CE0) ? iv : key[3];
	if (k == 4)
	{
		SKYPE_RC4_IV32();
		SKYPE_RC4_IV20n (0x682AB283);
		skype_rc4_iv17 (key, iv);
	}
	iv |= rotl32 (key[14], 102);
	if (k == 3)
	{
		SKYPE_RC4_IV41();
		SKYPE_RC4_IV12();
		skype_rc4_iv18 (key, iv);
	}
	key[3] += rotr32 (iv, 19);
	if (k == 2)
	{
		SKYPE_RC4_IV65();
		SKYPE_RC4_IV7n (0x33822AB7);
		skype_rc4_iv19 (key, iv);
	}
	key[9] ^= 52 * key[7];
	if (k == 5)
	{
		SKYPE_RC4_IV35();
		SKYPE_RC4_IV16n (0xD0D19C61);
		skype_rc4_iv20 (key, key[1]);
	}
	iv *= 0xB1DB36B2 * key[0];
	if (k == 0)
	{
		SKYPE_RC4_IV19();
		SKYPE_RC4_IV58n (0xCE82C1C);
		skype_rc4_iv21 (key, key[17]);
	}
	key[10] *= (key[17] < 0xD066ADA) ? key[17] : iv;
	return iv;
}

u32 skype_rc4_iv23 (u32 * const key, u32 iv)
{
	u32			k = (key[6] ^ iv ^ key[5]) % 12;
	
	if (k == 2)
	{
		SKYPE_RC4_IV43();
		SKYPE_RC4_IV19();
		SKYPE_RC4_IV71n (key[11]);
	}
	key[17] = rotl32 (key[17], key[5] - 0x2E74D55C);
	if (k == 9)
	{
		SKYPE_RC4_IV49n (0x5A1FF296);
		SKYPE_RC4_IV29n (0x0EC7C0E9);
		SKYPE_RC4_IV72n (key[10]);
	}
	iv = rotl32 (iv, 0x3FCB120B + key[2]);
	if (k == 11)
	{
		SKYPE_RC4_IV61n (0xAF5BAF57);
		SKYPE_RC4_IV14();
		skype_rc4_iv13 (key, iv);
	}
	key[14] |= 0x3FC983D0 + iv;
	if (k == 8)
	{
		SKYPE_RC4_IV62n (0x59806A89);
		SKYPE_RC4_IV38();
		skype_rc4_iv14 (key, key[2]);
	}
	if (k == 10)
	{
		SKYPE_RC4_IV23();
		SKYPE_RC4_IV6();
		skype_rc4_iv15 (key, key[2]);
	}
	key[10] = rotl32 (key[10], u32cos(key[11]) ? 0x3FC983D0 : key[11]);
	if (k == 0)
	{
		SKYPE_RC4_IV62n (0x50F929B8);
		SKYPE_RC4_IV70();
		skype_rc4_iv16 (key, iv);
	}
	key[0] += key[2] + 0x56775D25;
	if (k == 6)
	{
		SKYPE_RC4_IV52n (7);
		SKYPE_RC4_IV41();
		skype_rc4_iv17 (key, iv);
	}
	iv |= 123 * key[9];
	if (k == 1)
	{
		SKYPE_RC4_IV4();
		SKYPE_RC4_IV24();
		skype_rc4_iv18 (key, key[6]);
	}
	if (k == 5)
	{
		SKYPE_RC4_IV3n (0x693CB362);
		SKYPE_RC4_IV38();
		skype_rc4_iv19 (key, key[0]);
	}
	key[8] = rotr32 (key[8], (iv < 0x48821AB) ? iv : key[8]);
	if (k == 0)
	{
		SKYPE_RC4_IV43();
		SKYPE_RC4_IV62n (0x56278C42);
		skype_rc4_iv20 (key, key[10]);
	}
	key[12] ^= 48 * key[18];
	if (k == 3)
	{
		SKYPE_RC4_IV59();
		SKYPE_RC4_IV16n (0xD061FA2F);
		skype_rc4_iv21 (key, key[17]);
	}
	key[18] |= 58 * key[4];
	if (k == 7)
	{
		SKYPE_RC4_IV4();
		SKYPE_RC4_IV70();
		skype_rc4_iv22 (key, key[5]);
	}
	if (k == 4)
	{
		SKYPE_RC4_IV21();
		SKYPE_RC4_IV56();
		SKYPE_RC4_IV71n (key[4]);
	}
	key[5] ^= u32cos(key[8]) ? 0x16925B88 : key[8];
	return iv;
}

u32 skype_rc4_iv24 (u32 * const key, u32 iv)
{
	u32			k = (iv ^ key[8] ^ key[11]) % 13;
	
	if (k == 7)
	{
		SKYPE_RC4_IV12();
		SKYPE_RC4_IV63n (0x3EB1A4B6);
		SKYPE_RC4_IV72n (iv);
	}
	key[2] += u32sin(key[16]) ? 0xBE51568 : key[17];
	if (k == 0)
	{
		SKYPE_RC4_IV31();
		SKYPE_RC4_IV1();
		skype_rc4_iv13 (key, key[0]);
	}
	iv -= (key[15] < 0x214D38C3) ? key[15] : key[14];
	if (k == 6)
	{
		SKYPE_RC4_IV18n (0xE50F49F3);
		SKYPE_RC4_IV19();
		skype_rc4_iv14 (key, key[9]);
	}
	iv += 0x20F1E8E5 - key[17];
	if (k == 1)
	{
		SKYPE_RC4_IV55();
		SKYPE_RC4_IV16n (0x91B25DAB);
		skype_rc4_iv15 (key, key[19]);
	}
	iv *= 113;
	if (k == 5)
	{
		SKYPE_RC4_IV19();
		SKYPE_RC4_IV40();
		skype_rc4_iv16 (key, key[5]);
	}
	key[4] |= iv - 0x0D896A46;
	if (k == 11)
	{
		SKYPE_RC4_IV63n (0x291937D1);
		SKYPE_RC4_IV70();
		skype_rc4_iv17 (key, key[2]);
	}
	if (k == 10)
	{
		SKYPE_RC4_IV34();
		SKYPE_RC4_IV3n (0x2DE98C73);
		skype_rc4_iv18 (key, key[16]);
	}
	key[19] = rotl32 (key[19], u32cos(key[4]) ? 0x0D896A46 : key[4]);
	if (k == 12)
	{
		SKYPE_RC4_IV26();
		SKYPE_RC4_IV49n (0x054BD742);
		skype_rc4_iv19 (key, key[9]);
	}
	key[5] = rotr32 (key[5], 9 * key[15]);
	if (k == 0)
	{
		SKYPE_RC4_IV2();
		SKYPE_RC4_IV62n (0x57B2250F);
		skype_rc4_iv20 (key, key[12]);
	}
	iv = rotl32 (iv, key[14] + 8);
	if (k == 4)
	{
		SKYPE_RC4_IV44();
		SKYPE_RC4_IV48();
		skype_rc4_iv21 (key, key[1]);
	}
	key[10] ^= key[12] ^ 0x17E47765;
	if (k == 3)
	{
		SKYPE_RC4_IV25();
		SKYPE_RC4_IV37();
		skype_rc4_iv22 (key, iv);
	}
	key[1] ^= 58 * iv;
	if (k == 2)
	{
		SKYPE_RC4_IV50();
		SKYPE_RC4_IV17();
		skype_rc4_iv23 (key, key[16]);
	}
	iv *= 77 * iv;
	if (k == 8)
	{
		SKYPE_RC4_IV45();
		SKYPE_RC4_IV17();
		SKYPE_RC4_IV72n (iv);
	}
	if (k == 9)
	{
		SKYPE_RC4_IV41();
		SKYPE_RC4_IV39n (0x3ED322E1);
		skype_rc4_iv13 (key, key[0]);
	}
	key[3] = rotr32 (key[3], iv * 0xA588A375);
	return iv;
}

/*
 * This is where the RC4 cryptography happens
 *
 * Obvious question:
 * What is the RC4_round-stuff about,
 * why is RC4_crypt more complicated than simple "C=A xor B"s ?
 */

#define RC4_round(i,j,t,k,RC4) ((t)=RC4[i],(j)=((j)+(t)+(k))&0xFF,RC4[i]=RC4[j],RC4[j]=(u8)(t),RC4[(RC4[i]+(t))&0xFF])

void RC4_crypt (u8 * buffer, u32 bytes, RC4_context * const rc4, const u32 test)
{
	u8			t, i = rc4->i, j = rc4->j, z[256], *s;
	
	if (test) memcpy (s = z, rc4->s, 256); else s = rc4->s;
	for (; bytes; bytes--) i++, *buffer++ ^= RC4_round (i, j, t, 0, s);
	if (!test) rc4->i = i, rc4->j = j;
}

/*
 * This is where the secret key is expanded
 */
void Skype_RC4_Expand_IV (const u32 iv, const void *iv2, RC4_context * const rc4, const u32 flags, const u32 iv2_bytes)
{
	u32			i, j, key[20];
	u8			t;
	
	for (i = 0; i < 20; i++)
	   key[i] = iv;
	
	if (!flags || (flags & 1))
	   skype_rc4_iva (key, iv);

	// if (flags & 2) skype_rc4_ivb (key, iv);	// not included here as it is not in use
	
	/*
	 * According to rsc at runtux.com, iv2 is only for the latest DH-384 based protocol.
	 * Otherwise iv2 is an empty string and iv2_bytes equal to 0.
	 */
	for (i = 0, j = __min (iv2_bytes,80); i < j; i+=4)
	   dword(key,i) ^= dword(iv2,i);
	for (; i < j; i++)
	   byte(key,i) ^= byte(iv2,i);

	// now the standard RC4 init
	for (i = 0, j = 0x03020100; i < 256; i += 4, j += 0x04040404)
	   dword(rc4->s,i) = j;
	for (i = 0, j = 0; i < 256; i++)
	   RC4_round (i, j, t, byte(key,i%80), rc4->s);

	rc4->i = 0, rc4->j = 0;
}
