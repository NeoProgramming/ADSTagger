#pragma once
#include <list>
#include <string>
#include <atlbase.h>
#include <atlstr.h>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <filesystem>

struct Tag {
	std::string tag;// utf8
	std::string val;// utf8
	int chk;
};
typedef Tag* TagPtr;
typedef std::list<Tag> TagList;
typedef std::list<Tag>::iterator TagIter;


struct FileTags {
	std::wstring m_fpath;
	std::filesystem::file_time_type m_wtime;
	std::list<Tag*> m_tags;	
};

class TaggerCore
{
public:
	void init();
	void apply();
	TagIter parseTags(const char* tags, std::list<Tag*> *pFileTags);
	void makeTags(std::list<Tag*> &fileTags, std::string &tags);
	void parseCommandLine();
	void loadRecentTags();
	void saveRecentTags();
	bool addTag(const char* tag, int chk, TagIter *pIt);
	void buildUsedTags();
	void loadFileTags(FileTags &f);
	void saveFileTags(FileTags &f);
	void loadIni();
public:
	std::wstring m_AppPath;
	std::list<FileTags> m_Files;
	std::list<Tag> m_Tags;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t> > m_converter;
};

extern TaggerCore Core;
