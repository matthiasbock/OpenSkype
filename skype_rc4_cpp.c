
typedef unsigned char u8;
typedef unsigned long u32;

typedef struct _RC4_context
{
 u8 i, j, s[256];
} RC4_context;


void RC4_crypt (u8 * buffer, u32 bytes, RC4_context * const rc4, const u32 test);


void Skype_RC4_Expand_IV (const u32 iv, const void *iv2, RC4_context * const rc4, const u32 flags, const u32 iv2_bytes);
# 27 "skype_rc4.c" 2
# 51 "skype_rc4.c"
 static __inline__ u32 __min (u32 a, u32 b) { return a<b?a:b; }
# 65 "skype_rc4.c"
static const u8 u8sqrt[256] =
{
  1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
  9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
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

static const u8 u8fcos[256] =
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

static const u8 u8fsin[256] =
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
# 206 "skype_rc4.c"
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
 u32 k = iv & 15;

 if (k == 4)
 {
  (key[10] ^= key[7] - 0x354C1FF2);
  (key[17] += key[13] - 0x292C1156);
  skype_rc4_iv1 (key, key[2]);
 }
 if (k == 8)
 {
  (key[13] |= (u8fcos[(0x767255F0)&0xFF]) ? 0x1510A109 : key[14]);
  (key[15] ^= (key[14] < 0x291B9650) ? key[14] : key[2]);
  skype_rc4_iv2 (key, iv);
 }
 key[12] -= 28 * key[19];
 if (k == 1)
 {
  (key[ 3] ^= key[0] + 0x4376FF7);
  (key[17] += key[13] - 0x292C1156);
  skype_rc4_iv3 (key, key[16]);
 }
 if (k == 6)
 {
  {key[ 9] = (((key[9])<<(((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31))))&31))|(((key[9])&0xFFFFFFFF)>>((0-((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31)))))&31))); if (key[9] & 1) return 1;};
  (key[13] ^= (0x258A329D < 0x2E0AF4F7) ? 0x258A329D : key[15]);
  skype_rc4_iv4 (key, key[16]);
 }
 if (k == 14)
 {
  (key[ 6] *= 0x1304694A * key[11]);
  (key[ 6] ^= (u8fcos[(key[7])&0xFF]) ? 0x1AB1E599 : key[18]);
  skype_rc4_iv5 (key, iv);
 }
 key[15] ^= 45 * key[9];
 if (k == 0)
 {
  (key[ 5] += key[11] | 0xEA02A83);
  (key[ 6] = (((key[6])<<((key[13] - 18)&31))|(((key[6])&0xFFFFFFFF)>>((0-(key[13] - 18))&31))));
  skype_rc4_iv6 (key, key[12]);
 }
 if (k == 9)
 {
  (key[11] ^= key[15] | 0x11273409);
  (key[ 2] += 0xEA2D3D5D * key[7]);
  skype_rc4_iv7 (key, key[19]);
 }
 key[18] ^= 0x327BAFFB * key[2];
 if (k == 7)
 {
  (key[17] += key[13] - 0x292C1156);
  {key[ 3] -= key[17] | 0x2433636; if (key[3] & 1) return 1;};
  skype_rc4_iv8 (key, key[8]);
 }
 if (k == 13)
 {
  (key[ 3] += key[9] + 0x48210C78);
  (key[ 0] = (((key[0])<<(((0xA1C70157>>17)&0x1F)&31))|(((key[0])&0xFFFFFFFF)>>((0-((0xA1C70157>>17)&0x1F))&31))));
  skype_rc4_iv9 (key, iv);
 }
 if (k == 3)
 {
  (key[13] |= (u8fcos[(0x5947B4C0)&0xFF]) ? 0x1510A109 : key[14]);
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>(((u8fcos[(key[9])&0xFF]) ? 20 : key[0])&31))|((key[9])<<((0-((u8fcos[(key[9])&0xFF]) ? 20 : key[0]))&31))));
  skype_rc4_iv10 (key, key[12]);
 }
 key[13] *= (u8sqrt[(iv)&0xFF]);
 if (k == 15)
 {
  (key[ 5] *= (((0x59BBBCF2)<<((3)&31))|(((0x59BBBCF2)&0xFFFFFFFF)>>((0-(3))&31))));
  (key[16] &= (key[11] < 0x5578A05) ? key[11] : key[16]);
  skype_rc4_iv11 (key, key[8]);
 }
 if (k == 2)
 {
  (key[17] ^= 0xB00FB3F3);
  (key[ 4] ^= 17 * key[0]);
  skype_rc4_iv12 (key, key[3]);
 }
 iv &= key[19] ^ 0x22BD05B7;
 if (k == 0)
 {
  (key[ 2] ^= (u8fsin[(key[17])&0xFF]) ? 0x1C0E70BF : key[5]);
  (key[16] = ((((key[16])&0xFFFFFFFF)>>((key[10] - 11)&31))|((key[16])<<((0-(key[10] - 11))&31))));
  skype_rc4_iv1 (key, iv);
 }
 if (k == 6)
 {
  (key[ 6] += 0x975C61BA - key[8]);
  {key[ 7] += ((((key[7])&0xFFFFFFFF)>>((21)&31))|((key[7])<<((0-(21))&31))); if (key[7] & 1) return 1;};
  skype_rc4_iv2 (key, key[6]);
 }
 if (k == 3)
 {
  {key[ 1] ^= (u8fcos[(key[3])&0xFF]) ? 0x7C23395 : key[18]; if (key[1] & 1) return 1;};
  (key[ 9] += 0x3A82007 - key[14]);
  skype_rc4_iv3 (key, key[0]);
 }
 key[1] ^= key[15] & 0x1ED68333;
 if (k == 11)
 {
  {key[ 0] *= 33 * key[0]; if (key[0] & 1) return 1;};
  {key[10] = (((key[10])<<((0x8FD14B43 -6)&31))|(((key[10])&0xFFFFFFFF)>>((0-(0x8FD14B43 -6))&31))); if (key[10] & 1) return 1;};
  skype_rc4_iv4 (key, iv);
 }
 if (k == 7)
 {
  (key[ 2] -= (u8fsin[(0x173A48D4)&0xFF]) ? 0x73423C3 : key[7]);
  (key[10] ^= key[7] - 0x354C1FF2);
  skype_rc4_iv5 (key, key[6]);
 }
 if (k == 9)
 {
  (key[ 2] ^= key[15] + 0x57CE331);
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>((key[17]*18)&31))|((key[9])<<((0-(key[17]*18))&31))));
  skype_rc4_iv6 (key, iv);
 }
 key[12] ^= iv - 0x7F670F2F;
 if (k == 12)
 {
  (key[17] += key[13] - 0x292C1156);
  (key[16] &= (key[11] < 0x5578A05) ? key[11] : key[16]);
  skype_rc4_iv7 (key, iv);
 }
 if (k == 10)
 {
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>((key[17]*18)&31))|((key[9])<<((0-(key[17]*18))&31))));
  (key[ 7] += 0x62E34BC8);
  skype_rc4_iv8 (key, key[4]);
 }
 key[3] = ((((key[3])&0xFFFFFFFF)>>((key[10]*4)&31))|((key[3])<<((0-(key[10]*4))&31)));
 if (k == 5)
 {
  (key[18] ^= key[10] + 0x1EE65B0C);
  (key[14] ^= (u8fcos[(key[9])&0xFF]) ? 0x73CD560C : key[4]);
  skype_rc4_iv9 (key, key[4]);
 }
 if (k == 10)
 {
  (key[13] ^= (0x224B5A8A < 0x2E0AF4F7) ? 0x224B5A8A : key[15]);
  (key[ 7] -= 0xE729AFE3);
  skype_rc4_iv10 (key, key[5]);
 }
 if (k == 8)
 {
  {key[10] = (((key[10])<<((0x7B403A3E -6)&31))|(((key[10])&0xFFFFFFFF)>>((0-(0x7B403A3E -6))&31))); if (key[10] & 1) return 1;};
  (key[13] ^= (0x2606748C < 0x2E0AF4F7) ? 0x2606748C : key[15]);
  skype_rc4_iv11 (key, iv);
 }
 iv *= (0x72E979C7 ^ key[7]);
 if (k == 1)
 {
  (key[13] ^= (0x3F541FC9 < 0x2E0AF4F7) ? 0x3F541FC9 : key[15]);
  (key[ 2] += 0xEA2D3D5D * key[7]);
  skype_rc4_iv12 (key, key[6]);
 }
 if (k == 4)
 {
  (key[17] ^= 0xDB079C63);
  (key[10] ^= key[7] - 0x354C1FF2);
  skype_rc4_iv1 (key, key[13]);
 }
 key[15] ^= (u8sqrt[(key[15])&0xFF]);
 if (k == 5)
 {
  (key[ 7] ^= key[10] - 0x3035E544);
  (key[ 9] += 0x3A82007 - key[14]);
  skype_rc4_iv2 (key, key[1]);
 }
 if (k == 2)
 {
  (key[16] = ((((key[16])&0xFFFFFFFF)>>((key[10] - 11)&31))|((key[16])<<((0-(key[10] - 11))&31))));
  (key[ 7] ^= key[10] - 0x3035E544);
  skype_rc4_iv3 (key, key[17]);
 }
 key[9] -= (((key[18])<<((15)&31))|(((key[18])&0xFFFFFFFF)>>((0-(15))&31)));
 return 0;
}

u32 skype_rc4_iv1 (u32 * const key, u32 iv)
{
 u32 k = (key[13] ^ key[12] ^ key[8]) % 14;

 if (k == 0)
 {
  {if (key[5] & 1) return 1;};
  (key[15] ^= (key[14] < 0x291B9650) ? key[14] : key[2]);
  skype_rc4_iv13 (key, key[10]);
 }
 key[10] *= 87 * key[9];
 if (k == 13)
 {
  (key[9 ] *= (u8fsin[(0x2F378459)&0xFF]) ? 0x28D781D2 : key[10]);
  (key[ 6] *= 0x1304694A * key[11]);
  skype_rc4_iv14 (key, key[15]);
 }
 if (k == 11)
 {
  (key[11] -= key[12] << 5);
  (key[8 ] ^= (u8fcos[(key[17])&0xFF]) ? 0x3544CA5E : key[8]);
  skype_rc4_iv15 (key, key[7]);
 }
 iv *= key[13] - 0x1B6664FC;
 if (k == 10)
 {
  (key[ 1] -= key[16] | 0x59C1677);
  (key[10] ^= key[7] - 0x354C1FF2);
  skype_rc4_iv16 (key, key[7]);
 }
 key[19] = ((((key[19])&0xFFFFFFFF)>>(((u8sqrt[(key[18])&0xFF]))&31))|((key[19])<<((0-((u8sqrt[(key[18])&0xFF])))&31)));
 if (k == 4)
 {
  (key[9 ] *= (u8fsin[(0x55CE2E77)&0xFF]) ? 0x28D781D2 : key[10]);
  {key[11] += 0xF6B10986 - key[14]; if (key[11] & 1) return 1;};
  skype_rc4_iv17 (key, iv);
 }
 if (k == 2)
 {
  (key[ 4] ^= key[19] - 0x303D46FE);
  (key[13] |= (u8fcos[(0xEEDD2781)&0xFF]) ? 0x1510A109 : key[14]);
  skype_rc4_iv18 (key, key[19]);
 }
 key[17] += (u8sqrt[(key[3])&0xFF]);
 if (k == 12)
 {
  (key[ 6] *= 0x1304694A * key[11]);
  (key[ 9] ^= (u8fcos[(key[11])&0xFF]) ? 0xEEB638B : key[6]);
  skype_rc4_iv19 (key, key[4]);
 }
 key[2] *= iv + 0x3D4CFA4F;
 if (k == 9)
 {
  (key[16] ^= (key[18] < 0xE87F32) ? key[18] : key[11]);
  (key[16] &= (key[11] < 0x5578A05) ? key[11] : key[16]);
  skype_rc4_iv20 (key, iv);
 }
 if (k == 1)
 {
  (key[ 7] += 0x505E90D8);
  (key[12] *= (u8fcos[(0x13951A5E)&0xFF]) ? 0x1734D89C : key[5]);
  skype_rc4_iv21 (key, key[6]);
 }
 key[12] *= 79 * key[6];
 if (k == 6)
 {
  (key[ 6] = (((key[6])<<((key[13] - 18)&31))|(((key[6])&0xFFFFFFFF)>>((0-(key[13] - 18))&31))));
  (key[13] |= (u8fcos[(0x6F001987)&0xFF]) ? 0x1510A109 : key[14]);
  skype_rc4_iv22 (key, key[17]);
 }
 key[3] &= (u8fsin[(key[17])&0xFF]) ? 0x4C35C59A : iv;
 if (k == 3)
 {
  (key[11] |= key[4] - 0x224114CD);
  {key[ 3] -= key[17] | 0x2433636; if (key[3] & 1) return 1;};
  skype_rc4_iv23 (key, key[11]);
 }
 if (k == 5)
 {
  (key[12] *= (u8fcos[(0xE2AEC47F)&0xFF]) ? 0x1734D89C : key[5]);
  (key[14] ^= (u8fcos[(key[9])&0xFF]) ? 0x73CD560C : key[4]);
  skype_rc4_iv24 (key, iv);
 }
 key[10] = (((key[10])<<((25 * key[16])&31))|(((key[10])&0xFFFFFFFF)>>((0-(25 * key[16]))&31)));
 if (k == 0)
 {
  {key[ 0] *= 33 * key[0]; if (key[0] & 1) return 1;};
  (key[15] ^= (key[14] < 0x291B9650) ? key[14] : key[2]);
  skype_rc4_iv13 (key, iv);
 }
 key[15] += 0xC7308059 - key[10];
 if (k == 8)
 {
  (key[11] &= 0x45AE0A86);
  (key[ 7] ^= key[10] - 0x3035E544);
  skype_rc4_iv14 (key, iv);
 }
 if (k == 7)
 {
  (key[16] ^= (key[18] < 0xE87F32) ? key[18] : key[11]);
  (key[ 2] &= key[18] - 0x37CF1A3F);
  skype_rc4_iv15 (key, key[12]);
 }
 key[1] ^= iv & 0xF42F3BCF;
 return iv;
}

u32 skype_rc4_iv2 (u32 * const key, u32 iv)
{
 u32 k = key[4] % 15;

 if (k == 5)
 {
  (key[ 9] ^= (u8fcos[(key[11])&0xFF]) ? 0xEEB638B : key[6]);
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>((key[17]*18)&31))|((key[9])<<((0-(key[17]*18))&31))));
  skype_rc4_iv14 (key, key[9]);
 }
 key[1] += (((iv)<<((4)&31))|(((iv)&0xFFFFFFFF)>>((0-(4))&31)));
 if (k == 9)
 {
  (key[19] &= 0xB0AB0E55);
  (key[13] |= (u8fcos[(0x1A8398B1)&0xFF]) ? 0x1510A109 : key[14]);
  skype_rc4_iv15 (key, key[19]);
 }
 key[5] ^= 0x39C770C4 & key[2];
 if (k == 0)
 {
  (key[ 9] = (((key[9])<<((11)&31))|(((key[9])&0xFFFFFFFF)>>((0-(11))&31))));
  (key[ 1] -= key[16] | 0x59C1677);
  skype_rc4_iv16 (key, key[14]);
 }
 if (k == 1)
 {
  (key[18] ^= key[10] + 0x1EE65B0C);
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>(((u8fcos[(key[9])&0xFF]) ? 20 : key[0])&31))|((key[9])<<((0-((u8fcos[(key[9])&0xFF]) ? 20 : key[0]))&31))));
  skype_rc4_iv17 (key, key[7]);
 }
 iv -= (((iv)<<((1)&31))|(((iv)&0xFFFFFFFF)>>((0-(1))&31)));
 if (k == 13)
 {
  (key[18] -= 122 * key[6]);
  (key[11] = (((key[11])<<(((u8fcos[(key[5])&0xFF]) ? 19 : key[11])&31))|(((key[11])&0xFFFFFFFF)>>((0-((u8fcos[(key[5])&0xFF]) ? 19 : key[11]))&31))));
  skype_rc4_iv18 (key, key[9]);
 }
 if (k == 8)
 {
  (key[ 6] *= 0x1304694A * key[11]);
  (key[ 3] += key[9] + 0x48210C78);
  skype_rc4_iv19 (key, key[17]);
 }
 iv += key[17] + 0x25FB77C1;
 if (k == 14)
 {
  (key[11] &= 0x094909EA);
  (key[11] += 0x29CC7F53 - key[5]);
  skype_rc4_iv20 (key, key[7]);
 }
 key[4] *= (u8fcos[(key[19])&0xFF]) ? 0x336C9268 : key[4];
 if (k == 11)
 {
  (key[ 6] *= 0x1304694A * key[11]);
  {key[ 9] = (((key[9])<<(((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31))))&31))|(((key[9])&0xFFFFFFFF)>>((0-((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31)))))&31))); if (key[9] & 1) return 1;};
  skype_rc4_iv21 (key, iv);
 }
 key[17] *= (u8sqrt[(iv)&0xFF]);
 if (k == 2)
 {
  (key[ 9] = (((key[9])<<((17)&31))|(((key[9])&0xFFFFFFFF)>>((0-(17))&31))));
  (key[12] -= 66 * key[2]);
  skype_rc4_iv22 (key, key[12]);
 }
 if (k == 7)
 {
  {key[ 7] += key[2] ^ 0x376E1538; if (key[7] & 1) return 1;};
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>(((u8fcos[(key[9])&0xFF]) ? 20 : key[0])&31))|((key[9])<<((0-((u8fcos[(key[9])&0xFF]) ? 20 : key[0]))&31))));
  skype_rc4_iv23 (key, key[14]);
 }
 key[5] += (u8sqrt[(key[0])&0xFF]);
 if (k == 4)
 {
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>((key[17]*18)&31))|((key[9])<<((0-(key[17]*18))&31))));
  (key[ 9] += 0x3A82007 - key[14]);
  skype_rc4_iv24 (key, key[13]);
 }
 key[14] ^= ((((key[18])&0xFFFFFFFF)>>((20)&31))|((key[18])<<((0-(20))&31)));
 if (k == 3)
 {
  (key[11] -= key[12] << 5);
  (key[11] = (((key[11])<<(((u8fcos[(key[5])&0xFF]) ? 19 : key[11])&31))|(((key[11])&0xFFFFFFFF)>>((0-((u8fcos[(key[5])&0xFF]) ? 19 : key[11]))&31))));
  skype_rc4_iv1 (key, key[15]);
 }
 if (k == 0)
 {
  (key[18] ^= key[10] + 0x1EE65B0C);
  (key[19] &= 0xA14E4231);
  skype_rc4_iv14 (key, key[5]);
 }
 key[10] *= 34 * key[19];
 if (k == 6)
 {
  (key[15] -= (u8fcos[(0xA7E8E811)&0xFF]) ? 0x344432F : key[18]);
  {key[ 1] ^= (u8fcos[(key[3])&0xFF]) ? 0x7C23395 : key[18]; if (key[1] & 1) return 1;};
  skype_rc4_iv15 (key, iv);
 }
 if (k == 10)
 {
  (key[ 3] ^= key[0] + 0x4376FF7);
  (key[13] ^= (0x27546CBA < 0x2E0AF4F7) ? 0x27546CBA : key[15]);
  skype_rc4_iv16 (key, key[16]);
 }
 iv |= 21 * key[10];
 if (k == 12)
 {
  (key[ 7] ^= (u8sqrt[(key[15])&0xFF]));
  {key[10] = (((key[10])<<((0xE8357BC6 -6)&31))|(((key[10])&0xFFFFFFFF)>>((0-(0xE8357BC6 -6))&31))); if (key[10] & 1) return 1;};
  skype_rc4_iv17 (key, key[8]);
 }
 key[17] &= (u8fsin[(iv)&0xFF]) ? 0x24D1E601 : key[17];
 return iv;
}

