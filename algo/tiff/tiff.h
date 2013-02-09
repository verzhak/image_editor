
#ifndef TIFF_H
#define TIFF_H

// Типы ключей IFD
#define BYTE		1
#define ASCII		2
#define SHORT		3
#define LONG		4
#define RATIONAL	5
#define SBYTE		6
#define UNDEFINED	7
#define SSHORT		8
#define SLONG		9
#define SRATIONAL	10
#define FLOAT		11
#define DOUBLE		12

// Ключ IFD
struct __attribute__((packed)) s_ifd_key
{
	uint16_t tag;		// Тэг ключа
	uint16_t type;		// Тип ключа
	uint32_t count;		// Количество элементов типа "Type", хранящихся в ключе
	uint32_t value;		// Значение ключа (если (type * count) <= 4) или смещение в файле, по которому хранится значение ключа
};

struct __attribute__((packed)) s_header
{
	uint16_t magic;			// Магическая последовательность (0x4949 - файл использует little-endian последовательность байт; 0x4D4D - файл использует big-endian последовательность байт)
	uint16_t not_used42;	// 42
	uint32_t first_ifd;		// Смещение в файле IFD первой страницы
};

// Файлы формата TIFF могут использовать как little-endian последовательность байт, так и big-endian последовательность байт. Данная программа поддерживает только little-endian последовательность байт
#define BIG_ENDIAN_MAGIC 0x4D4D
#define LITTLE_ENDIAN_MAGIC 0x4949

// Теги

#define IMAGE_WIDTH_TAG				256 // Ширина изображения в пикселях
#define IMAGE_HEIGHT_TAG			257 // Высота изображения в строках пикселей
#define BITS_PER_SAMPLE_TAG			258 // Разрядность спектральных каналов
#define STRIP_OFFSETS_TAG			273 // Смещение кадров изображения
#define SAMPLES_PER_PIXEL_TAG		277 // Количество спектральных каналов
#define ROWS_PER_STRIP_TAG			278 // Количество строк изображения на кадр (последний кадр формируется по остаточному принципу)
#define STRIP_BYTE_COUNT_TAG		279 // Размеры в байтах кадров изображения
#define X_RESOLUTION_TAG			282 // Количество пикселей по горизонтали в единице измерения, описанной тегом RESOLUTION_UNIT_TAG
#define Y_RESOLUTION_TAG			283 // Количество пикселей по вертикали в единице измерения, описанной тегом RESOLUTION_UNIT_TAG

#define COMPRESSION_TAG				259 // Сжатие
#define NO_COMPRESSION_C			1	// Сжатие отсутствует

#define PHOTOMETRIC_INTERP_TAG		262 // Фотометрическая интерпретация каналов изображения
#define WHITE_IS_ZERO_PHIN			0	// Одноканальное изображения, белый цвет - 0
#define BLACK_IS_ZERO_PHIN			1	// Одноканальное изображения, черный цвет - 0
#define RGB_PHIN					2	// RGB-изображение

#define PLANAR_CONFIGURATION_TAG	284 // Принцип организации изображения 
#define CHUNKY_PLCON				1	// Построчная развертка

#define SAMPLE_FORMAT_TAG			339 // Способ представления спектральных яркостей
#define UINT_SF						1	// Беззнаковое целое
#define SINT_SF						2	// Знаковое целое
#define FLOAT_SF					3	// Вещественное число с плавающей точкой

#define RESOLUTION_UNIT_TAG			296 // Единица измерения
#define NO_UNIT_RU					1	// Единицы измерения не используются

#endif

