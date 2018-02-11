#define _CRT_SECURE_NO_WARNINGS 1

#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <vector>
#include <assert.h>

using namespace std;

const size_t N = 2 * 32 * 1024;	//64k缓存
enum{ SlipBlock = 32 * 1024 };	//滑动窗口大小

typedef long long LongType;
typedef unsigned char Unchar;
class zipCompress{
public:
	zipCompress()
	{
		_window.reserve(N);
		_first = 0;
		_last = 0;
		for (int i = 0; i < 256; ++i){
			_cha[i] = i;
		}
	}

	string Compress(const string& filename)
	{
		return _zipCompress(filename);
	}

	string unCompress(const string& filename)
	{
		return _unCompress(filename);
	}
protected:
	string _zipCompress(const string& filename)
	{
		assert(filename.c_str());
		/*第一次压缩文件*/
		//////////////
		FILE* f_input = fopen(filename.c_str(), "rb+");
		assert(f_input);
		//////////////

		string first_compress_name = filename + ".ipzz";
		FILE* f_output = fopen(first_compress_name.c_str(), "wb+");
		assert(f_output);

		FILE* start_window = fopen(filename.c_str(), "rb+");
		assert(start_window);

		int ch = 0;
		ch = fgetc(f_input);
		LongType count = 0;
		Unchar value = 0;
		int pos = 7;
		while (!feof(f_input))
		{
			long OFFSET = ftell(f_input);	/*文件指针距离文件起始位置的偏移量*/
			long distance = 0;
			int length = 0;

			if (OFFSET > SlipBlock)
			{
				fseek(start_window, OFFSET - SlipBlock - 1, SEEK_SET);
				distance = SlipBlock;
			}
			else
			{
				fseek(start_window, 0, SEEK_SET);
				distance = OFFSET - 1;	/* 开始时已经读取一个字符在 value 中，所以 distance 从 OFFSET - 1 开始，即当前 f_input 指针处 */
			}
			if (distance > 1)
			{
				fseek(f_input, OFFSET - 1, SEEK_SET);
				length = findRepetion(start_window, f_input, distance);
				fseek(f_input, OFFSET, SEEK_SET);
			}

			if (length)	/*如果有重复字符串 length为该字符串长度，没有重复字符串 length 为 0 */
			{
				fseek(f_input, length - 1, SEEK_CUR);
				value |= (1 << pos);	
				--pos;
				if (pos < 0)
				{
					fputc(value, f_output);
					pos = 7;
					value = 0;
				}

				int flag = 15;
				while (flag >= 0)
				{
					if (distance & (1 << flag))
						value |= (1 << pos);
					else
						value &= (-(1) << pos);
					--pos;
					if (pos < 0)
					{
						fputc(value, f_output);
						pos = 7;
						value = 0;
					}
					--flag;
				}
				flag = 7;
				while (flag >= 0)
				{
					if (length & (1 << flag))
						value |= (1 << pos);
					else
						value &= (-(1 << pos));
					--pos;
				}
				count += 3;
			}
			else  /* 没有重复字符串，将该字符写入并给 count++ */
			{
				/*value先 将最后一位置 0*/
				value &= (-(1 << pos));
				--pos;
				if (pos < 0)
				{
					fputc(value, f_output);
					pos = 7;
					value = 0;
				}
				int flag = 7;
				while (flag >= 0)
				{
					if (ch & (1 << flag))
						value |= (1 << pos);
					else
						value &= (-(1 << pos));
					--pos;
					if (pos < 0)
					{
						fputc(value, f_output);
						value = 0;
						pos = 7;
					}
					--flag;
				}
				count++;
			}
			ch = fgetc(f_input);
		}
		if (pos != 7)
			fputc(value, f_output);
		fwrite(&count, 1, 8, f_output);
		fclose(f_input);
		fclose(f_output);
		fclose(start_window);
		return first_compress_name;
	}