u32 skype_rc4_iv3 (u32 * const key, u32 iv)
{
 u32 k = (key[2] ^ key[11] ^ key[19]) & 15;

 if (k == 7)
 {
  (key[ 9] ^= (u8fcos[(key[11])&0xFF]) ? 0xEEB638B : key[6]);
  (key[11] += 0x29CC7F53 - key[5]);
  skype_rc4_iv15 (key, key[7]);
 }
 iv = (((iv)<<((20 * key[15])&31))|(((iv)&0xFFFFFFFF)>>((0-(20 * key[15]))&31)));
 if (k == 2)
 {
  (key[11] &= 0x159A2134);
  (key[13] |= (u8fcos[(0x855DE620)&0xFF]) ? 0x1510A109 : key[14]);
  skype_rc4_iv16 (key, key[2]);
 }
 if (k == 13)
 {
  (key[13] |= (u8fcos[(0xC4146E71)&0xFF]) ? 0x1510A109 : key[14]);
  (key[ 7] -= 0x3F3F6FFB);
  skype_rc4_iv17 (key, iv);
 }
 key[13] += key[18] & 0x2581026D;
 if (k == 11)
 {
  (key[ 9] += 0x3A82007 - key[14]);
  (key[ 7] ^= key[10] - 0x3035E544);
  skype_rc4_iv18 (key, iv);
 }
 key[1] &= iv & 0x64BB010;
 if (k == 5)
 {
  (key[10] = ((((key[10])&0xFFFFFFFF)>>((key[14] + 6)&31))|((key[10])<<((0-(key[14] + 6))&31))));
  (key[16] = ((((key[16])&0xFFFFFFFF)>>((key[10] - 11)&31))|((key[16])<<((0-(key[10] - 11))&31))));
  skype_rc4_iv19 (key, key[13]);
 }
 if (k == 3)
 {
  (key[17] += key[13] - 0x292C1156);
  (key[19] &= 0x7EC9C31F);
  skype_rc4_iv20 (key, iv);
 }
 key[10] |= 67 * key[13];
 if (k == 8)
 {
  (key[ 6] += (0xDFDC4A68 < 0x61F0BAA) ? 0xDFDC4A68 : key[16]);
  (key[ 4] ^= key[19] - 0x303D46FE);
  skype_rc4_iv21 (key, key[5]);
 }
 key[9] += 0xAA213313 * iv;
 if (k == 0)
 {
  (key[ 7] ^= key[10] - 0x3035E544);
  (key[15] -= (u8fcos[(0x16E7FB2A)&0xFF]) ? 0x344432F : key[18]);
  skype_rc4_iv22 (key, iv);
 }
 if (k == 15)
 {
  (key[10] ^= key[7] - 0x354C1FF2);
  (key[ 1] ^= (((0x6D43A2A8)<<((8)&31))|(((0x6D43A2A8)&0xFFFFFFFF)>>((0-(8))&31))));
  skype_rc4_iv23 (key, iv);
 }
 key[14] |= (u8sqrt[(key[12])&0xFF]);
 if (k == 12)
 {
  (key[12] *= (u8fcos[(0xE5FB063D)&0xFF]) ? 0x1734D89C : key[5]);
  (key[ 1] -= key[16] | 0x59C1677);
  skype_rc4_iv24 (key, iv);
 }
 if (k == 10)
 {
  (key[ 5] += key[11] | 0xEA02A83);
  (key[ 4] ^= 17 * key[0]);
  skype_rc4_iv1 (key, key[4]);
 }
 iv = 107 * key[1] ^ iv;
 if (k == 1)
 {
  (key[ 7] += 0x357B0AC2);
  (key[ 3] ^= key[0] + 0x4376FF7);
  skype_rc4_iv2 (key, key[1]);
 }
 key[9] = ((((key[9])&0xFFFFFFFF)>>(((u8fsin[(key[16])&0xFF]) ? 26 : key[12])&31))|((key[9])<<((0-((u8fsin[(key[16])&0xFF]) ? 26 : key[12]))&31)));
 if (k == 4)
 {
  (key[ 7] ^= key[10] - 0x3035E544);
  (key[ 7] ^= key[10] - 0x3035E544);
  skype_rc4_iv15 (key, key[7]);
 }
 if (k == 0)
 {
  (key[12] = ((((key[12])&0xFFFFFFFF)>>((key[18] ^ 9)&31))|((key[12])<<((0-(key[18] ^ 9))&31))));
  (key[ 2] ^= (u8fsin[(key[17])&0xFF]) ? 0x1C0E70BF : key[5]);
  skype_rc4_iv16 (key, iv);
 }
 key[13] *= key[18] ^ 0x1D347B67;
 if (k == 6)
 {
  (key[ 2] -= (u8fsin[(0xAC2CA8C4)&0xFF]) ? 0x73423C3 : key[7]);
  (key[ 6] += (0xFF8CF458 < 0x61F0BAA) ? 0xFF8CF458 : key[16]);
  skype_rc4_iv17 (key, key[6]);
 }
 key[2] = (u8fsin[(key[0])&0xFF]) ? key[2] : ((((key[2])&0xFFFFFFFF)>>((key[0])&31))|((key[2])<<((0-(key[0]))&31)));
 if (k == 14)
 {
  (key[ 4] ^= 17 * key[0]);
  {key[10] = (((key[10])<<((0x52041287 -6)&31))|(((key[10])&0xFFFFFFFF)>>((0-(0x52041287 -6))&31))); if (key[10] & 1) return 1;};
  skype_rc4_iv18 (key, key[7]);
 }
 if (k == 9)
 {
  (key[ 7] -= 0xBF896FA3);
  (key[ 0] = (((key[0])<<((8 * key[18])&31))|(((key[0])&0xFFFFFFFF)>>((0-(8 * key[18]))&31))));
  skype_rc4_iv19 (key, key[3]);
 }
 return 50 * iv * iv;
}

u32 skype_rc4_iv4 (u32 * const key, u32 iv)
{
 u32 k = iv & 15;

 if (k == 10)
 {
  (key[ 2] += 0xEA2D3D5D * key[7]);
  (key[19] &= 0x590CCB85);
  skype_rc4_iv16 (key, key[7]);
 }
 key[10] ^= (iv < 0x3D18A13) ? iv : key[9];
 if (k == 6)
 {
  (key[ 6] = (((key[6])<<((key[13] - 18)&31))|(((key[6])&0xFFFFFFFF)>>((0-(key[13] - 18))&31))));
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>(((u8fcos[(key[9])&0xFF]) ? 20 : key[0])&31))|((key[9])<<((0-((u8fcos[(key[9])&0xFF]) ? 20 : key[0]))&31))));
  skype_rc4_iv17 (key, key[17]);
 }
 if (k == 14)
 {
  (key[15] -= (u8fcos[(0xA6F86E90)&0xFF]) ? 0x344432F : key[18]);
  (key[ 2] &= key[18] - 0x37CF1A3F);
  skype_rc4_iv18 (key, key[17]);
 }
 iv -= key[10] & 0x16926664;
 if (k == 0)
 {
  (key[ 5] *= (((0x95AAFBC8)<<((3)&31))|(((0x95AAFBC8)&0xFFFFFFFF)>>((0-(3))&31))));
  (key[11] ^= key[15] | 0x11273409);
  skype_rc4_iv19 (key, key[7]);
 }
 if (k == 11)
 {
  (key[ 2] += 0xEA2D3D5D * key[7]);
  (key[ 2] ^= (u8fsin[(key[17])&0xFF]) ? 0x1C0E70BF : key[5]);
  skype_rc4_iv20 (key, key[6]);
 }
 key[6] ^= key[7] - 0xD669F4D;
 if (k == 1)
 {
  (key[ 6] = (((key[6])<<((key[13] - 18)&31))|(((key[6])&0xFFFFFFFF)>>((0-(key[13] - 18))&31))));
  (key[ 6] += 0x975C61BA - key[8]);
  skype_rc4_iv21 (key, key[7]);
 }
 key[1] ^= ((((key[15])&0xFFFFFFFF)>>((14)&31))|((key[15])<<((0-(14))&31)));
 if (k == 5)
 {
  (key[ 4] ^= 17 * key[0]);
  (key[17] ^= 0x2F961 * key[4]);
  skype_rc4_iv22 (key, iv);
 }
 if (k == 4)
 {
  (key[13] |= (u8fcos[(0x8EEA1FE3)&0xFF]) ? 0x1510A109 : key[14]);
  (key[ 2] ^= (u8fsin[(key[17])&0xFF]) ? 0x1C0E70BF : key[5]);
  skype_rc4_iv23 (key, key[13]);
 }
 iv = ((((iv)&0xFFFFFFFF)>>((0x19 & key[5])&31))|((iv)<<((0-(0x19 & key[5]))&31)));
 if (k == 2)
 {
  (key[8 ] ^= (u8fcos[(key[17])&0xFF]) ? 0x3544CA5E : key[8]);
  (key[17] ^= 0x34C48CA3);
  skype_rc4_iv24 (key, key[9]);
 }
 if (k == 3)
 {
  (key[ 1] -= key[16] | 0x59C1677);
  (key[15] -= (u8fcos[(0x104BE7AE)&0xFF]) ? 0x344432F : key[18]);
  skype_rc4_iv1 (key, key[9]);
 }
 key[3] *= (u8sqrt[(iv)&0xFF]);
 if (k == 0)
 {
  (key[11] = (((key[11])<<(((u8fcos[(key[5])&0xFF]) ? 19 : key[11])&31))|(((key[11])&0xFFFFFFFF)>>((0-((u8fcos[(key[5])&0xFF]) ? 19 : key[11]))&31))));
  (key[ 1] ^= (((0x507DA30D)<<((8)&31))|(((0x507DA30D)&0xFFFFFFFF)>>((0-(8))&31))));
  skype_rc4_iv2(key, key[2]);
 }
 key[16] -= (u8sqrt[(iv)&0xFF]);
 if (k == 13)
 {
  (key[ 7] -= 0x3FCB3FA3);
  (key[12] *= (u8fcos[(0xD0D9B11D)&0xFF]) ? 0x1734D89C : key[5]);
  skype_rc4_iv3 (key, key[4]);
 }
 if (k == 7)
 {
  (key[ 9] ^= (u8fcos[(key[11])&0xFF]) ? 0xEEB638B : key[6]);
  (key[ 7] += 0x6E4473BD);
  skype_rc4_iv16 (key, key[19]);
 }
 iv += 0x720E12F5 + key[5];
 if (k == 15)
 {
  (key[15] ^= (key[14] < 0x291B9650) ? key[14] : key[2]);
  (key[11] += 0x29CC7F53 - key[5]);
  skype_rc4_iv17 (key, key[1]);
 }
 if (k == 9)
 {
  {key[11] += 0xF6B10986 - key[14]; if (key[11] & 1) return 1;};
  (key[ 6] *= 0x1304694A * key[11]);
  skype_rc4_iv18 (key, key[10]);
 }
 key[1] ^= (iv < 0x585C6D88) ? iv : key[13];
 if (k == 12)
 {
  (key[11] |= key[4] - 0x224114CD);
  {if (key[5] & 1) return 1;};
  skype_rc4_iv19 (key, key[14]);
 }
 if (k == 8)
 {
  (key[ 4] ^= key[19] - 0x303D46FE);
  (key[ 2] &= key[18] - 0x37CF1A3F);
  skype_rc4_iv20 (key, key[11]);
 }
 iv |= key[13] + 0x38D39E93;
 if (k == 1)
 {
  {key[ 9] = (((key[9])<<(((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31))))&31))|(((key[9])&0xFFFFFFFF)>>((0-((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31)))))&31))); if (key[9] & 1) return 1;};
  (key[ 6] ^= (u8fcos[(key[7])&0xFF]) ? 0x1AB1E599 : key[18]);
  skype_rc4_iv21 (key, key[8]);
 }
 key[19] ^= key[9] + 0x23280350;
 return iv;
}

u32 skype_rc4_iv5 (u32 * const key, u32 iv)
{
 u32 k = key[13] & 15;

 if (k == 1)
 {
  (key[16] &= (key[11] < 0x5578A05) ? key[11] : key[16]);
  (key[ 1] -= key[16] | 0x59C1677);
  skype_rc4_iv17 (key, iv);
 }
 iv += 0xCB72BB0E + key[10];
 if (k == 0)
 {
  (key[11] = (((key[11])<<(((u8fcos[(key[5])&0xFF]) ? 19 : key[11])&31))|(((key[11])&0xFFFFFFFF)>>((0-((u8fcos[(key[5])&0xFF]) ? 19 : key[11]))&31))));
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>((key[17]*18)&31))|((key[9])<<((0-(key[17]*18))&31))));
  skype_rc4_iv18 (key, key[0]);
 }
 if (k == 5)
 {
  (key[17] += key[13] - 0x292C1156);
  (key[17] ^= 0xEFE4F823);
  skype_rc4_iv19 (key, key[3]);
 }
 key[7] += 0x72A1B49 - key[6];
 if (k == 1)
 {
  {key[ 1] ^= (u8fcos[(key[3])&0xFF]) ? 0x7C23395 : key[18]; if (key[1] & 1) return 1;};
  (key[17] ^= 0x2F961 * key[4]);
  skype_rc4_iv20 (key, key[15]);
 }
 if (k == 4)
 {
  (key[ 6] ^= (u8fcos[(key[7])&0xFF]) ? 0x1AB1E599 : key[18]);
  (key[12] *= (u8fcos[(0x391FA13E)&0xFF]) ? 0x1734D89C : key[5]);
  skype_rc4_iv21 (key, iv);
 }
 key[17] += (u8fcos[(key[16])&0xFF]) ? 0xD1C5DCA : key[10];
 if (k == 11)
 {
  (key[ 3] ^= key[0] + 0x4376FF7);
  {key[11] += 0xF6B10986 - key[14]; if (key[11] & 1) return 1;};
  skype_rc4_iv22 (key, key[5]);
 }
 if (k == 14)
 {
  (key[ 9] = (((key[9])<<((28)&31))|(((key[9])&0xFFFFFFFF)>>((0-(28))&31))));
  (key[ 7] += 0x5BAAA0D8);
  skype_rc4_iv23 (key, key[3]);
 }
 iv += 0xA4631CA4 & key[0];
 if (k == 10)
 {
  (key[ 9] ^= (u8fcos[(key[11])&0xFF]) ? 0xEEB638B : key[6]);
  {key[ 7] += ((((key[7])&0xFFFFFFFF)>>((21)&31))|((key[7])<<((0-(21))&31))); if (key[7] & 1) return 1;};
  skype_rc4_iv24 (key, key[6]);
 }
 if (k == 3)
 {
  (key[11] += 0x29CC7F53 - key[5]);
  (key[ 7] ^= (u8sqrt[(key[15])&0xFF]));
  skype_rc4_iv1 (key, key[11]);
 }
 key[18] = (((key[18])<<(((u8sqrt[(key[13])&0xFF]))&31))|(((key[18])&0xFFFFFFFF)>>((0-((u8sqrt[(key[13])&0xFF])))&31)));
 if (k == 2)
 {
  {key[10] = (((key[10])<<((0x255DB12E -6)&31))|(((key[10])&0xFFFFFFFF)>>((0-(0x255DB12E -6))&31))); if (key[10] & 1) return 1;};
  (key[12] = ((((key[12])&0xFFFFFFFF)>>((key[18] ^ 9)&31))|((key[12])<<((0-(key[18] ^ 9))&31))));
  skype_rc4_iv2 (key, key[19]);
 }
 iv += key[12] & 0x3EB958F;
 if (k == 13)
 {
  (key[17] += key[13] - 0x292C1156);
  (key[ 2] &= key[18] - 0x37CF1A3F);
  skype_rc4_iv3 (key, key[19]);
 }
 if (k == 2)
 {
  (key[11] ^= key[15] | 0x11273409);
  (key[ 2] -= (u8fsin[(0xFD54B81D)&0xFF]) ? 0x73423C3 : key[7]);
  skype_rc4_iv4 (key, iv);
 }
 key[10] *= 61 * iv;
 if (k == 15)
 {
  (key[17] ^= 0x2F961 * key[4]);
  {key[11] += 0xF6B10986 - key[14]; if (key[11] & 1) return 1;};
  skype_rc4_iv17 (key, key[9]);
 }
 if (k == 0)
 {
  (key[11] |= key[4] - 0x224114CD);
  (key[11] ^= key[15] | 0x11273409);
  skype_rc4_iv18 (key, key[17]);
 }
 iv = (((iv)<<((key[16])&31))|(((iv)&0xFFFFFFFF)>>((0-(key[16]))&31)));
 if (k == 9)
 {
  (key[13] |= (u8fcos[(0xD59BE521)&0xFF]) ? 0x1510A109 : key[14]);
  (key[9 ] *= (u8fsin[(0x59621E0F)&0xFF]) ? 0x28D781D2 : key[10]);
  skype_rc4_iv19 (key, key[6]);
 }
 if (k == 7)
 {
  (key[11] += 0x29CC7F53 - key[5]);
  (key[19] &= 0x935A86BD);
  skype_rc4_iv20 (key, key[4]);
 }
 key[16] -= (key[5] < 0x4DEA623B) ? key[5] : key[2];
 if (k == 8)
 {
  (key[17] ^= 0xAC1EA487);
  (key[13] |= (u8fcos[(0x99B3E5F0)&0xFF]) ? 0x1510A109 : key[14]);
  skype_rc4_iv21(key, key[10]);
 }
 if (k == 6)
 {
  (key[ 9] ^= (u8fcos[(key[11])&0xFF]) ? 0xEEB638B : key[6]);
  (key[18] -= 122 * key[6]);
  skype_rc4_iv22 (key, key[11]);
 }
 key[14] = ((((key[14])&0xFFFFFFFF)>>(((u8fsin[(key[19])&0xFF]) ? 27 : key[5])&31))|((key[14])<<((0-((u8fsin[(key[19])&0xFF]) ? 27 : key[5]))&31)));
 if (k == 12)
 {
  {key[ 9] = (((key[9])<<(((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31))))&31))|(((key[9])&0xFFFFFFFF)>>((0-((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31)))))&31))); if (key[9] & 1) return 1;};
  (key[11] = (((key[11])<<(((u8fcos[(key[5])&0xFF]) ? 19 : key[11])&31))|(((key[11])&0xFFFFFFFF)>>((0-((u8fcos[(key[5])&0xFF]) ? 19 : key[11]))&31))));
  skype_rc4_iv23 (key, key[12]);
 }
 return iv * (iv - 0x497488D1);
}

