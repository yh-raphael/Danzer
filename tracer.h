#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <time.h>
#include <string>
#include <mutex>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <filesystem>
#include <algorithm>
#include <openssl/sha.h>
#include "uthash.h"

using namespace std;

namespace danzer{
    // enum chunk_modes{
    //     FIXED_SIZE,
    //     FULL_FILE,
    //     CONTENT_DEFINED_CHUNK
    // };

    enum chunk_size{
        SZ_4K,
        SZ_8K,
        SZ_16K,
        SZ_32K,
        SZ_128K,
        SZ_256K,
        SZ_512K,
        SZ_1MB,
        SZ_4MB,
        FULL_FILE
    };

    enum fingerprint_modes{
        FP_SHA1,
        FP_SHA256,
        FP_SHA512,
        FP_MD5
    };
    struct chunk_entry{
        uint64_t start_pos;
        unsigned char fingerprint[SHA_DIGEST_LENGTH];
    };

    class ChunkData{
    public:
        int mode;
        vector<chunk_entry *> chunks;
        ChunkData(){
            mode = 0;
        }
        void init(){
            for(vector<chunk_entry *>::iterator i = chunks.begin(); i != chunks.end(); ++i){
                delete *i;
            }
        }
        ~ChunkData(){
            init();
        }
    };

    typedef struct {
        uint32_t mi;
        uint32_t av;
        uint32_t ma;
        uint32_t ns;
        uint32_t mask_s;
        uint32_t mask_l;
        size_t pos;
    }fcdc_ctx;

    typedef struct {
        size_t offset;
        size_t len;
    }chunk;
    
    typedef kvec_t(chunk) chunk_vec;

    class Dedupe{
        int chunk_mode;
        int chunk_block;
        uint64_t chunk_size;

        int fp_mode;
        ChunkData cd;

    public:
        // void set_chunk_size(){
        //      switch (chunk_block)
        //     {
        //     case SZ_4K:
        //         chunk_size = 4*1024;
        //         break;
        //     case SZ_8K:
        //         chunk_size = 8*1024;
        //         break;
        //     case SZ_16K:
        //         chunk_size = 16*1024;
        //         break;
        //     case SZ_32K:
        //         chunk_size = 32*1024;
        //         break;
        //     case SZ_128K:
        //         chunk_size = 128*1024;
        //         break;
        //     case SZ_256K:
        //         chunk_size = 256*1024;
        //         break;
        //     case SZ_512K:
        //         chunk_size = 512*1024;
        //         break;
        //     case SZ_1MB:
        //         chunk_size = 1024*1024;
        //         break;
        //     case SZ_4MB:
        //         chunk_size = 4*1024*1024;
        //         break;
        //     case FULL_FILE:
        //         chunk_size = 0;
        //         break;
        //     default:
        //         cout<<"Enter a valid chunk size\n";
        //         exit(0);
        //     }
        // }
        // int get_chunk_size(){
        //    return this->chunk_size; 
        // }

        fcdc_ctx fastcdc_init(uint32_t min_ch, uint32_t avg_ch, uint32_t max_ch);
        size_t fastcdc_update(fcdc_ctx *ctx, uint8_t *data, size_t len, int end, chunk_vec *cv, ofstream*);
        size_t fastcdc_stream(FILE *stream, uint32_t min_ch, uint32_t avg_ch, uint32_t max_ch, chunk_vec *cv, ofstream*);
        
        Dedupe(int chunk_mode_from, uint64_t ch_size, int fp_mode){
            chunk_mode = chunk_mode_from;
            chunk_size = ch_size;
            this->fp_mode = fp_mode;
        }
        void chunk_full_file(string file_name, ofstream*);
        void chunk_fixed_size(string file_name, ofstream*);
        void chunk_cdc(string file_name, ofstream*);
        void traverse_directory(string directory_path, ofstream*);
        string GetHexRepresentation(const unsigned char *Bytes, size_t Length);
        string readFile(const string &fileName, ofstream*);
    };

}
