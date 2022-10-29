#include "tracer.h"
#include <cstring>
namespace danzer{
    
    #define FASTCDC_CLAMP(x, a, b) ((x < (a)) ? (a) : ((x > b) ? b : x))
    #define FASTCDC_DIVCEIL(a, b) ((a) / (b) + ((a) % (b) ? 1 : 0))
    #define FASTCDC_MASK(x) (uint32_t)((1 << FASTCDC_CLAMP(x, 1, 31)) - 1)

    #define AVERAGE_MIN (1 << 8)
    #define AVERAGE_MAX (1 << 28)
    #define MINIMUM_MIN (AVERAGE_MIN >> 2)
    #define MINIMUM_MAX (AVERAGE_MAX >> 2)
    #define MAXIMUM_MIN (AVERAGE_MIN << 2)
    #define MAXIMUM_MAX (AVERAGE_MAX << 2)

    static const uint32_t GEAR[] = {
        0x5C95C078, 0x22408989, 0x2D48A214, 0x12842087, 0x530F8AFB, 0x474536B9,
        0x2963B4F1, 0x44CB738B, 0x4EA7403D, 0x4D606B6E, 0x074EC5D3, 0x3AF39D18,
        0x726003CA, 0x37A62A74, 0x51A2F58E, 0x7506358E, 0x5D4AB128, 0x4D4AE17B,
        0x41E85924, 0x470C36F7, 0x4741CBE1, 0x01BB7F30, 0x617C1DE3, 0x2B0C3A1F,
        0x50C48F73, 0x21A82D37, 0x6095ACE0, 0x419167A0, 0x3CAF49B0, 0x40CEA62D,
        0x66BC1C66, 0x545E1DAD, 0x2BFA77CD, 0x6E85DA24, 0x5FB0BDC5, 0x652CFC29,
        0x3A0AE1AB, 0x2837E0F3, 0x6387B70E, 0x13176012, 0x4362C2BB, 0x66D8F4B1,
        0x37FCE834, 0x2C9CD386, 0x21144296, 0x627268A8, 0x650DF537, 0x2805D579,
        0x3B21EBBD, 0x7357ED34, 0x3F58B583, 0x7150DDCA, 0x7362225E, 0x620A6070,
        0x2C5EF529, 0x7B522466, 0x768B78C0, 0x4B54E51E, 0x75FA07E5, 0x06A35FC6,
        0x30B71024, 0x1C8626E1, 0x296AD578, 0x28D7BE2E, 0x1490A05A, 0x7CEE43BD,
        0x698B56E3, 0x09DC0126, 0x4ED6DF6E, 0x02C1BFC7, 0x2A59AD53, 0x29C0E434,
        0x7D6C5278, 0x507940A7, 0x5EF6BA93, 0x68B6AF1E, 0x46537276, 0x611BC766,
        0x155C587D, 0x301BA847, 0x2CC9DDA7, 0x0A438E2C, 0x0A69D514, 0x744C72D3,
        0x4F326B9B, 0x7EF34286, 0x4A0EF8A7, 0x6AE06EBE, 0x669C5372, 0x12402DCB,
        0x5FEAE99D, 0x76C7F4A7, 0x6ABDB79C, 0x0DFAA038, 0x20E2282C, 0x730ED48B,
        0x069DAC2F, 0x168ECF3E, 0x2610E61F, 0x2C512C8E, 0x15FB8C06, 0x5E62BC76,
        0x69555135, 0x0ADB864C, 0x4268F914, 0x349AB3AA, 0x20EDFDB2, 0x51727981,
        0x37B4B3D8, 0x5DD17522, 0x6B2CBFE4, 0x5C47CF9F, 0x30FA1CCD, 0x23DEDB56,
        0x13D1F50A, 0x64EDDEE7, 0x0820B0F7, 0x46E07308, 0x1E2D1DFD, 0x17B06C32,
        0x250036D8, 0x284DBF34, 0x68292EE0, 0x362EC87C, 0x087CB1EB, 0x76B46720,
        0x104130DB, 0x71966387, 0x482DC43F, 0x2388EF25, 0x524144E1, 0x44BD834E,
        0x448E7DA3, 0x3FA6EAF9, 0x3CDA215C, 0x3A500CF3, 0x395CB432, 0x5195129F,
        0x43945F87, 0x51862CA4, 0x56EA8FF1, 0x201034DC, 0x4D328FF5, 0x7D73A909,
        0x6234D379, 0x64CFBF9C, 0x36F6589A, 0x0A2CE98A, 0x5FE4D971, 0x03BC15C5,
        0x44021D33, 0x16C1932B, 0x37503614, 0x1ACAF69D, 0x3F03B779, 0x49E61A03,
        0x1F52D7EA, 0x1C6DDD5C, 0x062218CE, 0x07E7A11A, 0x1905757A, 0x7CE00A53,
        0x49F44F29, 0x4BCC70B5, 0x39FEEA55, 0x5242CEE8, 0x3CE56B85, 0x00B81672,
        0x46BEECCC, 0x3CA0AD56, 0x2396CEE8, 0x78547F40, 0x6B08089B, 0x66A56751,
        0x781E7E46, 0x1E2CF856, 0x3BC13591, 0x494A4202, 0x520494D7, 0x2D87459A,
        0x757555B6, 0x42284CC1, 0x1F478507, 0x75C95DFF, 0x35FF8DD7, 0x4E4757ED,
        0x2E11F88C, 0x5E1B5048, 0x420E6699, 0x226B0695, 0x4D1679B4, 0x5A22646F,
        0x161D1131, 0x125C68D9, 0x1313E32E, 0x4AA85724, 0x21DC7EC1, 0x4FFA29FE,
        0x72968382, 0x1CA8EEF3, 0x3F3B1C28, 0x39C2FB6C, 0x6D76493F, 0x7A22A62E,
        0x789B1C2A, 0x16E0CB53, 0x7DECEEEB, 0x0DC7E1C6, 0x5C75BF3D, 0x52218333,
        0x106DE4D6, 0x7DC64422, 0x65590FF4, 0x2C02EC30, 0x64A9AC67, 0x59CAB2E9,
        0x4A21D2F3, 0x0F616E57, 0x23B54EE8, 0x02730AAA, 0x2F3C634D, 0x7117FC6C,
        0x01AC6F05, 0x5A9ED20C, 0x158C4E2A, 0x42B699F0, 0x0C7C14B3, 0x02BD9641,
        0x15AD56FC, 0x1C722F60, 0x7DA1AF91, 0x23E0DBCB, 0x0E93E12B, 0x64B2791D,
        0x440D2476, 0x588EA8DD, 0x4665A658, 0x7446C418, 0x1877A774, 0x5626407E,
        0x7F63BD46, 0x32D2DBD8, 0x3C790F4A, 0x772B7239, 0x6F8B2826, 0x677FF609,
        0x0DC82C11, 0x23FFE354, 0x2EAC53A6, 0x16139E09, 0x0AFD0DBC, 0x2A4D4237,
        0x56A368C7, 0x234325E4, 0x2DCE9187, 0x32E8EA7E};

