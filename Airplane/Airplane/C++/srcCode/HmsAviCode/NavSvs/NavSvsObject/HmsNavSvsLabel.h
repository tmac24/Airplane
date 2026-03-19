#pragma once

#include "math/HmsMath.h"
#include "NavSvs/HmsNavSvsMathHelper.h"
#include "display/HmsLabel.h"
#include <vector>

class CHmsNavSvsLabel : public CHmsLabel
{
public:
    CHmsNavSvsLabel();
    ~CHmsNavSvsLabel();
    
    static CHmsNavSvsLabel* Create(const char * text);

    virtual void Update(float delta) override;
    void OnDraw();
    void UpdateVertexData();

    void SetAttitudeStu(CalAttitudeStu stu);

    void SetCameraAttitudeStu(CalAttitudeStu stu);
    void SetRotateWithCamera(bool b);

public:
    GLuint m_locUniformMVP;
    CalAttitudeStu m_attiStu;

    CalAttitudeStu m_cameraAttiStu;
    bool m_bRotateWithCamera;

    Mat4d m_mat4dM;
    bool m_bPositionDirty;
};

