//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/6/18
// Time: 下午3:21
// To change this template use File | Settings | File Templates.
//

#include "token.h"

NS_HIVE_BEGIN

void Token::readConfig(const std::string& file, TokenMap& tokenMap){
	std::ifstream in(file);
	if(!in.is_open()){
		fprintf(stderr, "config file can not open: %s\n", file.c_str());
		return;
	}
	char buffer[1024];
	std::string str;
	while(!in.eof()){
		in.getline(buffer, 1024);
		str = buffer;
		split(str, "=", tokenMap);
	};
	in.close();
}
void Token::split(const std::string& str, const std::string& delimiter, TokenMap& tokenMap){
	if(str.empty()){
		return;
	}
	size_t pos = str.find_first_of(delimiter);
	if( pos == std::string::npos ){
		return;
	}
	std::string key = str.substr(0, pos);	// pos, len
	std::string value = str.substr(pos+1);
	trim(key);
	trim(value);
	tokenMap[key] = value;
}
void Token::trim(std::string& str){
	if(str.empty()){
		return;
	}
	replace(str, "\t", "");
	str.erase(0, str.find_first_not_of(" "));
	str.erase(str.find_last_not_of(" ")+1, str.length());
}
void Token::replace(std::string& str, const std::string& rp, const std::string& by){
	if(str.empty()){
		return;
	}
	size_t pos = str.find(rp, 0);
	while(pos != std::string::npos){
		str.replace(pos, rp.length(), by);
		pos = str.find(rp, pos);
	};
}

NS_HIVE_END
