#ifndef SHA_H
#define SHA_H
#include <bits/stdc++.h>
#include<QString>

using namespace std;

class SHA {
private:
    static string add_padding(string&);
    static string compute_hash(string&);
    static void create_message_schedule(string&, vector<uint64_t>&);
    static void run_compression_function(uint64_t H[], uint64_t k[], vector<uint64_t>&);
    static string to_hex_string(uint64_t);

    static uint64_t ROTRIGHT(uint64_t word, uint64_t bits);
       static uint64_t SIGMA0(uint64_t x);
       static uint64_t SIGMA1(uint64_t x);
       static uint64_t EPSILON0(uint64_t x);
       static uint64_t EPSILON1(uint64_t x);
       static uint64_t CH(uint64_t x, uint64_t y, uint64_t z);
       static uint64_t MAJ(uint64_t x, uint64_t y, uint64_t z);
       static uint64_t TAKEMOD(uint64_t x);

public:
    static QString get_hash(QString);
};

#endif // SHA_H