u32 skype_rc4_iv6 (u32 * const key, u32 iv)
{
 u32 k = (key[11] ^ key[6] ^ key[4]) & 15;

 if (k == 2)
 {
  (key[ 6] = (((key[6])<<((key[13] - 18)&31))|(((key[6])&0xFFFFFFFF)>>((0-(key[13] - 18))&31))));
  (key[ 3] ^= key[0] + 0x4376FF7);
  skype_rc4_iv18 (key, iv);
 }
 key[10] |= ((((iv)&0xFFFFFFFF)>>((6)&31))|((iv)<<((0-(6))&31)));
 if (k == 14)
 {
  (key[ 7] ^= key[10] - 0x3035E544);
  (key[ 2] ^= key[15] + 0x57CE331);
  skype_rc4_iv19 (key, key[18]);
 }
 if (k == 9)
 {
  {key[11] += 0xF6B10986 - key[14]; if (key[11] & 1) return 1;};
  (key[15] -= (u8fcos[(0x13BC76D5)&0xFF]) ? 0x344432F : key[18]);
  skype_rc4_iv20 (key, key[1]);
 }
 iv &= (u8fsin[(key[11])&0xFF]) ? 0xDA03B206 : key[15];
 if (k == 12)
 {
  (key[ 7] ^= key[10] - 0x3035E544);
  (key[17] += key[13] - 0x292C1156);
  skype_rc4_iv21 (key, key[0]);
 }
 if (k == 3)
 {
  (key[8 ] ^= (u8fcos[(key[17])&0xFF]) ? 0x3544CA5E : key[8]);
  (key[ 7] ^= (u8sqrt[(key[15])&0xFF]));
  skype_rc4_iv22 (key, iv);
 }
 key[14] ^= 83 * key[14];
 if (k == 7)
 {
  (key[8 ] ^= (u8fcos[(key[17])&0xFF]) ? 0x3544CA5E : key[8]);
  (key[ 7] ^= (u8sqrt[(key[15])&0xFF]));
  skype_rc4_iv23 (key, key[12]);
 }
 if (k == 11)
 {
  {key[ 9] = (((key[9])<<(((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31))))&31))|(((key[9])&0xFFFFFFFF)>>((0-((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31)))))&31))); if (key[9] & 1) return 1;};
  (key[11] -= key[12] << 5);
  skype_rc4_iv24 (key, key[10]);
 }
 key[11] -= (u8fsin[(iv)&0xFF]) ? 0xCA758BFB : key[12];
 if (k == 10)
 {
  (key[ 6] ^= ((((key[14])&0xFFFFFFFF)>>((28)&31))|((key[14])<<((0-(28))&31))));
  {if (key[5] & 1) return 1;};
  skype_rc4_iv1 (key, iv);
 }
 if (k == 8)
 {
  {key[ 1] ^= (u8fcos[(key[3])&0xFF]) ? 0x7C23395 : key[18]; if (key[1] & 1) return 1;};
  (key[ 2] &= ((((0x26B5176D)&0xFFFFFFFF)>>((17)&31))|((0x26B5176D)<<((0-(17))&31))));
  skype_rc4_iv2 (key, key[14]);
 }
 iv *= 0xACA6EF1E ^ key[1];
 if (k == 13)
 {
  (key[ 4] ^= key[19] - 0x303D46FE);
  (key[ 6] += 0x975C61BA - key[8]);
  skype_rc4_iv3 (key, iv);
 }
 if (k == 6)
 {
  (key[ 4] ^= key[19] - 0x303D46FE);
  (key[11] |= key[4] - 0x224114CD);
  skype_rc4_iv4 (key, key[13]);
 }
 iv += 41 * key[11];
 if (k == 3)
 {
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>(((u8fcos[(key[9])&0xFF]) ? 20 : key[0])&31))|((key[9])<<((0-((u8fcos[(key[9])&0xFF]) ? 20 : key[0]))&31))));
  (key[16] ^= (key[18] < 0xE87F32) ? key[18] : key[11]);
  skype_rc4_iv5 (key, key[17]);
 }
 if (k == 1)
 {
  (key[11] = (((key[11])<<(((u8fcos[(key[5])&0xFF]) ? 19 : key[11])&31))|(((key[11])&0xFFFFFFFF)>>((0-((u8fcos[(key[5])&0xFF]) ? 19 : key[11]))&31))));
  (key[ 5] += key[11] | 0xEA02A83);
  skype_rc4_iv18 (key, iv);
 }
 key[8] += 35 * key[6];
 if (k == 4)
 {
  (key[ 2] &= key[18] - 0x37CF1A3F);
  (key[11] |= key[4] - 0x224114CD);
  skype_rc4_iv19 (key, key[1]);
 }
 if (k == 1)
 {
  (key[12] *= (u8fcos[(0x909F)&0xFF]) ? 0x1734D89C : key[5]);
  (key[ 4] ^= key[19] - 0x303D46FE);
  skype_rc4_iv20 (key, key[2]);
 }
 key[11] *= key[12] - 0x48766E9C;
 if (k == 0)
 {
  (key[13] ^= (0x29B6BEC0 < 0x2E0AF4F7) ? 0x29B6BEC0 : key[15]);
  {key[16] &= (key[12] < 0x28165E7B) ? key[12] : 0x5233F757; if (key[16] & 1) return 1;};
  skype_rc4_iv21 (key, key[9]);
 }
 if (k == 15)
 {
  (key[ 7] ^= (u8sqrt[(key[15])&0xFF]));
  {key[ 0] *= 33 * key[0]; if (key[0] & 1) return 1;};
  skype_rc4_iv22 (key, iv);
 }
 key[3] += (u8sqrt[(key[15])&0xFF]);
 if (k == 5)
 {
  (key[15] ^= (key[14] < 0x291B9650) ? key[14] : key[2]);
  {key[ 0] *= 33 * key[0]; if (key[0] & 1) return 1;};
  skype_rc4_iv23 (key, key[1]);
 }
 key[4] = (((key[4])<<(((u8fsin[(key[12])&0xFF]) ? 13 : key[14])&31))|(((key[4])&0xFFFFFFFF)>>((0-((u8fsin[(key[12])&0xFF]) ? 13 : key[14]))&31)));
 if (k == 2)
 {
  (key[15] ^= (key[14] < 0x291B9650) ? key[14] : key[2]);
  (key[17] ^= 0x2F961 * key[4]);
  skype_rc4_iv24 (key, key[4]);
 }
 if (k == 0)
 {
  (key[11] -= key[12] << 5);
  (key[ 4] ^= 17 * key[0]);
  skype_rc4_iv1 (key, key[5]);
 }
 key[5] &= 97 * key[16];
 return iv;
}

u32 skype_rc4_iv7 (u32 * const key, u32 iv)
{
 u32 k = (key[8] ^ key[7] ^ key[16]) & 15;

 if (k == 8)
 {
  (key[ 1] ^= (((0xCDBFC92)<<((8)&31))|(((0xCDBFC92)&0xFFFFFFFF)>>((0-(8))&31))));
  {key[ 9] = (((key[9])<<(((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31))))&31))|(((key[9])&0xFFFFFFFF)>>((0-((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31)))))&31))); if (key[9] & 1) return 1;};
  skype_rc4_iv19 (key, key[13]);
 }
 key[3] -= key[15] + 0xFE7A752E;
 if (k == 3)
 {
  (key[ 5] *= (((0xB706A577)<<((3)&31))|(((0xB706A577)&0xFFFFFFFF)>>((0-(3))&31))));
  (key[10] ^= key[7] - 0x354C1FF2);
  skype_rc4_iv20 (key, key[8]);
 }
 if (k == 7)
 {
  (key[ 9] -= ((((key[16])&0xFFFFFFFF)>>((25)&31))|((key[16])<<((0-(25))&31))));
  {key[ 1] ^= (key[4] < 0x196D816A) ? key[4] : key[17]; if (key[1] & 1) return 1;};
  skype_rc4_iv21 (key, iv);
 }
 key[0] ^= 25 * key[14];
 if (k == 2)
 {
  (key[11] &= 0x06839B1C);
  (key[11] = (((key[11])<<(((u8fcos[(key[5])&0xFF]) ? 19 : key[11])&31))|(((key[11])&0xFFFFFFFF)>>((0-((u8fcos[(key[5])&0xFF]) ? 19 : key[11]))&31))));
  skype_rc4_iv22 (key, key[6]);
 }
 if (k == 12)
 {
  {if (key[5] & 1) return 1;};
  (key[17] ^= 0x2D742F43);
  skype_rc4_iv23 (key, key[0]);
 }
 key[0] ^= (key[18] < 0xD339212E) ? key[18] : iv;
 if (k == 11)
 {
  (key[ 1] ^= (((0x5C1A35DA)<<((8)&31))|(((0x5C1A35DA)&0xFFFFFFFF)>>((0-(8))&31))));
  {key[ 3] -= key[17] | 0x2433636; if (key[3] & 1) return 1;};
  skype_rc4_iv24 (key, key[15]);
 }
 if (k == 0)
 {
  (key[9 ] *= (u8fsin[(0x24FE0880)&0xFF]) ? 0x28D781D2 : key[10]);
  (key[13] |= (u8fcos[(0x58FC9D30)&0xFF]) ? 0x1510A109 : key[14]);
  skype_rc4_iv1 (key, key[12]);
 }
 key[19] ^= key[2] ^ 0xACF3864;
 if (k == 15)
 {
  (key[14] ^= (u8fcos[(key[9])&0xFF]) ? 0x73CD560C : key[4]);
  (key[ 6] *= 0x1304694A * key[11]);
  skype_rc4_iv2 (key, iv);
 }
 if (k == 2)
 {
  (key[ 1] ^= (((0xAAB1F08)<<((8)&31))|(((0xAAB1F08)&0xFFFFFFFF)>>((0-(8))&31))));
  (key[17] += key[13] - 0x292C1156);
  skype_rc4_iv3 (key, iv);
 }
 iv -= (key[7] < 0xF2D2F7) ? key[7] : iv;
 if (k == 10)
 {
  {key[ 7] += ((((key[7])&0xFFFFFFFF)>>((21)&31))|((key[7])<<((0-(21))&31))); if (key[7] & 1) return 1;};
  (key[18] ^= key[10] + 0x1EE65B0C);
  skype_rc4_iv4 (key, key[3]);
 }
 key[12] = ((((key[12])&0xFFFFFFFF)>>((key[19] + 3)&31))|((key[12])<<((0-(key[19] + 3))&31)));
 if (k == 6)
 {
  {key[11] += 0xF6B10986 - key[14]; if (key[11] & 1) return 1;};
  {key[16] &= (key[12] < 0x28165E7B) ? key[12] : 0x4ADA50AE; if (key[16] & 1) return 1;};
  skype_rc4_iv5 (key, key[19]);
 }
 if (k == 9)
 {
  (key[ 1] -= key[16] | 0x59C1677);
  {key[ 0] *= 33 * key[0]; if (key[0] & 1) return 1;};
  skype_rc4_iv6 (key, key[9]);
 }
 iv ^= key[0] + 0xEC0FD36;
 if (k == 1)
 {
  (key[13] ^= (0x2AB43EDB < 0x2E0AF4F7) ? 0x2AB43EDB : key[15]);
  {key[ 0] *= 33 * key[0]; if (key[0] & 1) return 1;};
  skype_rc4_iv19 (key, iv);
 }
 if (k == 0)
 {
  (key[ 6] += (0xF7FB2E31 < 0x61F0BAA) ? 0xF7FB2E31 : key[16]);
  (key[17] += key[13] - 0x292C1156);
  skype_rc4_iv20 (key, key[6]);
 }
 iv += 93 * key[5];
 if (k == 13)
 {
  (key[ 2] ^= (u8fsin[(key[17])&0xFF]) ? 0x1C0E70BF : key[5]);
  (key[ 6] ^= (u8fcos[(key[7])&0xFF]) ? 0x1AB1E599 : key[18]);
  skype_rc4_iv21 (key, key[4]);
 }
 if (k == 5)
 {
  (key[ 7] ^= (u8sqrt[(key[15])&0xFF]));
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>((key[17]*18)&31))|((key[9])<<((0-(key[17]*18))&31))));
  skype_rc4_iv22 (key, key[5]);
 }
 iv &= iv - 0x6718263;
 if (k == 1)
 {
  (key[9 ] *= (u8fsin[(0x30A63C23)&0xFF]) ? 0x28D781D2 : key[10]);
  (key[11] ^= key[15] | 0x11273409);
  skype_rc4_iv23 (key, key[1]);
 }
 if (k == 14)
 {
  (key[ 5] *= (((0x98A2A62E)<<((3)&31))|(((0x98A2A62E)&0xFFFFFFFF)>>((0-(3))&31))));
  (key[10] ^= key[7] - 0x354C1FF2);
  skype_rc4_iv24 (key, key[18]);
 }
 if (k == 3)
 {
  (key[ 9] -= ((((key[16])&0xFFFFFFFF)>>((25)&31))|((key[16])<<((0-(25))&31))));
  {key[16] &= (key[12] < 0x28165E7B) ? key[12] : 0x87E63075; if (key[16] & 1) return 1;};
  skype_rc4_iv1 (key, key[14]);
 }
 iv = 0xEC076923 - key[10];
 if (k == 4)
 {
  (key[12] -= 66 * key[2]);
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>((key[17]*18)&31))|((key[9])<<((0-(key[17]*18))&31))));
  skype_rc4_iv2 (key, key[10]);
  {key[ 7] += key[2] ^ 0x376E1538; if (key[7] & 1) return 1;};
  (key[ 6] ^= (u8fcos[(key[7])&0xFF]) ? 0x1AB1E599 : key[18]);
  skype_rc4_iv3 (key, key[9]);
 }
 return 0xD271A9AD - key[8] + iv;
}

u32 skype_rc4_iv8 (u32 * const key, u32 iv)
{
 u32 k = (key[14] ^ key[1] ^ key[8]) & 15;

 if (k == 3)
 {
  (key[ 6] *= 0x1304694A * key[11]);
  (key[13] ^= (0x23841ACB < 0x2E0AF4F7) ? 0x23841ACB : key[15]);
  skype_rc4_iv20 (key, key[18]);
 }
 if (k == 0)
 {
  (key[ 3] += key[9] + 0x48210C78);
  (key[ 6] += (0x1656F2C < 0x61F0BAA) ? 0x1656F2C : key[16]);
  skype_rc4_iv21 (key, iv);
 }
 key[18] ^= (u8sqrt[(key[1])&0xFF]);
 if (k == 8)
 {
  (key[ 9] = (((key[9])<<((12)&31))|(((key[9])&0xFFFFFFFF)>>((0-(12))&31))));
  (key[19] &= 0xC98D4040);
  skype_rc4_iv22 (key, key[10]);
 }
 if (k == 13)
 {
  {key[ 9] = (((key[9])<<(((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31))))&31))|(((key[9])&0xFFFFFFFF)>>((0-((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31)))))&31))); if (key[9] & 1) return 1;};
  (key[17] ^= 0xC53796B3);
  skype_rc4_iv23 (key, key[18]);
 }
 key[2] += ((((key[14])&0xFFFFFFFF)>>((12)&31))|((key[14])<<((0-(12))&31)));
 if (k == 3)
 {
  (key[11] &= 0x29D9DEBC);
  (key[ 0] = (((key[0])<<((8 * key[18])&31))|(((key[0])&0xFFFFFFFF)>>((0-(8 * key[18]))&31))));
  skype_rc4_iv24 (key, iv);
 }
 if (k == 9)
 {
  {key[ 1] ^= (u8fcos[(key[3])&0xFF]) ? 0x7C23395 : key[18]; if (key[1] & 1) return 1;};
  {key[ 1] ^= (key[4] < 0x196D816A) ? key[4] : key[17]; if (key[1] & 1) return 1;};
  skype_rc4_iv1 (key, key[7]);
 }
 if (k == 11)
 {
  (key[16] &= (key[11] < 0x5578A05) ? key[11] : key[16]);
  (key[ 1] -= key[16] | 0x59C1677);
  skype_rc4_iv2 (key, key[6]);
 }
 key[18] ^= (u8sqrt[(key[17])&0xFF]);
 if (k == 10)
 {
  (key[ 2] ^= key[15] + 0x57CE331);
  (key[14] ^= (u8fcos[(key[9])&0xFF]) ? 0x73CD560C : key[4]);
  skype_rc4_iv3(key, iv);
 }
 if (k == 4)
 {
  (key[ 0] = (((key[0])<<(((0x41528AC>>17)&0x1F)&31))|(((key[0])&0xFFFFFFFF)>>((0-((0x41528AC>>17)&0x1F))&31))));
  (key[12] -= 66 * key[2]);
  skype_rc4_iv4(key, iv);
 }
 key[16] += 73 * key[2];
 if (k == 14)
 {
  (key[ 3] += key[9] + 0x48210C78);
  (key[12] *= (u8fcos[(0xA5DCFAFF)&0xFF]) ? 0x1734D89C : key[5]);
  skype_rc4_iv5(key, key[10]);
 }
 if (k == 5)
 {
  {key[ 3] -= key[17] | 0x2433636; if (key[3] & 1) return 1;};
  (key[ 9] -= ((((key[16])&0xFFFFFFFF)>>((25)&31))|((key[16])<<((0-(25))&31))));
  skype_rc4_iv6(key, iv);
 }
 iv += ((((iv)&0xFFFFFFFF)>>((19)&31))|((iv)<<((0-(19))&31)));
 if (k == 2)
 {
  (key[13] ^= (0x504F395C < 0x2E0AF4F7) ? 0x504F395C : key[15]);
  (key[ 6] += (0xB6E979A8 < 0x61F0BAA) ? 0xB6E979A8 : key[16]);
  skype_rc4_iv7 (key, iv);
  (key[8 ] ^= (u8fcos[(key[17])&0xFF]) ? 0x3544CA5E : key[8]);
  (key[ 2] += 0xEA2D3D5D * key[7]);
  skype_rc4_iv20 (key, iv);
 }
 key[9] ^= 110 * key[1];
 if (k == 1)
 {
  (key[12] -= 66 * key[2]);
  (key[12] *= (u8fcos[(0x6F09E141)&0xFF]) ? 0x1734D89C : key[5]);
  skype_rc4_iv21 (key, key[7]);
 }
 if (k == 4)
 {
  (key[ 7] ^= key[10] - 0x3035E544);
  (key[ 2] ^= key[15] + 0x57CE331);
  skype_rc4_iv22(key, key[7]);
 }
 if (k == 15)
 {
  {key[10] = (((key[10])<<((0xA09F88F1 -6)&31))|(((key[10])&0xFFFFFFFF)>>((0-(0xA09F88F1 -6))&31))); if (key[10] & 1) return 1;};
  (key[ 7] ^= (u8sqrt[(key[15])&0xFF]));
  skype_rc4_iv23(key, key[14]);
 }
 key[16] -= (key[0] < 0x22C8CF2) ? key[0] : key[5];
 if (k == 0)
 {
  {key[ 7] += key[2] ^ 0x376E1538; if (key[7] & 1) return 1;};
  (key[ 9] ^= (u8fcos[(key[11])&0xFF]) ? 0xEEB638B : key[6]);
  skype_rc4_iv24(key, key[2]);
 }
 if (k == 5)
 {
  (key[18] -= 122 * key[6]);
  {key[ 3] -= key[17] | 0x2433636; if (key[3] & 1) return 1;};
  skype_rc4_iv1(key, key[4]);
 }
 key[9] *= (iv < 0x5FD809D2) ? iv : key[4];
 if (k == 7)
 {
  (key[ 9] -= ((((key[16])&0xFFFFFFFF)>>((25)&31))|((key[16])<<((0-(25))&31))));
  (key[11] &= 0x332DA0B6);
  skype_rc4_iv2(key, key[5]);
 }
 if (k == 1)
 {
  (key[11] ^= key[15] | 0x11273409);
  (key[19] &= 0x86D5035B);
  skype_rc4_iv3 (key, key[0]);
 }
 iv += iv - 0x5790C685;
 if (k == 12)
 {
  (key[ 2] += 0xEA2D3D5D * key[7]);
  (key[15] -= (u8fcos[(0x121738E5)&0xFF]) ? 0x344432F : key[18]);
  skype_rc4_iv4 (key, iv);
 }
 if (k == 6)
 {
  (key[ 9] += 0x3A82007 - key[14]);
  (key[ 0] = (((key[0])<<(((0x60E885A6>>17)&0x1F)&31))|(((key[0])&0xFFFFFFFF)>>((0-((0x60E885A6>>17)&0x1F))&31))));
  skype_rc4_iv5 (key, key[4]);
 }
 key[18] *= 0x5790C685 ^ key[13];
 return iv;
}

