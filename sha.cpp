#include "sha.h"

using namespace std;

QString SHA :: get_hash(QString input) {
    string message = input.toStdString();
    string padded = add_padding(message);
    return QString::fromStdString(compute_hash(padded));
}


string SHA :: add_padding(string& message) {
    string padded = "";
    for (char c : message) {
        padded += bitset<8> ((uint64_t)c).to_string();
    }

    int L = padded.size();
    int K = 0;
    while ((L + 1 + K + 64) % 512 != 0) K++;
    // int K = (512 - ((L + 1 + 64) % 512)) % 512;

    string zero_padding = string(K, '0');
    string messageSize = bitset<64> (L).to_string();
    padded += "1" + zero_padding + messageSize;

    return padded;
}

string SHA :: compute_hash(string& padded) {
    uint64_t H[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,	0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    uint64_t k[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786,	0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,	0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b,	0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,	0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    vector<uint64_t> message_schedule(64);

    for (int j = 0; j < (int)padded.size(); j += 512) {
        string chunk = padded.substr(j, 512);

        create_message_schedule(chunk, message_schedule);
        run_compression_function(H, k, message_schedule);
    }

    // Append the hash segments together one after the other
    // to get the full 256 bit hash.
    string hash = "";
    for (int i = 0; i < 8; i++) {
        hash += to_hex_string(H[i]);
    }
    return hash;
}



/*
    takes reference to the current 512 bit chunk of binary string, and message_schedule array.
    modifies the array to contain the message schedule for current chunk
*/
void SHA :: create_message_schedule(string& chunk, vector<uint64_t>& message_schedule) {
    // copy first 16 words of 32 bit each from the chunk
    for (int i = 0; i < 512; i += 32) {
        message_schedule[i/32] = stoul(chunk.substr(i, 32), 0, 2);
    }
    // create remaining 48 words
    for (int i = 16; i < 64; i++) {
        uint64_t s0 = SIGMA0(message_schedule[i-15]);
        uint64_t s1 = SIGMA1(message_schedule[i-2]);
        message_schedule[i] = message_schedule[i-16] + s0 + message_schedule[i-7] + s1;
        message_schedule[i] = TAKEMOD(message_schedule[i]); // addition should be modulo 2^32
    }
}



/*
    takes arrays(pointers) of Hash constant and round constants, and message_schedule array.
    It runs compression funcion for the current 512 bit chunk,
    which mutates the hash constants
*/
void SHA :: run_compression_function(uint64_t H[], uint64_t k[], vector<uint64_t>& message_schedule) {
    // Initialize working variables to current hash value
    uint64_t temp1;
    uint64_t temp2;
    uint64_t a = H[0];
    uint64_t b = H[1];
    uint64_t c = H[2];
    uint64_t d = H[3];
    uint64_t e = H[4];
    uint64_t f = H[5];
    uint64_t g = H[6];
    uint64_t h = H[7];

    for (int i = 0; i < 64; i++) {
        uint64_t s1 = EPSILON1(e);
        uint64_t ch = CH(e, f, g);
        temp1 = TAKEMOD(h + s1 + ch + k[i] + message_schedule[i]); // modulo 2^32
        uint64_t s0 = EPSILON0(a);
        uint64_t maj = MAJ(a, b, c);
        temp2 = TAKEMOD(s0 + maj); // modulo 2^32

        h = g;
        g = f;
        f = e;
        e = TAKEMOD(d + temp1); // modulo 2^32
        d = c;
        c = b;
        b = a;
        a = TAKEMOD(temp1 + temp2); // modulo 2^32
    }

    H[0] = TAKEMOD(H[0] + a);
    H[1] = TAKEMOD(H[1] + b);
    H[2] = TAKEMOD(H[2] + c);
    H[3] = TAKEMOD(H[3] + d);
    H[4] = TAKEMOD(H[4] + e);
    H[5] = TAKEMOD(H[5] + f);
    H[6] = TAKEMOD(H[6] + g);
    H[7] = TAKEMOD(H[7] + h);
}


string SHA :: to_hex_string(uint64_t input) {
    stringstream sstream;
    sstream << hex << setw(8) << setfill('0') << input;
    string temp;
    sstream >> temp;
    return temp;
}

uint64_t SHA :: ROTRIGHT(uint64_t word, uint64_t bits) {
    return (((word) >> (bits)) | ((word) << (32-(bits))));
}

// to be used to create message schedule
uint64_t SHA :: SIGMA0(uint64_t x) {
    return (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3));
}
uint64_t SHA :: SIGMA1(uint64_t x) {
    return (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10));
}

// to be used in computing hash
uint64_t SHA :: EPSILON0(uint64_t x) {
    return (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22));
}
uint64_t SHA :: EPSILON1(uint64_t x) {
    return (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25));
}
uint64_t SHA :: CH(uint64_t x, uint64_t y, uint64_t z) {
    return (((x) & (y)) ^ (~(x) & (z)));
}
uint64_t SHA :: MAJ(uint64_t x, uint64_t y, uint64_t z) {
    return (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)));
}

// returns module 2^32
uint64_t SHA :: TAKEMOD(uint64_t x) {
    return (x & 0xFFFFFFFF);
}