	string _unCompress(const string& filename)
	{
		FILE* f_input = fopen(filename.c_str(), "rb+");
		assert(f_input);
		string unCompressFile = filename + ".uzip";
		FILE* f_output = fopen(unCompressFile.c_str(), "wb+");
		assert(f_output);
		FILE* start_window = fopen(filename.c_str(), "rb+");
		assert(start_window);

		LongType count;
		fseek(f_input, -8, SEEK_END);
		fread(&count, 1, 8, f_input);
		fseek(f_input, 0, SEEK_SET);
		/*uncompress*/

		int c = 0;
		int ch = 0;
		ch = fgetc(f_input);
		Unchar value = 0;
		int status = 0;
		int pos = 7;
		while (count > 0)
		{
			int distance = 0;
			int length = 0;
			status = ch & (1 << pos);
			--pos;
			if (pos < 0)
			{
				ch = fgetc(f_input);
				pos = 7;
			}
			if (status != 0)
			{
				/*还原 distance，连续读取两个字节*/
				int flag = 15;
				while (flag >= 0)
				{
					if (ch & (1 << pos))
						distance |= (1 << flag);
					else
						distance &= (-(1 << flag));
					--pos;
					if (pos < 0)
					{
						ch = fgetc(f_input);
						pos = 7;
					}
					--flag;
				}
				/*读取length*/
				flag = 7;
				while (flag >= 0)
				{
					if (ch & (1 << pos))
						length |= (1 << flag);
					else
						length &= (-(1 << flag));
					--pos;
					if (pos < 0)
					{
						ch = fgetc(f_input);
						pos = 7;
					}
					--flag;
				}
				/* 复制滑动窗口中的重复的字符 */
				int OFFSET = ftell(f_output);
				fseek(start_window, OFFSET - distance, SEEK_SET);
				while (length--)
				{
					int c = fgetc(start_window);
					fputc(c, f_output);
				}
				count -= 3;
			}
			else
			{
				int flag = 7;
				while (flag >= 0)
				{
					if (ch & (1 << pos))
						value |= (1 << flag);
					else
						value &= (-(1 << flag));
					--pos;
					if (pos < 0)
					{
						ch = fgetc(f_input);
						pos = 7;
					}
					--flag;
				}
				fputc(value, f_output);
				count--;
				value = 0;
			}
		}
		fclose(f_input);
		fclose(f_output);
		fclose(start_window);
		return unCompressFile;

	}
protected:
	int findRepetion(FILE* start_window, FILE* f_input, long& distance)
	{
		long OFFSET1 = ftell(start_window);
		long OFFSET2 = ftell(f_input);

		int ch = 0;
		if ((size_t)OFFSET2 > _last)
		{
			_window.clear();
			for (size_t i = 0; i < N; ++i)
			{
				ch = fgetc(f_input);
				if (ch == EOF)
					break;
				_window.push_back(ch);
			}
			_first = OFFSET1;
			_last = _window.size() + OFFSET1;
		}

		int length = getRepetionLength(f_input, distance, OFFSET1);
		return length;
	}

	int getRepetionLength(FILE* f_input, long& distance, long start_window_pos)
	{
		long OFFSET = ftell(f_input);
		vector<Unchar> v;
		if (!Getch(f_input, v))
			return 0;

		size_t size = OFFSET - start_window_pos - 1;
		size_t index = start_window_pos - _first;

		int length = 0;
		for (; index < size; ++index)
		{
			if (_window[index] == v[0]){
				size_t flag = index;
				size_t i = 0;
				while (flag < size && length < 255)
				{
					if (i == v.size() - 1)
					{
						if (!Getch(f_input, v))
						{
							break;
						}
					}
					if (_window[flag] == v[i])
					{
						length++;
						flag++;
						i++;
					}
					else
						break;
				}
				if (length >= 3)
				{
					distance = OFFSET - (index + _first);
					return length;
				}
				length = 0;
			}
		}
		return 0;
	}

	bool Getch(FILE* f_input, vector<Unchar>& v)
	{
		int ch = 0;
		ch = fgetc(f_input);
		v.push_back(ch);
		if (ch = EOF)
			return false;
		else
			return true;
	}
protected:
	vector<Unchar> _window;
	size_t _first;
	size_t _last;
	char _cha[256];
};