    uint32_t normal_size(const uint32_t mi, const uint32_t av,const uint32_t len) {
        uint32_t off = mi + FASTCDC_DIVCEIL(mi, 2);
        if (off > av)
            off = av;
        uint32_t sz = av - off;
        if (sz > len)
            return len;
        return sz;
    }

    uint32_t cut(const uint8_t *src, const uint32_t len, const uint32_t mi, const uint32_t ma, const uint32_t ns, const uint32_t mask_s, const uint32_t mask_l) {
        uint32_t n, fp = 0, i = (len < mi) ? len : mi;
        n = (ns < len) ? ns : len;
        for (; i < n; i++) {
            fp = (fp >> 1) + GEAR[src[i]];
            //*tf_name << fp <<"\n";
            if (!(fp & mask_s))
            return i + 1;
        }
        n = (ma < len) ? ma : len;
        for (; i < n; i++) {
            fp = (fp >> 1) + GEAR[src[i]];
            //*tf_name << fp << "\n";
            if (!(fp & mask_l))
            return i + 1;
        }
        return i;
    }

    fcdc_ctx Dedupe::fastcdc_init(uint32_t mi, uint32_t av, uint32_t ma) {
        uint32_t bits = (uint32_t)round(log2(av));
        fcdc_ctx ctx = {
            .mi = FASTCDC_CLAMP(mi, MINIMUM_MIN, MINIMUM_MAX),
            .av = FASTCDC_CLAMP(av, AVERAGE_MIN, AVERAGE_MAX),
            .ma = FASTCDC_CLAMP(ma, MAXIMUM_MIN, MAXIMUM_MAX),
            .ns = normal_size(mi, av, ma),
            .mask_s = FASTCDC_MASK(bits + 1),
            .mask_l = FASTCDC_MASK(bits - 1),
            .pos = 0
        };
        return ctx;
    }

