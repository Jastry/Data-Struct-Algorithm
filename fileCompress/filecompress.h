#include "huffmantree.h"


#define _CRT_SECURE_NO_WARNINGS 1


struct FileInformation{
	size_t _appearence_number;
	std::string _huffman_code;
	unsigned char _ch;

	FileInformation()
		:_appearence_number(0)
		, _ch(0)
	{}

	FileInformation(const FileInformation& other_info){
		_appearence_number = other_info._appearence_number;
		_huffman_code = other_info._huffman_code;
		_ch = other_info._ch;
	}

	FileInformation& operator()(const FileInformation& other_info){
		_appearence_number = other_info._appearence_number;
		_huffman_code = other_info._huffman_code;
		_ch = other_info._ch;

		return *this;
	}
	FileInformation(size_t sz)
		:_appearence_number(sz)
		, _ch(0)
	{}
	bool operator <(const FileInformation& other_info)const{
		return _appearence_number < other_info._appearence_number;
	}

	bool operator >(const FileInformation& other_info)const{
		return _appearence_number > other_info._appearence_number;
	}

	bool operator ==(const FileInformation& other_info)const{
		return _appearence_number == other_info._appearence_number;
	}

	bool operator !=(const FileInformation& other_info)const{
		return _appearence_number != other_info._appearence_number;
	}

	FileInformation operator+(const FileInformation& other_info){
		FileInformation* info = new FileInformation(_appearence_number + other_info._appearence_number);
		return *info;
	}

};
const int CHARNUM = 256;

class FileCompress{
private:
	struct ConfigInfo{
		unsigned char ch;
		long long count;
	};
public:
	FileCompress()
	{
		for (int i = 0; i < CHARNUM; ++i){
			_fileinfo[i]._ch = i;
		}
	}
	
	FileCompress(const std::string& filePath){
		/*prepar file*/
		FILE* fop = fopen(filePath.c_str(), "rb");
		assert(fop);
		size_t pos = filePath.find(".");
		std::string compress_file_name = filePath.substr(0, pos);
		compress_file_name += ".hzp";
		FILE* fin = fopen(compress_file_name.c_str(), "wb");
		assert(fin);

		for (int i = 0; i < CHARNUM; ++i){
			_fileinfo[i]._ch = i;
		}


		char ch = fgetc(fop);
		while (!feof(fop))
		{
			_fileinfo[(unsigned char)ch]._appearence_number++;
			ch = fgetc(fop);
		}
		/*create huffman tree*/
		HuffmanTree<FileInformation> ht(_fileinfo, CHARNUM, 0);
		HuffmanNode<FileInformation>* huffmanTreeRoot = ht.get_root();
		

		/*Use the number of characters appear to build Huffman coding */
		Conversion(huffmanTreeRoot);

		/*创建压缩文件写入解压信息*/
		ConfigurationFileInformation(fin);

		/*write hufuman code to compress file*/
		/*将原文件中字符以huffmancode的形式写入压缩文件*/
		CompressToFile(fop, fin);

		/*关闭源文件以及压缩文件*/
		fclose(fop);
		fclose(fin);
	}
	
