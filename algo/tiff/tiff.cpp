
#include "algo/tiff/tiff.h"

// TODO Возможный баг с чтением / сохранением s16

unsigned type_size[] = {0, 1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8};

void algo::load_tiff(const string & fname, vector<shared_ptr<c_bitmap> > & bmp_array)
{
	s_header header;
	shared_ptr<c_bitmap> bmp;
	shared_ptr<FILE> fl(fopen(fname.c_str(), "r"), fclose);
	FILE * p_fl = fl.get();

	fread(& header, 8, 1, p_fl);

	// Чтение заголовка

	if(
		header.not_used42 != 42
		||
		header.magic == BIG_ENDIAN_MAGIC
	  )
		throw string("Файл не является файлом формата TIFF или использует BIG ENDIAN порядок байт");

	uint16_t key_num;
	uint32_t ifd_off = header.first_ifd;
	unsigned u, v, t, tv, size, count, value, ph_interp, width = 0, height = 0, rows_per_strip = 0, samples_num = 0, strip_num = 0, page_sc = 0;
	shared_ptr<uint8_t> buffer;
	uint8_t * p_buffer;
	shared_ptr<unsigned> strip_offset, strip_size;
	unsigned * p_strip_offset, * p_strip_size;
	shared_ptr<uint16_t> byte_per_sample, sample_format;
	uint16_t * p_byte_per_sample, * p_sample_format;
	shared_ptr<s_ifd_key> key;
	stringstream stream;
	s_ifd_key * p_key;

	bmp_array.clear();

	while(ifd_off)
	{
		// Последовательное чтение каждого изображения

		// Чтение количества ключей в IFD обрабатываемого изображения
		fseek(p_fl, ifd_off, SEEK_SET);
		fread(& key_num, 2, 1, p_fl);

		// Обработка ключей IFD очередного изображения

		key.reset(new s_ifd_key[key_num]);
		p_key = key.get();
		fread(p_key, sizeof(s_ifd_key), key_num, p_fl);

		for(u = 0; u < key_num; u++, p_key++)
			switch(p_key->tag)
			{
				case IMAGE_WIDTH_TAG:
				{
					width = p_key->value;

					break;
				}
				case IMAGE_HEIGHT_TAG:
				{
					height = p_key->value;

					break;
				}
				case BITS_PER_SAMPLE_TAG:
				{
					value = p_key->value;
					count = p_key->count;
					byte_per_sample.reset(new uint16_t[count]);
					p_byte_per_sample = byte_per_sample.get();

					switch(count)
					{
						case 1:
						{
							p_byte_per_sample[0] = value >> 3;

							break;
						}
						default:
						{
							fseek(p_fl, value, SEEK_SET);
							fread(p_byte_per_sample, 2, count, p_fl);

							for(v = 0; v < count; v++)
								p_byte_per_sample[v] >>= 3;
						}
					}

					break;
				}
				case COMPRESSION_TAG:
				{
					if(p_key->value != NO_COMPRESSION_C)
						throw string("Сжатые изображения не поддерживаются");

					break;
				}
				case PHOTOMETRIC_INTERP_TAG:
				{
					switch(p_key->value)
					{
						case WHITE_IS_ZERO_PHIN:
						{
							ph_interp = WHITE_IS_ZERO_PHIN;

							break;
						}
						case BLACK_IS_ZERO_PHIN:
						{
							ph_interp = BLACK_IS_ZERO_PHIN;

							break;
						}
						case RGB_PHIN:
						{
							ph_interp = RGB_PHIN;

							break;
						}
						default:
							throw string("Неподдерживаемая фотометрическая интерпретация каналов изображения");
					}

					break;
				}
				case STRIP_OFFSETS_TAG:
				{
					count = p_key->count;
					strip_num = count;
					size = type_size[p_key->type];
					buffer.reset(new uint8_t[count * size]);
					p_buffer = buffer.get();
					strip_offset.reset(new unsigned [count]);
					p_strip_offset = strip_offset.get();

					fseek(p_fl, p_key->value, SEEK_SET);
					fread(p_buffer, size, count, p_fl);

					if(size == 2)
						for(v = 0; v < count; v++)
							p_strip_offset[v] = ((uint16_t *) p_buffer)[v];
					else
						for(v = 0; v < count; v++)
							p_strip_offset[v] = ((uint32_t *) p_buffer)[v];

					break;
				}
				case SAMPLES_PER_PIXEL_TAG:
				{
					samples_num = p_key->value;

					if(samples_num > 3)
						throw string("Не поддерживаются изображения, имеющие больше трех спектральных каналов");

					break;
				}
				case ROWS_PER_STRIP_TAG:
				{
					rows_per_strip = p_key->value;

					break;
				}
				case STRIP_BYTE_COUNT_TAG:
				{
					count = p_key->count;
					size = type_size[p_key->type];
					strip_size.reset(new unsigned [count]);
					p_strip_size = strip_size.get();
					buffer.reset(new uint8_t[count * size]);
					p_buffer = buffer.get();

					fseek(p_fl, p_key->value, SEEK_SET);
					fread(p_buffer, size, count, p_fl);

					if(size == 2)
						for(v = 0; v < count; v++)
							p_strip_size[v] = ((uint16_t *) p_buffer)[v];
					else
						for(v = 0; v < count; v++)
							p_strip_size[v] = ((uint32_t *) p_buffer)[v];

					break;
				}
				case PLANAR_CONFIGURATION_TAG:
				{
					if(p_key->value != CHUNKY_PLCON)
						throw string("Матричный способ хранения изображения не поддерживается");

					break;
				}
				case SAMPLE_FORMAT_TAG:
				{
					value = p_key->value;
					count = p_key->count;
					sample_format.reset(new uint16_t[count]);
					p_sample_format = sample_format.get();

					switch(count)
					{
						case 1:
						{
							p_sample_format[0] = value;

							break;
						}
						default:
						{
							fseek(p_fl, value, SEEK_SET);
							fread(p_sample_format, 2, count, p_fl);
						}
					}

					for(v = 0; v < count; v++)
						if(
							p_sample_format[v] != UINT_SF
							&&
							p_sample_format[v] != SINT_SF
							&&
							p_sample_format[v] != FLOAT_SF
						  )
							throw string("Неподдерживаемый тип спектральных каналов");

					break;
				}
			};

		// Проверка "качества" прочитанной информации об очередном изображении

		// BLACK_IS_ZERO и WHITE_IS_ZERO изображения содержат только один канал (тег SAMPLES_PER_PIXEL может быть опущен)
		if(ph_interp != RGB_PHIN)
			samples_num = 1;

		if(! (width && height && rows_per_strip && samples_num && strip_offset && byte_per_sample && strip_size))
			throw string("Неполный файл (отсутствуют важные поля)");

		if(samples_num != 3 && ph_interp == RGB_PHIN)
			throw string("Некорректный файл (некорректная фотометрическая интерпретация каналов изображения");

		// По умолчанию формат спектральных каналов - беззнаковое целое
		if(! sample_format)
		{
			sample_format.reset(new uint16_t[samples_num]);
			p_sample_format = sample_format.get();

			for(u = 0; u < samples_num; u++)
				p_sample_format[u] = UINT_SF;
		}

		// Чтение кадров изображения и заполнение внутреннего описателя изображения

		bmp.reset(new c_bitmap(height, width));

		for(v = 0, size = 0; v < strip_num; v++)
			if(p_strip_size[v] > size)
				size = p_strip_size[v];

		count = 1;
		buffer.reset(new uint8_t[count * size]);

		// Для каждого кадра повторять
		for(u = 0, v = 0; u < strip_num; u++)
		{
			p_buffer = buffer.get();

			fseek(p_fl, p_strip_offset[u], SEEK_SET);
			fread(p_buffer, 1, p_strip_size[u], p_fl);

			if(
				u == strip_num - 1
				&&
				height % rows_per_strip
			  )
				rows_per_strip = height % rows_per_strip;
			else
				rows_per_strip = rows_per_strip;

#define INT_CH(ch, fn_name, size, type)\
{\
case size:\
{\
bmp->pixel(v, t).fn_name(ch, * ((type *) p_buffer));\
p_buffer += size;\
\
break;\
}\
}

#define FLOAT_CH(ch, size, type)\
{\
case size:\
{\
bmp->pixel(v, t).set_real(ch, * ((type *) p_buffer));\
p_buffer += size;\
\
break;\
}\
}

#define SAMPLE(ch) \
{\
switch(p_sample_format[ch]) \
{\
case UINT_SF:\
{\
	switch(p_byte_per_sample[ch])\
	{\
		INT_CH(ch, set_u8, 1, uint8_t);\
		INT_CH(ch, set_u16, 2, uint16_t);\
		INT_CH(ch, set_u32, 4, uint32_t);\
	}\
\
	break;\
}\
\
case SINT_SF:\
{\
	switch(p_byte_per_sample[ch])\
	{\
		INT_CH(ch, set_s8, 1, int8_t);\
		INT_CH(ch, set_s16, 2, int16_t);\
		INT_CH(ch, set_s32, 4, int32_t);\
	}\
\
	break;\
}\
\
case FLOAT_SF:\
{\
	switch(p_byte_per_sample[ch])\
	{\
		FLOAT_CH(ch, 4, float);\
		FLOAT_CH(ch, 8, double);\
	}\
\
	break;\
}\
}\
}

			// Для каждой строки кадра повторять
			for(tv = 0; tv < rows_per_strip; v++, tv++)
				// Для каждого пикселя каждой строки кадра повторять
				for(t = 0; t < width; t++)
				{
					SAMPLE(R);

					if(samples_num == 1)
					{
						if(ph_interp == WHITE_IS_ZERO_PHIN)
							bmp->pixel(v, t).set(R, c_color::max_ch - bmp->pixel(v, t).get(R));

						bmp->pixel(v, t).set_all(bmp->pixel(v, t).get(R));

						continue;
					}

					SAMPLE(G);
					SAMPLE(B);
				}
		}

		bmp_array.push_back(bmp);

		// Переход к очередному изображению

		fseek(p_fl, ifd_off + 2 + sizeof(s_ifd_key) * key_num, SEEK_SET);
		fread(& ifd_off, 4, 1, p_fl);

		page_sc ++;
	}
}

