#pragma once
#include "Source/UIBase/UIBase.h"
#include "Source/UIBase/Load/Load.h"
class Title :
    public UIBase
{
public:
    Title();
    ~Title();
    virtual bool Start() override;
    virtual void Update() override;
    virtual void Render(RenderContext& rc)override;
};