    size_t Dedupe::fastcdc_update(fcdc_ctx *ctx, uint8_t *data, size_t len, int end,chunk_vec *cv) {
        size_t offset = 0;
        while (((len - offset) >= ctx->ma) || (end && (offset < len))) {
            uint32_t cp = cut(data + offset, len - offset, ctx->mi, ctx->ma, ctx->ns,
                            ctx->mask_s, ctx->mask_l);     //Returns the chunk length
            chunk blk = {.offset = ctx->pos + offset, .len = cp};
            kv_push(chunk, *cv, blk);
            offset += cp;
        }
        ctx->pos += offset;
        return offset;
    }

    size_t Dedupe::fastcdc_stream(FILE *stream, uint32_t mi, uint32_t av, uint32_t ma, chunk_vec *cv) {
        size_t offset = 0;
        int end = 0;
        fcdc_ctx cdc = fastcdc_init(mi, av, ma), *ctx = &cdc;
        size_t rs = ctx->ma * 4;
        rs = FASTCDC_CLAMP(rs, 0, UINT32_MAX);
        //printf("rs: %ld\n", rs);
        uint8_t *data = (uint8_t *)malloc(rs);
        while (!end) {
            size_t ar = fread(data, 1, rs, stream);
            end = feof(stream);
            offset += fastcdc_update(ctx, data, ar, end, cv);
            fseek(stream, offset, SEEK_SET);
        }
        free(data);
        return kv_size(*cv);
    }

    // void FStat::traverse_directory (string directory_path, ofstream& output_file)
    // {
    //     cout << "File Statistics session start . . ." << endl;

    //     // Iterate through whole directory.
    //     for (auto const& dir_entry : filesystem::recursive_directory_iterator(directory_path))
    //     {
    //         if (filesystem::is_symlink(dir_entry)) {
    //             cout << "Symlink encountered !" << endl;
    //             continue;
    //         }

    //         if (dir_entry.is_regular_file()) {
    //             string fname = filesystem::absolute(dir_entry.path().string());

    //             cout << "File name: " << fname << endl;
    //             cout << "File size: " << filesystem::file_size(fname) << endl;

    //             // output_file << "File Name: " << fname << endl;
    //             // output_file << "File Size: " << filesystem::file_size(fname) << endl;
		        
    //             fstat_table[fname] = filesystem::file_size(fname);
    //         }
    //     }

    //     // Traverse an unordered map.
    //     for (auto x : fstat_table)
    //     {
    //         // cout << x.first << " " << x.second << endl;
    //         total_file_count ++;
    //         total_file_size += x.second;
    //     }

    //     mean_file_size = (long long) ((double) total_file_size / (double) total_file_count);

    //     output_file << "total_file_count," << total_file_count << endl;
    //     output_file << "total_file_size," << total_file_size << endl;
    //     output_file << "mean_file_size," << mean_file_size << endl;

    //     return;
    // }

