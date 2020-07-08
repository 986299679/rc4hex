#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SWAP(x, y)                                                             \
  {                                                                            \
    tmp = x;                                                                   \
    x = y;                                                                     \
    y = tmp;                                                                   \
  }

void hexbyte2str(const char *str_src, char *str_dest, int src_len) {
  char sz_tmp[3];

  for (int i = 0; i < src_len; i++) {
    sprintf(sz_tmp, "%02X", (unsigned char)str_src[i]);
    memcpy(&str_dest[i * 2], sz_tmp, 2);
  }

  str_dest[src_len * 2] = '\0';

  return;
}

void str2hexbyte(const char *source, unsigned char *dest, int src_len) {
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
void rc4_init(unsigned char *s, unsigned char *key, unsigned long Len) {
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
void rc4_crypt(unsigned char *s, unsigned char *Data, unsigned long len) {
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

char *encrypt_hex(char *Data, const char *Key) {
  /*
   * const char *Data = luaL_checkstring(L, 1); // changed
   * copy. const char *Key = luaL_checkstring(L, 2); // not changed
   */
  unsigned long d_len = strlen(Data);
  unsigned long k_len = strlen(Key);
  printf("%ld\n", d_len);
  /*char *cData = {0}; // Copy string for Data.*/
  unsigned char s[256] = {0};
  static char pGetData[512];
  printf("%s\n", Data);
  char cData[512] = {0}; // Copy string for Data.

  /*strcpy(cData, Data);*/
  strncpy(cData, Data, 10);
  printf("get here\n");
  rc4_init(s, (unsigned char *)Key, k_len);
  rc4_crypt(s, (unsigned char *)cData, d_len);
  hexbyte2str(cData, pGetData, d_len);

  return pGetData;
}

char *decrypt_hex(const char *pGetData, char *Key) {
  /*
   * const char *pGetData = luaL_checkstring(L, 1); // not changed
   * changed. *const char *Key = luaL_checkstring(L, 2); // not changed
   */
  unsigned char s[256] = {0};
  static char Data[512];
  unsigned long d_len = strlen(pGetData);
  unsigned long k_len = strlen(Key);

  str2hexbyte(pGetData, (unsigned char *)Data, d_len);
  rc4_init(s, (unsigned char *)Key, k_len);
  rc4_crypt(s, (unsigned char *)Data, d_len);
  Data[d_len / 2] = '\0';
  printf("final pGetData = %s\n", pGetData);
  printf("final Key = %s\n", Key);

  return Data;
}

int main(void) {
  char t_Data[] = "569908541";
  char t_Key[] = "VMI9yw0eTtQ0z14l";
  char *Data;
  Data = encrypt_hex(t_Data, t_Key);
  printf("%s\n", Data);
  printf("-------------------------------------\n");
  char t_getData[] = "A3396500A76913E34B";
  Data = decrypt_hex(t_getData, t_Key);
  printf("%s\n", Data);
  getchar();

  return 0;
}
