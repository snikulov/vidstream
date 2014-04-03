#include "settings.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

void LoadSettingsFromFile(std::string filename, Settings &mode1, Settings &mode2)
{
    std::ifstream fin(filename);
    if (!fin) {
        return;
    }
    Settings *cur = &mode1;
    std::string line, line_copy;
    size_t line_number = 0;
    while (getline(fin, line)) {
        line_copy = line;
        line_number++;
        size_t comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line.erase(line.find('#'), std::string::npos);
        }
        if (line.empty()) {
            continue;
        }
        size_t eq_pos = line.find('=');
        std::string varname = line.substr(0, eq_pos);
        if (varname == "mode1") {
            cur = &mode1;
            continue;
        } else if (varname == "mode2") {
            cur = &mode2;
            continue;
        }
        if (eq_pos == std::string::npos) {
            std::cerr << "Error reading " << filename <<", line " << line_number;
            std::cerr << ":" << std::endl << line_copy << std::endl;
            std::cerr << "No value specified for \"" << line << "\"" << std::endl;
            continue;
        }
        std::string val = line.substr(eq_pos + 1, std::string::npos);
        std::istringstream ss(val);
        if (varname == "lum_quality") {
            ss >> cur->lum_quality;
        } else if (varname == "chrom_quality") {
            ss >> cur->chrom_quality;
        } else if (varname == "bch_m") {
            ss >> cur->bch_m;
        } else if (varname == "bch_t") {
            ss >> cur->bch_t;
        } else if (varname == "interlace_row_num") {
            ss >> cur->row_num;
        } else if (varname == "interlace_row_denom") {
            ss >> cur->row_denom;
        } else if (varname == "rst_block_size") {
            ss >> cur->rst_block_size;
        } else {
            std::cerr << "Error reading " << filename << ", line " << line_number;
            std::cerr << ":" << std::endl << line_copy << std::endl;
            std::cerr << "Variable " << varname << " not recognized" << std::endl;
        }
        if (!ss) {
            std::cerr << "Error reading " << filename << ", line " << line_number;
            std::cerr << ":" << std::endl << line_copy << std::endl;
            std::cerr << "Invalid value \"" << val << "\" for " << varname << std::endl;
        }
    }
}

void SaveSettingsToFile(std::ofstream &fout, const Settings &s)
{
    fout << "lum_quality=" << s.lum_quality << std::endl;
    fout << "chrom_quality=" << s.chrom_quality << std::endl;
    fout << "bch_m=" << s.bch_m << std::endl;
    fout << "bch_t=" << s.bch_t << std::endl;
    fout << "interlace_row_num=" << s.row_num << std::endl;
    fout << "interlace_row_denom=" << s.row_denom << std::endl;
    fout << "rst_block_size=" << s.rst_block_size << std::endl;
}

void SaveSettingsToFile(std::string filename,
                        const Settings &mode1, const Settings &mode2)\
{
    std::ofstream fout(filename);
    fout << "mode1" << std::endl;
    SaveSettingsToFile(fout, mode1);
    fout << "mode2" << std::endl;
    SaveSettingsToFile(fout, mode2);
    fout.close();
}