u32 skype_rc4_iv9 (u32 * const key, u32 iv)
{
 u32 k = (key[18] ^ key[5] ^ key[2]) & 15;

 if (k == 10)
 {
  (key[ 7] -= 0xA7EF6FAB);
  (key[ 2] ^= key[15] + 0x57CE331);
  skype_rc4_iv21 (key, iv);
 }
 if (k == 6)
 {
  {key[ 0] *= 33 * key[0]; if (key[0] & 1) return 1;};
  (key[ 6] += 0x975C61BA - key[8]);
  skype_rc4_iv22(key, key[19]);
 }
 key[5] |= (u8sqrt[(key[11])&0xFF]);
 if (k == 5)
 {
  (key[ 3] += key[9] + 0x48210C78);
  (key[11] += 0x29CC7F53 - key[5]);
  skype_rc4_iv23(key, key[5]);
 }
 if (k == 4)
 {
  (key[ 9] += 0x3A82007 - key[14]);
  (key[ 6] *= 0x1304694A * key[11]);
  skype_rc4_iv24(key, key[9]);
 }
 key[15] &= 88 * key[12];
 if (k == 0)
 {
  (key[12] *= (u8fcos[(0xF5D726FF)&0xFF]) ? 0x1734D89C : key[5]);
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>((key[17]*18)&31))|((key[9])<<((0-(key[17]*18))&31))));
  skype_rc4_iv1 (key, key[0]);
 }
 if (k == 7)
 {
  (key[12] -= 66 * key[2]);
  (key[ 1] ^= (((0x6C0A34BC)<<((8)&31))|(((0x6C0A34BC)&0xFFFFFFFF)>>((0-(8))&31))));
  skype_rc4_iv2 (key, key[10]);
 }
 if (k == 4)
 {
  (key[11] = (((key[11])<<(((u8fcos[(key[5])&0xFF]) ? 19 : key[11])&31))|(((key[11])&0xFFFFFFFF)>>((0-((u8fcos[(key[5])&0xFF]) ? 19 : key[11]))&31))));
  {if (key[5] & 1) return 1;};
  skype_rc4_iv3 (key, key[2]);
 }
 iv += 0xFDFB78FE * key[11];
 if (k == 5)
 {
  (key[11] -= key[12] << 5);
  (key[18] ^= key[10] + 0x1EE65B0C);
  skype_rc4_iv4 (key, key[17]);
 }
 if (k == 12)
 {
  (key[15] ^= (key[14] < 0x291B9650) ? key[14] : key[2]);
  (key[12] *= (u8fcos[(0x4EFAD4C)&0xFF]) ? 0x1734D89C : key[5]);
  skype_rc4_iv5 (key, key[9]);
 }
 key[12] += iv - 0x4C7E1193;
 if (k == 3)
 {
  (key[ 1] ^= (((0x6FFD6E50)<<((8)&31))|(((0x6FFD6E50)&0xFFFFFFFF)>>((0-(8))&31))));
  (key[ 6] ^= (u8fcos[(key[7])&0xFF]) ? 0x1AB1E599 : key[18]);
  skype_rc4_iv6 (key, key[17]);
 }
 if (k == 2)
 {
  (key[ 7] ^= (u8sqrt[(key[15])&0xFF]));
  (key[ 2] += 0xEA2D3D5D * key[7]);
  skype_rc4_iv7 (key, key[15]);
 }
 if (k == 3)
 {
  (key[ 6] ^= (u8fcos[(key[7])&0xFF]) ? 0x1AB1E599 : key[18]);
  (key[ 0] = (((key[0])<<((8 * key[18])&31))|(((key[0])&0xFFFFFFFF)>>((0-(8 * key[18]))&31))));
  skype_rc4_iv8 (key, iv);
 }
 key[16] += (u8sqrt[(key[2])&0xFF]);
 if (k == 6)
 {
  (key[16] &= (key[11] < 0x5578A05) ? key[11] : key[16]);
  (key[12] *= (u8fcos[(0x28288B07)&0xFF]) ? 0x1734D89C : key[5]);
  skype_rc4_iv21 (key, key[15]);
 }
 if (k == 2)
 {
  (key[ 2] -= (u8fsin[(0xA623C3F4)&0xFF]) ? 0x73423C3 : key[7]);
  (key[17] ^= 0x2F961 * key[4]);
  skype_rc4_iv22 (key, key[5]);
 }
 key[0] *= 123 * key[19];
 if (k == 9)
 {
  (key[8 ] ^= (u8fcos[(key[17])&0xFF]) ? 0x3544CA5E : key[8]);
  (key[14] ^= (u8fcos[(key[9])&0xFF]) ? 0x73CD560C : key[4]);
  skype_rc4_iv23 (key, key[0]);
 }
 if (k == 15)
 {
  (key[10] ^= key[7] - 0x354C1FF2);
  (key[ 9] = (((key[9])<<((5)&31))|(((key[9])&0xFFFFFFFF)>>((0-(5))&31))));
  skype_rc4_iv24 (key, iv);
 }
 if (k == 14)
 {
  (key[18] ^= key[10] + 0x1EE65B0C);
  {key[ 9] = (((key[9])<<(((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31))))&31))|(((key[9])&0xFFFFFFFF)>>((0-((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31)))))&31))); if (key[9] & 1) return 1;};
  skype_rc4_iv1 (key, key[8]);
 }
 key[4] |= 0x89C7C4D3 ^ key[13];
 if (k == 13)
 {
  {key[ 9] = (((key[9])<<(((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31))))&31))|(((key[9])&0xFFFFFFFF)>>((0-((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31)))))&31))); if (key[9] & 1) return 1;};
  (key[16] &= (key[11] < 0x5578A05) ? key[11] : key[16]);
  skype_rc4_iv2 (key, key[3]);
 }
 if (k == 8)
 {
  (key[ 2] += 0xEA2D3D5D * key[7]);
  (key[ 9] += 0x3A82007 - key[14]);
  skype_rc4_iv3 (key, key[5]);
 }
 iv -= (u8fcos[(key[14])&0xFF]) ? 0x111DCA68 : key[18];
 if (k == 11)
 {
  (key[ 9] = (((key[9])<<((29)&31))|(((key[9])&0xFFFFFFFF)>>((0-(29))&31))));
  (key[15] -= (u8fcos[(0xA35F29BB)&0xFF]) ? 0x344432F : key[18]);
  skype_rc4_iv4 (key, key[3]);
 }
 if (k == 0)
 {
  (key[ 9] -= ((((key[16])&0xFFFFFFFF)>>((25)&31))|((key[16])<<((0-(25))&31))));
  (key[ 5] *= (((0x20F609B2)<<((3)&31))|(((0x20F609B2)&0xFFFFFFFF)>>((0-(3))&31))));
  skype_rc4_iv5 (key, key[11]);
 }
 if (k == 1)
 {
  (key[ 1] -= key[16] | 0x59C1677);
  (key[18] ^= key[10] + 0x1EE65B0C);
  skype_rc4_iv6 (key, key[4]);
 }
 key[11] *= (u8sqrt[(iv)&0xFF]);
 if (k == 1)
 {
  (key[ 5] *= (((0x8FE3C3F3)<<((3)&31))|(((0x8FE3C3F3)&0xFFFFFFFF)>>((0-(3))&31))));
  (key[10] ^= key[7] - 0x354C1FF2);
  skype_rc4_iv7 (key, iv);
 }
 key[16] ^= (u8fsin[(iv)&0xFF]) ? 0x16283882 : key[12];
 return iv;
}

u32 skype_rc4_iv10 (u32 * const key, u32 iv)
{
 u32 k = key[14] & 15;

 if (k == 5)
 {
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>(((u8fcos[(key[9])&0xFF]) ? 20 : key[0])&31))|((key[9])<<((0-((u8fcos[(key[9])&0xFF]) ? 20 : key[0]))&31))));
  (key[ 7] ^= (u8sqrt[(key[15])&0xFF]));
  skype_rc4_iv22 (key, iv);
 }
 if (k == 9)
 {
  (key[ 2] &= ((((0xBD460637)&0xFFFFFFFF)>>((17)&31))|((0xBD460637)<<((0-(17))&31))));
  (key[ 2] &= ((((0x23D5BA3F)&0xFFFFFFFF)>>((17)&31))|((0x23D5BA3F)<<((0-(17))&31))));
  skype_rc4_iv23 (key, key[19]);
 }
 key[19] *= (u8sqrt[(iv)&0xFF]);
 if (k == 10)
 {
  (key[11] += 0x29CC7F53 - key[5]);
  (key[ 7] -= 0x7789BFF3);
  skype_rc4_iv24 (key, key[12]);
 }
 if (k == 11)
 {
  (key[17] ^= 0x2F961 * key[4]);
  (key[ 9] = (((key[9])<<((1)&31))|(((key[9])&0xFFFFFFFF)>>((0-(1))&31))));
  skype_rc4_iv1 (key, iv);
 }
 key[12] ^= (u8fcos[(key[11])&0xFF]) ? 0x1594E0E0 : key[7];
 if (k == 15)
 {
  {key[ 7] += key[2] ^ 0x376E1538; if (key[7] & 1) return 1;};
  (key[16] = ((((key[16])&0xFFFFFFFF)>>((key[10] - 11)&31))|((key[16])<<((0-(key[10] - 11))&31))));
  skype_rc4_iv2 (key, iv);
 }
 if (k == 4)
 {
  {key[10] = (((key[10])<<((0x7D76C942 -6)&31))|(((key[10])&0xFFFFFFFF)>>((0-(0x7D76C942 -6))&31))); if (key[10] & 1) return 1;};
  (key[18] -= 122 * key[6]);
  skype_rc4_iv3 (key, iv);
 }
 key[18] &= ((((key[17])&0xFFFFFFFF)>>((29)&31))|((key[17])<<((0-(29))&31)));
 if (k == 12)
 {
  (key[ 6] += 0x975C61BA - key[8]);
  (key[ 0] = (((key[0])<<(((0xB82B0DAB>>17)&0x1F)&31))|(((key[0])&0xFFFFFFFF)>>((0-((0xB82B0DAB>>17)&0x1F))&31))));
  skype_rc4_iv4 (key, key[3]);
 }
 if (k == 1)
 {
  (key[ 7] -= 0x3F7FFFE3);
  (key[ 7] += 0x6AEC3B22);
  skype_rc4_iv5 (key, key[5]);
 }
 key[1] -= 92 * key[19];
 if (k == 14)
 {
  (key[ 2] -= (u8fsin[(0x752B3066)&0xFF]) ? 0x73423C3 : key[7]);
  (key[17] += key[13] - 0x292C1156);
  skype_rc4_iv6 (key, key[0]);
 }
 if (k == 3)
 {
  (key[17] ^= 0x2F961 * key[4]);
  (key[9 ] *= (u8fsin[(0x2CE8A84E)&0xFF]) ? 0x28D781D2 : key[10]);
  skype_rc4_iv7 (key, iv);
 }
 key[14] += 0x1579CA5 - iv;
 if (k == 1)
 {
  (key[14] ^= (u8fcos[(key[9])&0xFF]) ? 0x73CD560C : key[4]);
  (key[ 0] = (((key[0])<<((8 * key[18])&31))|(((key[0])&0xFFFFFFFF)>>((0-(8 * key[18]))&31))));
  skype_rc4_iv8 (key, iv);
 }
 if (k == 6)
 {
  (key[17] ^= 0x2EFE59F4);
  (key[ 6] ^= ((((key[14])&0xFFFFFFFF)>>((28)&31))|((key[14])<<((0-(28))&31))));
  skype_rc4_iv9 (key, iv);
 }
 key[8] -= 78 * key[1];
 if (k == 13)
 {
  (key[17] ^= 0x18A96853);
  (key[12] *= (u8fcos[(0x0F6AF31E)&0xFF]) ? 0x1734D89C : key[5]);
  skype_rc4_iv22 (key, key[14]);
 }
 if (k == 2)
 {
  (key[11] |= key[4] - 0x224114CD);
  (key[ 9] = (((key[9])<<((4)&31))|(((key[9])&0xFFFFFFFF)>>((0-(4))&31))));
  skype_rc4_iv23 (key, key[13]);
 }
 key[16] = ((((key[16])&0xFFFFFFFF)>>((key[15] | 30)&31))|((key[16])<<((0-(key[15] | 30))&31)));
 if (k == 0)
 {
  (key[ 7] -= 0xA7E9B7A3);
  (key[ 4] ^= key[19] - 0x303D46FE);
  skype_rc4_iv24 (key, key[7]);
  (key[11] &= 0xF7849C32);
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>((key[17]*18)&31))|((key[9])<<((0-(key[17]*18))&31))));
  skype_rc4_iv1 (key, key[6]);
 }
 key[8] -= 12 * key[18];
 if (k == 4)
 {
  (key[ 9] -= ((((key[16])&0xFFFFFFFF)>>((25)&31))|((key[16])<<((0-(25))&31))));
  {key[ 1] ^= (key[4] < 0x196D816A) ? key[4] : key[17]; if (key[1] & 1) return 1;};
  skype_rc4_iv2 (key, iv);
 }
 if (k == 3)
 {
  (key[ 7] += 0x351E5020);
  (key[11] -= key[12] << 5);
  skype_rc4_iv3 (key, key[1]);
 }
 key[14] ^= (u8fcos[(key[5])&0xFF]) ? 0x579B4B85 : key[6];
 if (k == 2)
 {
  (key[ 0] = (((key[0])<<((8 * key[18])&31))|(((key[0])&0xFFFFFFFF)>>((0-(8 * key[18]))&31))));
  {key[ 7] += key[2] ^ 0x376E1538; if (key[7] & 1) return 1;};
  skype_rc4_iv4 (key, key[10]);
 }
 if (k == 5)
 {
  {if (key[5] & 1) return 1;};
  (key[ 5] += key[11] | 0xEA02A83);
  skype_rc4_iv5 (key, iv);
 }
 key[10] += 0x3575DC9 * key[13];
 if (k == 7)
 {
  (key[ 2] &= key[18] - 0x37CF1A3F);
  (key[ 0] = (((key[0])<<(((0xB8AD3ED9>>17)&0x1F)&31))|(((key[0])&0xFFFFFFFF)>>((0-((0xB8AD3ED9>>17)&0x1F))&31))));
  skype_rc4_iv6 (key, key[16]);
  {key[10] = (((key[10])<<((0xC279F292 -6)&31))|(((key[10])&0xFFFFFFFF)>>((0-(0xC279F292 -6))&31))); if (key[10] & 1) return 1;};
  (key[11] |= key[4] - 0x224114CD);
  skype_rc4_iv7 (key, key[16]);
 }
 if (k == 8)
 {
  (key[17] += key[13] - 0x292C1156);
  (key[ 9] ^= (u8fcos[(key[11])&0xFF]) ? 0xEEB638B : key[6]);
  skype_rc4_iv8 (key, key[14]);
 }
 key[4] ^= key[5] & 0x136008B9;
 if (k == 6)
 {
  (key[ 6] ^= ((((key[14])&0xFFFFFFFF)>>((28)&31))|((key[14])<<((0-(28))&31))));
  (key[ 5] += key[11] | 0xEA02A83);
  skype_rc4_iv9 (key, key[4]);
 }
 key[3] ^= 0x29909382 + key[13];
 return iv;
}

