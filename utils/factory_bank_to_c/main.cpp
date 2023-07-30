/****************************************************************************
   factory_bank_to_c main.cpp: Factory bank converter

   Copyright (C) 2022-2023, AnClark Liu. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 3
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include "binary_to_compressed_c.hpp"

#include "fb2c_config.h"

#ifndef _WIN32
#include <dirent.h>
#include <unistd.h>
#else
#include <io.h>
#endif

struct BankInfo_Converter {
    ~BankInfo_Converter() { }

    std::string name;
    std::string file_path;

    std::string file_array_name;
    int file_array_size;

    bool success;
};

constexpr const char* BANK_CODE_NAME = _BANK_CODE_NAME;
constexpr const char* PATCH_ENTRY_SYMBOL = "patch_%d";
constexpr const char* MINATON_PATCH_MAGIC = "minaton1";

static std::vector<BankInfo_Converter> s_banks;

static bool is_minaton_patch_file(std::string filename)
{
    std::ifstream check_file;
    std::string line;
    int preset_category_number = 0;
    bool ret;

    check_file.open(filename);

    getline(check_file, line);
    ret = (line == MINATON_PATCH_MAGIC);

    check_file.close();

    return ret;
}

static void scan_preset_patch(const std::string dir_path, const std::string file_name)
{
    std::string file_path = dir_path + std::string("/") + std::string(file_name);

    std::string bank_name = std::string(file_name);
    if (bank_name == "default") {
        bank_name = "User bank";
    } else {
        std::string::size_type pos = bank_name.find_first_of(".");
        if (pos != std::string::npos)
            bank_name.erase(pos, std::string::npos);
    }

    std::replace(bank_name.begin(), bank_name.end(), '_', ' ');

    if (!is_minaton_patch_file(file_path.c_str())) {
        fprintf(stderr, "FILE %s is not a patch file\n", file_path.c_str());
        return;
    }

    BankInfo_Converter bank_info;
    bank_info.name = bank_name;
    bank_info.file_path = file_path;

    s_banks.push_back(bank_info);
}

static void scan_preset_patches(const std::string scan_path)
{
    // Clean up bank list first
    s_banks.clear();

    // Do not continue if dir_path is empty
    if (scan_path.empty())
        return;

    std::vector<std::string> filenames;

#if _WIN32
    /** Implement preset scanner on Windows
     * Reference: https://www.cnblogs.com/collectionne/p/6815924.html
     */
    intptr_t handle;
    _finddata_t findData;
    const std::string dir_path_for_search(scan_path + "\\*.*");

    handle = _findfirst(dir_path_for_search.c_str(), &findData); // Find the first file
    if (handle == -1)
        return;

    do {
        if (findData.attrib & _A_SUBDIR
            && strcmp(findData.name, ".directory") == 0
            && strcmp(findData.name, ".") == 0
            && strcmp(findData.name, "..") == 0) // If sub directory, "."/".." or manifest.ttl detected, ignore
            continue;
        else
            filenames.push_back(std::string(findData.name));
    } while (_findnext(handle, &findData) == 0); // Find next file

    _findclose(handle); // Close search handle
#else
    DIR* dir = opendir(scan_path.c_str());
    if (!dir)
        return;

    struct dirent* entry;
    while ((entry = readdir(dir))) {
        // clang-format off
        if (entry->d_type == 8  // means normal file
            && std::string(entry->d_name) != ".directory"
            && std::string(entry->d_name) != "."
            && std::string(entry->d_name) != "..") { // If sub directory, "."/".." or manifest.ttl detected, ignore
            // clang-format on

            filenames.push_back(std::string(entry->d_name));
        }
    }

    closedir(dir);
#endif

    std::sort(filenames.begin(), filenames.end());

    for (auto& filename : filenames)
        scan_preset_patch(scan_path, filename);
}

