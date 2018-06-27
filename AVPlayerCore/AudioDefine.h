#pragma once
typedef struct AudioParams
{
	int freq;  //ÆµÂÊ
	int channels;
	__int64	channel_layout;
	enum AVSampleFormat fmt;
	int frame_size;
	int bytes_per_sec;
}AudioParams;
