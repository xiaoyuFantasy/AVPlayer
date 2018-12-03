#pragma once
#include <d3d9.h>
#include <dxva2api.h>
#include "DecoderDefine.h"
#include "Queue.h"

EXTERN_C{
#include <libavcodec\dxva2.h>
#include <libavutil\fifo.h>
#include <libavutil\avassert.h>
}

enum HWAccelID {
	HWACCEL_NONE = 0,
	HWACCEL_AUTO,
	HWACCEL_VDPAU,
	HWACCEL_DXVA2,
	HWACCEL_VDA,
	HWACCEL_VIDEOTOOLBOX,
	HWACCEL_QSV,
};

typedef struct InputFilter {
	AVFilterContext    *filter;
	struct InputStream *ist;
	struct FilterGraph *graph;
	uint8_t            *name;
	enum AVMediaType    type;   // AVMEDIA_TYPE_SUBTITLE for sub2video

	AVFifoBuffer *frame_queue;

	// parameters configured for this input
	int format;

	int width, height;
	AVRational sample_aspect_ratio;

	int sample_rate;
	int channels;
	uint64_t channel_layout;

	AVBufferRef *hw_frames_ctx;

	int eof;
} InputFilter;

typedef struct InputStream {
	int file_index;
	AVStream *st;
	int discard;             /* true if stream data should be discarded */
	int user_set_discard;
	int decoding_needed;     /* non zero if the packets must be decoded in 'raw_fifo', see DECODING_FOR_* */
#define DECODING_FOR_OST    1
#define DECODING_FOR_FILTER 2

	AVCodecContext *dec_ctx;
	AVCodec *dec;
	AVFrame *decoded_frame;
	AVFrame *filter_frame; /* a ref of decoded_frame, to be sent to filters */

	int64_t       start;     /* time when read started */
							 /* predicted dts of the next packet read for this stream or (when there are
							 * several frames in a packet) of the next frame in current packet (in AV_TIME_BASE units) */
	int64_t       next_dts;
	int64_t       dts;       ///< dts of the last packet read for this stream (in AV_TIME_BASE units)

	int64_t       next_pts;  ///< synthetic pts for the next decode frame (in AV_TIME_BASE units)
	int64_t       pts;       ///< current pts of the decoded frame  (in AV_TIME_BASE units)
	int           wrap_correction_done;

	int64_t filter_in_rescale_delta_last;

	int64_t min_pts; /* pts with the smallest value in a current stream */
	int64_t max_pts; /* pts with the higher value in a current stream */

					 // when forcing constant input framerate through -r,
					 // this contains the pts that will be given to the next decoded frame
	int64_t cfr_next_pts;

	int64_t nb_samples; /* number of samples in the last decoded audio frame before looping */

	double ts_scale;
	int saw_first_ts;
	AVDictionary *decoder_opts;
	AVRational framerate;               /* framerate forced with -r */
	int top_field_first;
	int guess_layout_max;

	int autorotate;

	int fix_sub_duration;
	struct { /* previous decoded subtitle and related variables */
		int got_output;
		int ret;
		AVSubtitle subtitle;
	} prev_sub;

	struct sub2video {
		int64_t last_pts;
		int64_t end_pts;
		AVFifoBuffer *sub_queue;    ///< queue of AVSubtitle* before filter init
		AVFrame *frame;
		int w, h;
	} sub2video;

	int dr1;

	/* decoded data from this stream goes into all those filters
	* currently video and audio only */
	InputFilter **filters;
	int        nb_filters;

	int reinit_filters;

	/* hwaccel options */
	enum HWAccelID hwaccel_id;
	enum AVHWDeviceType hwaccel_device_type;
	char  *hwaccel_device;
	enum AVPixelFormat hwaccel_output_format;

	/* hwaccel context */
	void  *hwaccel_ctx;
	void(*hwaccel_uninit)(AVCodecContext *s);
	int(*hwaccel_get_buffer)(AVCodecContext *s, AVFrame *frame, int flags);
	int(*hwaccel_retrieve_data)(AVCodecContext *s, AVFrame *frame);
	enum AVPixelFormat hwaccel_pix_fmt;
	enum AVPixelFormat hwaccel_retrieved_pix_fmt;
	AVBufferRef *hw_frames_ctx;

	/* stats */
	// combined size of all the packets read
	uint64_t data_size;
	/* number of packets successfully read for this stream */
	uint64_t nb_packets;
	// number of frames/samples retrieved from the decoder
	uint64_t frames_decoded;
	uint64_t samples_decoded;

	int64_t *dts_buffer;
	int nb_dts_buffer;

	int got_output;
} InputStream;

int dxva2_init(AVCodecContext *s, HWND hwnd);
void dxva2_uninit(AVCodecContext *s);
int dxva2_retrieve_data_call(AVCodecContext *s, AVFrame *frame);
int dxva2_retrieve_data_call2(AVCodecContext *s, AVFrame *dst, AVFrame *src);