u32 skype_rc4_iv11 (u32 * const key, u32 iv)
{
 u32 k = key[3] & 15;

 if (k == 5)
 {
  (key[ 9] ^= (u8fcos[(key[11])&0xFF]) ? 0xEEB638B : key[6]);
  {key[ 3] -= key[17] | 0x2433636; if (key[3] & 1) return 1;};
  skype_rc4_iv23 (key, key[4]);
 }
 if (k == 7)
 {
  (key[17] ^= 0xBEF59983);
  (key[18] ^= key[10] + 0x1EE65B0C);
  skype_rc4_iv24 (key, iv);
 }
 key[4] *= (u8fsin[(iv)&0xFF]) ? 0x23D1B75F : key[11];
 if (k == 10)
 {
  (key[ 4] ^= key[19] - 0x303D46FE);
  (key[ 3] += key[9] + 0x48210C78);
  skype_rc4_iv1 (key, iv);
 }
 if (k == 8)
 {
  (key[ 3] += key[9] + 0x48210C78);
  (key[ 2] += 0xEA2D3D5D * key[7]);
  skype_rc4_iv2 (key, key[11]);
 }
 iv *= key[13] - 0x21175CB;
 if (k == 4)
 {
  (key[12] = ((((key[12])&0xFFFFFFFF)>>((key[18] ^ 9)&31))|((key[12])<<((0-(key[18] ^ 9))&31))));
  (key[11] ^= key[15] | 0x11273409);
  skype_rc4_iv3 (key, key[11]);
 }
 if (k == 2)
 {
  (key[18] ^= key[10] + 0x1EE65B0C);
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>((key[17]*18)&31))|((key[9])<<((0-(key[17]*18))&31))));
  skype_rc4_iv4 (key, key[15]);
  (key[ 4] ^= 17 * key[0]);
  (key[10] ^= key[7] - 0x354C1FF2);
  skype_rc4_iv5 (key, key[8]);
 }
 iv = (((iv)<<((iv + 5)&31))|(((iv)&0xFFFFFFFF)>>((0-(iv + 5))&31)));
 if (k == 4)
 {
  (key[ 2] &= ((((0xF2F0F729)&0xFFFFFFFF)>>((17)&31))|((0xF2F0F729)<<((0-(17))&31))));
  (key[ 6] ^= (u8fcos[(key[7])&0xFF]) ? 0x1AB1E599 : key[18]);
  skype_rc4_iv6 (key, key[7]);
 }
 if (k == 0)
 {
  (key[ 7] ^= (u8sqrt[(key[15])&0xFF]));
  (key[ 9] = (((key[9])<<((12)&31))|(((key[9])&0xFFFFFFFF)>>((0-(12))&31))));
  skype_rc4_iv7 (key, key[17]);
 }
 key[3] = (((key[3])<<(((key[8] < 0x354AE538) ? key[8] : iv)&31))|(((key[3])&0xFFFFFFFF)>>((0-((key[8] < 0x354AE538) ? key[8] : iv))&31)));
 if (k == 0)
 {
  {key[ 0] *= 33 * key[0]; if (key[0] & 1) return 1;};
  (key[ 6] *= 0x1304694A * key[11]);
  skype_rc4_iv8 (key, key[7]);
 }
 if (k == 11)
 {
  (key[ 6] += (0xD083A1C7 < 0x61F0BAA) ? 0xD083A1C7 : key[16]);
  (key[16] = ((((key[16])&0xFFFFFFFF)>>((key[10] - 11)&31))|((key[16])<<((0-(key[10] - 11))&31))));
  skype_rc4_iv9 (key, key[3]);
 }
 key[7] -= key[18] ^ 0xF1CDC7D;
 if (k == 8)
 {
  (key[ 6] ^= (u8fcos[(key[7])&0xFF]) ? 0x1AB1E599 : key[18]);
  (key[12] *= (u8fcos[(0xC67A38DE)&0xFF]) ? 0x1734D89C : key[5]);
  skype_rc4_iv10 (key, key[16]);
 }
 if (k == 1)
 {
  {key[ 3] -= key[17] | 0x2433636; if (key[3] & 1) return 1;};
  (key[18] ^= key[10] + 0x1EE65B0C);
  skype_rc4_iv23 (key, iv);
 }
 if (k == 15)
 {
  (key[ 4] ^= key[19] - 0x303D46FE);
  (key[13] |= (u8fcos[(0xE6E035C2)&0xFF]) ? 0x1510A109 : key[14]);
  skype_rc4_iv24 (key, key[3]);
 }
 iv += (key[13] < 0x32081481) ? key[13] : iv;
 if (k == 9)
 {
  (key[19] &= 0xBFAF8559);
  (key[ 2] -= (u8fsin[(0x8494F082)&0xFF]) ? 0x73423C3 : key[7]);
  skype_rc4_iv1 (key, key[10]);
 }
 if (k == 3)
 {
  (key[ 6] += 0x975C61BA - key[8]);
  (key[17] ^= 0x0D4CA763);
  skype_rc4_iv2 (key, key[3]);
 }
 key[18] += 0xD3BE6F0E - key[17];
 if (k == 1)
 {
  (key[19] &= 0xAEB00548);
  (key[12] = ((((key[12])&0xFFFFFFFF)>>((key[18] ^ 9)&31))|((key[12])<<((0-(key[18] ^ 9))&31))));
  skype_rc4_iv3 (key, key[13]);
 }
 if (k == 5)
 {
  (key[ 3] ^= key[0] + 0x4376FF7);
  (key[ 7] -= 0x379DEFB3);
  skype_rc4_iv4 (key, iv);
 }
 if (k == 6)
 {
  (key[16] &= (key[11] < 0x5578A05) ? key[11] : key[16]);
  (key[15] ^= (key[14] < 0x291B9650) ? key[14] : key[2]);
  skype_rc4_iv5 (key, key[7]);
 }
 key[7] -= (iv < 0xD28B93FE) ? iv : key[18];
 if (k == 14)
 {
  (key[15] ^= (key[14] < 0x291B9650) ? key[14] : key[2]);
  (key[ 2] &= ((((0xD0D89937)&0xFFFFFFFF)>>((17)&31))|((0xD0D89937)<<((0-(17))&31))));
  skype_rc4_iv6 (key, iv);
 }
 if (k == 3)
 {
  (key[13] |= (u8fcos[(0x91403FF1)&0xFF]) ? 0x1510A109 : key[14]);
  (key[11] = (((key[11])<<(((u8fcos[(key[5])&0xFF]) ? 19 : key[11])&31))|(((key[11])&0xFFFFFFFF)>>((0-((u8fcos[(key[5])&0xFF]) ? 19 : key[11]))&31))));
  skype_rc4_iv7 (key, key[10]);
 }
 key[2] = (((key[2])<<((-80 * key[17])&31))|(((key[2])&0xFFFFFFFF)>>((0-(-80 * key[17]))&31)));
 if (k == 13)
 {
  (key[12] = ((((key[12])&0xFFFFFFFF)>>((key[18] ^ 9)&31))|((key[12])<<((0-(key[18] ^ 9))&31))));
  (key[12] *= (u8fcos[(0xD9FEC55F)&0xFF]) ? 0x1734D89C : key[5]);
  skype_rc4_iv8 (key, key[10]);
 }
 if (k == 12)
 {
  (key[17] ^= 0xDFBCF197);
  (key[ 7] -= 0x3F4F37BB);
  skype_rc4_iv9 (key, iv);
 }
 key[17] -= key[3] & 0x5C40869;
 if (k == 7)
 {
  (key[ 6] *= 0x1304694A * key[11]);
  (key[12] *= (u8fcos[(0xE8B5F17D)&0xFF]) ? 0x1734D89C : key[5]);
  skype_rc4_iv10 (key, key[4]);
 }
 if (k == 6)
 {
  (key[12] = ((((key[12])&0xFFFFFFFF)>>((key[18] ^ 9)&31))|((key[12])<<((0-(key[18] ^ 9))&31))));
  (key[17] += key[13] - 0x292C1156);
  skype_rc4_iv23 (key, key[7]);
 }
 key[6] += iv - 0xAD58C16C;
 return iv;
}

u32 skype_rc4_iv12 (u32 * const key, u32 iv)
{
 u32 k = (key[19] ^ iv ^ key[0]) & 15;

 if (k == 2)
 {
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>((key[17]*18)&31))|((key[9])<<((0-(key[17]*18))&31))));
  {if (key[5] & 1) return 1;};
  skype_rc4_iv24 (key, key[6]);
 }
 if (k == 3)
 {
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>(((u8fcos[(key[9])&0xFF]) ? 20 : key[0])&31))|((key[9])<<((0-((u8fcos[(key[9])&0xFF]) ? 20 : key[0]))&31))));
  (key[ 9] -= ((((key[16])&0xFFFFFFFF)>>((25)&31))|((key[16])<<((0-(25))&31))));
  skype_rc4_iv1 (key, key[0]);
 }
 iv -= ((((key[7])&0xFFFFFFFF)>>((10)&31))|((key[7])<<((0-(10))&31)));
 if (k == 8)
 {
  (key[ 9] += 0x3A82007 - key[14]);
  (key[9 ] *= (u8fsin[(0x454B4CB0)&0xFF]) ? 0x28D781D2 : key[10]);
  skype_rc4_iv2 (key, key[5]);
 }
 if (k == 13)
 {
  (key[ 7] ^= key[10] - 0x3035E544);
  {key[ 3] -= key[17] | 0x2433636; if (key[3] & 1) return 1;};
  skype_rc4_iv3 (key, key[4]);
 }
 key[5] ^= key[13] - 0x05648E1C;
 if (k == 1)
 {
  (key[12] = ((((key[12])&0xFFFFFFFF)>>((key[18] ^ 9)&31))|((key[12])<<((0-(key[18] ^ 9))&31))));
  (key[15] ^= (key[14] < 0x291B9650) ? key[14] : key[2]);
  skype_rc4_iv4 (key, key[5]);
 }
 if (k == 15)
 {
  {key[ 7] += key[2] ^ 0x376E1538; if (key[7] & 1) return 1;};
  (key[ 6] += (0xDBC32E55 < 0x61F0BAA) ? 0xDBC32E55 : key[16]);
  skype_rc4_iv5 (key, iv);
 }
 if (k == 7)
 {
  (key[12] -= 66 * key[2]);
  (key[ 9] -= ((((key[16])&0xFFFFFFFF)>>((25)&31))|((key[16])<<((0-(25))&31))));
  skype_rc4_iv6 (key, key[16]);
 }
 key[0] ^= 0xC8350BCD * key[17];
 if (k == 1)
 {
  {key[ 9] = (((key[9])<<(((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31))))&31))|(((key[9])&0xFFFFFFFF)>>((0-((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31)))))&31))); if (key[9] & 1) return 1;};
  (key[ 6] += (0xF14A0809 < 0x61F0BAA) ? 0xF14A0809 : key[16]);
  skype_rc4_iv7 (key, key[12]);
 }
 if (k == 10)
 {
  (key[ 1] -= key[16] | 0x59C1677);
  (key[ 7] -= 0xEF4BAFA3);
  skype_rc4_iv8 (key, key[16]);
 }
 if (k == 4)
 {
  {key[ 7] += key[2] ^ 0x376E1538; if (key[7] & 1) return 1;};
  (key[15] ^= (key[14] < 0x291B9650) ? key[14] : key[2]);
  skype_rc4_iv9 (key, key[18]);
 }
 iv &= (0xC8350BCD + key[13]);
 if (k == 5)
 {
  (key[ 0] = (((key[0])<<(((0x75B10262>>17)&0x1F)&31))|(((key[0])&0xFFFFFFFF)>>((0-((0x75B10262>>17)&0x1F))&31))));
  (key[ 4] ^= 17 * key[0]);
  skype_rc4_iv10 (key, key[7]);
 }
 if (k == 6)
 {
  (key[12] -= 66 * key[2]);
  (key[10] ^= key[7] - 0x354C1FF2);
  skype_rc4_iv11 (key, key[2]);
 }
 key[2] = ((((key[2])&0xFFFFFFFF)>>((key[17] & 8)&31))|((key[2])<<((0-(key[17] & 8))&31)));
 if (k == 5)
 {
  (key[ 2] ^= (u8fsin[(key[17])&0xFF]) ? 0x1C0E70BF : key[5]);
  (key[ 6] += 0x975C61BA - key[8]);
  skype_rc4_iv24 (key, iv);
 }
 if (k == 0)
 {
  (key[16] = ((((key[16])&0xFFFFFFFF)>>((key[10] - 11)&31))|((key[16])<<((0-(key[10] - 11))&31))));
  {key[ 7] += key[2] ^ 0x376E1538; if (key[7] & 1) return 1;};
  skype_rc4_iv1 (key, key[8]);
 }
 iv = ((((iv)&0xFFFFFFFF)>>((iv - 16)&31))|((iv)<<((0-(iv - 16))&31)));
 if (k == 9)
 {
  (key[10] ^= key[7] - 0x354C1FF2);
  (key[ 1] ^= (((0x7C854F50)<<((8)&31))|(((0x7C854F50)&0xFFFFFFFF)>>((0-(8))&31))));
  skype_rc4_iv2 (key, key[2]);
 }
 if (k == 3)
 {
  {if (key[5] & 1) return 1;};
  {key[ 7] += ((((key[7])&0xFFFFFFFF)>>((21)&31))|((key[7])<<((0-(21))&31))); if (key[7] & 1) return 1;};
  skype_rc4_iv3 (key, key[16]);
 }
 if (k == 6)
 {
  (key[ 7] ^= key[10] - 0x3035E544);
  (key[ 2] &= key[18] - 0x37CF1A3F);
  skype_rc4_iv4 (key, key[12]);
 }
 key[17] ^= (((key[3])<<((5)&31))|(((key[3])&0xFFFFFFFF)>>((0-(5))&31)));
 if (k == 4)
 {
  (key[ 7] ^= key[10] - 0x3035E544);
  (key[11] = (((key[11])<<(((u8fcos[(key[5])&0xFF]) ? 19 : key[11])&31))|(((key[11])&0xFFFFFFFF)>>((0-((u8fcos[(key[5])&0xFF]) ? 19 : key[11]))&31))));
  skype_rc4_iv5 (key, key[19]);
 }
 if (k == 11)
 {
  (key[ 5] += key[11] | 0xEA02A83);
  (key[ 2] -= (u8fsin[(0xBDDD6945)&0xFF]) ? 0x73423C3 : key[7]);
  skype_rc4_iv6 (key, iv);
 }
 key[15] += 0x531BF4BD - key[5];
 if (k == 0)
 {
  (key[ 6] += 0x975C61BA - key[8]);
  {key[ 1] ^= (key[4] < 0x196D816A) ? key[4] : key[17]; if (key[1] & 1) return 1;};
  skype_rc4_iv7 (key, iv);
 }
 if (k == 9)
 {
  (key[ 9] += 0x3A82007 - key[14]);
  (key[ 2] ^= (u8fsin[(key[17])&0xFF]) ? 0x1C0E70BF : key[5]);
  skype_rc4_iv8 (key, iv);
 }
 if (k == 14)
 {
  (key[ 4] ^= 17 * key[0]);
  (key[ 6] *= 0x1304694A * key[11]);
  skype_rc4_iv9 (key, key[7]);
 }
 key[5] &= (key[9] < 0x802D5786) ? key[9] : key[16];
 if (k == 12)
 {
  (key[ 3] ^= key[0] + 0x4376FF7);
  (key[11] |= key[4] - 0x224114CD);
  skype_rc4_iv10 (key, iv);
 }
 if (k == 7)
 {
  (key[ 1] ^= (((0x75382920)<<((8)&31))|(((0x75382920)&0xFFFFFFFF)>>((0-(8))&31))));
  (key[ 0] = (((key[0])<<((8 * key[18])&31))|(((key[0])&0xFFFFFFFF)>>((0-(8 * key[18]))&31))));
  skype_rc4_iv11 (key, key[10]);
 }
 key[1] *= 104 * key[12];
 if (k == 8)
 {
  (key[ 6] += 0x975C61BA - key[8]);
  (key[13] |= (u8fcos[(0xCCB27D90)&0xFF]) ? 0x1510A109 : key[14]);
  skype_rc4_iv24 (key, key[19]);
 }
 if (k == 2)
 {
  (key[ 2] += 0xEA2D3D5D * key[7]);
  (key[11] = (((key[11])<<(((u8fcos[(key[5])&0xFF]) ? 19 : key[11])&31))|(((key[11])&0xFFFFFFFF)>>((0-((u8fcos[(key[5])&0xFF]) ? 19 : key[11]))&31))));
  skype_rc4_iv1 (key, key[4]);
 }
 key[17] ^= (iv < 0x136FA01) ? iv : key[18];
 return iv;
}

u32 skype_rc4_iv13 (u32 * const key, u32 iv)
{
 u32 k = (iv ^ key[0] ^ key[18]) & 1;

 key[11] ^= key[5] | 0x16AE2234;
 key[12] += (key[8] & 0xAE32B8E);
 key[0] -= 12 * key[16];
 key[16] ^= iv + 0xA8BDFE15;
 key[9] -= 0x8181F7E7 + key[10];
 if (k == 0)
 {
  (key[12] *= (u8fcos[(0x24B5D4C1)&0xFF]) ? 0x1734D89C : key[5]);
  (key[ 9] -= ((((key[16])&0xFFFFFFFF)>>((25)&31))|((key[16])<<((0-(25))&31))));
  {u32 jv = iv + ((u8fsin[(key[7])&0xFF]) ? 0xCC95AFBF : key[9]); key[ 2] += jv + 0xE6ECDA3, key[ 9] &= (u8fsin[(key[7])&0xFF]) ? 0x13D68223 : jv, key[15] += 0x38245913 - key[12], key[16] = ((((key[16])&0xFFFFFFFF)>>((30 * key[17])&31))|((key[16])<<((0-(30 * key[17]))&31))), key[11] += 0x36F87E5B - key[5], key[ 2] += 102 * key[3], jv *= (((key[5])<<((30)&31))|(((key[5])&0xFFFFFFFF)>>((0-(30))&31))), key[ 9] += 123 * jv, key[ 5] = (((key[5])<<((key[16] - 11)&31))|(((key[5])&0xFFFFFFFF)>>((0-(key[16] - 11))&31))), key[10] |= (u8fsin[(key[4])&0xFF]) ? 0x84EDC63 : key[4];};
 }
 iv = ((((iv)&0xFFFFFFFF)>>((30 * key[16])&31))|((iv)<<((0-(30 * key[16]))&31)));
 iv ^= ((((key[15])&0xFFFFFFFF)>>((20)&31))|((key[15])<<((0-(20))&31)));
 key[13] += key[13] ^ 0x486B19AC;
 key[2] |= key[9] + 0x1086C59F;
 key[14] |= (u8fcos[(iv)&0xFF]) ? 0x5BADEF93 : key[10];
 if (k == 1)
 {
  (key[ 6] ^= (u8fcos[(key[7])&0xFF]) ? 0x1AB1E599 : key[18]);
  {key[10] = (((key[10])<<((0xB0DAD1C3 -6)&31))|(((key[10])&0xFFFFFFFF)>>((0-(0xB0DAD1C3 -6))&31))); if (key[10] & 1) return 1;};
  {u32 jv = key[17] & ((u8fcos[(key[10])&0xFF]) ? 0xF998E196 : key[10]); key[ 6] ^= (((jv)<<((7)&31))|(((jv)&0xFFFFFFFF)>>((0-(7))&31))), key[ 1] ^= jv - 0x4B327DA, key[ 7] += jv ^ 0x672E5A7, jv ^= (u8fsin[(jv)&0xFF]) ? 0xBC91B04 : key[8], key[11] ^= key[6] & 0xBE53718, jv ^= (u8fcos[(key[2])&0xFF]) ? 0x9DADA8A4 : jv, key[ 0] -= 0x9DADA8A4 & key[6], key[13] += key[1] - 0x7B284744, key[ 3] ^= 20 * key[18], key[ 2] |= jv - 0x313BB22;};
 }
 key[12] += (key[10] < 0x2EE7C8C3) ? key[10] : iv;
 return iv;
}

u32 skype_rc4_iv14 (u32 * const key, u32 iv)
{
 u32 k = (iv ^ key[0] ^ key[16]) % 3;

 key[12] += (iv < 0x3FEC317) ? iv : key[12];
 iv = ((((iv)&0xFFFFFFFF)>>((iv - 8)&31))|((iv)<<((0-(iv - 8))&31)));
 key[10] *= key[10] - 0x54940E;
 if (k == 2)
 {
  (key[ 9] = (((key[9])<<((29)&31))|(((key[9])&0xFFFFFFFF)>>((0-(29))&31))));
  (key[10] ^= key[7] - 0x354C1FF2);
  {u32 jv = key[19] + ((u8fsin[(key[7])&0xFF]) ? 0xCC95AFBF : key[9]); key[ 2] += jv + 0xE6ECDA3, key[ 9] &= (u8fsin[(key[7])&0xFF]) ? 0x13D68223 : jv, key[15] += 0x38245913 - key[12], key[16] = ((((key[16])&0xFFFFFFFF)>>((30 * key[17])&31))|((key[16])<<((0-(30 * key[17]))&31))), key[11] += 0x36F87E5B - key[5], key[ 2] += 102 * key[3], jv *= (((key[5])<<((30)&31))|(((key[5])&0xFFFFFFFF)>>((0-(30))&31))), key[ 9] += 123 * jv, key[ 5] = (((key[5])<<((key[16] - 11)&31))|(((key[5])&0xFFFFFFFF)>>((0-(key[16] - 11))&31))), key[10] |= (u8fsin[(key[4])&0xFF]) ? 0x84EDC63 : key[4];};
 }
 key[2] = ((((key[2])&0xFFFFFFFF)>>((((((key[6])&0xFFFFFFFF)>>((22)&31))|((key[6])<<((0-(22))&31))))&31))|((key[2])<<((0-(((((key[6])&0xFFFFFFFF)>>((22)&31))|((key[6])<<((0-(22))&31)))))&31)));
 key[12] = ((((key[12])&0xFFFFFFFF)>>((key[1] - 0x2206F590)&31))|((key[12])<<((0-(key[1] - 0x2206F590))&31)));
 key[15] = (((key[15])<<((key[8] ^ 150)&31))|(((key[15])&0xFFFFFFFF)>>((0-(key[8] ^ 150))&31)));
 if (k == 0)
 {
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>((key[17]*18)&31))|((key[9])<<((0-(key[17]*18))&31))));
  {key[11] += 0xF6B10986 - key[14]; if (key[11] & 1) return 1;};
  {u32 jv = key[5] & ((u8fcos[(key[10])&0xFF]) ? 0xF998E196 : key[10]); key[ 6] ^= (((jv)<<((7)&31))|(((jv)&0xFFFFFFFF)>>((0-(7))&31))), key[ 1] ^= jv - 0x4B327DA, key[ 7] += jv ^ 0x672E5A7, jv ^= (u8fsin[(jv)&0xFF]) ? 0xBC91B04 : key[8], key[11] ^= key[6] & 0xBE53718, jv ^= (u8fcos[(key[2])&0xFF]) ? 0x9DADA8A4 : jv, key[ 0] -= 0x9DADA8A4 & key[6], key[13] += key[1] - 0x7B284744, key[ 3] ^= 20 * key[18], key[ 2] |= jv - 0x313BB22;};
 }
 iv += (u8sqrt[(key[9])&0xFF]);
 key[10] -= (((iv)<<((0x209B4CB8)&31))|(((iv)&0xFFFFFFFF)>>((0-(0x209B4CB8))&31)));
 key[1] += 41 * iv;
 key[9] *= 105 * key[3];
 if (k == 1)
 {
  (key[ 6] += 0x975C61BA - key[8]);
  (key[11] += 0x29CC7F53 - key[5]);
  skype_rc4_iv13 (key, key[0]);
 }
 iv -= 0xFCCEF753 | key[10];
 return iv;
}

