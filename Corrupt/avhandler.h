#ifndef AVHANDLER_H
#define AVHANDLER_H

#include <QtGui/QImageIOHandler>
#include <QImage>
#include <QColor>
#include <QtDebug>

// workaround for libav UINT64_C error, not required if QMAKE_CXXFLAGS += -D__STDC_CONSTANT_MACROS added into .pro file
#ifndef INT64_C
    #define INT64_C(c) (c ## LL)
    #define UINT64_C(c) (c ## ULL)
#endif

extern "C" {
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libavdevice/avdevice.h"
    #include <libavfilter/avfiltergraph.h>
    #include <libavfilter/buffersink.h>
    #include <libswscale/swscale.h>
}

class AVHandler
{
public:
  static AVHandler* Instance();

  int open_input_file(const char *filename);
  bool ReadFrame2QImage(int64_t frame_timestamp, QImage& image);

// Other non-static member functions
private:
    AVHandler();                                 // Private constructor
    ~AVHandler();                                 // Private destructor
    AVHandler(const AVHandler&);                 // Prevent copy-construction
    AVHandler& operator=(const AVHandler&);      // Prevent assignment
    static AVHandler* _instance;
/* */
    quint32 fromYuv(qint16 y, qint16 u, qint16 v);
    void yuv420_rgb(AVFrame *frame, char *buffer, const QRect &roi);
    void yuv422_rgb(AVFrame *frame, char *buffer, const QRect &roi);
    void bgr24_rgb(AVFrame *frame, char *buffer, const QRect &roi);
/* */

    AVFormatContext *fmt_ctx;
    AVCodecContext *dec_ctx;
    AVFrame *frame;
    //static AVFilterContext *buffersink_ctx;
    //static AVFilterContext *buffersrc_ctx;
    //static AVFilterGraph *filter_graph;
    int video_stream_index;
    //int64_t last_pts = AV_NOPTS_VALUE;
};

#endif // AVHANDLER_H