    /*
        -f option 1:
        file size percentile
    */
    void FStat::measure_file_sizes (string directory_path, ofstream& output_file)
    {
        cout << "Measuring file sizes session start . . ." << endl;


        long long dir_capacity = 0;
        cout << "[1] Iterate through whole directory . . ." << endl;
        // (1) Iterate through whole directory.
        for (auto const& dir_entry : filesystem::recursive_directory_iterator(directory_path))
        {
            if (filesystem::is_symlink(dir_entry)) {
                cout << "Symlink encountered !" << endl;
                continue;
            }

            if (dir_entry.is_regular_file()) {
                string fname = filesystem::absolute(dir_entry.path().string());

                // cout << "File name: " << fname << endl;
                // cout << "File size: " << filesystem::file_size(fname) << endl;

                fsize_table.push_back({filesystem::file_size(fname), fname});

                dir_capacity += filesystem::file_size(fname);
            }
        }
        cout        << "dir_capacity == " << dir_capacity << endl;
        output_file << "dir_capacity == " << dir_capacity << endl << endl;


        cout << endl;
        cout << "[2] Analyze the file size table information . . ." << endl;

        cout        << "-- length: " << fsize_table.size() << endl;
        output_file << "-- length: " << fsize_table.size() << endl;

        // (2) Sort the list.
        sort(fsize_table.begin(), fsize_table.end());
        // for (auto x : fsize_table)
        //     cout << x.first << "               " << x.second << endl;
        // cout << endl;

        // (3) Calculate specific percentile file count values.
        // In order to avoid numerical overflow, use 8 bytes integer type!
        long long percentile_10 = fsize_table.size() * 0.1; //cout << percentile_10 << endl;
        long long percentile_25 = fsize_table.size() * 0.25; //cout << percentile_25 << endl;
        long long percentile_50 = fsize_table.size() * 0.5; //cout << percentile_50 << endl;
        long long percentile_75 = fsize_table.size() * 0.75; //cout << percentile_75 << endl;
        long long percentile_90 = fsize_table.size() * 0.9; //cout << percentile_90 << endl;

        long long fs_10 = 0;
        long long fs_25 = 0;
        long long fs_50 = 0;
        long long fs_75 = 0;
        long long fs_90 = 0;

        long long count = 0;

        // (4) Iterate the list one more time - check the corresponding file sizes for each file count percentile.
        for (auto x : fsize_table)
        {
            //cout << count << "      " << x.first << "            " << x.second << endl;
            //output_file << x.second << ", " << x.first << endl;

            total_file_count ++;
            total_file_size += x.first;

            if      (count == percentile_10) {
                cout        << "10%    " << count << "      " << x.first << "            " << x.second << endl;
                output_file << "10%, " << count << ", " << x.first << ", " << x.second << endl;
                fs_10 = x.first;
            }
            else if (count == percentile_25) {
                cout        << "25%    " << count << "      " << x.first << "            " << x.second << endl;
                output_file << "25%, " << count << ", " << x.first << ", " << x.second << endl;
                fs_25 = x.first;
            }
            else if (count == percentile_50) {
                cout        << "50%    " << count << "      " << x.first << "            " << x.second << endl;
                output_file << "50%, " << count << ", " << x.first << ", " << x.second << endl;
                fs_50 = x.first;
            }
            else if (count == percentile_75) {
                cout        << "75%    " << count << "      " << x.first << "            " << x.second << endl;
                output_file << "75%, " << count << ", " << x.first << ", " << x.second << endl;
                fs_75 = x.first;
            }
            else if (count == percentile_90) {
                cout        << "90%    " << count << "      " << x.first << "            " << x.second << endl;
                output_file << "90%, " << count << ", " << x.first << ", " << x.second << endl;
                fs_90 = x.first;
            }

            count++;
        }

        cout << "\n";
        cout << "[3] Calculate the aggregate information . . ." << endl;
        // (5) Additionally, let's calcualte the mean file size as well.
        mean_file_size = (long long) ((double) total_file_size / (double) total_file_count);

        cout        << "total_file_count : " << total_file_count << endl;
        cout        << "total_file_size  : " << total_file_size << endl;
        cout        << "mean_file_size   : " << mean_file_size << endl;

        output_file << endl;
        output_file << "total_file_count, " << total_file_count << endl;
        output_file << "total_file_size, " << total_file_size << endl;
        output_file << "mean_file_size, " << mean_file_size << endl;


        // (6) csv format
        cout        << endl;
        cout        << total_file_count << ", " << total_file_size << ", " << mean_file_size << ", " << 
        fs_10 << ", " << fs_25 << ", " << fs_50 << ", " << fs_75 << ", " << fs_90 << endl;

        output_file << endl;
        output_file << total_file_count << ", " << total_file_size << ", " << mean_file_size << ", " << 
        fs_10 << ", " << fs_25 << ", " << fs_50 << ", " << fs_75 << ", " << fs_90 << endl;


        return;
    }



