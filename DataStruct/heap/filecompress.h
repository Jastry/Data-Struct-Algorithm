#include "Huffman.h"
#include <string.h>
#include <string>
#include <stack>

const char INVALID = '#';
struct FileInformation{
	size_t _appearence_number;
	std::string _huffman_code;
	char _ch;

	FileInformation()
		:_appearence_number(0)
		,_ch(0)
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
		,_ch(INVALID)
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

public:
	FileCompress()
	{
		for (int i = 0; i < CHARNUM; ++i){
			_fileinfo[i]._ch = i;
		}
	}

	FileCompress(const std::string& filePath){
		FILE* fop = fopen(filePath.c_str(), "r");
		assert(fop);

		/*int fop_fd = open(filename, "r");
		if (fd < 0){
			perror("opene");
			exit(1);
		}*/

		
		for (int i = 0; i < CHARNUM; ++i){
			_fileinfo[i]._ch = i;
		}
		char readbuf[1024] = {0};

		for(;;){
			memset(readbuf, 0, sizeof(readbuf));
			size_t size = fread(readbuf, sizeof (char), 1024, fop);
			//ssize_t size = read(fop_fd, sizeof(buf), readbuf);
			if (0 == size)
				break;

			for (int i = 0; i < size; ++i){
				_fileinfo[readbuf[i]]._appearence_number++;
			}
		}

		fclose(fop);
	}
	void fileCompressing(){
		HuffmanTree<FileInformation> ht(_fileinfo, CHARNUM);
		HuffmanNode<FileInformation>* huffmanTreeRoot = ht.get_root();

		Conversion(huffmanTreeRoot);
	}
	void Conversion(HuffmanNode<FileInformation>* huffman_node){

		if (NULL == huffman_node)	return;

		huffman_node->_wt._huffman_code += "0";

		std::stack< HuffmanNode < FileInformation >* > s_hf;

		while (huffman_node || !s_hf.empty()){
			while (huffman_node){
				s_hf.push(huffman_node);
				huffman_node = huffman_node->_left;
				if (huffman_node){
					huffman_node->_wt._huffman_code += (huffman_node->_prev)->_wt._huffman_code;
					huffman_node->_wt._huffman_code += "1";
				}
			}

			huffman_node = s_hf.top();
			if (NULL == huffman_node->_left && NULL == huffman_node->_right){
				_fileinfo[huffman_node->_wt._ch]._huffman_code = huffman_node->_wt._huffman_code;
			}
			s_hf.pop();
			huffman_node = huffman_node->_right;
			if (huffman_node){
				huffman_node->_wt._huffman_code += (huffman_node->_prev)->_wt._huffman_code;
				huffman_node->_wt._huffman_code += "0";
			}
		}
	}

protected:
	FileInformation _fileinfo[256];
};

void testfile(){
	std::string pathname = "./log.txt";
	
	FileCompress f(pathname);
	f.fileCompressing();
}
