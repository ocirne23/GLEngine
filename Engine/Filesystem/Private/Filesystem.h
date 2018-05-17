#pragma once

#include "IFilesystem.h"

class Filesystem final : public IFilesystem
{
	virtual owner<IFilesystem*> create() override;
};