    /*
        -f option 2: 
        cumulative file size percentile
    */
    void FStat::measure_cumulative_fs (string directory_path, ofstream& output_file)
    {
        cout << "* Cumulative distribution session start . . ." << endl;


        long long dir_capacity = 0;
        cout << "[1] Iterate through whole directory . . ." << endl;
        // (1) Iterate through whole directory.
        for (auto const& dir_entry : filesystem::recursive_directory_iterator(directory_path))
        {
            if (filesystem::is_symlink(dir_entry)) {
                cout << "Symlink encountered !" << endl;
                continue;
            }

            if (dir_entry.is_regular_file()) {
                string fname = filesystem::absolute(dir_entry.path().string());

                // cout << "File name: " << fname << endl;
                // cout << "File size: " << filesystem::file_size(fname) << endl;

                fsize_table.push_back({filesystem::file_size(fname), fname});

                dir_capacity += filesystem::file_size(fname);
            }
        }
        cout        << "dir_capacity == " << dir_capacity << endl;
        output_file << "dir_capacity == " << dir_capacity << endl << endl;


        cout << endl;
        cout << "[2] Analyze the file size table information . . ." << endl;

        cout        << "-- length: " << fsize_table.size() << endl;
        output_file << "-- length: " << fsize_table.size() << endl;

        // (2) Sort the list.
        sort(fsize_table.begin(), fsize_table.end());
        // for (auto x : fsize_table)
        //     cout << x.first << "               " << x.second << endl;
        // cout << endl;


        // (3) Calculate specific percentile file count values.
        // In order to avoid numerical overflow, use 8 bytes integer type!
        long long c_percentile_10 = dir_capacity * 0.1; //cout << percentile_10 << endl;
        long long c_percentile_25 = dir_capacity * 0.25; //cout << percentile_25 << endl;
        long long c_percentile_50 = dir_capacity * 0.5; //cout << percentile_50 << endl;
        long long c_percentile_75 = dir_capacity * 0.75; //cout << percentile_75 << endl;
        long long c_percentile_90 = dir_capacity * 0.9; //cout << percentile_90 << endl;

        long long c_fs_10 = 0;
        long long c_fs_25 = 0;
        long long c_fs_50 = 0;
        long long c_fs_75 = 0;
        long long c_fs_90 = 0;

        long long count = 0;

        long long sum_capacity = 0;
        // (4) Iterate the list one more time - check the corresponding file sizes for each file count percentile.
        for (auto x : fsize_table)
        {
            //cout << count << "      " << x.first << "            " << x.second << endl;
            //output_file << x.second << ", " << x.first << endl;

            total_file_count ++;
            total_file_size += x.first;

            if      (c_fs_10 == 0 && total_file_size >= c_percentile_10) {
                cout        << "10%    " << count << "      " << x.first << "            " << x.second << endl;
                output_file << "10%, " << count << ", " << x.first << ", " << x.second << endl;
                c_fs_10 = x.first;      // assign
            }
            else if (c_fs_25 == 0 && total_file_size >= c_percentile_25) {
                cout        << "25%    " << count << "      " << x.first << "            " << x.second << endl;
                output_file << "25%, " << count << ", " << x.first << ", " << x.second << endl;
                c_fs_25 = x.first;      // assign
            }
            else if (c_fs_50 == 0 && total_file_size >= c_percentile_50) {
                cout        << "50%    " << count << "      " << x.first << "            " << x.second << endl;
                output_file << "50%, " << count << ", " << x.first << ", " << x.second << endl;
                c_fs_50 = x.first;      // assign
            }
            else if (c_fs_75 == 0 && total_file_size >= c_percentile_75) {
                cout        << "75%    " << count << "      " << x.first << "            " << x.second << endl;
                output_file << "75%, " << count << ", " << x.first << ", " << x.second << endl;
                c_fs_75 = x.first;      // assign
            }
            else if (c_fs_90 == 0 && total_file_size >= c_percentile_90) {
                cout        << "90%    " << count << "      " << x.first << "            " << x.second << endl;
                output_file << "90%, " << count << ", " << x.first << ", " << x.second << endl;
                c_fs_90 = x.first;
            }

            count++;
        }


        cout << "\n";
        cout << "[3] Calculate the aggregate information . . ." << endl;
        // (5) Additionally, let's calcualte the mean file size as well.
        mean_file_size = (long long) ((double) total_file_size / (double) total_file_count);

        cout        << "total_file_count : " << total_file_count << endl;
        cout        << "total_file_size  : " << total_file_size << endl;
        cout        << "mean_file_size   : " << mean_file_size << endl;

        output_file << endl;
        output_file << "total_file_count, " << total_file_count << endl;
        output_file << "total_file_size, " << total_file_size << endl;
        output_file << "mean_file_size, " << mean_file_size << endl;


        // (6) csv format
        cout        << endl;
        cout        << total_file_count << ", " << total_file_size << ", " << mean_file_size << ", " << 
        c_fs_10 << ", " << c_fs_25 << ", " << c_fs_50 << ", " << c_fs_75 << ", " << c_fs_90 << endl;

        output_file << endl;
        output_file << total_file_count << ", " << total_file_size << ", " << mean_file_size << ", " << 
        c_fs_10 << ", " << c_fs_25 << ", " << c_fs_50 << ", " << c_fs_75 << ", " << c_fs_90 << endl;


        return;
    }



