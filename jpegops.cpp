#include "jpegops.h"

#include <cstdio>
#include <cstdlib>
#include <jpeglib.h>
#include <setjmp.h>
#include <iostream>
#include <cstring>
int change_qual = 1;
JSAMPLE to_grayscale(JSAMPLE r, JSAMPLE g, JSAMPLE b)
{
    using std::cout;
    using std::flush;

    JSAMPLE tmp,tmp1;
    tmp = 0.299 * r + 0.587 * g + 0.114 * b;
    tmp = (0.299 * r + 0.587 * g + 0.114 * b) || 0xFC;
  //  cout <<" tmpls"<<  tmp <<" "<<(int)(tmp1)<<"\n"<<flush;

    return (0.299 * r + 0.587 * g + 0.114 * b) ;
}

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */
  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  my_error_ptr myerr = (my_error_ptr) cinfo->err;
  (*cinfo->err->output_message) (cinfo);
  longjmp(myerr->setjmp_buffer, 1);
}

void decompress_JPEG(jpeg_decompress_struct &cinfo,
              size_t &image_width, size_t &image_height,
              JSAMPLE *image_buffer)
{
    jpeg_read_header(&cinfo, TRUE);
    cinfo.out_color_space = JCS_RGB;
    jpeg_start_decompress(&cinfo);

    image_width = cinfo.output_width;
    image_height = cinfo.output_height;
    int row_stride = cinfo.output_width * cinfo.output_components;
    //bool grayscale = (cinfo.output_components == 1);
    JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)
        ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    size_t cur_buffer_pos = 0;
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        // put scanline from buffer into image_buffer
        //if (grayscale) {
        //    for (size_t i = 0; i < cinfo.image_width; i++) {
        //        image_buffer[cur_buffer_pos + 3 * i + 0] = buffer[0][i];
        //        image_buffer[cur_buffer_pos + 3 * i + 1] = buffer[0][i];
        //        image_buffer[cur_buffer_pos + 3 * i + 2] = buffer[0][i];
        //    }
        //    cur_buffer_pos += row_stride * 3;
        //} else {
            memcpy(image_buffer + cur_buffer_pos, buffer[0], row_stride);
            cur_buffer_pos += row_stride;
        //}
    }

    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

}

void set_compress_params(jpeg_compress_struct &cinfo,
                         size_t image_width, size_t image_height,
                         int lum_quality, int chrom_quality,
                         size_t rst_block_size,
                         bool grayscale)
{

        using std::cout;
        using std::flush;

    cinfo.image_width = image_width;
    cinfo.image_height = image_height;
    if (grayscale) {
        cinfo.input_components = 1;
        cinfo.in_color_space = JCS_GRAYSCALE;
    } else {
        cinfo.input_components = 3;
        cinfo.in_color_space = JCS_RGB;
    }
    cinfo.smoothing_factor = 100;
    jpeg_set_defaults(&cinfo);

    //jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);
    if (change_qual>0 ) {
  //      lum_quality -=10;
  //      chrom_quality -=10;
        change_qual = -1;

        cout <<"\n down "<< lum_quality << "\n" <<flush;


    }
       else {
//        lum_quality +=10;
//        chrom_quality +=10;
        change_qual = 1;
        cout <<"\n up "<< lum_quality <<"\n" << flush;

    }
    cinfo.q_scale_factor[0] = jpeg_quality_scaling(lum_quality);
    cinfo.q_scale_factor[1] = jpeg_quality_scaling(chrom_quality);
    jpeg_default_qtables(&cinfo, true);
    cinfo.restart_interval = rst_block_size;
    cinfo.comp_info[0].h_samp_factor = 1; //for Y
    cinfo.comp_info[0].v_samp_factor = 1;
    cinfo.comp_info[1].h_samp_factor = 1; //for Cb
    cinfo.comp_info[1].v_samp_factor = 1;
    cinfo.comp_info[2].h_samp_factor = 1; //for Cr
    cinfo.comp_info[2].v_samp_factor = 1;
}

void compress_JPEG(jpeg_compress_struct &cinfo,
                   size_t image_width,
                   JSAMPLE *image_buffer,
                   bool grayscale)
{
    jpeg_start_compress(&cinfo, TRUE);

    int row_stride;
    JSAMPLE *buffer;

    if (grayscale) {
        row_stride = image_width;
        buffer = new JSAMPLE[image_width * cinfo.image_height];
        for (size_t row = 0; row < cinfo.image_height; row++) {
            for(size_t col = 0; col < image_width; col++) {
                buffer[row * image_width + col] = to_grayscale(
                            image_buffer[row * image_width * 3 + col * 3 + 0],
                            image_buffer[row * image_width * 3 + col * 3 + 1],
                            image_buffer[row * image_width * 3 + col * 3 + 2]);
            }
        }
    } else {
        row_stride = image_width * 3;
        buffer = image_buffer;
    }

    JSAMPROW row_pointer[1];

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = & buffer[cinfo.next_scanline * row_stride];
        (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    if (grayscale) {
        delete[] buffer;
    }

    jpeg_finish_compress(&cinfo);
}

int
read_JPEG_file (void* image_buffer_ptr,
                size_t &image_width, size_t &image_height,
                const char *filename)
{
    JSAMPLE* image_buffer = (JSAMPLE *) image_buffer_ptr;
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    FILE * infile;		/* source file */

    if ((infile = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", filename);
        return 0;
    }

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        fprintf(stderr, "Error occurred, aborting\n");
        return 0;
    }
    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, infile);
    decompress_JPEG(cinfo, image_width, image_height, image_buffer);

    fclose(infile);
    return 1;
}

int read_JPEG_mem(void *image_buffer_ptr,
                  size_t &image_width, size_t &image_height,
                  void *src, unsigned long bufsize)
{
    JSAMPLE* image_buffer = (JSAMPLE *) image_buffer_ptr;
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        fprintf(stderr, "Error occurred, aborting\n");
        return 0;
    }
    jpeg_create_decompress(&cinfo);

    jpeg_mem_src(&cinfo, (unsigned char *) src, bufsize);
    decompress_JPEG(cinfo, image_width, image_height, image_buffer);

    return 1;
}

void
write_JPEG_file (void *image_buffer_ptr,
                 size_t image_width, size_t image_height,
                 const char * filename,
                 int lum_quality, int chrom_quality,
                 size_t rst_block_size,
                 bool grayscale)
{
    JSAMPLE *image_buffer = (JSAMPLE *) image_buffer_ptr;
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE * outfile;		/* target file */
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    if ((outfile = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "Can't open file %s\n", filename);
        exit(1);
    }

    set_compress_params(cinfo, image_width, image_height,
                        lum_quality, chrom_quality,
                        rst_block_size, grayscale);
    jpeg_stdio_dest(&cinfo, outfile);
    compress_JPEG(cinfo, image_width, image_buffer, grayscale);

    fclose(outfile);

    jpeg_destroy_compress(&cinfo);

}

void write_JPEG_mem(void *image_buffer_ptr,
                    size_t image_width, size_t image_height,
                    void **dest, unsigned long *bufsize,
                    int lum_quality, int chrom_quality, size_t rst_block_size,
                    bool grayscale)
{
    JSAMPLE *image_buffer = (JSAMPLE *) image_buffer_ptr;
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    set_compress_params(cinfo, image_width, image_height,
                        lum_quality, chrom_quality,
                        rst_block_size, grayscale);
    jpeg_mem_dest(&cinfo,(unsigned char **) dest, bufsize);
    compress_JPEG(cinfo, image_width, image_buffer, grayscale);

    jpeg_destroy_compress(&cinfo);
}
