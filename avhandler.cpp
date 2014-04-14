#include "avhandler.h"

#include <fstream>

// Singletone implementation
AVHandler* AVHandler::_instance = NULL;

AVHandler* AVHandler::Instance() {
    if (_instance == NULL) {
        _instance = new AVHandler;
    }
    return _instance;
}

AVHandler::AVHandler() :
    fmt_ctx(avformat_alloc_context()),
    //dec_ctx(), // not sure if we need to init this
    frame(avcodec_alloc_frame()),
    timestamp(0)

{
    avcodec_register_all();
    av_register_all();
}

AVHandler::~AVHandler()
{
    av_free(frame);
    if (dec_ctx) avcodec_close(dec_ctx);
    avformat_close_input(&fmt_ctx);
}


int AVHandler::open_input_file(const char *filename)
{
    int ret;
    AVCodec *dec;

    if ((ret = avformat_open_input(&fmt_ctx, filename, NULL, NULL)) < 0) {
        qCritical("Cannot open input file\n");
        return ret;
    }

    if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
        qCritical("Cannot find stream information\n");
        return ret;
    }

    /* select the video stream */
    ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
    if (ret < 0) {
        qCritical() << "Cannot find a video stream in the input file\n";
        return ret;
    }
    video_stream_index = ret;
    dec_ctx = fmt_ctx->streams[video_stream_index]->codec;

    /* init the video decoder */
    if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
        qCritical() << "Cannot open video decoder\n";
        return ret;
    }
    if (timestamp > 0) {
        av_seek_frame(fmt_ctx, video_stream_index, timestamp, AVSEEK_FLAG_BYTE);
    } else {
        std::ofstream log("log.txt", std::ios_base::app);
        log << "timestamp = 0!\n";
        log.close();
        // Seek to the beginning
        av_seek_frame(fmt_ctx, video_stream_index, 0, AVSEEK_FLAG_ANY);
    }

    return 0;
}


bool AVHandler::ReadFrame2QImage(int64_t frame_timestamp, QImage& image)
{

    //av_seek_frame(fmt_ctx, video_stream_index, frame_timestamp, AVSEEK_FLAG_ANY);
    while (1)
    {
        AVPacket packet;
        if(av_read_frame(fmt_ctx, &packet) != 0) {
            qWarning("Libav image handler: frame not found.");
            av_free_packet(&packet);
            return false;
        }

        int success;
        avcodec_get_frame_defaults(frame);
        avcodec_decode_video2(dec_ctx, frame, &success, &packet);
        if (success) {
// 2 different methods of converting AVFrame to QImage
// second methond is not working yet and seems much heavy
#if 1
            // TODO move initialisation to constructor
            SwsContext *img_convert_ctx = NULL;
            // Convert the image format (init the context the first time)
            int w = dec_ctx->width;
            int h = dec_ctx->height;
            img_convert_ctx = sws_getCachedContext(img_convert_ctx,w, h, dec_ctx->pix_fmt, w, h, PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

            if(img_convert_ctx == NULL)
            {
               qCritical("Cannot initialize the conversion context!\n");
               return false;
            }

            // Allocate an AVFrame structure
            AVFrame *pFrameRGB;
            uint8_t *buffer;
            pFrameRGB = avcodec_alloc_frame();
            if(pFrameRGB==NULL)
                return false;

            // Determine required buffer size and allocate buffer
            int numBytes=avpicture_get_size(PIX_FMT_RGB24, dec_ctx->width, dec_ctx->height);
            buffer=new uint8_t[numBytes];

            // Assign appropriate parts of buffer to image planes in pFrameRGB
            avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24, dec_ctx->width, dec_ctx->height);

            sws_scale(img_convert_ctx, frame->data, frame->linesize, 0, dec_ctx->height, pFrameRGB->data, pFrameRGB->linesize);

            // Convert the frame to QImage
            //QImage *image1 = QImage(w,h,QImage::Format_RGB888);
            image = QImage(w, h, QImage::Format_RGB888);

            for(int y=0;y<h;y++)
               memcpy(image.scanLine(y),pFrameRGB->data[0]+y*pFrameRGB->linesize[0],w*3);
#else
            QByteArray bytes;
            // Convert colors if needed...
            switch (dec_ctx->pix_fmt) {
            case PIX_FMT_YUV420P:
                bytes.resize(frame->width * frame->height * 4);
                yuv420_rgb(frame, bytes.data(), QRect(0, 0, frame->width, frame->height));
                // *image = QImage(reinterpret_cast<const uchar*>(bytes.constData()), frame->width, frame->height, QImage::Format_RGB32);
                image = QImage((uchar*)bytes.data(), frame->width, frame->height, QImage::Format_RGB32);
                break;
            case PIX_FMT_BGR24:
                bytes.resize(frame->width * frame->height * 4);
                bgr24_rgb(frame, bytes.data(), QRect(0, 0, frame->width, frame->height));
                image = QImage(reinterpret_cast<const uchar*>(bytes.constData()), frame->width, frame->height, QImage::Format_RGB32);
                break;
            case PIX_FMT_YUVJ422P:
                bytes.resize(frame->width * frame->height * 4);
                yuv422_rgb(frame, bytes.data(), QRect(0, 0, frame->width, frame->height));
                image = QImage(reinterpret_cast<const uchar*>(bytes.constData()), frame->width, frame->height, QImage::Format_RGB32);
                break;
            default:
                image = QImage(frame->data[0], frame->width, frame->height, frame->linesize[0], QImage::Format_ARGB32);
                break;
            }
#endif
            image.detach();    // Copies the data which we are about to destroy
            av_free_packet(&packet);
            av_free(pFrameRGB); // NEW
            delete[] buffer; // NEW
            sws_freeContext(img_convert_ctx); // NEW
            return true;
        }
        qWarning("AVHandler: unable to decode frame.");
        av_free_packet(&packet);
    }
    return false;
}