    /*
        -f option 3: 
        file extension
    */
    void FStat::measure_file_extensions (string directory_path, ofstream& output_file)
    {
        cout << "** Measuring file extensions session start . . ." << endl;

        // (1) Iterate through whole directory.
        cout << "[1] Iterate through whole directory . . ." << endl;
        for (auto const& dir_entry : filesystem::recursive_directory_iterator(directory_path))
        {
            if (filesystem::is_symlink(dir_entry)) {
                cout << "Symlink encountered" << endl;
                continue;
            }

            if (dir_entry.is_regular_file()) {
                string fname = filesystem::absolute(dir_entry.path().string());

                // cout << "File name: " << fname << endl;
                // cout << "File size: " << filesystem::file_size(fname) << endl;

                // tf_stream << "File Name: " << fname << endl;
                // tf_stream << "File Size: " << filesystem::file_size(fname) << endl;

                // Parse a string of file extension.
                size_t end = fname.length();
                size_t cur = fname.rfind('.');
                string file_extension;

                if (cur == string::npos)
	            	file_extension = "none";                                // no file extension
                else 
                    file_extension = fname.substr(cur + 1, end);

                cout        << "File extension: " << file_extension << endl;
                output_file << "File Extension: " << file_extension << endl;

                // (2) Make hash table for file extension.
                fe_table[file_extension] ++;
            }
        }


        for (auto x : fe_table)
            cout << x.first << "        " << x.second << endl;

        return;
    }

    void Dedupe::traverse_directory (string directory_path, ofstream& tf_name) {
        cout<<"Directory traversing started\n";
        for (auto const& dir_entry : filesystem::recursive_directory_iterator(directory_path))
        {
            if(filesystem::is_symlink(dir_entry)){
                cout<<"Symlink encountered\n";
                continue;
            }
            if(dir_entry.is_regular_file()){
                string fname = filesystem::absolute(dir_entry.path().string());

                cout << "File name: " << fname << endl;
                cout << "File size: " << filesystem::file_size(fname) << endl;

                tf_name << "File Name: " << fname << ", Size: " << filesystem::file_size(fname) << '\n';
		        tf_name << "Fingerprints: \n";
                //Pass the file to the appropriate chunking method.
                //Store the metadata to the trace file
                //Hash of the file name, file size, file extension.
                if(this->chunk_mode == 0){
                    //Full File 
                    Dedupe::chunk_full_file(fname, tf_name);
                } else if(this->chunk_mode == 1){
                    //Fixed Size Chunking 
                    string buffer;
                    
                    uint64_t max_buffer_size = 25769803776;                 // 24 x 1024 x 1024 x 1024 = 24G = 25,769,803,776
                    if(filesystem::file_size(fname) >  max_buffer_size){
                        ifstream ifs(fname, ios::binary);
                        if(!ifs){
                            cerr<<"Input file error\n";
                            exit(0);
                        }
                        
                        do{
                            vector<char> bytes(max_buffer_size);
                            ifs.read(&bytes[0], max_buffer_size);
                        
                            Dedupe::chunk_fixed_size(string(&bytes[0], max_buffer_size), tf_name);
                        }while(ifs);
                    } else{
                        buffer = readFile(fname, tf_name);
                        Dedupe::chunk_fixed_size(buffer, tf_name);
                    }
                    
                } else{
                    //CDC
                    chunk_vec cv;
                    kv_init(cv);
                    string buffer;
                    FILE *s = fopen((const char *)fname.c_str(), "r");
                    fastcdc_stream(s, 2048, 2048*2, 2048*4, &cv);   //Need to update it
                    fclose(s);
                    cerr<<"Total number of chunks: "<<kv_size(cv)<<endl;;
                    uint64_t kv_count = 0;
                    uint64_t chunk_cnt = 0;

                    //uint64_t max_buffer_size = 1048576000;
                    // if(filesystem::file_size(fname) >  max_buffer_size){  //Greater than 100MB
                        ifstream ifs(fname, std::ifstream::in);
                        if(!ifs){
                            cerr<<"Input file error\n";
                            exit(0);
                        }
                    
                        do{
                            int chunk_length = kv_A(cv, chunk_cnt).len;
                            if(chunk_length == 0)
                                break;
                            //cerr<<"chunk_length: "<<chunk_length<<endl;
                            vector<char> bytes(chunk_length);
                            ifs.read(&bytes[0], chunk_length);
                        
                            // Dedupe::chunk_cdc(string(&bytes[0], max_buffer_size), tf_name, &cv, &kv_count);
                            Dedupe::chunk_cdc(string(&bytes[0], chunk_length), tf_name);
                            chunk_cnt +=1;
                        }while(ifs);
                    // } else{
                    //     buffer = readFile(fname, tf_name);
                    //     Dedupe::chunk_cdc(buffer, tf_name, &cv, &kv_count);
                    // }
                    kv_destroy(cv);
                }
            }
        }
    }