void algo::save_tiff(vector<shared_ptr<c_bitmap> > & bmp_array, vector<e_channel_type> & ch_type, vector<t_channel> & ch, const string & fname)
{
	bool is_last_strip_small;
	unsigned u, v, t, height, width, sample_format, strip_num, strip_size, ch_num, ch_bits, strip_buf_size, samples_buf_size, size, bmp_num = bmp_array.size();
	unsigned ul_y, ul_x, lr_y, lr_x, strip_height, buf_size = 0;
	const uint16_t ifd_key_num = 14;
	const uint16_t ifd_size = ifd_key_num * sizeof(s_ifd_key) + 2 + 4;
	uint32_t next_ifd_off, pos;
	shared_ptr<uint8_t> buffer;
	uint8_t * p_buffer;
	shared_ptr<s_ifd_key> key(new s_ifd_key[ifd_key_num]);
	s_ifd_key * p_key = key.get();
	shared_ptr<FILE> fl(fopen(fname.c_str(), "w"), fclose); // TODO
	FILE * p_fl = fl.get();
	t_channel tch;
	e_channel_type tch_type;
	s_header header;
	c_bitmap * bmp;
	
	header.magic = LITTLE_ENDIAN_MAGIC;
	header.not_used42 = 42;
	header.first_ifd = sizeof(s_header);

	fwrite(& header, sizeof(header), 1, p_fl);

	p_key[0].tag = IMAGE_WIDTH_TAG;
	p_key[0].type = LONG;
	p_key[0].count = 1;

	p_key[1].tag = IMAGE_HEIGHT_TAG;
	p_key[1].type = LONG;
	p_key[1].count = 1;

	p_key[2].tag = BITS_PER_SAMPLE_TAG;
	p_key[2].type = SHORT;

	p_key[3].tag = COMPRESSION_TAG;
	p_key[3].type = SHORT;
	p_key[3].count = 1;
	p_key[3].value = NO_COMPRESSION_C;

	p_key[4].tag = PHOTOMETRIC_INTERP_TAG;
	p_key[4].type = SHORT;
	p_key[4].count = 1;

	p_key[5].tag = STRIP_OFFSETS_TAG;
	p_key[5].type = LONG;

	p_key[6].tag = SAMPLES_PER_PIXEL_TAG;
	p_key[6].type = SHORT;
	p_key[6].count = 1;

	p_key[7].tag = ROWS_PER_STRIP_TAG;
	p_key[7].type = SHORT;
	p_key[7].count = 1;

	p_key[8].tag = STRIP_BYTE_COUNT_TAG;
	p_key[8].type = LONG;

	p_key[9].tag = X_RESOLUTION_TAG;
	p_key[9].type = RATIONAL;
	p_key[9].count = 1;
	p_key[9].value = 0; // В предположении, что тег X_RESOLUTION игнорируется

	p_key[10].tag = Y_RESOLUTION_TAG;
	p_key[10].type = RATIONAL;
	p_key[10].count = 1;
	p_key[10].value = 0; // В предположении, что тег Y_RESOLUTION игнорируется

	p_key[11].tag = PLANAR_CONFIGURATION_TAG;
	p_key[11].type = SHORT;
	p_key[11].count = 1;
	p_key[11].value = CHUNKY_PLCON;

	p_key[12].tag = RESOLUTION_UNIT_TAG;
	p_key[12].type = SHORT;
	p_key[12].count = 1;
	p_key[12].value = NO_UNIT_RU;
		
	p_key[13].tag = SAMPLE_FORMAT_TAG;
	p_key[13].type = SHORT;

	for(u = 0; u < bmp_num; u++)
	{
		bmp = bmp_array[u].get();
		ul_y = bmp->aoi().ul_y();
		ul_x = bmp->aoi().ul_x();
		lr_y = bmp->aoi().lr_y();
		lr_x = bmp->aoi().lr_x();
		height = bmp->aoi().height();
		width = bmp->aoi().width();
		tch_type = ch_type[u];
		tch = ch[u];
		strip_height = 50;
		strip_num = height / strip_height;
		pos = ftell(p_fl) + ifd_size;

		if(height % strip_height || height < strip_height)
		{
			strip_num ++;
			is_last_strip_small = true;
		}
		else
			is_last_strip_small = false;

		strip_buf_size = strip_num * type_size[LONG];

		p_key[0].value = width;
		p_key[1].value = height;
		p_key[5].count = strip_num;
		p_key[5].value = pos;
		p_key[7].value = strip_height;
		p_key[8].count = strip_num;
		p_key[8].value = p_key[5].value + strip_buf_size;

		pos = p_key[8].value + strip_buf_size;

		switch(tch_type)
		{
			case CHANNEL_TYPE_U8:
			{
				ch_bits = 8;
				sample_format = UINT_SF;

				break;
			}
			case CHANNEL_TYPE_U16:
			{
				ch_bits = 16;
				sample_format = UINT_SF;

				break;
			}
			case CHANNEL_TYPE_U32:
			{
				ch_bits = 32;
				sample_format = UINT_SF;

				break;
			}
			case CHANNEL_TYPE_S8:
			{
				ch_bits = 8;
				sample_format = SINT_SF;

				break;
			}
			case CHANNEL_TYPE_S16:
			{
				ch_bits = 16;
				sample_format = SINT_SF;

				break;
			}
			case CHANNEL_TYPE_S32:
			{
				ch_bits = 32;
				sample_format = SINT_SF;

				break;
			}
			case CHANNEL_TYPE_FLOAT:
			{
				ch_bits = 32;
				sample_format = FLOAT_SF;

				break;
			}
			case CHANNEL_TYPE_DOUBLE:
			{
				ch_bits = 64;
				sample_format = FLOAT_SF;

				break;
			}
		}

		switch(tch)
		{
			case ALL_CHANNEL:
			{
				ch_num = 3;
				samples_buf_size = ch_num * type_size[SHORT];
				
				p_key[2].value = pos;
				p_key[4].value = RGB_PHIN;
				p_key[13].value = p_key[2].value + samples_buf_size;

				pos = p_key[13].value + samples_buf_size;

				break;
			}
			default:
			{
				ch_num = 1;

				p_key[2].value = ch_bits;
				p_key[4].value = BLACK_IS_ZERO_PHIN;
				p_key[13].value = sample_format;
			}
		}

		p_key[2].count = ch_num;
		p_key[6].value = ch_num;
		p_key[13].count = ch_num;

		strip_size = strip_height * width * ch_num * (ch_bits >> 3);
		size = width * height * (ch_bits >> 3) * ch_num;
		buf_size = (strip_buf_size << 1) + size + ((ch_num == 3) ? (samples_buf_size << 1) : 0);

		next_ifd_off = (u == bmp_num - 1) ? 0 : pos + size;

		buffer.reset(new uint8_t[buf_size]);
		p_buffer = buffer.get();

		fwrite(& ifd_key_num, sizeof(ifd_key_num), 1, p_fl);
		fwrite(p_key, sizeof(s_ifd_key), ifd_key_num, p_fl);
		fwrite(& next_ifd_off, sizeof(next_ifd_off), 1, p_fl);

		for(v = 0; v < strip_num; v++, pos += strip_size, p_buffer += 4)
			* ((uint32_t *) p_buffer) = pos;

		for(v = 0, strip_num --; v < strip_num; v++, p_buffer += 4)
			* ((uint32_t *) p_buffer) = strip_size;

		* ((uint32_t *) p_buffer) = is_last_strip_small ? size % strip_size : strip_size;

		if(ch_num == 3)
		{
			* ((uint16_t *) (p_buffer += 4)) = ch_bits;
			* ((uint16_t *) (p_buffer += 2)) = ch_bits;
			* ((uint16_t *) (p_buffer += 2)) = ch_bits;
			* ((uint16_t *) (p_buffer += 2)) = sample_format;
			* ((uint16_t *) (p_buffer += 2)) = sample_format;
			* ((uint16_t *) (p_buffer += 2)) = sample_format;
			p_buffer += 2;
		}
		else
			p_buffer += 4;

#define PUT_DATA(type_id, type, type_size, fun) \
case type_id: \
{\
	if(ch_num == 3) \
		for(v = ul_y; v < lr_y; v++) \
			for(t = ul_x; t < lr_x; t++, p_buffer += type_size) \
			{\
				* ((type *) p_buffer) = bmp->pixel(v, t).fun(R);\
				* ((type *) (p_buffer += type_size)) = bmp->pixel(v, t).fun(G);\
				* ((type *) (p_buffer += type_size)) = bmp->pixel(v, t).fun(B);\
			}\
	else \
		for(v = ul_y; v < lr_y; v++) \
			for(t = ul_x; t < lr_x; t++, p_buffer += type_size) \
				* ((type *) p_buffer) = bmp->pixel(v, t).fun(tch);\
\
	break;\
}

		switch(tch_type)
		{
			PUT_DATA(CHANNEL_TYPE_U8, uint8_t, 1, get_u8);
			PUT_DATA(CHANNEL_TYPE_U16, uint16_t, 2, get_u16);
			PUT_DATA(CHANNEL_TYPE_U32, uint32_t, 4, get_u32);
			PUT_DATA(CHANNEL_TYPE_S8, int8_t, 1, get_s8);
			PUT_DATA(CHANNEL_TYPE_S16, int16_t, 2, get_s16);
			PUT_DATA(CHANNEL_TYPE_S32, int32_t, 4, get_s32);
			PUT_DATA(CHANNEL_TYPE_FLOAT, float, 4, get_real);
			PUT_DATA(CHANNEL_TYPE_DOUBLE, double, 8, get_real);
		}

		fwrite(buffer.get(), buf_size, 1, p_fl);
	}
}