void AVHandler::save_timestamp(const char *filename)
{
    std::ofstream fout(filename, std::ios_base::trunc);
    fout << fmt_ctx->pb->pos;
    fout.close();
}

void AVHandler::load_timestamp(const char *filename)
{
    std::ifstream fin(filename);
    std::ofstream log("log.txt", std::ios_base::app);
    if (fin) {
        fin >> timestamp;
  //      log << "timestamp read: "  << timestamp << std::endl;
    } else {
        timestamp = 0;
        log << "failed to read timestamp\n";
    }
    log.close();
    fin.close();
}

QRgb AVHandler::fromYuv(qint16 y, qint16 u, qint16 v)
{
    QColor pixel;
    y -= 16;
    u -= 128;
    v -= 128;
    pixel.setRed(qBound(0,   (298 * y           + 409 * v + 128) >> 8, 255));
    pixel.setGreen(qBound(0, (298 * y - 100 * u - 208 * v + 128) >> 8, 255));
    pixel.setBlue(qBound(0,  (298 * y + 516 * u           + 128) >> 8, 255));
    return pixel.rgb();
}

void AVHandler::yuv420_rgb(AVFrame *frame, char *buffer, const QRect &roi)
{
    QRgb *pixels = reinterpret_cast<QRgb*>(buffer);
    int stride1 = frame->linesize[0];
    int stride2 = frame->linesize[1];
    int stride3 = frame->linesize[2];
    for (int j = 0; j < roi.height(); ++j) {
        int wrap = j * roi.width();
        for (int i = 0; i < roi.width(); ++i) {
            qint16 y = frame->data[0][j * stride1 + i];
            qint16 u = frame->data[1][j/2 * stride2 + i/2];
            qint16 v = frame->data[2][j/2 * stride3 + i/2];
            pixels[wrap + i] = fromYuv(y, u, v);
        }
    }
}

void AVHandler::yuv422_rgb(AVFrame *frame, char *buffer, const QRect &roi)
{
    QRgb *pixels = reinterpret_cast<QRgb*>(buffer);
    int stride1 = frame->linesize[0];
    int stride2 = frame->linesize[1];
    int stride3 = frame->linesize[2];
    for (int j = 0; j < roi.height(); ++j) {
        int wrap = j * roi.width();
        for (int i = 0; i < roi.width(); i += 2) {
            qint16 y1 = frame->data[0][j * stride1 + i];
            qint16 y2 = frame->data[0][j * stride1 + i + 1];
            qint16 u = frame->data[1][j * stride2 + i/2];
            qint16 v = frame->data[2][j * stride3 + i/2];
            pixels[wrap + i] = fromYuv(y1, u, v);
            pixels[wrap + i + 1] = fromYuv(y2, u, v);
        }
    }
}

void AVHandler::bgr24_rgb(AVFrame *frame, char *buffer, const QRect &roi)
{
    QColor pixel;
    QRgb *pixels = reinterpret_cast<QRgb*>(buffer);
    quint8 *data = frame->data[0];
    int stride = frame->linesize[0];
    for (int j = 0; j < roi.height(); ++j) {
        int wrap = j * roi.width();
        for (int i = 0; i < roi.width(); ++i) {
            int idx = j * stride + i*3;
            pixel.setBlue(data[idx]);
            pixel.setGreen(data[idx + 1]);
            pixel.setRed(data[idx + 2]);
            pixels[wrap + i] = pixel.rgb();
        }
    }
}