u32 skype_rc4_iv15 (u32 * const key, u32 iv)
{
 u32 k = key[16] & 3;

 iv += (((key[10])<<((4)&31))|(((key[10])&0xFFFFFFFF)>>((0-(4))&31)));
 iv = ((((iv)&0xFFFFFFFF)>>(((u8fcos[(key[18])&0xFF]) ? 14 : key[15])&31))|((iv)<<((0-((u8fcos[(key[18])&0xFF]) ? 14 : key[15]))&31)));
 if (k == 3)
 {
  (key[ 9] += 0x3A82007 - key[14]);
  (key[18] -= 122 * key[6]);
  {u32 jv = key[2] + ((u8fsin[(key[7])&0xFF]) ? 0xCC95AFBF : key[9]); key[ 2] += jv + 0xE6ECDA3, key[ 9] &= (u8fsin[(key[7])&0xFF]) ? 0x13D68223 : jv, key[15] += 0x38245913 - key[12], key[16] = ((((key[16])&0xFFFFFFFF)>>((30 * key[17])&31))|((key[16])<<((0-(30 * key[17]))&31))), key[11] += 0x36F87E5B - key[5], key[ 2] += 102 * key[3], jv *= (((key[5])<<((30)&31))|(((key[5])&0xFFFFFFFF)>>((0-(30))&31))), key[ 9] += 123 * jv, key[ 5] = (((key[5])<<((key[16] - 11)&31))|(((key[5])&0xFFFFFFFF)>>((0-(key[16] - 11))&31))), key[10] |= (u8fsin[(key[4])&0xFF]) ? 0x84EDC63 : key[4];};
 }
 iv = (((iv)<<((((((key[6])&0xFFFFFFFF)>>((21)&31))|((key[6])<<((0-(21))&31))))&31))|(((iv)&0xFFFFFFFF)>>((0-(((((key[6])&0xFFFFFFFF)>>((21)&31))|((key[6])<<((0-(21))&31)))))&31)));
 key[17] |= key[11] - 0xBF85104;
 key[17] -= 78 * iv;
 if (k == 1)
 {
  {key[ 1] ^= (key[4] < 0x196D816A) ? key[4] : key[17]; if (key[1] & 1) return 1;};
  (key[ 6] ^= (u8fcos[(key[7])&0xFF]) ? 0x1AB1E599 : key[18]);
  {u32 jv = iv & ((u8fcos[(key[10])&0xFF]) ? 0xF998E196 : key[10]); key[ 6] ^= (((jv)<<((7)&31))|(((jv)&0xFFFFFFFF)>>((0-(7))&31))), key[ 1] ^= jv - 0x4B327DA, key[ 7] += jv ^ 0x672E5A7, jv ^= (u8fsin[(jv)&0xFF]) ? 0xBC91B04 : key[8], key[11] ^= key[6] & 0xBE53718, jv ^= (u8fcos[(key[2])&0xFF]) ? 0x9DADA8A4 : jv, key[ 0] -= 0x9DADA8A4 & key[6], key[13] += key[1] - 0x7B284744, key[ 3] ^= 20 * key[18], key[ 2] |= jv - 0x313BB22;};
 }
 key[15] += (u8fcos[(iv)&0xFF]) ? 0xA82B365 : key[7];
 key[17] -= (u8fcos[(key[11])&0xFF]) ? 0x9AA8540 : iv;
 iv ^= (u8fsin[(key[12])&0xFF]) ? 0x3B018731 : key[12];
 if (k == 2)
 {
  (key[ 9] -= ((((key[16])&0xFFFFFFFF)>>((25)&31))|((key[16])<<((0-(25))&31))));
  {key[ 1] ^= (key[4] < 0x196D816A) ? key[4] : key[17]; if (key[1] & 1) return 1;};
  skype_rc4_iv13 (key, iv);
 }
 key[7] ^= (u8fsin[(key[17])&0xFF]) ? 0x836B3E03 : key[6];
 key[2] ^= key[6] + 0x1E0BBA53;
 key[15] += 0x812EAA9D - key[1];
 if (k == 0)
 {
  (key[ 1] -= key[16] | 0x59C1677);
  (key[ 7] ^= key[10] - 0x3035E544);
  skype_rc4_iv14 (key, key[2]);
 }
 key[1] ^= (key[14] < 0x812EAA9D) ? key[14] : iv;
 return iv;
}

u32 skype_rc4_iv16 (u32 * const key, u32 iv)
{
 u32 k = (key[12] ^ iv ^ key[5]) % 5;

 key[17] = (((key[17])<<(((u8fcos[(iv)&0xFF]) ? 2 : key[19])&31))|(((key[17])&0xFFFFFFFF)>>((0-((u8fcos[(iv)&0xFF]) ? 2 : key[19]))&31)));
 iv -= iv | 0x16E2A7D1;
 if (k == 2)
 {
  (key[ 6] *= 0x1304694A * key[11]);
  (key[ 6] *= 0x1304694A * key[11]);
  {u32 jv = key[0] + ((u8fsin[(key[7])&0xFF]) ? 0xCC95AFBF : key[9]); key[ 2] += jv + 0xE6ECDA3, key[ 9] &= (u8fsin[(key[7])&0xFF]) ? 0x13D68223 : jv, key[15] += 0x38245913 - key[12], key[16] = ((((key[16])&0xFFFFFFFF)>>((30 * key[17])&31))|((key[16])<<((0-(30 * key[17]))&31))), key[11] += 0x36F87E5B - key[5], key[ 2] += 102 * key[3], jv *= (((key[5])<<((30)&31))|(((key[5])&0xFFFFFFFF)>>((0-(30))&31))), key[ 9] += 123 * jv, key[ 5] = (((key[5])<<((key[16] - 11)&31))|(((key[5])&0xFFFFFFFF)>>((0-(key[16] - 11))&31))), key[10] |= (u8fsin[(key[4])&0xFF]) ? 0x84EDC63 : key[4];};
 }
 key[15] *= 118 * iv;
 key[12] = (((key[12])<<((key[10] - 7)&31))|(((key[12])&0xFFFFFFFF)>>((0-(key[10] - 7))&31)));
 if (k == 1)
 {
  (key[15] ^= (key[14] < 0x291B9650) ? key[14] : key[2]);
  (key[16] &= (key[11] < 0x5578A05) ? key[11] : key[16]);
  {u32 jv = key[3] & ((u8fcos[(key[10])&0xFF]) ? 0xF998E196 : key[10]); key[ 6] ^= (((jv)<<((7)&31))|(((jv)&0xFFFFFFFF)>>((0-(7))&31))), key[ 1] ^= jv - 0x4B327DA, key[ 7] += jv ^ 0x672E5A7, jv ^= (u8fsin[(jv)&0xFF]) ? 0xBC91B04 : key[8], key[11] ^= key[6] & 0xBE53718, jv ^= (u8fcos[(key[2])&0xFF]) ? 0x9DADA8A4 : jv, key[ 0] -= 0x9DADA8A4 & key[6], key[13] += key[1] - 0x7B284744, key[ 3] ^= 20 * key[18], key[ 2] |= jv - 0x313BB22;};
 }
 key[2] += (key[13] < 0x588BA7CA) ? key[13] : iv;
 key[19] -= (key[12] < 0x3F5E1B0) ? key[12] : key[9];
 if (k == 3)
 {
  (key[11] ^= key[15] | 0x11273409);
  (key[ 7] -= 0xBFA93FA3);
  skype_rc4_iv13 (key, key[14]);
 }
 iv *= -11;
 key[3] += 76 * key[5];
 if (k == 4)
 {
  (key[11] |= key[4] - 0x224114CD);
  (key[11] += 0x29CC7F53 - key[5]);
  skype_rc4_iv14 (key, key[17]);
 }
 key[11] *= key[3] | 0xA760EE25;
 key[19] += key[12] + 0xA760EE25;
 if (k == 0)
 {
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>((key[17]*18)&31))|((key[9])<<((0-(key[17]*18))&31))));
  (key[ 6] = (((key[6])<<((key[13] - 18)&31))|(((key[6])&0xFFFFFFFF)>>((0-(key[13] - 18))&31))));
  skype_rc4_iv15 (key, key[6]);
 }
 key[15] = ((((key[15])&0xFFFFFFFF)>>(((key[14] < 0x9161D683) ? key[14] : key[9])&31))|((key[15])<<((0-((key[14] < 0x9161D683) ? key[14] : key[9]))&31)));
 return iv;
}

u32 skype_rc4_iv17 (u32 * const key, u32 iv)
{
 u32 k = (key[14] ^ key[2] ^ key[6]) % 6;

 key[1] += (u8fsin[(key[1])&0xFF]) ? 0x1FAE4F0 : key[17];
 if (k == 4)
 {
  (key[ 2] &= ((((0xF37F8F83)&0xFFFFFFFF)>>((17)&31))|((0xF37F8F83)<<((0-(17))&31))));
  (key[ 9] = (((key[9])<<((31)&31))|(((key[9])&0xFFFFFFFF)>>((0-(31))&31))));
  {u32 jv = key[7] + ((u8fsin[(key[7])&0xFF]) ? 0xCC95AFBF : key[9]); key[ 2] += jv + 0xE6ECDA3, key[ 9] &= (u8fsin[(key[7])&0xFF]) ? 0x13D68223 : jv, key[15] += 0x38245913 - key[12], key[16] = ((((key[16])&0xFFFFFFFF)>>((30 * key[17])&31))|((key[16])<<((0-(30 * key[17]))&31))), key[11] += 0x36F87E5B - key[5], key[ 2] += 102 * key[3], jv *= (((key[5])<<((30)&31))|(((key[5])&0xFFFFFFFF)>>((0-(30))&31))), key[ 9] += 123 * jv, key[ 5] = (((key[5])<<((key[16] - 11)&31))|(((key[5])&0xFFFFFFFF)>>((0-(key[16] - 11))&31))), key[10] |= (u8fsin[(key[4])&0xFF]) ? 0x84EDC63 : key[4];};
 }
 key[3] -= (((key[9])<<((5)&31))|(((key[9])&0xFFFFFFFF)>>((0-(5))&31)));
 key[12] *= ((((key[4])&0xFFFFFFFF)>>((19)&31))|((key[4])<<((0-(19))&31)));
 if (k == 3)
 {
  {key[ 0] *= 33 * key[0]; if (key[0] & 1) return 1;};
  (key[12] -= 66 * key[2]);
  {u32 jv = iv & ((u8fcos[(key[10])&0xFF]) ? 0xF998E196 : key[10]); key[ 6] ^= (((jv)<<((7)&31))|(((jv)&0xFFFFFFFF)>>((0-(7))&31))), key[ 1] ^= jv - 0x4B327DA, key[ 7] += jv ^ 0x672E5A7, jv ^= (u8fsin[(jv)&0xFF]) ? 0xBC91B04 : key[8], key[11] ^= key[6] & 0xBE53718, jv ^= (u8fcos[(key[2])&0xFF]) ? 0x9DADA8A4 : jv, key[ 0] -= 0x9DADA8A4 & key[6], key[13] += key[1] - 0x7B284744, key[ 3] ^= 20 * key[18], key[ 2] |= jv - 0x313BB22;};
 }
 iv -= (((iv)<<((18)&31))|(((iv)&0xFFFFFFFF)>>((0-(18))&31)));
 key[19] ^= iv + 0x222E1310;
 if (k == 2)
 {
  {if (key[5] & 1) return 1;};
  (key[ 9] = (((key[9])<<((6)&31))|(((key[9])&0xFFFFFFFF)>>((0-(6))&31))));
  skype_rc4_iv13 (key, key[18]);
 }
 key[17] ^= iv - 0x7FAB735B;
 if (k == 0)
 {
  (key[13] |= (u8fcos[(0xD2430E50)&0xFF]) ? 0x1510A109 : key[14]);
  {key[ 7] += ((((key[7])&0xFFFFFFFF)>>((21)&31))|((key[7])<<((0-(21))&31))); if (key[7] & 1) return 1;};
  skype_rc4_iv14 (key, iv);
 }
 key[14] |= key[1] ^ 0x71B103BD;
 iv = ((((iv)&0xFFFFFFFF)>>((6 * key[16])&31))|((iv)<<((0-(6 * key[16]))&31)));
 if (k == 1)
 {
  (key[11] |= key[4] - 0x224114CD);
  (key[10] = ((((key[10])&0xFFFFFFFF)>>((key[14] + 6)&31))|((key[10])<<((0-(key[14] + 6))&31))));
  skype_rc4_iv15 (key, key[17]);
 }
 key[0] ^= (u8sqrt[(key[15])&0xFF]);
 key[14] = ((((key[14])&0xFFFFFFFF)>>(((u8fcos[(key[15])&0xFF]) ? 8 : key[1])&31))|((key[14])<<((0-((u8fcos[(key[15])&0xFF]) ? 8 : key[1]))&31)));
 if (k == 0)
 {
  (key[12] -= 66 * key[2]);
  (key[ 2] &= key[18] - 0x37CF1A3F);
  skype_rc4_iv16 (key, key[9]);
 }
 key[10] -= (u8fcos[(key[13])&0xFF]) ? 0xF83DD71A : key[10];
 return iv;
}

u32 skype_rc4_iv18 (u32 * const key, u32 iv)
{
 u32 k = key[14] % 7;

 iv -= 31 * key[6];
 if (k == 5)
 {
  {key[ 9] = (((key[9])<<(((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31))))&31))|(((key[9])&0xFFFFFFFF)>>((0-((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31)))))&31))); if (key[9] & 1) return 1;};
  (key[13] ^= (0x3DB5CDC8 < 0x2E0AF4F7) ? 0x3DB5CDC8 : key[15]);
  {u32 jv = key[0] + ((u8fsin[(key[7])&0xFF]) ? 0xCC95AFBF : key[9]); key[ 2] += jv + 0xE6ECDA3, key[ 9] &= (u8fsin[(key[7])&0xFF]) ? 0x13D68223 : jv, key[15] += 0x38245913 - key[12], key[16] = ((((key[16])&0xFFFFFFFF)>>((30 * key[17])&31))|((key[16])<<((0-(30 * key[17]))&31))), key[11] += 0x36F87E5B - key[5], key[ 2] += 102 * key[3], jv *= (((key[5])<<((30)&31))|(((key[5])&0xFFFFFFFF)>>((0-(30))&31))), key[ 9] += 123 * jv, key[ 5] = (((key[5])<<((key[16] - 11)&31))|(((key[5])&0xFFFFFFFF)>>((0-(key[16] - 11))&31))), key[10] |= (u8fsin[(key[4])&0xFF]) ? 0x84EDC63 : key[4];};
 }
 key[14] ^= key[15] - 0x274BB11;
 if (k == 4)
 {
  (key[ 6] = (((key[6])<<((key[13] - 18)&31))|(((key[6])&0xFFFFFFFF)>>((0-(key[13] - 18))&31))));
  (key[ 2] += 0xEA2D3D5D * key[7]);
  {u32 jv = key[12] & ((u8fcos[(key[10])&0xFF]) ? 0xF998E196 : key[10]); key[ 6] ^= (((jv)<<((7)&31))|(((jv)&0xFFFFFFFF)>>((0-(7))&31))), key[ 1] ^= jv - 0x4B327DA, key[ 7] += jv ^ 0x672E5A7, jv ^= (u8fsin[(jv)&0xFF]) ? 0xBC91B04 : key[8], key[11] ^= key[6] & 0xBE53718, jv ^= (u8fcos[(key[2])&0xFF]) ? 0x9DADA8A4 : jv, key[ 0] -= 0x9DADA8A4 & key[6], key[13] += key[1] - 0x7B284744, key[ 3] ^= 20 * key[18], key[ 2] |= jv - 0x313BB22;};
 }
 iv ^= 8 * key[1];
 if (k == 2)
 {
  (key[12] *= (u8fcos[(0x1D06A87F)&0xFF]) ? 0x1734D89C : key[5]);
  (key[11] |= key[4] - 0x224114CD);
  skype_rc4_iv13 (key, key[13]);
 }
 key[16] = (((key[16])<<(((u8sqrt[(key[15])&0xFF]))&31))|(((key[16])&0xFFFFFFFF)>>((0-((u8sqrt[(key[15])&0xFF])))&31)));
 key[19] *= (u8sqrt[(key[10])&0xFF]);
 if (k == 3)
 {
  (key[ 3] ^= key[0] + 0x4376FF7);
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>((key[17]*18)&31))|((key[9])<<((0-(key[17]*18))&31))));
  skype_rc4_iv14 (key, key[4]);
 }
 iv += key[16] | 0x1DF5D7AD;
 if (k == 0)
 {
  (key[14] ^= (u8fcos[(key[9])&0xFF]) ? 0x73CD560C : key[4]);
  (key[11] ^= key[15] | 0x11273409);
  skype_rc4_iv15 (key, key[4]);
 }
 iv *= key[2] + 0xC70282A8;
 if (k == 0)
 {
  (key[ 3] ^= key[0] + 0x4376FF7);
  (key[11] = (((key[11])<<(((u8fcos[(key[5])&0xFF]) ? 19 : key[11])&31))|(((key[11])&0xFFFFFFFF)>>((0-((u8fcos[(key[5])&0xFF]) ? 19 : key[11]))&31))));
  skype_rc4_iv16 (key, key[9]);
 }
 key[6] &= iv - 0x9798A83;
 iv = ((((iv)&0xFFFFFFFF)>>((2 * key[11])&31))|((iv)<<((0-(2 * key[11]))&31)));
 if (k == 1)
 {
  (key[ 9] ^= (u8fcos[(key[11])&0xFF]) ? 0xEEB638B : key[6]);
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>(((u8fcos[(key[9])&0xFF]) ? 20 : key[0])&31))|((key[9])<<((0-((u8fcos[(key[9])&0xFF]) ? 20 : key[0]))&31))));
  skype_rc4_iv17 (key, key[7]);
 }
 key[2] ^= (key[9] < 0x2EEDED9) ? key[9] : key[12];
 return iv;
}

