

#include "CommonPath.h"
#include "string/FilenameUtils.h"
#include "string/StringUtils.h"

std::string CommonPath::_sSceneRoot;

#if defined(_MSC_VER)
#pragma warning(disable : 4245)
#endif

#include <boost/filesystem.hpp>
#include <cstdlib> 

//-----------------------------------------------------------------------------

inline std::string _getenvvar(const std::string& key) 
{
	char* str = getenv(key.c_str());
	if (str == 0)
		return std::string();
	return std::string(str);
}

//-----------------------------------------------------------------------------

std::string CommonPath::GetApplicationDirectory(const std::string& appname)
{
   std::string sPath = _getenvvar("APPDATA");
   if (sPath.empty())
   {
      sPath = _getenvvar("HOME");
   }

   if (!sPath.empty())
   {
      sPath = sPath + "/";
   }

   sPath = sPath + appname;

   std::string sPathUnified = FilenameUtils::UnifyPathDelimiter(sPath);
   return FilenameUtils::DelimitPath(sPathUnified);
}

//-----------------------------------------------------------------------------

std::string CommonPath::GetCwd()
{
   std::wstring wstr;
   boost::filesystem2::detail::get_current_path_api(wstr);
   std::string sPath = StringUtils::wstring_To_Utf8(wstr);
   std::string sPathUnified = FilenameUtils::UnifyPathDelimiter(sPath);
   return FilenameUtils::DelimitPath(sPathUnified);
}

//-----------------------------------------------------------------------------

std::string CommonPath::GetSceneRoot()
{
   if (_sSceneRoot.length()>0)
      return FilenameUtils::DelimitPath(FilenameUtils::UnifyPathDelimiter(_sSceneRoot));
   else
   {
      return GetCwd();
   }
}

//-----------------------------------------------------------------------------

void CommonPath::SetCwd(std::string& sPath)
{
   std::wstring sPathW = StringUtils::Utf8_To_wstring(sPath);

   boost::filesystem::wpath dir_path(sPathW);
   boost::filesystem::current_path(dir_path);
}

//-----------------------------------------------------------------------------

std::string CommonPath::ConvertWildcard(std::string& sPath) 
{
   std::wstring sReturn = StringUtils::Utf8_To_wstring(sPath);
   std::wstring sWildcard = L"{cwd}";
   std::wstring::size_type sPos = sReturn.find(sWildcard);

   if (sPos != std::wstring::npos)
   {
      std::wstring sCwd = StringUtils::Utf8_To_wstring(CommonPath::GetCwd());
      sReturn.replace(sPos, sWildcard.length(), sCwd);
   }
   else
   {
      sWildcard = L"{scene}";
      sPos = sReturn.find(sWildcard);
      if (sPos != std::wstring::npos)
      {
         std::wstring sRoot = StringUtils::Utf8_To_wstring(_sSceneRoot);
         sReturn.replace(sPos, sWildcard.length(), sRoot);
      }
   }

   return StringUtils::wstring_To_Utf8(sReturn);
}

//-----------------------------------------------------------------------------

bool CommonPath::HasWildcard(std::string& sPath)
{
   std::wstring sPathW = StringUtils::Utf8_To_wstring(sPath);

   if (sPathW.find(L'}') != std::wstring::npos)
      return true;

   return false;
}

//-----------------------------------------------------------------------------

