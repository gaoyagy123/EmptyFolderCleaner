// EmptyFolderCleaner.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "framework.h"
#include "EmptyFolderCleaner.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <map>

// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

CString  OnBnClickedDecgen(CString str = "请选择弹幕的目录：")
{
	char szPath[MAX_PATH];     //存放选择的目录路径 
	LPITEMIDLIST lp;
	bool Res = true;
	do
	{
		ZeroMemory(szPath, sizeof(szPath));

		BROWSEINFO bi;
		bi.hwndOwner = 0;
		bi.pidlRoot = NULL;
		bi.pszDisplayName = szPath;
		bi.lpszTitle = str;
		bi.ulFlags = 0;
		bi.lpfn = NULL;
		bi.lParam = 0;
		bi.iImage = 0;
		//弹出选择目录对话框
		lp = SHBrowseForFolder(&bi);
		Res = true;
		if (!lp || !SHGetPathFromIDList(lp, szPath))
		{
			Res = false;
			AfxMessageBox("无效的目录，请重新选择");
		}
	} while (!Res);
	return szPath;
}
CString stargateDanMuFile(CString &path)
{
	CFileFind finder;
	BOOL bworking = finder.FindFile(path + "\\*.*");
	if (!bworking)
		return "";
	while (bworking)
	{
		bworking = finder.FindNextFile();	//查找文件夹
		if (finder.IsDots())
			continue;
		CString strFileName = finder.GetFileName();	//获取子文件夹名字
		CString subPath = path + "\\" + strFileName;
		CFileStatus fileStatus;
		CFile::GetStatus(subPath, fileStatus);
		CString attribute_str;
		if ((fileStatus.m_attribute & 0x08) == 0x08)
			attribute_str += "Volume|";

		if ((fileStatus.m_attribute & 0x04) == 0x04)
			attribute_str += "System|";

		if (!finder.IsDirectory() && strFileName.Right(3).MakeLower() == "xml")
		{
			return subPath;
		}
	}
	return "";
}
map<int, CString> stargateVideoFile(CString& path)
{
	CFileFind finder;
	map<int, CString> FileFolderStatus;
	BOOL bworking = finder.FindFile(path + "\\*.*");
	if (!bworking)
		return FileFolderStatus;
	while (bworking)
	{
		bworking = finder.FindNextFile();	//查找文件夹
		if (finder.IsDots())
			continue;
		CString strFileName = finder.GetFileName();	//获取子文件夹名字
		CString FileName = finder.GetFileTitle();
		CString subPath = path + "\\" + strFileName;
		CFileStatus fileStatus;
		CFile::GetStatus(subPath, fileStatus);
		CString attribute_str;
		if ((fileStatus.m_attribute & 0x08) == 0x08)
			attribute_str += "Volume|";

		if ((fileStatus.m_attribute & 0x04) == 0x04)
			attribute_str += "System|";

		if (!finder.IsDirectory() && (strFileName.Right(4).MakeLower() == "rmvb" || strFileName.Right(3).MakeLower() == "mp4" || strFileName.Right(3).MakeLower() == "mkv" || strFileName.Right(3).MakeLower() == "flv" || strFileName.Right(3).MakeLower() == "avi"))
		{
			int index = FileName.Find("集");
			if (index != string::npos)
			{
				CString strSouce = FileName.Mid(index - 2, 2), strLoad;
				if (strLoad = strSouce.SpanIncluding(_T("0123456789")), strLoad == strSouce && !strLoad.IsEmpty())
				{
					int VIndex = atoi(strLoad);
					FileFolderStatus[VIndex] = FileName;
					continue;
				}
				else
				{
					strSouce = FileName.Mid(index - 1, 1);
					if (strLoad = strSouce.SpanIncluding(_T("0123456789")), strLoad == strSouce && !strLoad.IsEmpty())
					{
						int VIndex = atoi(strLoad);
						FileFolderStatus[VIndex] = FileName;
						continue;
					}
				}
			}
			CString cName = FileName;
			cName.MakeLower();
			index = cName.Find("x");
			if (index != string::npos)
			{
				CString strSouce = FileName.Mid(index + 3, 1), strLoad;
				if (strSouce == ']' || index + 3 == FileName.Trim().GetLength())
				{
					strSouce = FileName.Mid(index + 1, 2);
					if (strLoad = strSouce.SpanIncluding(_T("0123456789")), strLoad == strSouce && !strLoad.IsEmpty())
					{
						int VIndex = atoi(strLoad);
						FileFolderStatus[VIndex] = FileName;
						continue;
					}
				}
			}
			cName.MakeUpper();
		    index = cName.Find("S");
			if (index == 0 && cName.Mid(3, 1) == "E")
			{
				CString strSouce = FileName.Right(2), strLoad;
				if (strLoad = strSouce.SpanIncluding(_T("0123456789")), strLoad == strSouce && !strLoad.IsEmpty())
				{
					int VIndex = atoi(strLoad);
					FileFolderStatus[VIndex] = FileName;
					continue;
				}
			}
		}
	}
	return FileFolderStatus;
}
int stargate(CString& path)
{
	CString szPath = OnBnClickedDecgen("选择视频文件夹：");
	map<int, CString> videoName = stargateVideoFile(szPath);
	CFileFind finder;
	int FileFolderStatus = 0;
	BOOL bworking = finder.FindFile(path + "\\*.*");
	if (!bworking)
		return -1;
	while (bworking)
	{
		bworking = finder.FindNextFile();	//查找文件夹
		if (finder.IsDots())
			continue;
		++FileFolderStatus;
		CString strFileName = finder.GetFileName();	//获取子文件夹名字
		CString FileName = finder.GetFileTitle();
		CString subPath = path + "\\" + strFileName;
		CFileStatus fileStatus;
		CFile::GetStatus(subPath, fileStatus);
		CString attribute_str;
		if ((fileStatus.m_attribute & 0x08) == 0x08)
			attribute_str += "Volume|";

		if ((fileStatus.m_attribute & 0x04) == 0x04)
			attribute_str += "System|";

		if (finder.IsDirectory() && attribute_str.IsEmpty())
		{
			CString subFile = stargateDanMuFile(subPath);
			if (!subFile.IsEmpty())
			{
				CString cName = FileName;
				cName.MakeLower();
				int index = cName.Find("x");
				if (index != string::npos)
				{
					CString strSouce = FileName.Mid(index + 3, 1), strLoad;
					if (strSouce == ']' || strSouce == '[' || strSouce == ' ' || strSouce == '|' || strSouce == '_' || strSouce == '-' || index + 3 == FileName.Trim().GetLength())
					{
						strSouce = FileName.Mid(index + 1, 2);
						if (strLoad = strSouce.SpanIncluding(_T("0123456789")), strLoad == strSouce && !strLoad.IsEmpty())
						{
							int VIndex = atoi(strLoad);
							CString Newfile = szPath + "\\弹幕\\" + videoName[VIndex] + ".xml";
							printf(Newfile + "\n");
							::CreateDirectory(szPath + "\\弹幕", NULL);
							CopyFile(subFile, Newfile, TRUE);
						}
					}
				}
			}
		}
	}
	return FileFolderStatus;
}
int ListFolder(CString path)
{
	CFileFind finder;
    int FileFolderStatus = 0;
	BOOL bworking = finder.FindFile(path + "\\*.*");
    if (!bworking)
        return -1;
	while (bworking)
	{
		bworking = finder.FindNextFile();	//查找文件夹
		if (finder.IsDots())
			continue;
        ++FileFolderStatus;
		CString strFileName = finder.GetFileName();	//获取子文件夹名字
		CString subPath = path + "\\" + strFileName;
		CFileStatus fileStatus;
		CFile::GetStatus(subPath, fileStatus);
		CString attribute_str;
		if ((fileStatus.m_attribute & 0x08) == 0x08)
			attribute_str += "Volume|";

		if ((fileStatus.m_attribute & 0x04) == 0x04)
			attribute_str += "System|";

       // if (finder.IsDirectory()&& (subPath.MakeLower().Find("c:\\program") != string::npos|| subPath.MakeLower().Find("c:\\users") != string::npos))
        if (finder.IsDirectory()&& attribute_str.IsEmpty())
		{
            int SubFileFolderStatus = ListFolder(subPath);
            if (SubFileFolderStatus == 0)
			{
				CTime ctime = fileStatus.m_ctime;	//获取文件夹创建时间
                BOOL Res = ::RemoveDirectory(subPath.GetBuffer());
				if (Res)
					--FileFolderStatus;
				CString StrLog;
				StrLog.Format("%s\\                     %s|删除%s\n", subPath, ctime.Format("%Y-%m-%d %H:%M:%S"), Res ? "成功" : "失败");
				printf(StrLog);
			}
        }
	}
    return FileFolderStatus;
}

int main()
{
    int nRetCode = 0;
	CString szPath = OnBnClickedDecgen();
	//stargate(szPath);
    ListFolder(szPath);
    system("pause");
    HMODULE hModule = ::GetModuleHandle(nullptr);
    if (hModule != nullptr)
    {
        // 初始化 MFC 并在失败时显示错误
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: 在此处为应用程序的行为编写代码。
            wprintf(L"错误: MFC 初始化失败\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: 在此处为应用程序的行为编写代码。
        }
    }
    else
    {
        // TODO: 更改错误代码以符合需要
        wprintf(L"错误: GetModuleHandle 失败\n");
        nRetCode = 1;
    }

    return nRetCode;
}