u32 skype_rc4_iv19 (u32 * const key, u32 iv)
{
 u32 k = (key[13] ^ iv ^ key[4]) & 7;

 key[16] *= key[15];
 if (k == 0)
 {
  (key[ 9] += 0x3A82007 - key[14]);
  (key[11] -= key[12] << 5);
  {u32 jv = iv + ((u8fsin[(key[7])&0xFF]) ? 0xCC95AFBF : key[9]); key[ 2] += jv + 0xE6ECDA3, key[ 9] &= (u8fsin[(key[7])&0xFF]) ? 0x13D68223 : jv, key[15] += 0x38245913 - key[12], key[16] = ((((key[16])&0xFFFFFFFF)>>((30 * key[17])&31))|((key[16])<<((0-(30 * key[17]))&31))), key[11] += 0x36F87E5B - key[5], key[ 2] += 102 * key[3], jv *= (((key[5])<<((30)&31))|(((key[5])&0xFFFFFFFF)>>((0-(30))&31))), key[ 9] += 123 * jv, key[ 5] = (((key[5])<<((key[16] - 11)&31))|(((key[5])&0xFFFFFFFF)>>((0-(key[16] - 11))&31))), key[10] |= (u8fsin[(key[4])&0xFF]) ? 0x84EDC63 : key[4];};
 }
 key[14] = ((((key[14])&0xFFFFFFFF)>>((87 * key[11])&31))|((key[14])<<((0-(87 * key[11]))&31)));
 if (k == 2)
 {
  (key[11] ^= key[15] | 0x11273409);
  {key[16] &= (key[12] < 0x28165E7B) ? key[12] : 0x73492C37; if (key[16] & 1) return 1;};
  {u32 jv = key[16] & ((u8fcos[(key[10])&0xFF]) ? 0xF998E196 : key[10]); key[ 6] ^= (((jv)<<((7)&31))|(((jv)&0xFFFFFFFF)>>((0-(7))&31))), key[ 1] ^= jv - 0x4B327DA, key[ 7] += jv ^ 0x672E5A7, jv ^= (u8fsin[(jv)&0xFF]) ? 0xBC91B04 : key[8], key[11] ^= key[6] & 0xBE53718, jv ^= (u8fcos[(key[2])&0xFF]) ? 0x9DADA8A4 : jv, key[ 0] -= 0x9DADA8A4 & key[6], key[13] += key[1] - 0x7B284744, key[ 3] ^= 20 * key[18], key[ 2] |= jv - 0x313BB22;};
 }
 iv &= iv ^ 0xBFEECFFE;
 if (k == 6)
 {
  (key[ 2] += 0xEA2D3D5D * key[7]);
  {key[16] &= (key[12] < 0x28165E7B) ? key[12] : 0x7FE25F6B; if (key[16] & 1) return 1;};
  skype_rc4_iv13 (key, key[7]);
 }
 iv += (u8fcos[(iv)&0xFF]) ? 0xD68DC95 : key[5];
 key[2] ^= (u8fsin[(key[7])&0xFF]) ? 0x2124BD5 : iv;
 if (k == 4)
 {
  (key[ 4] ^= 17 * key[0]);
  {key[ 1] ^= (u8fcos[(key[3])&0xFF]) ? 0x7C23395 : key[18]; if (key[1] & 1) return 1;};
  skype_rc4_iv14 (key, key[5]);
 }
 key[2] ^= (iv < 0xA427B60) ? iv : key[19];
 if (k == 0)
 {
  (key[10] ^= key[7] - 0x354C1FF2);
  (key[ 6] ^= ((((key[14])&0xFFFFFFFF)>>((28)&31))|((key[14])<<((0-(28))&31))));
  skype_rc4_iv15 (key, iv);
 }
 key[12] -= (u8sqrt[(iv)&0xFF]);
 if (k == 1)
 {
  (key[11] |= key[4] - 0x224114CD);
  (key[17] ^= 0x2F961 * key[4]);
  skype_rc4_iv16 (key, key[15]);
 }
 key[16] -= 0x61A83016 | key[4];
 if (k == 3)
 {
  (key[10] ^= key[7] - 0x354C1FF2);
  (key[11] |= key[4] - 0x224114CD);
  skype_rc4_iv17 (key, iv);
 }
 iv *= (u8sqrt[(iv)&0xFF]);
 key[13] *= 0x59F1E662 + key[16];
 if (k == 7)
 {
  {key[ 1] ^= (u8fcos[(key[3])&0xFF]) ? 0x7C23395 : key[18]; if (key[1] & 1) return 1;};
  (key[ 3] += key[9] + 0x48210C78);
  skype_rc4_iv18 (key, key[19]);
 }
 key[3] ^= 0x699B23 * key[12];
 return iv;
}

u32 skype_rc4_iv20 (u32 * const key, u32 iv)
{
 u32 k = (iv ^ key[7] ^ key[12]) % 9;

 iv = ((((iv)&0xFFFFFFFF)>>(((u8sqrt[(iv)&0xFF]))&31))|((iv)<<((0-((u8sqrt[(iv)&0xFF])))&31)));
 if (k == 8)
 {
  (key[16] ^= (key[18] < 0xE87F32) ? key[18] : key[11]);
  {key[10] = (((key[10])<<((0x141586A -6)&31))|(((key[10])&0xFFFFFFFF)>>((0-(0x141586A -6))&31))); if (key[10] & 1) return 1;};
  {u32 jv = key[15] + ((u8fsin[(key[7])&0xFF]) ? 0xCC95AFBF : key[9]); key[ 2] += jv + 0xE6ECDA3, key[ 9] &= (u8fsin[(key[7])&0xFF]) ? 0x13D68223 : jv, key[15] += 0x38245913 - key[12], key[16] = ((((key[16])&0xFFFFFFFF)>>((30 * key[17])&31))|((key[16])<<((0-(30 * key[17]))&31))), key[11] += 0x36F87E5B - key[5], key[ 2] += 102 * key[3], jv *= (((key[5])<<((30)&31))|(((key[5])&0xFFFFFFFF)>>((0-(30))&31))), key[ 9] += 123 * jv, key[ 5] = (((key[5])<<((key[16] - 11)&31))|(((key[5])&0xFFFFFFFF)>>((0-(key[16] - 11))&31))), key[10] |= (u8fsin[(key[4])&0xFF]) ? 0x84EDC63 : key[4];};
 }
 key[4] += 15 * key[16];
 if (k == 0)
 {
  (key[ 0] = (((key[0])<<(((0x1CB835FD>>17)&0x1F)&31))|(((key[0])&0xFFFFFFFF)>>((0-((0x1CB835FD>>17)&0x1F))&31))));
  (key[12] = ((((key[12])&0xFFFFFFFF)>>((key[18] ^ 9)&31))|((key[12])<<((0-(key[18] ^ 9))&31))));
  {u32 jv = key[16] & ((u8fcos[(key[10])&0xFF]) ? 0xF998E196 : key[10]); key[ 6] ^= (((jv)<<((7)&31))|(((jv)&0xFFFFFFFF)>>((0-(7))&31))), key[ 1] ^= jv - 0x4B327DA, key[ 7] += jv ^ 0x672E5A7, jv ^= (u8fsin[(jv)&0xFF]) ? 0xBC91B04 : key[8], key[11] ^= key[6] & 0xBE53718, jv ^= (u8fcos[(key[2])&0xFF]) ? 0x9DADA8A4 : jv, key[ 0] -= 0x9DADA8A4 & key[6], key[13] += key[1] - 0x7B284744, key[ 3] ^= 20 * key[18], key[ 2] |= jv - 0x313BB22;};
 }
 key[10] ^= ((((key[15])&0xFFFFFFFF)>>((30)&31))|((key[15])<<((0-(30))&31)));
 if (k == 1)
 {
  (key[ 2] += 0xEA2D3D5D * key[7]);
  (key[ 1] -= key[16] | 0x59C1677);
  skype_rc4_iv13 (key, iv);
 }
 key[7] -= (u8fsin[(iv)&0xFF]) ? 0x0D93E92C : key[16];
 if (k == 2)
 {
  {key[16] &= (key[12] < 0x28165E7B) ? key[12] : 0x66E0FF5C; if (key[16] & 1) return 1;};
  (key[ 7] ^= (u8sqrt[(key[15])&0xFF]));
  skype_rc4_iv14 (key, key[17]);
 }
 key[6] *= ((((key[10])&0xFFFFFFFF)>>((7)&31))|((key[10])<<((0-(7))&31)));
 if (k == 6)
 {
  (key[ 3] += key[9] + 0x48210C78);
  (key[11] += 0x29CC7F53 - key[5]);
  skype_rc4_iv15 (key, key[14]);
 }
 iv -= 90 * key[16];
 if (k == 4)
 {
  {key[ 9] = (((key[9])<<(((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31))))&31))|(((key[9])&0xFFFFFFFF)>>((0-((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31)))))&31))); if (key[9] & 1) return 1;};
  (key[9 ] *= (u8fsin[(0x7C7D7E42)&0xFF]) ? 0x28D781D2 : key[10]);
  skype_rc4_iv16 (key, key[4]);
 }
 key[3] -= key[16] & 0xEFF9DB02;
 if (k == 0)
 {
  (key[ 3] += key[9] + 0x48210C78);
  (key[ 6] ^= ((((key[14])&0xFFFFFFFF)>>((28)&31))|((key[14])<<((0-(28))&31))));
  skype_rc4_iv17 (key, key[13]);
 }
 iv += (u8fcos[(key[19])&0xFF]) ? 0x39AEBDF : key[5];
 if (k == 7)
 {
  (key[ 0] = (((key[0])<<((8 * key[18])&31))|(((key[0])&0xFFFFFFFF)>>((0-(8 * key[18]))&31))));
  (key[ 7] ^= (u8sqrt[(key[15])&0xFF]));
  skype_rc4_iv18 (key, key[6]);
 }
 key[13] |= (u8sqrt[(key[10])&0xFF]);
 if (k == 5)
 {
  {key[ 7] += ((((key[7])&0xFFFFFFFF)>>((21)&31))|((key[7])<<((0-(21))&31))); if (key[7] & 1) return 1;};
  {key[16] &= (key[12] < 0x28165E7B) ? key[12] : 0x54AE62D0; if (key[16] & 1) return 1;};
  skype_rc4_iv19 (key, key[14]);
 }
 iv = (((iv)<<((26 * iv)&31))|(((iv)&0xFFFFFFFF)>>((0-(26 * iv))&31)));
 key[14] -= (u8fsin[(key[1])&0xFF]) ? 0xB95DC2D3 : key[18];
 return iv;
}

u32 skype_rc4_iv21 (u32 * const key, u32 iv)
{
 u32 k = (key[17] ^ key[16] ^ key[7]) % 10;

 key[15] -= 13 * key[7];
 if (k == 2)
 {
  {key[ 0] *= 33 * key[0]; if (key[0] & 1) return 1;};
  (key[11] = (((key[11])<<(((u8fcos[(key[5])&0xFF]) ? 19 : key[11])&31))|(((key[11])&0xFFFFFFFF)>>((0-((u8fcos[(key[5])&0xFF]) ? 19 : key[11]))&31))));
  {u32 jv = key[14] + ((u8fsin[(key[7])&0xFF]) ? 0xCC95AFBF : key[9]); key[ 2] += jv + 0xE6ECDA3, key[ 9] &= (u8fsin[(key[7])&0xFF]) ? 0x13D68223 : jv, key[15] += 0x38245913 - key[12], key[16] = ((((key[16])&0xFFFFFFFF)>>((30 * key[17])&31))|((key[16])<<((0-(30 * key[17]))&31))), key[11] += 0x36F87E5B - key[5], key[ 2] += 102 * key[3], jv *= (((key[5])<<((30)&31))|(((key[5])&0xFFFFFFFF)>>((0-(30))&31))), key[ 9] += 123 * jv, key[ 5] = (((key[5])<<((key[16] - 11)&31))|(((key[5])&0xFFFFFFFF)>>((0-(key[16] - 11))&31))), key[10] |= (u8fsin[(key[4])&0xFF]) ? 0x84EDC63 : key[4];};
 }
 key[7] += key[5] ^ 0x28CA4358;
 if (k == 1)
 {
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>(((u8fcos[(key[9])&0xFF]) ? 20 : key[0])&31))|((key[9])<<((0-((u8fcos[(key[9])&0xFF]) ? 20 : key[0]))&31))));
  (key[ 2] -= (u8fsin[(0xC14393E2)&0xFF]) ? 0x73423C3 : key[7]);
  {u32 jv = key[10] & ((u8fcos[(key[10])&0xFF]) ? 0xF998E196 : key[10]); key[ 6] ^= (((jv)<<((7)&31))|(((jv)&0xFFFFFFFF)>>((0-(7))&31))), key[ 1] ^= jv - 0x4B327DA, key[ 7] += jv ^ 0x672E5A7, jv ^= (u8fsin[(jv)&0xFF]) ? 0xBC91B04 : key[8], key[11] ^= key[6] & 0xBE53718, jv ^= (u8fcos[(key[2])&0xFF]) ? 0x9DADA8A4 : jv, key[ 0] -= 0x9DADA8A4 & key[6], key[13] += key[1] - 0x7B284744, key[ 3] ^= 20 * key[18], key[ 2] |= jv - 0x313BB22;};
 }
 key[10] += iv & 0xE9F1006;
 if (k == 9)
 {
  (key[ 2] &= ((((0x3C50D16)&0xFFFFFFFF)>>((17)&31))|((0x3C50D16)<<((0-(17))&31))));
  (key[17] += key[13] - 0x292C1156);
  skype_rc4_iv13 (key, iv);
 }
 key[19] *= (key[0] < 0xB71D9592) ? key[0] : key[14];
 if (k == 5)
 {
  (key[ 0] = (((key[0])<<(((0x6481D348>>17)&0x1F)&31))|(((key[0])&0xFFFFFFFF)>>((0-((0x6481D348>>17)&0x1F))&31))));
  (key[11] = (((key[11])<<(((u8fcos[(key[5])&0xFF]) ? 19 : key[11])&31))|(((key[11])&0xFFFFFFFF)>>((0-((u8fcos[(key[5])&0xFF]) ? 19 : key[11]))&31))));
  skype_rc4_iv14 (key, iv);
 }
 key[16] += 0x7C476077 - key[0];
 if (k == 4)
 {
  {key[ 1] ^= (key[4] < 0x196D816A) ? key[4] : key[17]; if (key[1] & 1) return 1;};
  (key[14] ^= (u8fcos[(key[9])&0xFF]) ? 0x73CD560C : key[4]);
  skype_rc4_iv15 (key, key[10]);
 }
 iv ^= key[10] << 6;
 if (k == 0)
 {
  {key[ 7] += ((((key[7])&0xFFFFFFFF)>>((21)&31))|((key[7])<<((0-(21))&31))); if (key[7] & 1) return 1;};
  (key[ 1] -= key[16] | 0x59C1677);
  skype_rc4_iv16 (key, key[4]);
 }
 key[3] += (u8fsin[(key[12])&0xFF]) ? 0x7B0CA6E0 : iv;
 if (k == 6)
 {
  (key[ 6] += (0xFCD29227 < 0x61F0BAA) ? 0xFCD29227 : key[16]);
  (key[12] *= (u8fcos[(0x3FFF3F1E)&0xFF]) ? 0x1734D89C : key[5]);
  skype_rc4_iv17 (key, key[2]);
 }
 iv |= 0x2FF561CA * iv;
 if (k == 8)
 {
  (key[ 1] ^= (((0x5BCC4FBE)<<((8)&31))|(((0x5BCC4FBE)&0xFFFFFFFF)>>((0-(8))&31))));
  (key[12] *= (u8fcos[(0x8328D41F)&0xFF]) ? 0x1734D89C : key[5]);
  skype_rc4_iv18 (key, key[3]);
 }
 key[14] += ((((key[0])&0xFFFFFFFF)>>((26)&31))|((key[0])<<((0-(26))&31)));
 if (k == 3)
 {
  {key[16] &= (key[12] < 0x28165E7B) ? key[12] : 0x7F1F0A41; if (key[16] & 1) return 1;};
  (key[ 7] ^= key[10] - 0x3035E544);
  skype_rc4_iv19 (key, iv);
 }
 key[14] -= (key[6] < 0x2C23ABF1) ? key[6] : key[19];
 if (k == 7)
 {
  (key[12] = ((((key[12])&0xFFFFFFFF)>>((key[18] ^ 9)&31))|((key[12])<<((0-(key[18] ^ 9))&31))));
  (key[13] ^= (0x302EE99C < 0x2E0AF4F7) ? 0x302EE99C : key[15]);
  skype_rc4_iv20 (key, key[17]);
 }
 key[4] &= 14 * key[11];
 return iv;
}

u32 skype_rc4_iv22 (u32 * const key, u32 iv)
{
 u32 k = (iv ^ key[2] ^ key[6]) % 11;

 key[17] = ((((key[17])&0xFFFFFFFF)>>((iv - 7)&31))|((key[17])<<((0-(iv - 7))&31)));
 if (k == 8)
 {
  (key[9 ] *= (u8fsin[(0x74FA76D5)&0xFF]) ? 0x28D781D2 : key[10]);
  (key[ 1] ^= (((0x6D3C052C)<<((8)&31))|(((0x6D3C052C)&0xFFFFFFFF)>>((0-(8))&31))));
  {u32 jv = key[19] + ((u8fsin[(key[7])&0xFF]) ? 0xCC95AFBF : key[9]); key[ 2] += jv + 0xE6ECDA3, key[ 9] &= (u8fsin[(key[7])&0xFF]) ? 0x13D68223 : jv, key[15] += 0x38245913 - key[12], key[16] = ((((key[16])&0xFFFFFFFF)>>((30 * key[17])&31))|((key[16])<<((0-(30 * key[17]))&31))), key[11] += 0x36F87E5B - key[5], key[ 2] += 102 * key[3], jv *= (((key[5])<<((30)&31))|(((key[5])&0xFFFFFFFF)>>((0-(30))&31))), key[ 9] += 123 * jv, key[ 5] = (((key[5])<<((key[16] - 11)&31))|(((key[5])&0xFFFFFFFF)>>((0-(key[16] - 11))&31))), key[10] |= (u8fsin[(key[4])&0xFF]) ? 0x84EDC63 : key[4];};
 }
 key[16] -= 0xF9263500 & key[8];
 if (k == 0)
 {
  (key[ 6] ^= ((((key[14])&0xFFFFFFFF)>>((28)&31))|((key[14])<<((0-(28))&31))));
  (key[11] += 0x29CC7F53 - key[5]);
  {u32 jv = key[14] & ((u8fcos[(key[10])&0xFF]) ? 0xF998E196 : key[10]); key[ 6] ^= (((jv)<<((7)&31))|(((jv)&0xFFFFFFFF)>>((0-(7))&31))), key[ 1] ^= jv - 0x4B327DA, key[ 7] += jv ^ 0x672E5A7, jv ^= (u8fsin[(jv)&0xFF]) ? 0xBC91B04 : key[8], key[11] ^= key[6] & 0xBE53718, jv ^= (u8fcos[(key[2])&0xFF]) ? 0x9DADA8A4 : jv, key[ 0] -= 0x9DADA8A4 & key[6], key[13] += key[1] - 0x7B284744, key[ 3] ^= 20 * key[18], key[ 2] |= jv - 0x313BB22;};
 }
 iv += (key[9] ^ 0x32C5F54);
 if (k == 1)
 {
  (key[12] *= (u8fcos[(0x187AA563)&0xFF]) ? 0x1734D89C : key[5]);
  (key[ 4] ^= key[19] - 0x303D46FE);
  skype_rc4_iv13 (key, iv);
 }
 key[15] += (key[4] < 0x763BB14) ? key[4] : iv;
 if (k == 6)
 {
  {key[ 0] *= 33 * key[0]; if (key[0] & 1) return 1;};
  (key[12] = ((((key[12])&0xFFFFFFFF)>>((key[18] ^ 9)&31))|((key[12])<<((0-(key[18] ^ 9))&31))));
  skype_rc4_iv14 (key, key[18]);
 }
 key[4] += key[11] + 0x1B2FADB0;
 if (k == 10)
 {
  (key[ 7] ^= key[10] - 0x3035E544);
  {key[ 1] ^= (u8fcos[(key[3])&0xFF]) ? 0x7C23395 : key[18]; if (key[1] & 1) return 1;};
  skype_rc4_iv15 (key, key[14]);
 }
 iv &= key[3] & 0x4E081541;
 if (k == 9)
 {
  (key[11] += 0x29CC7F53 - key[5]);
  (key[13] |= (u8fcos[(0xD89C09D3)&0xFF]) ? 0x1510A109 : key[14]);
  skype_rc4_iv16 (key, key[14]);
 }
 key[18] ^= (iv < 0x22E75CE0) ? iv : key[3];
 if (k == 4)
 {
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>((key[17]*18)&31))|((key[9])<<((0-(key[17]*18))&31))));
  (key[17] ^= 0x682AB283);
  skype_rc4_iv17 (key, iv);
 }
 iv |= (((key[14])<<((102)&31))|(((key[14])&0xFFFFFFFF)>>((0-(102))&31)));
 if (k == 3)
 {
  (key[8 ] ^= (u8fcos[(key[17])&0xFF]) ? 0x3544CA5E : key[8]);
  (key[11] ^= key[15] | 0x11273409);
  skype_rc4_iv18 (key, iv);
 }
 key[3] += ((((iv)&0xFFFFFFFF)>>((19)&31))|((iv)<<((0-(19))&31)));
 if (k == 2)
 {
  (key[17] ^= 0x2F961 * key[4]);
  (key[13] ^= (0x33822AB7 < 0x2E0AF4F7) ? 0x33822AB7 : key[15]);
  skype_rc4_iv19 (key, iv);
 }
 key[9] ^= 52 * key[7];
 if (k == 5)
 {
  (key[14] ^= (u8fcos[(key[9])&0xFF]) ? 0x73CD560C : key[4]);
  (key[ 0] = (((key[0])<<(((0xD0D19C61>>17)&0x1F)&31))|(((key[0])&0xFFFFFFFF)>>((0-((0xD0D19C61>>17)&0x1F))&31))));
  skype_rc4_iv20 (key, key[1]);
 }
 iv *= 0xB1DB36B2 * key[0];
 if (k == 0)
 {
  (key[16] &= (key[11] < 0x5578A05) ? key[11] : key[16]);
  (key[15] -= (u8fcos[(0xCE82C1C)&0xFF]) ? 0x344432F : key[18]);
  skype_rc4_iv21 (key, key[17]);
 }
 key[10] *= (key[17] < 0xD066ADA) ? key[17] : iv;
 return iv;
}

