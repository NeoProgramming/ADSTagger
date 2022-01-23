#include "TaggerCore.h"
#include <string>
#include <string.h>
#include <fstream>
#include <algorithm>

const wchar_t AdsTagger[] = _T("\\adstagger.ini");
const wchar_t RecentTags[] = _T("\\recent_tags.ini");

bool isTag(char c)
{
	return c >= '0'&&c <= '9' || c >= 'A'&&c <= 'Z' || c >= 'a'&&c <= 'z';
}

bool isVal(char c)
{
	return  c != ' ' && c != '\t' && c != '\0';
}

bool isDef(char c)
{
	return c == '=' || c == ':';
}

std::wstring GetTagsPath(LPCTSTR fpath)
{
	std::wstring adspath;
	if (_tcsstr(fpath, _T("\\\\?\\")))
		return fpath;
	if (!_tcsncmp(fpath, _T("\\\\"), 2)) {
		adspath = _T("\\\\?\\UNC\\");
		adspath += (fpath + 2);
	}
	else {
		adspath = _T("\\\\?\\");
		adspath += fpath;
	}
	adspath += _T(":Tags");
	return adspath;
}

TaggerCore Core;

TagIter TaggerCore::parseTags(const char* tags, std::list<Tag*> *pFileTags)
{
	enum EState {
		S_SP1,
		S_TAG,
		S_SP2,
		S_SP3,
		S_VAL
	} s = S_SP1;
	const char* start = tags;
	TagIter it = m_Tags.end(), first = m_Tags.end();
	bool found;

	while (1) {
		switch (s) {
		case S_SP1:	// waiting for the start of the tag
			if (isTag(*tags)) {
				s = S_TAG;
				start = tags;
			}
			break;
		case S_TAG:	// waiting for the end of the tag
			if (!isTag(*tags)) {
				std::string tag(start, tags - start);
				if(addTag(tag.c_str(), 1, &it))
					if (first == m_Tags.end())
						first = it;
				if (pFileTags)
					pFileTags->push_back(&(*it));
				if (isDef(*tags))	// if there is a ':' or '=', then we switch to waiting for the beginning of the value
					s = S_SP3;
				else
					s = S_SP2;
			}
			break;
		case S_SP2:	// waiting for the ':' or '=' sign or the beginning of the next tag
			if (isDef(*tags)) {
				s = S_SP3;
			}
			else if (isTag(*tags)) {
				s = S_TAG;
				start = tags;
			}
			break;
		case S_SP3:	// waiting for the beginning of the value
			if (isVal(*tags)) {
				s = S_VAL;
				start = tags;
			}
			break;
		case S_VAL:	// waiting for the end of the value
			if (!isVal(*tags)) {
				std::string val(start, tags - start);
				it->val = val;
				s = S_SP1;
			}
			break;
		}
		// exit from the middle, we need at least one iteration, for a null character it is valid
		if (!*tags)
			break;
		tags++;
	}
	return first;
}

void TaggerCore::loadFileTags(FileTags &f)
{
	// load file tags from ADS
	std::wstring tpath = GetTagsPath(f.m_fpath.c_str());
	std::ifstream t;
	t.open(tpath);
	if (t) {
		std::string line;
		std::getline(t, line);
		parseTags(line.c_str(), &f.m_tags);
	}
	t.close();
}

void TaggerCore::saveFileTags(FileTags &f)
{
	// save file tags to ADS
	std::wstring tpath = GetTagsPath(f.m_fpath.c_str());
	std::ofstream t;
	t.open(tpath, std::ofstream::out | std::ofstream::trunc);
	if (t) {
		std::string line;
		makeTags(f.m_tags, line);
		t << line;
	}
	t.close();
	std::filesystem::last_write_time(f.m_fpath, f.m_wtime);
}

void TaggerCore::makeTags(std::list<Tag*> &fileTags, std::string &tags)
{
	// make 
	for (auto &t : fileTags) {
		if (t->chk == -1) {
			if (!tags.empty())
				tags += " ";
			tags += t->tag;
			if (!t->val.empty()) {
				tags += ":";
				tags += t->val;
			}
		}
	}
	//
	for (auto &t : m_Tags) {
		if (t.chk == 1) {
			if (!tags.empty())
				tags += " ";
			tags += t.tag;
			// val of first file (!!!)
			if (!t.val.empty()) {
				tags += ":";
				tags += t.val;
			}
		}
	}
}

void TaggerCore::parseCommandLine()
{
	// parse command line
	auto cl = GetCommandLine();
	int nArgc = 0;
	LPWSTR *pArgv = ::CommandLineToArgvW(cl, &nArgc);
	
	for (int i = 1; i < nArgc; i++) {
		if (std::filesystem::exists(pArgv[i])) {
			FileTags f;
			f.m_fpath = pArgv[i];
			f.m_wtime = std::filesystem::last_write_time(pArgv[i]);
			loadFileTags(f);
			m_Files.push_back(f);
		}
	}
}

void TaggerCore::buildUsedTags()
{
	// convert count to check status
	for (auto &t : m_Tags) {
		if (t.chk == Core.m_Files.size())
			t.chk = 1;
		else if (t.chk != 0)
			t.chk = -1;
	}
}

void TaggerCore::init()
{
	wchar_t path[MAX_PATH]=_T("");
	GetModuleFileName(NULL, path, MAX_PATH);
	wchar_t *ls = _tcsrchr(path, '\\');
	if (ls)
		*ls = 0;
	m_AppPath = path;
	loadIni();
	parseCommandLine();
	buildUsedTags();
	loadRecentTags();
}

void TaggerCore::loadIni()
{

}

void TaggerCore::apply()
{
	for (auto &f : m_Files) 
		saveFileTags(f);
	saveRecentTags();
}

void TaggerCore::loadRecentTags()
{
	// load recent tags from ini
	std::ifstream t;
	t.open(m_AppPath + RecentTags);
	std::string line;
	while (t) {
		std::getline(t, line);
		if(!line.empty())
			addTag(line.c_str(), 0, nullptr);
	}
	t.close();
}

void TaggerCore::saveRecentTags()
{
	// save all used tags to ini
	std::string input;
	std::ofstream out(m_AppPath + RecentTags, std::ofstream::out | std::ofstream::trunc);
	for (Tag &t : m_Tags) {
		out << t.tag << "\n";
	}
	out.close();
}

bool TaggerCore::addTag(const char* t, int chk, TagIter *pIt)
{
	// find tag in comon list
	auto it = std::find_if(m_Tags.begin(), m_Tags.end(), [&t](Tag &a) {
		return a.tag == t;
	});

	// if not found - insert at end
	bool added = false;
	if (it == m_Tags.end()) {
		it = m_Tags.insert(it, Tag{ t, "", chk });
		added = true;
	}
	else if (chk != 0) {
		it->chk++;
	}
	if (pIt)
		*pIt = it;
	return added;
}

