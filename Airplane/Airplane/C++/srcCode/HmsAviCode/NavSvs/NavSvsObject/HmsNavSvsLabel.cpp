#include "HmsNavSvsLabel.h"
#include "render/HmsGLProgramState.h"
#include "render/HmsGLProgram.h"
#include "render/HmsTexture2D.h"
#include "render/HmsGLStateCache.h"
#include "render/HmsRenderer.h"
#include "base/HmsFileUtils.h"
#include "Calculate/CalculateZs.h"

#include "NavSvs/HmsNavSvsMgr.h"

CHmsNavSvsLabel::CHmsNavSvsLabel()
{
    m_bPositionDirty = true;
    m_locUniformMVP = 0;
    m_bRotateWithCamera = false;
}

CHmsNavSvsLabel::~CHmsNavSvsLabel()
{

}

CHmsNavSvsLabel* CHmsNavSvsLabel::Create(const char * text)
{
    TTFConfig ttfConfig;
    ttfConfig.fontFilePath = CHmsFileUtils::getInstance()->fullPathForFilename("fonts/msyh.ttc");
    ttfConfig.fontSize = 32;
    auto pNavSvsLabel = new CHmsNavSvsLabel;
    pNavSvsLabel->SetTTFConfig(ttfConfig);
    pNavSvsLabel->SetString(text);
    pNavSvsLabel->SetTextColor(Color4B::BLACK);
    return pNavSvsLabel;
}

void CHmsNavSvsLabel::Update(float delta)
{
    if (m_bPositionDirty)
    {
        m_bPositionDirty = false;

        if (m_bRotateWithCamera)
        {
            auto angle = MATH_RAD_TO_DEG_DOUBLE(CalculateAngle(m_cameraAttiStu.LngLat.x, m_cameraAttiStu.LngLat.y
                , m_attiStu.LngLat.x, m_attiStu.LngLat.y));

            m_attiStu.yaw = 180 + angle;

            auto length = CalculateDistanceM(m_cameraAttiStu.LngLat.x, m_cameraAttiStu.LngLat.y
                , m_attiStu.LngLat.x, m_attiStu.LngLat.y);
            
            if (length > 1)
            {
                auto pitch = MATH_RAD_TO_DEG_DOUBLE(atan((m_cameraAttiStu.Height - m_attiStu.Height) / length));
                m_attiStu.pitch = pitch;
            }
        }

        float ratio = CHmsNavSvsMathHelper::MeterToWorldUnit(64) / 32;
        Mat4d scale;
        Mat4d::createScale(Vec3d(ratio, ratio, ratio), &scale);

        CHmsNavSvsMathHelper::CaculateAttitude(&m_attiStu, &m_mat4dM, NULL, NULL, NULL, NULL);
        m_mat4dM = m_mat4dM * scale;
    }
    UpdateTextString();
}

void CHmsNavSvsLabel::OnDraw()
{
    auto glProgram = GetGLProgram();
    glProgram->use();

    if (m_locUniformMVP == 0)
    {
        m_locUniformMVP = glProgram->getUniformLocation("HMS_MVPMatrix");
    }

    if (Renderer::UseGlVao())
    {
        GL::bindVAO(m_vao);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);

        // vertices
        glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid*)offsetof(V3F_C4B_T2F, vertices));

        // colors
        glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_COLOR);
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B_T2F), (GLvoid*)offsetof(V3F_C4B_T2F, colors));

        // tex coords
        glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_TEX_COORD);
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid*)offsetof(V3F_C4B_T2F, texCoords));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[1]);
    }
    if (m_nUniformTextColor)
    {
        glProgram->setUniformLocationWith4f(m_nUniformTextColor, m_colorDisplay.r / 255.0f, m_colorDisplay.g / 255.0f, m_colorDisplay.b / 255.0f, m_opacityDisplay / 255.0f);
    }
    if (m_nUniformEffectColor)
    {
        glProgram->setUniformLocationWith4fv(m_nUniformEffectColor, &m_colorEffect.r, 1);
    }

    auto model = (*CHmsNavSvsMgr::GetInstance()->GetMat4dTrans()) * m_mat4dM;
    auto pMat4dPV = CHmsNavSvsMgr::GetInstance()->GetMat4dPVRelative();
    model = (*pMat4dPV) * model;

    Mat4 mat;
    CHmsNavSvsMathHelper::GetMatFromMat4d(&mat, &model);
    glProgram->setUniformLocationWithMatrix4fv(m_locUniformMVP, mat.m, 1);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    GL::blendFunc(m_blendFunc.src, m_blendFunc.dst);

    for (auto & c : m_mapAtlasDrawInfo)
    {
        GL::bindTexture2D(c.second.nAtlasTexture);
        glDrawElements(GL_TRIANGLES, (GLsizei)c.second.vQuadIndex.size() * 6, GL_UNSIGNED_SHORT, (GLvoid*)(c.second.nQuadIndexStart*sizeof(GLushort)));
    }
}

void CHmsNavSvsLabel::UpdateVertexData()
{

}

void CHmsNavSvsLabel::SetAttitudeStu(CalAttitudeStu stu)
{
    m_attiStu = stu;
    m_bPositionDirty = true;
}

void CHmsNavSvsLabel::SetCameraAttitudeStu(CalAttitudeStu stu)
{
    m_cameraAttiStu = stu;
}

void CHmsNavSvsLabel::SetRotateWithCamera(bool b)
{
    m_bRotateWithCamera = b;
}