    void Dedupe::chunk_full_file(string file_name, ofstream& trace_file){
        cout<<"Full file chunk: "<<file_name<<endl;
        //Consider file as a single chunk
        unsigned char temp_fp[SHA_DIGEST_LENGTH];
        string buffer = readFile(file_name, trace_file);
        int f_size = buffer.length();

        SHA1((const unsigned char *)buffer.c_str(), f_size, temp_fp);
  
        std::string fp = GetHexRepresentation(temp_fp, SHA_DIGEST_LENGTH);

        trace_file << "\nFingerprint: "+fp;
    }

    void Dedupe::chunk_fixed_size(string buffer, ofstream &trace_file){
        //partition the file into fixed size chunks
        uint64_t f_size = buffer.length();
        if(buffer.empty()){
            cout<<"Buffer empty\n";
            return;
        }
        uint64_t f_pos = 0;
        
        //*trace_file->write("File Name: "+ file_name+ ", Size: "+ f_size+"\n");
        //trace_file->write("Fingerprints: \n");
        
        uint64_t file_size = f_size;
        while(f_pos < file_size){
            const char *chunk;
            if(f_pos > file_size){
                break;
            }
                
            if((f_pos + chunk_size) > file_size){
                string tmp_chunk = buffer.substr(f_pos);
                if(chunk_size > tmp_chunk.length()){
                    chunk = (tmp_chunk + string(chunk_size - tmp_chunk.length(), '0')).c_str();   //Fill constructor of string class
                } else{
                    chunk = tmp_chunk.c_str();
                }
            } else{
                chunk =  buffer.substr(f_pos, chunk_size).c_str();
            }
            f_pos += chunk_size;
            f_size -= chunk_size;
            unsigned char temp_fp[SHA_DIGEST_LENGTH];
            SHA1((const unsigned char *)chunk, chunk_size, temp_fp);
            string fp = GetHexRepresentation(temp_fp, SHA_DIGEST_LENGTH);
            trace_file << fp+"\n";
        }
    }

    void Dedupe::chunk_cdc(string buffer, ofstream& trace_file){
        unsigned char temp_fp[SHA_DIGEST_LENGTH];
        const char *chunk = buffer.c_str();
        int tmp_chunk_size = buffer.length();
        SHA1((const unsigned char *)chunk, tmp_chunk_size, temp_fp);
        string fp = GetHexRepresentation(temp_fp, SHA_DIGEST_LENGTH);
        trace_file << fp+"\n";
    }

    void Dedupe::chunk_cdc(string buffer, ofstream& trace_file, chunk_vec *cv, uint64_t *kv_count){
        //parition the file based on window size of cdc
        //first read the file and then pass the buffer
        if(buffer.empty()){
            return;
        }
        uint64_t f_size = buffer.length();
        cerr<<"Buffer length: "<<f_size<<endl;
        uint64_t f_pos = 0;
        uint64_t file_size = f_size;
        uint64_t chunk_cnt = *kv_count;
        cerr<<"chunk_cnt: "<<chunk_cnt<<endl;
        int tmp_chunk_size = 0;
        while(f_pos < file_size){
            // cerr<<"f_pos: "<<f_pos<<endl;
            const char *chunk;
            if(f_pos > file_size){
                break;
            }
            
            tmp_chunk_size = kv_A(*cv, chunk_cnt).len;
            if(chunk_cnt > kv_size(*cv)){
                cerr<<"Chunk count increased the cv size. chunk_cnt: "<<chunk_cnt<<endl;
            }
            if((f_pos + tmp_chunk_size) > file_size){
                string tmp_chunk = buffer.substr(f_pos);
                if(tmp_chunk_size > tmp_chunk.length()){
                    chunk = string(tmp_chunk_size - tmp_chunk.length(), '0').c_str();   //Fill constructor of string class
                } else{
                    chunk = tmp_chunk.c_str();
                }
            } else{
                chunk =  buffer.substr(f_pos, tmp_chunk_size).c_str();
                //cerr<<"Else - tmp_chunk_size: "<<tmp_chunk_size<<"\n";
            }
            f_pos += tmp_chunk_size;
            f_size -= tmp_chunk_size;
            chunk_cnt += 1;
            unsigned char temp_fp[SHA_DIGEST_LENGTH];
            SHA1((const unsigned char *)chunk, tmp_chunk_size, temp_fp);
            string fp = GetHexRepresentation(temp_fp, SHA_DIGEST_LENGTH);
            trace_file << fp+"\n";
        }
        *kv_count = chunk_cnt;
    }