u32 skype_rc4_iv23 (u32 * const key, u32 iv)
{
 u32 k = (key[6] ^ iv ^ key[5]) % 12;

 if (k == 2)
 {
  {key[11] += 0xF6B10986 - key[14]; if (key[11] & 1) return 1;};
  (key[16] &= (key[11] < 0x5578A05) ? key[11] : key[16]);
  {u32 jv = key[11] + ((u8fsin[(key[7])&0xFF]) ? 0xCC95AFBF : key[9]); key[ 2] += jv + 0xE6ECDA3, key[ 9] &= (u8fsin[(key[7])&0xFF]) ? 0x13D68223 : jv, key[15] += 0x38245913 - key[12], key[16] = ((((key[16])&0xFFFFFFFF)>>((30 * key[17])&31))|((key[16])<<((0-(30 * key[17]))&31))), key[11] += 0x36F87E5B - key[5], key[ 2] += 102 * key[3], jv *= (((key[5])<<((30)&31))|(((key[5])&0xFFFFFFFF)>>((0-(30))&31))), key[ 9] += 123 * jv, key[ 5] = (((key[5])<<((key[16] - 11)&31))|(((key[5])&0xFFFFFFFF)>>((0-(key[16] - 11))&31))), key[10] |= (u8fsin[(key[4])&0xFF]) ? 0x84EDC63 : key[4];};
 }
 key[17] = (((key[17])<<((key[5] - 0x2E74D55C)&31))|(((key[17])&0xFFFFFFFF)>>((0-(key[5] - 0x2E74D55C))&31)));
 if (k == 9)
 {
  (key[11] &= 0x5A1FF296);
  {key[10] = (((key[10])<<((0x0EC7C0E9 -6)&31))|(((key[10])&0xFFFFFFFF)>>((0-(0x0EC7C0E9 -6))&31))); if (key[10] & 1) return 1;};
  {u32 jv = key[10] & ((u8fcos[(key[10])&0xFF]) ? 0xF998E196 : key[10]); key[ 6] ^= (((jv)<<((7)&31))|(((jv)&0xFFFFFFFF)>>((0-(7))&31))), key[ 1] ^= jv - 0x4B327DA, key[ 7] += jv ^ 0x672E5A7, jv ^= (u8fsin[(jv)&0xFF]) ? 0xBC91B04 : key[8], key[11] ^= key[6] & 0xBE53718, jv ^= (u8fcos[(key[2])&0xFF]) ? 0x9DADA8A4 : jv, key[ 0] -= 0x9DADA8A4 & key[6], key[13] += key[1] - 0x7B284744, key[ 3] ^= 20 * key[18], key[ 2] |= jv - 0x313BB22;};
 }
 iv = (((iv)<<((0x3FCB120B + key[2])&31))|(((iv)&0xFFFFFFFF)>>((0-(0x3FCB120B + key[2]))&31)));
 if (k == 11)
 {
  (key[ 6] += (0xAF5BAF57 < 0x61F0BAA) ? 0xAF5BAF57 : key[16]);
  {key[ 3] -= key[17] | 0x2433636; if (key[3] & 1) return 1;};
  skype_rc4_iv13 (key, iv);
 }
 key[14] |= 0x3FC983D0 + iv;
 if (k == 8)
 {
  (key[ 1] ^= (((0x59806A89)<<((8)&31))|(((0x59806A89)&0xFFFFFFFF)>>((0-(8))&31))));
  {if (key[5] & 1) return 1;};
  skype_rc4_iv14 (key, key[2]);
 }
 if (k == 10)
 {
  (key[16] = ((((key[16])&0xFFFFFFFF)>>((key[10] - 11)&31))|((key[16])<<((0-(key[10] - 11))&31))));
  {key[ 9] = (((key[9])<<(((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31))))&31))|(((key[9])&0xFFFFFFFF)>>((0-((((key[1])<<((14)&31))|(((key[1])&0xFFFFFFFF)>>((0-(14))&31)))))&31))); if (key[9] & 1) return 1;};
  skype_rc4_iv15 (key, key[2]);
 }
 key[10] = (((key[10])<<(((u8fcos[(key[11])&0xFF]) ? 0x3FC983D0 : key[11])&31))|(((key[10])&0xFFFFFFFF)>>((0-((u8fcos[(key[11])&0xFF]) ? 0x3FC983D0 : key[11]))&31)));
 if (k == 0)
 {
  (key[ 1] ^= (((0x50F929B8)<<((8)&31))|(((0x50F929B8)&0xFFFFFFFF)>>((0-(8))&31))));
  {key[ 1] ^= (key[4] < 0x196D816A) ? key[4] : key[17]; if (key[1] & 1) return 1;};
  skype_rc4_iv16 (key, iv);
 }
 key[0] += key[2] + 0x56775D25;
 if (k == 6)
 {
  (key[ 9] = (((key[9])<<((7)&31))|(((key[9])&0xFFFFFFFF)>>((0-(7))&31))));
  (key[8 ] ^= (u8fcos[(key[17])&0xFF]) ? 0x3544CA5E : key[8]);
  skype_rc4_iv17 (key, iv);
 }
 iv |= 123 * key[9];
 if (k == 1)
 {
  (key[15] ^= (key[14] < 0x291B9650) ? key[14] : key[2]);
  (key[ 6] += 0x975C61BA - key[8]);
  skype_rc4_iv18 (key, key[6]);
 }
 if (k == 5)
 {
  (key[13] |= (u8fcos[(0x693CB362)&0xFF]) ? 0x1510A109 : key[14]);
  {if (key[5] & 1) return 1;};
  skype_rc4_iv19 (key, key[0]);
 }
 key[8] = ((((key[8])&0xFFFFFFFF)>>(((iv < 0x48821AB) ? iv : key[8])&31))|((key[8])<<((0-((iv < 0x48821AB) ? iv : key[8]))&31)));
 if (k == 0)
 {
  {key[11] += 0xF6B10986 - key[14]; if (key[11] & 1) return 1;};
  (key[ 1] ^= (((0x56278C42)<<((8)&31))|(((0x56278C42)&0xFFFFFFFF)>>((0-(8))&31))));
  skype_rc4_iv20 (key, key[10]);
 }
 key[12] ^= 48 * key[18];
 if (k == 3)
 {
  (key[ 7] ^= (u8sqrt[(key[15])&0xFF]));
  (key[ 0] = (((key[0])<<(((0xD061FA2F>>17)&0x1F)&31))|(((key[0])&0xFFFFFFFF)>>((0-((0xD061FA2F>>17)&0x1F))&31))));
  skype_rc4_iv21 (key, key[17]);
 }
 key[18] |= 58 * key[4];
 if (k == 7)
 {
  (key[15] ^= (key[14] < 0x291B9650) ? key[14] : key[2]);
  {key[ 1] ^= (key[4] < 0x196D816A) ? key[4] : key[17]; if (key[1] & 1) return 1;};
  skype_rc4_iv22 (key, key[5]);
 }
 if (k == 4)
 {
  (key[ 4] ^= 17 * key[0]);
  (key[12] -= 66 * key[2]);
  {u32 jv = key[4] + ((u8fsin[(key[7])&0xFF]) ? 0xCC95AFBF : key[9]); key[ 2] += jv + 0xE6ECDA3, key[ 9] &= (u8fsin[(key[7])&0xFF]) ? 0x13D68223 : jv, key[15] += 0x38245913 - key[12], key[16] = ((((key[16])&0xFFFFFFFF)>>((30 * key[17])&31))|((key[16])<<((0-(30 * key[17]))&31))), key[11] += 0x36F87E5B - key[5], key[ 2] += 102 * key[3], jv *= (((key[5])<<((30)&31))|(((key[5])&0xFFFFFFFF)>>((0-(30))&31))), key[ 9] += 123 * jv, key[ 5] = (((key[5])<<((key[16] - 11)&31))|(((key[5])&0xFFFFFFFF)>>((0-(key[16] - 11))&31))), key[10] |= (u8fsin[(key[4])&0xFF]) ? 0x84EDC63 : key[4];};
 }
 key[5] ^= (u8fcos[(key[8])&0xFF]) ? 0x16925B88 : key[8];
 return iv;
}

u32 skype_rc4_iv24 (u32 * const key, u32 iv)
{
 u32 k = (iv ^ key[8] ^ key[11]) % 13;

 if (k == 7)
 {
  (key[11] ^= key[15] | 0x11273409);
  (key[12] = ((((key[12])&0xFFFFFFFF)>>((key[18] ^ 9)&31))|((key[12])<<((0-(key[18] ^ 9))&31))));
  {u32 jv = iv & ((u8fcos[(key[10])&0xFF]) ? 0xF998E196 : key[10]); key[ 6] ^= (((jv)<<((7)&31))|(((jv)&0xFFFFFFFF)>>((0-(7))&31))), key[ 1] ^= jv - 0x4B327DA, key[ 7] += jv ^ 0x672E5A7, jv ^= (u8fsin[(jv)&0xFF]) ? 0xBC91B04 : key[8], key[11] ^= key[6] & 0xBE53718, jv ^= (u8fcos[(key[2])&0xFF]) ? 0x9DADA8A4 : jv, key[ 0] -= 0x9DADA8A4 & key[6], key[13] += key[1] - 0x7B284744, key[ 3] ^= 20 * key[18], key[ 2] |= jv - 0x313BB22;};
 }
 key[2] += (u8fsin[(key[16])&0xFF]) ? 0xBE51568 : key[17];
 if (k == 0)
 {
  (key[ 2] ^= key[15] + 0x57CE331);
  (key[10] ^= key[7] - 0x354C1FF2);
  skype_rc4_iv13 (key, key[0]);
 }
 iv -= (key[15] < 0x214D38C3) ? key[15] : key[14];
 if (k == 6)
 {
  (key[ 5] *= (((0xE50F49F3)<<((3)&31))|(((0xE50F49F3)&0xFFFFFFFF)>>((0-(3))&31))));
  (key[16] &= (key[11] < 0x5578A05) ? key[11] : key[16]);
  skype_rc4_iv14 (key, key[9]);
 }
 iv += 0x20F1E8E5 - key[17];
 if (k == 1)
 {
  (key[11] += 0x29CC7F53 - key[5]);
  (key[ 0] = (((key[0])<<(((0x91B25DAB>>17)&0x1F)&31))|(((key[0])&0xFFFFFFFF)>>((0-((0x91B25DAB>>17)&0x1F))&31))));
  skype_rc4_iv15 (key, key[19]);
 }
 iv *= 113;
 if (k == 5)
 {
  (key[16] &= (key[11] < 0x5578A05) ? key[11] : key[16]);
  (key[11] -= key[12] << 5);
  skype_rc4_iv16 (key, key[5]);
 }
 key[4] |= iv - 0x0D896A46;
 if (k == 11)
 {
  (key[12] = ((((key[12])&0xFFFFFFFF)>>((key[18] ^ 9)&31))|((key[12])<<((0-(key[18] ^ 9))&31))));
  {key[ 1] ^= (key[4] < 0x196D816A) ? key[4] : key[17]; if (key[1] & 1) return 1;};
  skype_rc4_iv17 (key, key[2]);
 }
 if (k == 10)
 {
  (key[18] ^= key[10] + 0x1EE65B0C);
  (key[13] |= (u8fcos[(0x2DE98C73)&0xFF]) ? 0x1510A109 : key[14]);
  skype_rc4_iv18 (key, key[16]);
 }
 key[19] = (((key[19])<<(((u8fcos[(key[4])&0xFF]) ? 0x0D896A46 : key[4])&31))|(((key[19])&0xFFFFFFFF)>>((0-((u8fcos[(key[4])&0xFF]) ? 0x0D896A46 : key[4]))&31)));
 if (k == 12)
 {
  {key[ 1] ^= (u8fcos[(key[3])&0xFF]) ? 0x7C23395 : key[18]; if (key[1] & 1) return 1;};
  (key[11] &= 0x054BD742);
  skype_rc4_iv19 (key, key[9]);
 }
 key[5] = ((((key[5])&0xFFFFFFFF)>>((9 * key[15])&31))|((key[5])<<((0-(9 * key[15]))&31)));
 if (k == 0)
 {
  (key[17] += key[13] - 0x292C1156);
  (key[ 1] ^= (((0x57B2250F)<<((8)&31))|(((0x57B2250F)&0xFFFFFFFF)>>((0-(8))&31))));
  skype_rc4_iv20 (key, key[12]);
 }
 iv = (((iv)<<((key[14] + 8)&31))|(((iv)&0xFFFFFFFF)>>((0-(key[14] + 8))&31)));
 if (k == 4)
 {
  (key[ 4] ^= key[19] - 0x303D46FE);
  (key[11] |= key[4] - 0x224114CD);
  skype_rc4_iv21 (key, key[1]);
 }
 key[10] ^= key[12] ^ 0x17E47765;
 if (k == 3)
 {
  {key[ 7] += ((((key[7])&0xFFFFFFFF)>>((21)&31))|((key[7])<<((0-(21))&31))); if (key[7] & 1) return 1;};
  (key[ 7] ^= key[10] - 0x3035E544);
  skype_rc4_iv22 (key, iv);
 }
 key[1] ^= 58 * iv;
 if (k == 2)
 {
  (key[ 2] &= key[18] - 0x37CF1A3F);
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>(((u8fcos[(key[9])&0xFF]) ? 20 : key[0])&31))|((key[9])<<((0-((u8fcos[(key[9])&0xFF]) ? 20 : key[0]))&31))));
  skype_rc4_iv23 (key, key[16]);
 }
 iv *= 77 * iv;
 if (k == 8)
 {
  (key[ 9] ^= (u8fcos[(key[11])&0xFF]) ? 0xEEB638B : key[6]);
  (key[ 9] = ((((key[9])&0xFFFFFFFF)>>(((u8fcos[(key[9])&0xFF]) ? 20 : key[0])&31))|((key[9])<<((0-((u8fcos[(key[9])&0xFF]) ? 20 : key[0]))&31))));
  {u32 jv = iv & ((u8fcos[(key[10])&0xFF]) ? 0xF998E196 : key[10]); key[ 6] ^= (((jv)<<((7)&31))|(((jv)&0xFFFFFFFF)>>((0-(7))&31))), key[ 1] ^= jv - 0x4B327DA, key[ 7] += jv ^ 0x672E5A7, jv ^= (u8fsin[(jv)&0xFF]) ? 0xBC91B04 : key[8], key[11] ^= key[6] & 0xBE53718, jv ^= (u8fcos[(key[2])&0xFF]) ? 0x9DADA8A4 : jv, key[ 0] -= 0x9DADA8A4 & key[6], key[13] += key[1] - 0x7B284744, key[ 3] ^= 20 * key[18], key[ 2] |= jv - 0x313BB22;};
 }
 if (k == 9)
 {
  (key[8 ] ^= (u8fcos[(key[17])&0xFF]) ? 0x3544CA5E : key[8]);
  (key[9 ] *= (u8fsin[(0x3ED322E1)&0xFF]) ? 0x28D781D2 : key[10]);
  skype_rc4_iv13 (key, key[0]);
 }
 key[3] = ((((key[3])&0xFFFFFFFF)>>((iv * 0xA588A375)&31))|((key[3])<<((0-(iv * 0xA588A375))&31)));
 return iv;
}
# 2862 "skype_rc4.c"
void RC4_crypt (u8 * buffer, u32 bytes, RC4_context * const rc4, const u32 test)
{
 u8 t, i = rc4->i, j = rc4->j, z[256], *s;

 if (test) memcpy (s = z, rc4->s, 256); else s = rc4->s;
 for (; bytes; bytes--) i++, *buffer++ ^= ((t)=s[i],(j)=((j)+(t)+(0))&0xFF,s[i]=s[j],s[j]=(u8)(t),s[(s[i]+(t))&0xFF]);
 if (!test) rc4->i = i, rc4->j = j;
}




void Skype_RC4_Expand_IV (const u32 iv, const void *iv2, RC4_context * const rc4, const u32 flags, const u32 iv2_bytes)
{
 u32 i, j, key[20];
 u8 t;

 for (i = 0; i < 20; i++)
    key[i] = iv;

 if (!flags || (flags & 1))
    skype_rc4_iva (key, iv);







 for (i = 0, j = __min (iv2_bytes,80); i < j; i+=4)
    (*(u32*)(((u8*)(key))+(i))) ^= (*(u32*)(((u8*)(iv2))+(i)));
 for (; i < j; i++)
    (*(u8*)(((u8*)(key))+(i))) ^= (*(u8*)(((u8*)(iv2))+(i)));


 for (i = 0, j = 0x03020100; i < 256; i += 4, j += 0x04040404)
    (*(u32*)(((u8*)(rc4->s))+(i))) = j;
 for (i = 0, j = 0; i < 256; i++)
    ((t)=rc4->s[i],(j)=((j)+(t)+((*(u8*)(((u8*)(key))+(i%80)))))&0xFF,rc4->s[i]=rc4->s[j],rc4->s[j]=(u8)(t),rc4->s[(rc4->s[i]+(t))&0xFF]);

 rc4->i = 0, rc4->j = 0;
}
