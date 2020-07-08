/******************************************************************************
 * @author: waves(986299679@qq.com)
 * @requirement: lua5.1
 * @version: 0.0.1
 *
 * Description: A Lua lib written by C for encrypt/decrypt with
 *              RC4 and transform encoding to hex(base 16) string
 *
 * Usage:
 *   local rc4hex = require "rc4hex"
 *
 *     en: rc4hex.en_hex(data, key)
 *         Return: encrypted_data(string)
 *
 *     de: rc4hex.de_hex(encypted_data, key)
 *         Return: data(string)
 ******************************************************************************/
#include <ctype.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lua.h>
#include <lua5.1/lualib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SWAP(x, y)                                                             \
  {                                                                            \
    tmp = x;                                                                   \
    x = y;                                                                     \
    y = tmp;                                                                   \
  }

static void hexbyte2str(const char *str_src, char *str_dest, int src_len) {
  char sz_tmp[3];

  for (int i = 0; i < src_len; i++) {
    sprintf(sz_tmp, "%02X", (unsigned char)str_src[i]);
    memcpy(&str_dest[i * 2], sz_tmp, 2);
  }

  str_dest[src_len * 2] = '\0';

  return;
}

static void str2hexbyte(const char *source, unsigned char *dest, int src_len) {
  unsigned char h_byte, l_byte;

  for (int i = 0; i < src_len; i += 2) {
    h_byte = toupper(source[i]);
    l_byte = toupper(source[i + 1]);

    if (h_byte > 0x39) {
      h_byte -= 0x37;
    } else {
      h_byte -= 0x30;
    }

    if (l_byte > 0x39) {
      l_byte -= 0x37;
    } else {
      l_byte -= 0x30;
    }

    dest[i / 2] = (h_byte << 4) | l_byte;
  }

  return;
}

/* initialization function */
static void rc4_init(unsigned char *s, unsigned char *key, unsigned long Len) {
  int i = 0, j = 0;
  char k[256] = {0};
  unsigned char tmp; // For macro, necessary.

  for (i = 0; i < 256; i++) {
    s[i] = i;
    k[i] = key[i % Len];
  }

  for (i = 0; i < 256; i++) {
    j = (j + s[i] + k[i]) % 256;
    SWAP(s[i], s[j]);
  }
}

/* encrypt and decrypt */
static void rc4_crypt(unsigned char *s, unsigned char *Data,
                      unsigned long len) {
  int i = 0, j = 0, t = 0;
  unsigned long k = 0;
  unsigned char tmp; // For macro, necessary;

  for (k = 0; k < len; k++) {
    i = (i + 1) % 256;
    j = (j + s[i]) % 256;
    SWAP(s[i], s[j]);
    t = (s[i] + s[j]) % 256;
    Data[k] ^= s[t];
  }
}

static int encrypt_hex(lua_State *L) {
  const char *Data =
      luaL_checkstring(L, 1);               // First argument, changed, so copy.
  const char *Key = luaL_checkstring(L, 2); // Second argument, not changed.
  unsigned long d_len = strlen(Data);
  unsigned long k_len = strlen(Key);
  char cData[512] = {0}; // Copy string for Data.
  unsigned char s[256] = {0};
  char pGetData[512] = {0};

  strcpy(cData, Data);
  rc4_init(s, (unsigned char *)Key, k_len);
  rc4_crypt(s, (unsigned char *)cData, d_len);
  hexbyte2str(cData, pGetData, d_len);

  lua_pushstring(L, pGetData);

  return 1;
}

static int decrypt_hex(lua_State *L) {
  const char *pGetData = luaL_checkstring(L, 1); // First argument, not changed.
  const char *Key = luaL_checkstring(L, 2); // Second argument, not changed.
  unsigned char s[256] = {0};
  char Data[512] = {0};
  unsigned long d_len = strlen(pGetData);
  unsigned long k_len = strlen(Key);

  str2hexbyte(pGetData, (unsigned char *)Data, d_len);
  rc4_init(s, (unsigned char *)Key, k_len);
  rc4_crypt(s, (unsigned char *)Data, d_len);
  Data[d_len / 2] = '\0';

  lua_pushstring(L, Data);

  return 1; // The result count that returned for lua.
}

int luaopen_rc4hex(lua_State *L) {
  luaL_Reg l[] = {
      {"en_hex", encrypt_hex},
      {"de_hex", decrypt_hex},
      {NULL, NULL},
  };

  luaL_register(L, "rc4hex", l);

  return 1;
}