bool start_convert(const std::string bank_directory, const std::string output_dir)
{
    scan_preset_patches(bank_directory);

    std::stringstream patch_array_code; // C++ code of bank file encoded in an array
    std::string patch_array_symbol; // Generated C++ symbol of bank file array
    int patch_array_size = 0; // Array size of bank file array

    // Variables for generating bank file array's symbol
    char symbol[32];
    int symbol_index = 0;

    std::stringstream output_header_path, output_cpp_path;
    output_header_path << output_dir << "/" << BANK_CODE_NAME << ".h";
    output_cpp_path << output_dir << "/" << BANK_CODE_NAME << ".cpp";

    FILE* out_h = fopen(output_header_path.str().c_str(), "w");
    FILE* out_cpp = fopen(output_cpp_path.str().c_str(), "w");

    const char* reminder = "// Auto generated by factory_bank_to_c utility. DO NOT EDIT.\n\n";
    fprintf(out_h, "%s", reminder);
    fprintf(out_cpp, "%s", reminder);

    // fprintf(stderr, "* Factory bank path: %s\n", bank_directory.c_str());
    fprintf(stderr, "* Converting patch file:\n");

    for (auto iter = s_banks.begin(); iter != s_banks.end(); iter++) {
#ifdef _WIN32
        sprintf_s(symbol, PATCH_ENTRY_SYMBOL, symbol_index++); // sprintf_s() is Win32-specific
#else
        snprintf(symbol, 32, PATCH_ENTRY_SYMBOL, symbol_index++);
#endif

        bool res = binary_to_compressed_c_local(iter->file_path.c_str(), symbol, patch_array_code, patch_array_size, patch_array_symbol);
        if (res) {
            fprintf(out_h, "%s\n", patch_array_code.str().c_str());
            fprintf(stderr, "    - [ OK ] %s\n", iter->file_path.c_str());

            iter->success = true;
        } else {
            fprintf(stderr, "    - [FAIL] %s\n", iter->file_path.c_str());

            fprintf(out_h, "// WARNING: Patch file \"%s\" is failed to read\n", iter->name.c_str());
            fprintf(out_h, "//          (%s)\n\n", iter->file_path.c_str());

            iter->success = false;
        }

        iter->file_array_name = patch_array_symbol;
        iter->file_array_size = patch_array_size;
    }

    fprintf(stderr, "* Writing C++ initializer function InitializeEmbedFactoryPresets() ...");

    fprintf(out_cpp, "#include <MinatonPresets.hpp>\n");
    fprintf(out_cpp, "#include \"%s.h\"\n\n", BANK_CODE_NAME);

    fprintf(out_cpp, "void InitializeEmbedFactoryPresets(std::vector<EmbedPresetInfo> &factory_presets_list) {\n");
    fprintf(out_cpp, "    factory_presets_list.clear();\n\n");

    symbol_index = 0;
    for (auto iter = s_banks.begin(); iter != s_banks.end(); iter++) {
        if (iter->success) {
#ifdef _WIN32
            sprintf_s(symbol, PATCH_ENTRY_SYMBOL, symbol_index++); // sprintf_s() is Win32-specific
#else
            snprintf(symbol, 32, PATCH_ENTRY_SYMBOL, symbol_index++);
#endif
#ifndef MSVC_COMPATIBILITY
            fprintf(out_cpp, "    EmbedPresetInfo %s = {\n", symbol);
            fprintf(out_cpp, "        .name = \"%s\",\n", iter->name.c_str());
            fprintf(out_cpp, "        .file_path = \"%s\",\n", iter->file_path.c_str());
            fprintf(out_cpp, "        .file_array = %s,\n", iter->file_array_name.c_str());
            fprintf(out_cpp, "        .file_array_size = %d,\n", iter->file_array_size);
            fprintf(out_cpp, "    };\n");
#else
            fprintf(out_cpp, "    EmbedPresetInfo %s;\n", symbol);
            fprintf(out_cpp, "    %s.name = \"%s\";\n", symbol, iter->name.c_str());
            fprintf(out_cpp, "    %s.file_path = \"%s\";\n", symbol, iter->file_path.c_str());
            fprintf(out_cpp, "    %s.file_array = %s;\n", symbol, iter->file_array_name.c_str());
            fprintf(out_cpp, "    %s.file_array_size = %d;\n\n", symbol, iter->file_array_size);
#endif
            fprintf(out_cpp, "    factory_presets_list.push_back(std::move(%s));\n\n", symbol);
        } else {
            fprintf(out_cpp, "// WARNING: Patch file \"%s\" failed to read\n", iter->name.c_str());
            fprintf(out_cpp, "//          (%s)\n\n", iter->file_path.c_str());
        }
    }

    fprintf(out_cpp, "}\n");

    fprintf(stderr, " Success!\n");

    fclose(out_cpp);
    fclose(out_h);

    fprintf(stderr, "* Output C++ source files are:\n    - %s\n    - %s\n", output_header_path.str().c_str(), output_cpp_path.str().c_str());

    return true;
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        printf("Syntax: %s <bank_directory> <output_dir>\n", argv[0]);
        return 1;
    }

    int argn = 1;
    bool ret = start_convert(argv[argn], argv[argn + 1]);
    if (!ret) {
        fprintf(stderr, "Error opening or reading file: '%s'\n", argv[argn]);
    }

    return ret ? 0 : 1;
}
