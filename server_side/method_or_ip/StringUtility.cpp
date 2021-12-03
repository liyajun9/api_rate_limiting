#include <stdio.h>
#include <algorithm>
#include "StringUtility.h"

using namespace std;

namespace my_ns
{

/** 功能: 删除在strValue中两头的无效字符 */
string TrimBothSides(const string& strValue, 
									const std::string& strTarget)
{
	if (strValue.size() <= 0)
		return "";
	
	size_t dwBeginPos, dwEndPos;
	size_t dwStrLen = strValue.length();
	
	for (dwBeginPos = 0 ; dwBeginPos < dwStrLen; dwBeginPos++)
	{
		// 如果该字符不在要删除的字符串中
		if (string::npos == strTarget.find(strValue.at(dwBeginPos))) 
			break;
	}
	
	for (dwEndPos = dwStrLen - 1 ; dwEndPos > 0 ; dwEndPos--)
	{
		// 如果该字符不在要删除的字符串中
		if (string::npos == strTarget.find(strValue.at(dwEndPos))) 
			break;
	}
	
	if (dwBeginPos > dwEndPos)
		return "";
	
	return strValue.substr(dwBeginPos, dwEndPos - dwBeginPos + 1);
}

/** 功能: 删除在strValue中左边的无效字符 */
string TrimLeft(const string& strValue, 
							   const std::string& strTarget)
{
	if (strValue.size() <= 0)
		return "";
	
	size_t dwBeginPos;
	size_t dwStrLen = strValue.length();
	for (dwBeginPos = 0 ; dwBeginPos < dwStrLen; dwBeginPos++)
	{
		// 如果该字符不在要删除的字符串中
		if (string::npos == strTarget.find(strValue.at(dwBeginPos))) 
			break;
	}
	
	if (dwBeginPos == dwStrLen - 1)
		return "";
	
	return strValue.substr(dwBeginPos);
}

/** 功能: 删除在strValue中右边的无效字符 */
string TrimRight(const string& strValue, 
								const std::string& strTarget)
{
	if (strValue.size() <= 0)
		return "";
	
	size_t dwEndPos;
	size_t dwStrLen = strValue.length();
	for (dwEndPos = dwStrLen - 1 ; dwEndPos > 0 ; dwEndPos--)
	{
		// 如果该字符不在要删除的字符串中
		if (string::npos == strTarget.find(strValue.at(dwEndPos))) 
			break;
	}
	
	if (dwEndPos == 0)
		return "";
	
	return strValue.substr(0, dwEndPos + 1);
}

/** 功能: 把一个字符串划分成多个字符串
 *  参数: 
 *  输入参数 const std::string& strMain         主字符串
 *  输入参数 const  std::string& strSpliter     字符串分界符号
 *  输出参数 std::vector<std::string>& strList 分解后的结果
 */
void SplitString(const std::string& strMain, 
				 const  std::string& strSpliter, 
				 std::vector<std::string>& strList)
{
	strList.clear();

	if (strMain.empty() || strSpliter.empty())
		return;
	
	// 从下面开始都使用处理后的string
	string strMainTemp = TrimBothSides(strMain, " \r\n\t");
	
	// 要划分的字符串或分界符为空，说明调用该函数不正确
	if (strMainTemp.empty() || strSpliter.empty())
		return;
	
	size_t nPos = strMainTemp.find(strSpliter);

    while (string::npos != nPos)
	{
		strList.push_back(strMainTemp.substr(0,nPos));
		strMainTemp.erase(0, nPos+strSpliter.length());
		strMainTemp = TrimBothSides(strMainTemp, " \r\n\t");
		nPos = strMainTemp.find(strSpliter);
	}
	
	strMainTemp = TrimBothSides(strMainTemp, " \r\n\t");

	// 最后一项要求不为空才加入集合
	if(!strMainTemp.empty())
		strList.push_back(strMainTemp);
}

/** 功能:  简单的分词函数：把一个字符串按分隔符号划分成多个字符串 */
void SplitString(const std::string& strMain, 
				 std::vector<std::string>& strList)
{
	strList.clear();
	if (strMain.empty())
		return;

    string strTemp = strMain;
    int    dwBeginPos;
    int    dwEndPos;
	do
	{
        dwBeginPos = 0;
        while((size_t)dwBeginPos < strTemp.size() && IsSeparator(strTemp[dwBeginPos]))
            ++dwBeginPos;

        dwEndPos = dwBeginPos;
        while((size_t)dwEndPos < strTemp.size() && !IsSeparator(strTemp[dwEndPos]))
            ++dwEndPos;

        if (dwEndPos > dwBeginPos)
        {
            strList.push_back(strTemp.substr(dwBeginPos, dwEndPos - dwBeginPos));
            strTemp = strTemp.substr(dwEndPos);
        }
        else
        {
            break;
        }
	}
    while ( strTemp.size() > 0);
}

/** 把字符串A中的子串B替换为C */
string ReplaceStr(const string& strValue, 
								 const string& oldStr, 
								 const string& newStr)
{
	string strRet = strValue;

	// 找到第一个Target
	size_t dwPos = strValue.find(oldStr);  
	size_t dwNextPos;

    while (string::npos != dwPos)
	{
		dwNextPos = dwPos + oldStr.size();
		if (strRet.size() >= dwNextPos)
		{
			strRet.erase(dwPos, oldStr.size());
			strRet.insert(dwPos, newStr);
		}

		dwPos = strRet.find(oldStr, dwPos+newStr.size());
	}
	return strRet;
}

/** 把一个集合中的字符转换为对应的另一个集合中字符
 *  例如：把 <变为[, > 变为]
 *        strNew = ReplaceCharSet(strOld, "<>", "[]");
 */
string ReplaceCharSet(const string& strOld, 
							 string strFromSet, 
							 string strToSet)
{
	std::string strResult;
	for (unsigned int i = 0; i < strOld.size(); i++)
	{
		char ch = strOld[i];

		size_t dwPos = strFromSet.find(ch);
		if (dwPos == string::npos)
		{
			// 没有找到，说明不需要替换，直接复制过去
			strResult += ch;
		}
		else if (dwPos < strToSet.size())
		{
			// 找到，替换
			strResult += strToSet[dwPos];
		}
	}
	return strResult;

}

/** 把数字串按三位三位用分割号分隔123456789  123,456,789*/
string SeperateDigist(const string& strValue, const string& seperStr)
{
	string strRet = strValue;
	if(strValue.empty() || seperStr.empty())
		return strRet;
	int dwOldStrLen = strValue.size();
	dwOldStrLen -= 3;
	while (dwOldStrLen > 0)
	{
		strRet.insert(dwOldStrLen, seperStr);
		dwOldStrLen -= 3;
	}
	return strRet;
}

	
/** 替换字符串中的某些子字符串 */
string ReplaceStrSet(const string& strRawData, 
								    map<string, string>& mSet)
{
	string strRet = strRawData;
	std::map<std::string, std::string>::iterator it;
	for (it = mSet.begin(); it != mSet.end(); it++)
	{
		strRet = ReplaceStr(strRet, it->first, it->second);
	}
	return strRet;
}

/** 把字符串A中的子符删除 */
std::string RemoveChar(std::string& strValue, char ch)
{
	string strTmp = strValue;

	// 删除所有的字符ch
	strTmp.erase(std::remove(strTmp.begin(), strTmp.end(), ch), strTmp.end());

	return strTmp;
}

std::string RemoveChars(std::string& strRawData, const std::string & chs)
{
    string strTmp = strRawData;

    // 删除所有的字符ch
    for (size_t i = 0; i < chs.size(); ++i)
    {
        strTmp.erase(std::remove(strTmp.begin(), strTmp.end(), chs[i]), strTmp.end());
    }

    return strTmp;
}

/** 把字符串A中的某个子字符串删除 */
std::string RemoveStr(std::string& strRawData, std::string strSubStr, bool bNeedSeparator)
{
	int dwPos;
	string strTemp = strRawData;
	string strRet;
	
	do
	{
		dwPos = strTemp.find(strSubStr);
		if (dwPos == -1)
		{
			strRet += strTemp;
			break;
		}
        else
		{
            if (bNeedSeparator)
            {
                bool bHasPreSeparator = false;
                bool bHasPostSeparator = false;

                if (0 == dwPos)
                    bHasPreSeparator = true;
                else if (dwPos > 0 && IsSeparator(strTemp[dwPos-1]))
                    bHasPreSeparator = true;

                if (dwPos+strSubStr.size() >= strTemp.size())
                    bHasPostSeparator = true;
                else if (IsSeparator(strTemp[dwPos+strSubStr.size()]))
                    bHasPostSeparator = true;

                if (bHasPreSeparator && bHasPostSeparator)
                {
                    // 前后都有分界符，那么认为是可以删除的
			        strRet += strTemp.substr(0, dwPos);
			        strTemp = strTemp.substr(dwPos+strSubStr.size());
                }
                else
                {
                    // 如果前或后没有分界符，那么认为是不可以删除的
			        strRet += strTemp.substr(0, dwPos+strSubStr.size());
			        strTemp = strTemp.substr(dwPos+strSubStr.size());                    
                }
            }
            else
            {
			    strRet += strTemp.substr(0, dwPos);
			    strTemp = strTemp.substr(dwPos+strSubStr.size());
            }
		}
	}
	while (!strTemp.empty());
	return strRet;

}

/** 删除字符串中的某些子字符串 */
string RemoveStrSet(const string& strRawData, 
									                         set<string>& sStrSet,
                                                             bool bNeedSeparator)
{
    std::set<std::string>::iterator it;
    string strTmp = strRawData;


    it = sStrSet.end();
    for(it--; it != sStrSet.begin(); it--)
        strTmp = RemoveStr(strTmp, *it, bNeedSeparator);

    if (it == sStrSet.begin())
        strTmp = RemoveStr(strTmp, *it, bNeedSeparator);
    return strTmp;
}

/** 删除一个字符串中的一段子字符串，包括strBegin和strEnd也会被删除 */
string DeleteSubstr(std::string& strRawData, 
								   std::string strBegin, 
								   std::string strEnd)
{
	int dwPosBegin;
	int dwPosEnd;
	string strTemp = strRawData;
	string strRet;
	
	do
	{
		dwPosBegin = strTemp.find(strBegin);
		dwPosEnd = strTemp.find(strEnd);

        if ((dwPosBegin != -1) && (dwPosEnd != -1) && (dwPosEnd > dwPosBegin))
		{
            // 开始和结束字符串都找到的情况
			strRet += strTemp.substr(0, dwPosBegin);
			strTemp = strTemp.substr(dwPosEnd+strEnd.size());
			continue;
		}
        else
        {
            strRet += strTemp;
            break;
        }
	}
	while (!strTemp.empty());
	return strRet;
}

/** 从一个字符串中提取出关注的部分，例如"[*]" */
string ExtractSubstr( const string& strInput, string& strPattern)
{
    // 如果参数为空，或者参数为'*'，或者在参数中没有找到*这个子串，则返回源串
    if ((strPattern.empty())  || ("*" == strPattern))
        return strInput;

    // 找到 strInput 中的*号所在的位置
    string::size_type pos = strPattern.find("*");
    if (string::npos == pos)
        return strInput;

    string strLeft  = strPattern.substr(0, pos);
    string strRight = strPattern.substr(pos + 1, strPattern.length() - pos + 1);

    // 在输入串中找*左边子串的位置
    string::size_type leftPos = 0;
    string::size_type rightPos =  strInput.length() - 1;

    if (!strLeft.empty())
    {
        leftPos = strInput.find(strLeft);
        if (string::npos != leftPos)
        {        
            leftPos += strLeft.length();
        }
        else
        {
            leftPos = 0;
        }
    }

    if (!strRight.empty())
    {
        rightPos = strInput.rfind(strRight);
        if (string::npos != rightPos)
        {
            rightPos -= 1;
        }
        else
        {
            rightPos =  strInput.length() - 1;
        }
    }

    return strInput.substr(leftPos, rightPos - leftPos + 1);
}

/** 把字符串中一些需要转义的字符做转义处理 */
string QuoteWild(const char* strRaw)
{
	if (NULL == strRaw)
		return "";

	char* pcDest = new char[strlen(strRaw)*2];
  	int i = 0;
	int j = 0;

	while (strRaw[i]) 
	{
		switch((unsigned char)strRaw[i]) 
		{
			case '\n':
				pcDest[j++] = '\\';
				pcDest[j++] = 'n';
				break;
			case '\t':
				pcDest[j++] = '\\';
				pcDest[j++] = 't';
				break;
			case '\r':
				pcDest[j++] = '\\';
				pcDest[j++] = 'r';
				break;
			case '\b':
				pcDest[j++] = '\\';
				pcDest[j++] = 'b';
				break;
			case '\'':
				pcDest[j++] = '\\';
				pcDest[j++] = '\'';
				break;
			case '\"':
				pcDest[j++] = '\\';
				pcDest[j++] = '\"';
				break;
			case '\\':
				pcDest[j++] = '\\';
				pcDest[j++] = '\\';
				break;
			default:
				// 小于32是控制字符，都转换为空格(32)
				if ((unsigned char)strRaw[i] >= 32) 
					pcDest[j++] = strRaw[i];
				else 
					pcDest[j++] =32;
		}
		i++;
	}
	pcDest[j] = 0;

	string strRet;
	strRet.assign(pcDest);
	delete [] pcDest;
	
	return strRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 编码相关的字符串内容处理

//utf8字符长度1-6，可以根据每个字符第一个字节判断整个字符长度
//0xxxxxxx
//110xxxxx 10xxxxxx
//1110xxxx 10xxxxxx 10xxxxxx
//11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
//111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
//1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

//定义查找表，长度256，表中的数值表示以此为起始字节的utf8字符长度
static unsigned char szUtf8LookingTbl[] = 
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 1
};
#define UTFLEN(x)  szUtf8LookingTbl[(x)]

size_t Utf8StringLength(const char * strSrc)
{
    size_t len = 0; 
    for(const char *p = strSrc; *p!=0; ++len)
    {
        p += UTFLEN((unsigned char)*p);
    }
    return len;
}


bool Utf8SubString(const std::string & strSrc, unsigned int udwStart, unsigned int udwEnd, std::string & strDest)
{
    strDest.clear();

    const char *sp = strSrc.c_str();
    for(unsigned int i = 0; i < udwStart; ++i)
    {
        if (*sp == '\0')
        {
            break;
        }
        sp += UTFLEN((unsigned char)*sp);
    }

    const char *ep = sp;
    for(unsigned int i = udwStart; i < udwEnd; ++i)
    {
        if (*ep == '\0')
        {
            break;
        }
        ep += UTFLEN((unsigned char)*ep);
    }

    strDest.assign(sp, ep - sp);
    return true;
}

bool Utf8StringGetPrefix(const std::string & strSrc, unsigned int udwMaxBytes, std::string & strDest)
{
    strDest.clear();
    const char *p = strSrc.c_str();
    size_t c = 0;
    while (*p != '\0')
    {
        size_t l = UTFLEN((unsigned char)*p);
        c += l;
        if (c > udwMaxBytes)
        {
            break;
        }
        p += l;
    }

    strDest.assign(strSrc.c_str(), p - strSrc.c_str());
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 字符串内容判断

bool CmpNoCase(char c1, char c2)
{
	return toupper(c1) == toupper(c2);
}

/** 在字符串中查找子符串，不区分大小写 */
string::size_type FindNoCase(string strMain, string strSub, int nFrom)
{
	string::iterator pos;
	pos = search(strMain.begin()+nFrom, strMain.end(), 
			strSub.begin(),strSub.end(), CmpNoCase);

	if( pos == strMain.end())
	{
		return string::npos;
	}
	else
	{
		return (pos - strMain.begin());
	}
}

/** 检测字符串中是否包含了某些子字符串 */
bool IsSubStrContained(string& strRawData, set<string>& sSubStr)
{
	set<string>::iterator it;
	for (it = sSubStr.begin(); it != sSubStr.end(); it++)
	{
		if (string::npos != strRawData.find(*it))
			return true;
	}

	return false;
}

/** 返回当前位置开始的下一个字符 */
string GetNextChar(string& str, size_t dwCurPos)
{
	if(str.size() < dwCurPos+1)
		return "";
	if(str.at(dwCurPos) < 0)
	{
		return str.substr(dwCurPos, 2);
	}
	else
	{
		return str.substr(dwCurPos, 1);
	}
}

/** 获得一个字符串的字符个数, 中文等双字节字符只算1个 */
int GetCharCount(string& strWord)
{
	int dwCharCount = 0;
	for (size_t i = 0; i < strWord.size(); ++i, ++dwCharCount)
	{
		if (strWord[i] < 0) 
		{
			++i;
		}
	}
	return dwCharCount;
}

/** 是否是数字IP */
bool IsDigitIp(std::string strIp)
{
	vector<string> strList;
	int dwTmp;
		
	SplitString(strIp, ".", strList);
    if (4 != strList.size())
    	return false;
    
    for(int i = 0; i < 4; i++)
    {
    	if (strList[i].size() > 3 || strList[i].size() == 0 )
    		return false;
    	
		for (size_t j = 0; j < strList[i].size(); j++)
		
		{
			
			if (!(strList[i][j] >= '0' && strList[i][j] <= '9'))
				
				return false;
    	}
    	dwTmp = Str2Int(strList[i]);
    	if (dwTmp > 255)
    		return false;
    }
	return true;	
}

/**
* 判断一个字符串是不是只是包含空格字符,检查的空格字符包括' ', '\t'
* @param   pStr 要检查的字符串
* @return  bool 如果被检查字符串只是包含空格字符,就返回PORT_TRUE,
*          否则返回PORT_FALSE
*/
bool IsWhiteString(char const *pStr)
{
    while((*pStr == ' ')|| (*pStr == '\t'))
        pStr ++;
    if(*pStr == '\0')
        return true;
    else
        return false;
}

/** 判断一个字符串是否是完全由英文字符串组成 */
bool IsEnglishString(const char* pStr, bool bLowLineAsAlpha)
{
	char* p = (char*)pStr;
    while (*p != '\0')
	{
        if (bLowLineAsAlpha)
        {
            if( !IsAlpha(*p) && ('_' != *p))
                return false;
        }
        else
        {
            if( !IsAlpha(*p))
                return false;
        }
		p++;	
	}

    return true;  
}

/** 判断一个字符串是否全是数字组成 */
bool IsDigitString(const char* pStr)
{
	char* p = (char*)pStr;
    while (*p != '\0')
	{
        if( !IsDigit(*p))
            return false;
		p++;	
	}

    return true;  
}

/** 计算GB2312汉字的序号,返回值是0到6767之间的数 */
int GetGB2312HanziIndex(string& str)
{
	return ((unsigned char)str[0] - 176)*94 + (unsigned char)str[1] - 161;
}

/** 判断一个字符串的前两个字节是否是GB2312汉字 */
bool IsGB2312Hanzi(string& str)
{
	if(str.size() < 2)
		return false;
	int dwIndex = GetGB2312HanziIndex(str);
	if (dwIndex >=0 && dwIndex < 6768)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/** 判断一个字符串是否完全由GB2312的汉字组成 */
bool IsOnlyComposedOfGB2312Hanzi(string& strWord)
{
	//如果字符串的长度不是2的倍数,则返回false
	if (strWord.size()%2 != 0)
	{
		return false;
	}

    string strTmp;

	//逐个判断每两个字节是否组成一个汉字
	for (size_t i = 0; i+1 < strWord.size(); i=i+2)
	{
        strTmp = strWord.substr(i);
		if (!IsGB2312Hanzi(strTmp)) 
		{
			return false;
		}
	}
	return true;
}

/** 判断字符是否是空格，windows下的isspace()有bug */
bool IsSpace(char c)
{
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
        return true;
    else
        return false;
}

/** 判断字符是否是数字或字母，windows下的isalnum()有bug */
bool IsAlnum(char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
        return true;
    else
        return false;
}

/** 判断字符是否是数字，windows下的isdigit()有bug */
bool IsDigit(char c)
{
    if (c >= '0' && c <= '9')
        return true;
    else
        return false;
}

/** 判断字符是否是字母，windows下的isalpha()有bug */
bool IsAlpha(char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        return true;
    else
        return false;
}

/** 判断字符是否是分界符 */
bool IsSeparator(char c)
{
    string strSeparator(" \t\r\n-_[](){}:.,=*&^%$#@!~?<>/|'\"");
    if (string::npos == strSeparator.find(c))
        return false;
    else
        return true;
}

/** 把字符转为小写, 不用系统函数tolower是因为它有bug */
char CharToLower(char c)
{
	if (c >= 'A' && c <= 'Z')
		return c + 32;
	else
		return c;
}

/** 把字符转为大写, 不用系统函数toupper是因为它有bug */
char CharToUpper(char c)
{
	if (c >= 'a' && c <= 'z')
		return c - 32;
	else
		return c;
}

/** 把字符串转为小写 */
std::string ToLower(const char *pszValue)
{
    std::string  strRet;
    size_t dwLen = strlen(pszValue);
    for (unsigned long i = 0 ; i < dwLen; i ++)
    {
        strRet += CharToLower(pszValue[i]);
    }
	
    return strRet;
}

/** 把字符串转为大写 */
std::string ToUpper(const char *pszValue)
{
    std::string  strRet;
    size_t dwLen = strlen(pszValue);
    for (unsigned long i = 0 ; i < dwLen; i ++)
    {
        strRet += CharToUpper(pszValue[i]);
    }
	
    return strRet;
}

/** 把一个int值转换成为字符串*/
#ifdef WIN32
//string Int2Str(int dwValue)
//{
//    char szBuf[16];
//    sprintf(szBuf,"%d",dwValue);
//    return string(szBuf);
//}
#endif

string SignedInt2Str(int dwValue)
{
    char szBuf[16];
    sprintf(szBuf,"%d",dwValue);
    return string(szBuf);
}

/** 把一个unsigned int值转换成为字符串*/
string Int2Str(unsigned int  dwValue)
{
    char szBuf[16];
    sprintf(szBuf,"%u",dwValue);
    return string(szBuf);
}


/** 把二进制数据转为16进制的字符串 */
std::string Binary2HexFormatString(const char* pcBinData, size_t udwDataLen)
{
    std::string str;
    char* pBuff = (char*)malloc(2*udwDataLen + 1);
    for(size_t i = 0; i < udwDataLen; ++i)
    {
        sprintf(pBuff + i * 2, "%02x", (unsigned char)pcBinData[i]);
    }
    pBuff[2*udwDataLen] = '\0';
    str = pBuff;

    free(pBuff);
    return str;
}


/** 把一个字符转换成16进制的形式 */
std::string Char2Hex(unsigned char ch)
{
	std::string str("%");
	char high;
	char low;

	high = ch / 16;
	if(high > 9)
		high = 'A' + high - 10;
	else
		high = '0' + high;

	low  = ch % 16;
	if(low > 9)
		low = 'A' + low - 10;
	else
		low = '0' + low;

	str += high;
	str += low;

	return str;
}

/** 把一个数值字符转换成数字的形式 */
char char2Bin(const char cHex)
{
    char cBin;
	if (cHex >= 'A' && cHex <= 'F')
		cBin = cHex - 'A' + 10;
	else if (cHex >= 'a' && cHex <= 'f')
		cBin = cHex - 'a' + 10;
	else
		cBin = cHex - '0';
		
	return cBin;
}

/** 把一个数值字符串转换成数字数组 */
std::string Hex2Bin(const std::string &str)
{
    std::string strHex(str);
    std::string strBin;
    
    if (strHex.length() % 2 != 0)
    {
        strHex += "0";
    }
    
    char cH = 0, cL = 0, c=0;
    for (size_t idx=0; idx < strHex.length(); idx+=2)
    {
        cH = char2Bin(strHex[idx]);
        cL = char2Bin(strHex[idx+1]);
        
        c = cH << 4 | cL;
        strBin.append(1, c);
    }
    
    return strBin;
}

/** 判断一个字符是否在一个字符串中 */
bool IsCharInStr(unsigned char ch, const std::string& strTarget)
{
	for (size_t t = 0; t < strTarget.size(); t++)
	{
		if (ch == strTarget[t])
			return true;
	}

	return false;
}

/** 把字符串转为整型数 */
int Str2Int(const std::string& str)
{
	int dwRet = 0;

	for (size_t i = 0; i < str.size(); ++i)
		dwRet = dwRet*10 + str[i] - '0'; 

	return dwRet;
}

/** 把16进制的字符串转为整型数 */
int HexStr2long(const char* pcHex)
{
	int dwRet = 0;
	int dwTmp;
	size_t dwLen = strlen(pcHex);

	for (size_t i = 0; i < dwLen; ++i)
	{
		if (pcHex[i] >= 'A' && pcHex[i] <= 'F')
			dwTmp = pcHex[i] - 'A' + 10;
		else if (pcHex[i] >= 'a' && pcHex[i] <= 'f')
			dwTmp = pcHex[i] - 'a' + 10;
		else
			dwTmp = pcHex[i] - '0';
		
		dwRet = dwRet*16 + dwTmp; 
	}
	return dwRet;
}

/** 把一个字符串中的连续空格转为单个空格 */
string SingleBlank(const string &strRaw) 
{
	unsigned int dwCount = 0;
	bool bIsFirstSpace = true;
	const char *ptr = strRaw.c_str();

	string strRet(strRaw.length(), ' ');
	
	// 跳过字符串开始的空格
	while ((*ptr > 0) && isspace(*ptr)) ++ptr;
	
	while (*ptr)
	{
		if ((*ptr > 0) && isspace(*ptr))
		{
			if (bIsFirstSpace)
			{
				bIsFirstSpace = false;
				strRet[dwCount++] = ' ';
			}
		}
		else
		{
			bIsFirstSpace = true;
			strRet[dwCount++] = *ptr;
		}
		
		++ptr;
	}

	// 删除字符串结束的空格
	string::size_type a;
	a = strRet.find_last_not_of(' ', dwCount);
	if (a != string::npos)
		strRet.erase(a+1);
	else
	{
		a = 0;
		strRet.erase(a);
	}

	return strRet;
}

/** 删除html或xml格式的注释 <!-- --> */
string StripComments(const string& strRawFile) 
{
	string strNewFile;
	strNewFile.reserve(strRawFile.size());

	const char *ptr = strRawFile.c_str();
	const char *end = ptr + strRawFile.length();

	bool bIsInsideComment = false;
	while(1) 
	{
		if(!bIsInsideComment) 
		{
			if(ptr  + 4 < end) 
			{
				if(*ptr == '<' && *(ptr+1) == '!' && *(ptr+2) =='-' && *(ptr + 3) == '-' && isspace(*(ptr + 4))) 
				{
					bIsInsideComment = true;
				}
			}
		} 
		else 
		{
			if(ptr + 2 < end) 
			{
				if(*ptr == '-' && *(ptr+1) == '-' && *(ptr+2) == '>' ) 
				{
					bIsInsideComment = false;
					ptr += 3;
				}
			}
		}
		if(ptr == end) 
			break;
		if(!bIsInsideComment) 
			strNewFile += *ptr;
		ptr++;
	}

	strNewFile.resize(strNewFile.size());

	return strNewFile;
}

/** 计算一个字符串的hash值 */
unsigned int GetStrHashValue(const char* pcStr)
{    
    unsigned int dwMagic = 31;    
    const unsigned char* p = (const unsigned char*) pcStr;    
    unsigned int h = 0;    
    for (; *p != '\0'; ++p) 
    {        
        h = h * dwMagic + *p;    
    }    
    return h;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 字符串模糊匹配实现（就是简单的正则表达式以及范围匹配功能）

/** 检查一个字符串是否能匹配到一个通配符号，外部使用接口
 *  MatchWildcard("he*o","hello"): true
 *  MatchWildcard("he*p","hello"): false
 *  MatchWildcard("he??o","hello"): true
 *  MatchWildcard("he?o","hello"): false
 *  MatchWildcard("[a-z][0-9]","h7"): true
 *  MatchWildcard("172.16.*","172.16.68.29"): true
 */
bool MatchWildcard(const string& strWild, const string& strMatch)
{
  return _MatchRemainder(strWild, strWild.begin(), strMatch, strMatch.begin());
}

// 计算一个字符是否在一个集合中，例如'8'属于"0-9"， 内部函数
bool _MatchSet(const string& strPattern, char strMatch)
{
	// 实际的字符集，例如:strPattern为"a-z", 那么strRealCharSet是"abcd...z"
	string strRealCharSet; 
	string::const_iterator i;
	for (i = strPattern.begin(); i != strPattern.end(); ++i)
	{
		switch(*i)
		{
		case '-':
		{
			if (i == strPattern.begin())
				strRealCharSet += *i;
			else if (i+1 == strPattern.end())
		  		return false;
		    else
			{
				// 第一个字符已经在字符集中，所以首先删除(集合可能是空的)
				strRealCharSet.erase(strRealCharSet.end()-1);
				char last = *++i;
				for (char ch = *(i-2); ch <= last; ch++)
				{
				  strRealCharSet += ch;
				}
			}
			break;
		}
		case '\\':
			if (i+1 == strPattern.end()) 
				return false;
			strRealCharSet += *++i;
			break;
		default:
			strRealCharSet += *i;
			break;
		}
	}
	string::size_type dwResult = strRealCharSet.find(strMatch);
	return dwResult != string::npos;
}

// 匹配剩下的通配符，递归调用, 内部函数
bool _MatchRemainder(const string& strWild, 
							string::const_iterator itWild, 
							const string& strMatch, 
							string::const_iterator itMatch)
{
#ifdef _DEBUG_
	cout << "MatchRemainder called at " << *itMatch << " with wildcard " << *itWild << endl;
#endif
	while (itWild != strWild.end() && itMatch != strMatch.end())
	{
#ifdef _DEBUG_
		cout << "trying to strMatch " << *itMatch << " with wildcard " << *itWild << endl;
#endif
		switch(*itWild)
		{
		case '*':
		{
			++itWild;
			++itMatch;
			for (string::const_iterator i = itMatch; i != strMatch.end(); ++i)
			{
				// 处理"*"在通配符的结束处的情况，这种情况下就没有剩下的了
				if (itWild == strWild.end())
				{
				  if (i == strMatch.end()-1)
					return true;
				}
				else if (_MatchRemainder(strWild, itWild, strMatch, i))
				{
				  return true;
				}
			} // end for
			return false;
		} // end case '*'
		case '[':
		{
			// 找到集合的结束处
			bool bFound = false;
			string::const_iterator it = itWild + 1;
			for (; !bFound && it != strWild.end(); ++it)
			{
				switch(*it)
				{
				case ']':
				{
					// 拿字符*itMatch到集合"[...]"中去匹配（括号被去掉）
					if (!_MatchSet(strWild.substr(itWild - strWild.begin() + 1, it - itWild - 1), *itMatch))
						return false;
					bFound = true;
					break;
				}
				case '\\':

					// 转义字符不能在结尾
					if (it == strWild.end()-1)
						return false;
					++it;
					break;
				default:
					break;
				}
			} // end for
			if (!bFound)
				return false;
			++itMatch;
			itWild = it;
			break;
		} // end case '['
		case '?':
			++itWild;
			++itMatch;
			break;
		case '\\':
			if (itWild == strWild.end()-1)
				return false;
			++itWild;
			if (*itWild != *itMatch)
				return false;
			++itWild;
			++itMatch;
			break;
		default:
			if (*itWild != *itMatch)
				return false;
			++itWild;
			++itMatch;
			break;
		} // end switch
	} // end while

	return (itWild == strWild.end()) && (itMatch == strMatch.end());
}

}