    string Dedupe::GetHexRepresentation(const unsigned char *Bytes, size_t Length) {
        std::string ret;
        ret.reserve(Length * 2);
        for(const unsigned char *ptr = Bytes; ptr < Bytes+Length; ++ptr) {
            char buf[3];
            sprintf(buf, "%02x", (*ptr)&0xff);
            ret += buf;
        }
        return ret;
    }

    string Dedupe::readFile(const string &fileName, ofstream& tf_name){
        ifstream ifs(fileName.c_str(), ios::binary);
        if(!ifs){
            cout<<"File openning failed\n";
            exit(0);
        } 

        streampos ifs_start, ifs_end;
        ifs_start = ifs.tellg();
        ifs.seekg(0, ios::end);
        ifs_end = ifs.tellg();
	    ifstream::pos_type fileSize = ifs_end - ifs_start;
        //*tf_name << "File name: "+fileName;
	    ifs.seekg(0, ios::beg);

        vector<char> bytes(fileSize);
        ifs.read(&bytes[0], fileSize);

        ifs.close();
        return string(&bytes[0], fileSize);
    }
}

int main(int argc, char **argv)
{
        uint64_t chunk_size = 0;
        string directory_path;
        string output_file;
        int chunk_mode = 0;
        int c;
        int fstat_flag = 0;
        while ((c = getopt(argc, argv, "f:s:m:i:o:")) != -1)
        {
            switch (c)
            {
            case 'f':
                fstat_flag = atoi(optarg);
                break;
            case 's':
                chunk_size = atoi(optarg);
                break;
            case 'm':
                chunk_mode = atoi(optarg);
                break;
            case 'i':
                directory_path = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            default:
                break;
            }
        }

        // if (fstat_flag == 1) {
        //     danzer::FStat *fstat = new danzer::FStat();
        //     ofstream fstat_file;

        //     fstat_file.open(output_file, ios::out);
        //     if (!fstat_file) {
        //         cout << "Out file error" << endl;
        //         exit(0);
        //     } 

        //     fstat->traverse_directory(directory_path, fstat_file);

        //     cout << "File Statistics session end . . ." << endl;

        //     return 0;
        // }

        if (fstat_flag == 1) {
            danzer::FStat *fstat = new danzer::FStat();
            ofstream fstat_file;

            fstat_file.open(output_file, ios::out);
            if (!fstat_file) {
                cout << "Out file error" << endl;
                exit(0);
            } 

            fstat->measure_file_sizes(directory_path, fstat_file);

            cout << endl;
            cout << "File Statistics session end . . ." << endl;

            return 0;
        }

        else if (fstat_flag == 2) {
            danzer::FStat *fstat = new danzer::FStat();
            ofstream fstat_file;

            fstat_file.open(output_file, ios::out);
            if (!fstat_file) {
                cout << "Out file error" << endl;
                exit(0);
            } 

            fstat->measure_cumulative_fs(directory_path, fstat_file);

            cout << endl;
            cout << "* Cumulative distribution session end . . ." << endl;

            return 0;
        }

        else if (fstat_flag == 3) {
            danzer::FStat *fe = new danzer::FStat();
            ofstream fe_file;

            fe_file.open(output_file, ios::out);
            if (!fe_file) {
                cout << "Out file error" << endl;
                exit(0);
            } 

            fe->measure_file_extensions(directory_path, fe_file);   ////////////////////////// TODO!! start here

            cout << endl;
            cout << "** Measuring file extensions session end . . ." << endl;

            return 0;
        }

        else {
            danzer::Dedupe *dedup = new danzer::Dedupe(chunk_mode, chunk_size, 0);
            ofstream tracefile;

            tracefile.open(output_file, ios::out);
            if(!tracefile){
                cout<<"Out file error\n";
                exit(0);
            } 
            
            dedup->traverse_directory(directory_path, tracefile);
        }


        //cout<<"Done\n";

        return 0;
}

/*int main(int argc, char *argv[]) {
  danzer::Dedupe *dedup = new  danzer::Dedupe(1, 1024, 0);
  ofstream tracefile;
  tracefile.open("tracefile.txt", ios::out);
  if(!tracefile){
    cout<<"No tracefile\n";
    exit(0);
  }
  string f_name = "/home/safdar/dedupAnalyzer/test_dir/random_1M_1K.txt";
  dedup->chunk_cdc(f_name, &tracefile);
  return 0;
}*/

