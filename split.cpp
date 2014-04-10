#include "split.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <QDebug>

#include <mainwindow.h>

#include "corrupt.h"

const int ERR_PARSING = -2;
const int ERR_MEMORY  = -3;
const int ERR_CODING  = -4;


Marker identify_marker(int c)
{
    Marker m;
    m.type = c;
    m.id = c;
    m.no = 0;
    if ((c & 0xF0) == 0xC0) { // either SOF or DHT
        if (c != DHT) {
            m.no = c & 0x0F;
            m.type = SOF;
        }
    } else if ((c & 0xF0) == 0xD0 && (c & 0x0F) <= 7) { // RSTn, n = 0..7
        m.no = c & 0x0F;
        m.type = RST;
    } else if ((c & 0xF0) == 0xE0) { // 0xAPPn, n = any
        m.no = c & 0x0F;
        m.type = APP;
    }
    return m;
}

int is_rst(int c)
{
    return (c & 0xF0) == 0xD0 && (c & 0x0F) <= 7;
}

// debug eye candy
char *marker_name(int c)
{
    switch (c) {
        case 0xD8: return "SOI";
        case 0xC0: return "SOF";
        case 0xC4: return "DHT";
        case 0xDB: return "DQT";
        case 0xDD: return "DRI";
        case 0xDA: return "SOS";
        case 0xFE: return "COM";
        case 0xD9: return "EOI";
    }
    if ((c & 0xF0) == 0xC0) {
        return "SOF";
    }
    if ((c & 0xF0) == 0xD0) {
        return "RST";
    }
    if ((c & 0xF0) == 0xE0) {
        return "APP";
    }
    return "unrecognized";
}

// process a section that contains its length before other data
// i.e. any standard varying-length section
// returns 1 on success, 0 on error
int scan_section_with_len(Marker m, std::istream &fin, std::ostream *fout)
{
    // marker
    if (fout) {
        fout->put(0xFF);
        fout->put(m.id);
    }
    int c, t;
    // section length, including these 2 bytes
    c = fin.get();
    t = fin.get();
    if (fout) {
        fout->put(c);
        fout->put(t);
    }
    unsigned len = c * 256 + t - 2;
    char *buf = (char *) malloc(len);
    if (buf) {
        if (!fin.read(buf, len)) {
            free(buf);
            return 0;
        }
        if (fout) {
            fout->write(buf, len);
        }
        free(buf);
    } else {
        qDebug("Failed to allocate %u bytes for buffer, copying byte-by-byte\n", len);
        for (unsigned i = 0; i < len; i++) {
            if ((c = fin.get()) != EOF) {
                return 0;
            }
            if (fout) {
                fout->put(c);
            }
        }
    }
    return 1;
}

// returns next character on success, anything < 0 on error
int
scan_section(Marker m, std::istream &fin, std::ostream *fheader, std::ostream &fbody,
             char safe, size_t &rst_cnt)
{
    //qDebug("scanning section %s (%02X %02X)\n", marker_name(m.id),
    //                                            0xFF, m.id);
    if (m.type == SOI || m.type == EOI) {
        if (fheader && m.type != EOI) { // EOI will be appended later during merging
            //fprintf(fheader, "%c%c", 0xFF, m.id);
            fheader->put(0xFF);
            fheader->put(m.id);
        }
        return fin.get();
    } else if (m.type != SOS) {
        if (!scan_section_with_len(m, fin, fheader)) {
            qDebug("Failed to read %s section\n", marker_name(m.type));
            return ERR_PARSING;
        }
        return fin.get();
    } else  { // m.type == SOS
        // first 2 bytes of SOS denote the length of the section header
        // in safe mode header is copied to body file
        if (!scan_section_with_len(m, fin, safe ? fheader : &fbody)) {
            fprintf(stderr, "Failed to read SOS section header\n");
            return -2;
        }
        // section header is followed by a bulk of entropy-coded data
        // data goes into body file
        int p = 0, c;
        rst_cnt = 1;
        while ((c = fin.get()) != EOF) {
            // section can contain FF 00 and RST markers
            if (p == 0xFF && (c == 0x00 || is_rst(c))) {
                fbody.put(p);
                fbody.put(c);
                rst_cnt += is_rst(c);
            } else if (p == 0xFF) {
                // encountered a marker, entropy-coded section ends
                //qDebug("SOS interrupted by marker %s (%02X %02X)\n", marker_name(c), 0xFF, c);
                if (rst_cnt) {
                    qDebug("SOS contains %lu RST markers\n", rst_cnt);
                } else {
                    qDebug("no RST markers were found\n");
                }
                fin.unget();
                break;
            } else { // p != 0xFF
                if (c != 0xFF) { // possible marker
                    fbody.put(c);
                }
            }
            p = c;
        }
        if (c == EOF) {
            qDebug("Error reading SOS section data: end marker not present\n");
            return -2;
        }
        return 0xFF; // loop should only stop on markers
    }

    // shouldn't get here
    qDebug("Failed to process section\n");
    return ERR_PARSING;
}

// returns 1 on success, 0 on error
int split_file(std::istream &fin, std::ostream *fh, std::ostream &fb, char safe, size_t &rst_cnt)
{
    int p = 0;
    int c;
    Marker m;

    while ((c = fin.get()) != EOF) {
        if (p == 0xFF) { // marker
            m = identify_marker(c);
            c = scan_section(m, fin, fh, fb, safe, rst_cnt);
            if (c < 0) { // can be an error - or an EOF
				if (c == EOF) {
					break;
				}
				if (c == ERR_PARSING) {
                    qDebug("A parsing error occurred\n");
				} else if (c == ERR_MEMORY) {
                    qDebug("A memory error occurred\n");
				} else if (c == ERR_CODING) {
                    qDebug("An error occurred while coding/decoding data\n");
				}
                qDebug("scan_section returned error code %d, exiting\n", c);
                return 0;
            }
        }
        p = c;
    }

    return 1;
}
