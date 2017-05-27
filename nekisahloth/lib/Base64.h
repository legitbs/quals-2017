#ifndef NEKISAHLOTH_LIB_BASE64
#define NEKISAHLOTH_LIB_BASE64

#include <string>

class Base64
{
	public:
		Base64(){base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";}
		~Base64(){}

		virtual std::string encode(unsigned char const* bytes_to_encode, unsigned int in_len);
		virtual unsigned int decode(std::string const& encoded_string, char **Buffer);

	private:
		int is_base64(unsigned char c) {return (isalnum(c) || (c == '+') || (c == '/'));}
		std::string base64_chars;
};

#endif
