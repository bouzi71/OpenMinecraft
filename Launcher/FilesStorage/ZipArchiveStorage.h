#pragma once

#include "../assets/zip/miniz.h"

class CZipArchiveStorage
	: public IznFilesStorage
	, public IznFilesStorageExtended
{
public:
	CZipArchiveStorage(std::string PathToArchive);
	virtual ~CZipArchiveStorage();

	// IznFilesStorage
	std::shared_ptr<IFile>                          Open(std::string FileName) override;
	size_t                                          GetSize(std::string FileName) const override;
	bool                                            IsExists(std::string FileName) const override;

	// IznFilesStorageExtended
	std::shared_ptr<IFile>                          Create(std::string FileName) override;
	void                                            Delete(std::string FileName) const override;
	void                                            Save(std::shared_ptr<IFile> File) override;
	std::vector<std::string>                        GetAllFilesInFolder(std::string Directory, std::string Extension = "") const override;

private:
	mz_zip_archive m_Archive;
};
