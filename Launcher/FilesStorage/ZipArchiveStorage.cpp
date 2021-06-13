#include "stdafx.h"

// General
#include "ZipArchiveStorage.h"

namespace fs = std::experimental::filesystem;

CZipArchiveStorage::CZipArchiveStorage(std::string PathToArchive)
{
	mz_zip_zero_struct(&m_Archive);
	if (false == mz_zip_reader_init_file_v2(&m_Archive, PathToArchive.c_str(), MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY, 0, 0))
	{
		Log::Error("CZipArchiveStorage::CZipArchiveStorage: Unable to 'mz_zip_reader_init_file_v2' archive name '%s'.", PathToArchive.c_str());
		return;
	}
}

CZipArchiveStorage::~CZipArchiveStorage()
{
	mz_zip_reader_end(&m_Archive);
	mz_zip_zero_struct(&m_Archive);
}



//
// IznFilesStorage
//
std::shared_ptr<IFile> CZipArchiveStorage::Open(std::string FileName)
{
	unsigned int index;
	if (mz_zip_reader_locate_file_v2(&m_Archive, FileName.c_str(), nullptr, 0, &index))
	{
		size_t size;
		void* p = mz_zip_reader_extract_to_heap(&m_Archive, index, &size, 0);
		if (p != nullptr)
		{
			std::shared_ptr<CFile> file = MakeShared(CFile, FileName, shared_from_this());
			CByteBuffer& byteBuffer = file->GetByteBuffer();
			byteBuffer.writeBytes(p, size);

			mz_free(p);

			return file;
		}
	}

	return nullptr;
}

size_t CZipArchiveStorage::GetSize(std::string FileName) const
{
	CZipArchiveStorage* thisWithoutConst = const_cast<CZipArchiveStorage*>(this);

	unsigned int index;
	if (mz_zip_reader_locate_file_v2(&thisWithoutConst->m_Archive, FileName.c_str(), nullptr, 0, &index))
	{
		size_t size;
		void* p = mz_zip_reader_extract_to_heap(&thisWithoutConst->m_Archive, index, &size, 0);
		if (p != nullptr)
		{
			mz_free(p);

			return size;
		}
	}

	return 0;
}

bool CZipArchiveStorage::IsExists(std::string FileName) const
{
	CZipArchiveStorage* thisWithoutConst = const_cast<CZipArchiveStorage*>(this);

	unsigned int index;
	return mz_zip_reader_locate_file_v2(&thisWithoutConst->m_Archive, FileName.c_str(), nullptr, 0, &index);
}



//
// IznFilesStorageExtended
//
std::shared_ptr<IFile> CZipArchiveStorage::Create(std::string FileName)
{
	throw CException("CZipArchiveStorage::Create: Not implemented.");
}

void CZipArchiveStorage::Delete(std::string FileName) const
{
	throw CException("CZipArchiveStorage::FileName: Not implemented.");
}

void CZipArchiveStorage::Save(std::shared_ptr<IFile> File)
{
	throw CException("CZipArchiveStorage::Save: Not implemented.");
}

std::vector<std::string> CZipArchiveStorage::GetAllFilesInFolder(std::string Directory, std::string Extension) const
{
	CZipArchiveStorage* thisWithoutConst = const_cast<CZipArchiveStorage*>(this);

	std::vector<std::string> results;
	unsigned int count = mz_zip_reader_get_num_files(&thisWithoutConst->m_Archive);

	mz_zip_archive_file_stat stat;

	if (false == mz_zip_reader_file_stat(&thisWithoutConst->m_Archive, 0, &stat))
	{
		return results;
	}

	for (unsigned int i = 0; i < count; ++i)
	{
		if (false == mz_zip_reader_file_stat(&thisWithoutConst->m_Archive, i, &stat))
			continue;

		if (mz_zip_reader_is_file_a_directory(&thisWithoutConst->m_Archive, i))
			continue;

		const char* current = stat.m_filename;
		if (Directory.empty() || ::strstr(current, Directory.c_str()) != nullptr)
		{
			results.emplace_back(current);
		}
	}

	return results;
}