	void unCompress(std::string& filename){
		FILE* fop = fopen(filename.c_str(), "rb");
		assert(fop);
		int p = filename.find(".");
		std::string substr = filename.substr(p, 4);
		if (substr != ".hzp"){
			//usage
			return;
		}
		std::string newfile = filename.substr(0, p);
		newfile += "1.txt";
		FILE* fwt = fopen(newfile.c_str(), "wb");
		assert(fwt);

		ConfigInfo info;
		for (;;){
			fread(&info, sizeof (ConfigInfo), 1, fop);
			if (-1 == info.count)
				break;
			_fileinfo[(unsigned char)info.ch]._ch = info.ch;
			_fileinfo[(unsigned char)info.ch]._appearence_number = info.count;
		}

		HuffmanTree<FileInformation> ht(_fileinfo, CHARNUM, 0);
		HuffmanNode<FileInformation>* rt = ht.get_root();
		HuffmanNode<FileInformation>* cur = rt;
		long long total_num = rt->_wt._appearence_number;

		int pos = 7;
		char value = 0;
		value = fgetc(fop);
		for (;;){
			if (value & (1 << pos))
				cur = cur->_right;
			else
				cur = cur->_left;
			--pos;

			if (NULL == cur->_left && NULL == cur->_right){
				fputc(cur->_wt._ch, fwt);
				cur = rt;
				if (--total_num == 0)
					break;
			}
			if (pos < 0){
				value = fgetc(fop);
				pos = 7;
			}
		}
		fclose(fop);
		fclose(fwt);
	}

protected:
	void Conversion(HuffmanNode<FileInformation>* huffman_node){
		if (NULL == huffman_node)	return;

		/* there can improve */
		std::stack< HuffmanNode < FileInformation >* > s_hf;
		unsigned char value = 0;

		while (huffman_node || !s_hf.empty()){
			while (huffman_node){
				s_hf.push(huffman_node);
				huffman_node = huffman_node->_left;
				if (huffman_node){
					huffman_node->_wt._huffman_code += (huffman_node->_prev)->_wt._huffman_code;
					huffman_node->_wt._huffman_code += "0";
				}
			}

			huffman_node = s_hf.top();
			if (NULL == huffman_node->_left && NULL == huffman_node->_right){
				value = huffman_node->_wt._ch;
				_fileinfo[value]._huffman_code = huffman_node->_wt._huffman_code;
			}
			s_hf.pop();
			huffman_node = huffman_node->_right;
			if (huffman_node){
				huffman_node->_wt._huffman_code += (huffman_node->_prev)->_wt._huffman_code;
				huffman_node->_wt._huffman_code += "1";
			}
		}
	}

	void CompressToFile(FILE* fop, FILE* fin){
		
		/*Put the file pointer in the starting position */
		if (fseek(fop, 0, SEEK_SET) != 0){
			perror("fseek");
			exit(2);
		}

		/*The characters currently read */
		char current_value = 0;
		int pos = 0;
		char ch = 0;
		while (!feof(fop)){
			/*memset(write_buf, 0, sizeof (write_buf));*/
			ch = fgetc(fop);
			int n = 0;
			std::string current_huffmancode;
				//current huffman char code
			current_huffmancode = _fileinfo[(unsigned char)ch]._huffman_code;
				//current huffman char size
				size_t current_char_size = current_huffmancode.size();
				
				for (size_t j = 0; j < current_char_size; ++j){
					current_value <<= 1;
					if ('1' == current_huffmancode[j]){
						current_value |= 1;
					}
					pos++;
					if (8 == pos){
						fputc(current_value, fin);
						current_value = 0;
						pos = 0;
					}
				}
			}
			if (pos != 0){
				current_value <<= 8 - pos;
				fputc(current_value, fin);
			}
	}

	void ConfigurationFileInformation(FILE* fin){

		ConfigInfo info;
		for (int i = 0; i < 256; ++i){
			if (_fileinfo[i]._appearence_number != 0){
				info.ch = _fileinfo[i]._ch;
				info.count = _fileinfo[i]._appearence_number;
				fwrite(&info,sizeof (ConfigInfo), 1, fin);
			}
		}
		info.count = -1;
		fwrite(&info, sizeof (ConfigInfo), 1, fin);
	}

protected:
	FileInformation _fileinfo[256];
};
void menu()
{	
	std::cout << "please enter your choice" << std::endl;
	std::cout << "exit : 0" << std::endl;
	std::cout << "compress : 1" << std::endl;
	std::cout << "uncompress : 2" << std::endl;
}

void testfile(){
	int input = 0;
	do {
		menu();
		std::cin >> input;
		std::string pathname;
		std::string unpathname;
		switch (input){
		case 0:
			break;
		case 1:
		{
			std::cout << "Please enter filename which you want to compress." << std::endl;
			std::cin >> pathname;
			FileCompress f(pathname);
		}
			break;
		case 2:
		{
				  FileCompress f;
				  std::cout << "Please enter filename which you want to uncompress." << std::endl;
				  std::cin >> unpathname;
				  f.unCompress(unpathname);
		}
			break;
		default:
			std::cout << "wrong enter plrease enter again!" << std::endl;
			break;
		}
	} while (input);
}

void Testfile(){
	std::string filename("123.mp4");

	FileCompress f(filename);

	std::string ufilename("123.hzp");
	f.unCompress(ufilename);
}
