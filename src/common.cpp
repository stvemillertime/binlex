#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <openssl/sha.h>
#include <math.h>
#include <capstone/capstone.h>
#include "common.h"

using namespace std;
using namespace binlex;

string Common::SHA256(const char *trait){
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, trait, strlen(trait));
    SHA256_Final(hash, &ctx);
    string bytes = HexdumpBE(&hash, SHA256_DIGEST_LENGTH);
    return TrimRight(bytes);
}

vector<char> Common::TraitToChar(string trait){
    trait = RemoveSpaces(RemoveWildcards(trait));
    vector<char> bytes;
    for (int i = 0; i < trait.length(); i = i + 2){
        const char *s_byte = trait.substr(i, 2).c_str();
        unsigned char byte = (char)strtol(s_byte, NULL, 16);
        bytes.push_back(byte);
    }
    return bytes;
}

string Common::RemoveWildcards(string trait){
    string::iterator end_pos = remove(trait.begin(), trait.end(), '?');
    trait.erase(end_pos, trait.end());
    return trait;
}

uint Common::GetByteSize(string s){
    return RemoveSpaces(s).length() / 2;
}

string Common::RemoveSpaces(string s){
    string::iterator end_pos = remove(s.begin(), s.end(), ' ');
    s.erase(end_pos, s.end());
    return s;
}

string Common::WildcardTrait(string trait, string bytes){
    size_t index = trait.find(bytes, 0);
    if (index == string::npos){
        return bytes;
    }
    for (int i = index; i < trait.length(); i = i + 3){
        trait.replace(i, 2, "??");
    }
    return bytes;
}

string Common::HexdumpBE(const void *data, size_t size){
    stringstream bytes;
    bytes << "";
    const unsigned char *local_pc = (const unsigned char *)data;
    for (int i = 0; i < size; i++){
        bytes << hex << setfill('0') << setw(2) << (unsigned uint32_t)local_pc[i] << " ";
    }
    return TrimRight(bytes.str());
}

string Common::TrimRight(const string &s){
    const string whitespace = " \n\r\t\f\v";
    size_t end = s.find_last_not_of(whitespace);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

void Common::Hexdump(const char * desc, const void * addr, const int len){
    int i;
    unsigned char buff[17];
    const unsigned char * pc = (const unsigned char *)addr;
    if (desc != NULL)
        printf ("%s:\n", desc);
    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    else if (len < 0) {
        printf("  NEGATIVE LENGTH: %d\n", len);
        return;
    }
    for (i = 0; i < len; i++) {
        if ((i % 16) == 0) {
            if (i != 0)
                printf ("  %s\n", buff);
            printf ("  %04x ", i);
        }
        printf (" %02x", pc[i]);
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }
    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }
    printf ("  %s\n", buff);
}
