#pragma once

#include "Base.h"

BEGIN(Engine)
class CLayer;
END

BEGIN(Tool)

class CSaveLoader : public CBase
{
	DECLARE_SINGLETON(CSaveLoader)

private:
	CSaveLoader();
	virtual ~CSaveLoader() = default;

public:
	HRESULT SaveObjects(CLayer* pLayer);
	HRESULT LoadObjects(OBJ_DESC** pObjectDesc, _uint& iObjectCount_out);

public:
	virtual void Free() override;

};

END