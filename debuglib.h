#pragma once
#define ESC 0x1b
#define CSI 0x5b
#define VN "38;2;156;220;254m"
#define VV "38;2;181;206;168m"
#define FN "38;2;220;220;170m"
#define ick(value) printf("line %3d: \x1b[%s%s\x1b[0m | \x1b[%s%d\x1b[0m | \x1b[%s%s\x1b[0m\n", __LINE__, VN, #value, VV, value, FN, __func__);
#define cck(character) printf("line %3d: \x1b[%s%s\x1b[0m | \x1b[%s%c\x1b[0m | \x1b[%s%s\x1b[0m\n", __LINE__, VN, #character, VV, character, FN, __func);
#define sck(string) printf("line %3d: \x1b[%s%s\x1b[0m | \x1b[%s%s\x1b[0m | \x1b[%s%s\x1b[0m\n", __LINE__, VN, #string, VV, string, FN, __func__);
#define pck(pointer) printf("line %3d: \x1b[%s%s\x1b[0m | \x1b[%s%p\x1b[0m | \x1b[%s%s\x1b[0m\n", __LINE__, VN, #pointer, VV, pointer, FN, __func__);
