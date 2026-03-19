#include "HmsSvsMgr.h"
#include "render/HmsGLProgram.h"
#include "render/HmsGLProgramState.h"
#include "render/HmsGLStateCache.h"
#include "render/HmsRenderer.h"
#include "render/HmsTexture2D.h"
#include "base/HmsImage.h"
#include "HmsAviMacros.h"
#include "HmsGlobal.h"
#include "Svs/HmsSVSLoadThread.h"
#include "AndroidLogcatStreamBuf.h"
#include "DataInterface/SmartAviDataDef.h"
#include "DataInterface/HmsDataBus.h"
#include "Svs/Module.h"
//#include "Svs/TAWS.h"
#include "Svs/AltManager.h"
#include "Svs/FPPCamera.h"
#include "Svs/Global.h"

USING_NS_HMS;

GLuint shaderprogram;
GLuint shaderprogram2;
GLuint shaderprogram3;
GLuint shaderprogram4;
GLuint shaderprogramHvWire;
GLuint shaderprogramHvTower;
GLuint shaderprogramAircraft;
GLuint shaderProgramOnDraw;
char *s_taws_data_path = NULL;

bool g_supportVAO = false;

HmsSvsMgr::HmsSvsMgr()
: m_glprogram(nullptr)
, m_glprogram2(nullptr)
, m_glprogram3(nullptr)
, m_glprogram4(nullptr)
, m_glprogramHvWire(nullptr)
, m_glprogramHvTower(nullptr)
, m_glprogramAircraft(nullptr)
{
    m_filterLatitude.SetAlpha(0.025);
    m_filterLongitude.SetAlpha(0.025);
    m_filterAltitude.SetAlpha(0.025);
}


HmsSvsMgr::~HmsSvsMgr()
{
    HMS_SAFE_RELEASE_NULL(m_glprogram);
    HMS_SAFE_RELEASE_NULL(m_glprogram2);
    HMS_SAFE_RELEASE_NULL(m_glprogram3);
    HMS_SAFE_RELEASE_NULL(m_glprogram4);
    HMS_SAFE_RELEASE_NULL(m_glprogramHvWire);
    HMS_SAFE_RELEASE_NULL(m_glprogramHvTower);
    HMS_SAFE_RELEASE_NULL(m_glprogramAircraft);
}

bool HmsSvsMgr::Init()
{
    if (!CHmsNode::Init())
    {
        return false;
    }

    //TAWSInit();
    CModule_InitScence2();
    //TAWSSetDegYawPitchRoll(0, 0, 0);

    m_glprogram = HmsAviPf::GLProgram::createWithFilenames("Shaders/svs_terrain.vsh", "Shaders/svs_terrain.fsh");
    if (m_glprogram)
    {
        m_glprogram->retain();
        shaderprogram = m_glprogram->getProgram();
    }

    m_glprogram2 = HmsAviPf::GLProgram::createWithFilenames("Shaders/svs_runway.vsh", "Shaders/svs_runway.fsh");
    if (m_glprogram2)
    {
        m_glprogram2->retain();
        shaderprogram2 = m_glprogram2->getProgram();
    }

    m_glprogram3 = HmsAviPf::GLProgram::createWithFilenames("Shaders/svs_barrier.vsh", "Shaders/svs_barrier.fsh");
    if (m_glprogram3)
    {
        m_glprogram3->retain();
        shaderprogram3 = m_glprogram3->getProgram();
    }

    m_glprogram4 = HmsAviPf::GLProgram::createWithFilenames("Shaders/svs_flytube.vsh", "Shaders/svs_flytube.fsh");
    if (m_glprogram4)
    {
        m_glprogram4->retain();
        shaderprogram4 = m_glprogram4->getProgram();
    }

    m_glprogramHvWire = HmsAviPf::GLProgram::createWithFilenames("Shaders/svs_hvwire.vsh", "Shaders/svs_hvwire.fsh");
    if (m_glprogramHvWire)
    {
        m_glprogramHvWire->retain();
        shaderprogramHvWire = m_glprogramHvWire->getProgram();
    }

    m_glprogramHvTower = HmsAviPf::GLProgram::createWithFilenames("Shaders/svs_hvtower.vsh", "Shaders/svs_hvtower.fsh");
    if (m_glprogramHvTower)
    {
        m_glprogramHvTower->retain();
        shaderprogramHvTower = m_glprogramHvTower->getProgram();
    }

    m_glprogramAircraft = HmsAviPf::GLProgram::createWithFilenames("Shaders/svs_aircraft.vsh", "Shaders/svs_aircraft.fsh");
    if (m_glprogramAircraft)
    {
        m_glprogramAircraft->retain();
        shaderprogramAircraft = m_glprogramAircraft->getProgram();
    }

#if defined(ANDROID)
    g_supportVAO = false;
#else
    g_supportVAO = false;
#endif//defined(ANDROID)
    SetLocalZOrder(-7);

    SetScheduleUpdate();

    return true;
}

void HmsSvsMgr::OnEnter()
{
    CHmsNode::OnEnter();
    CHmsSVSLoadThread::GetInstance()->start();
}

void HmsSvsMgr::OnExit()
{
    CHmsNode::OnExit();
    CHmsSVSLoadThread::GetInstance()->stop();
}

void HmsSvsMgr::Update(float delta)
{
    CVec3d vlla;
    CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
    auto aftData = dataBus->GetData();
    auto gnssData_Alt = dataBus->GetMslAltitudeMeter();

    static bool s_bNeedInitPos = true;
    if (s_bNeedInitPos)
    {
        m_filterLatitude.SetInitValue(aftData->latitude);   //经纬使用gps数据，高度使用航姿数据
        m_filterLongitude.SetInitValue(aftData->longitude);
        m_filterAltitude.SetInitValue(gnssData_Alt);
        s_bNeedInitPos = false;
    }

    vlla._v[0] = m_filterLongitude.GetFilterValue(aftData->longitude);
    vlla._v[1] = m_filterLatitude.GetFilterValue(aftData->latitude);
    double Alt_ = m_filterAltitude.GetFilterValue(gnssData_Alt);
    
    //海拔高度限制在地表以上
    double alt_ground_surface = CAltManager_GetAltByLonLat(vlla._v[0], vlla._v[1]);
    vlla._v[2] = HMS_MAX(gnssData_Alt, alt_ground_surface);

    Global_SetLonLatAlt(vlla);

    CVec3d vypr;
    vypr._v[0] = aftData->trueHeading;
    vypr._v[1] = aftData->pitch;
    vypr._v[2] = aftData->roll;

    Global_SetYawPitchRoll(vypr);
}

void HmsSvsMgr::Visit(HmsAviPf::Renderer * renderer, const HmsAviPf::Mat4 & parentTransform, uint32_t parentFlags)
{
    CHmsNode::Visit(renderer, parentTransform, parentFlags);
}

void HmsSvsMgr::onDraw(const Mat4 &transform, uint32_t flags)
{
    //	GLuint program = GL::getCurrentShaderProgram();
    glClearColor(24 / 255.0f, 64 / 255.0f, 152 / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    GLenum  lastCullType = GL_FRONT;
    GLboolean lastCullEnable = glIsEnabled(GL_CULL_FACE);
    glGetIntegerv(GL_CULL_FACE_MODE, (GLint*)&lastCullType);

    GLboolean lastbDepth = GL_FALSE;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &lastbDepth);
    GLenum lastDepthFunc = GL_ALWAYS;
    glGetIntegerv(GL_DEPTH_FUNC, (GLint*)&lastDepthFunc);

    GL::bindVAO(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL::bindTexture2D(0u);
    GL::useProgram(0);

    //just Draw
    {
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        //TAWSDraw();
        CModule_PaintOpenGL();
    }

    lastCullEnable ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
    glCullFace(lastCullType);
    glDepthMask(lastbDepth);
    glDepthFunc(lastDepthFunc);
    //	glUseProgram(program);
    //	glCullFace(GL_FRONT);
}

void HmsSvsMgr::Draw(HmsAviPf::Renderer *renderer, const HmsAviPf::Mat4& transform, uint32_t flags)
{
    m_customCommand.init(m_localZOrder, transform, CHmsNode::FLAGS_RENDER_AS_3D);
    m_customCommand.func = HMS_CALLBACK_0(HmsSvsMgr::onDraw, this, transform, flags);
    renderer->addCommand(&m_customCommand);
}


void HmsSvsMgr::SetSvsDataPath(const std::string & strPath)
{
    std::string path;
    if (strPath.size())
    {
        path = strPath;
    }
    else
    {
        path = std::string("SvsDefaultPath/");
    }
    if (s_taws_data_path != NULL)
    {
        delete s_taws_data_path;
        s_taws_data_path = NULL;
    }
    s_taws_data_path = (char*)malloc(path.size() + 1);
    memset(s_taws_data_path, 0, path.size() + 1);
    path.copy(s_taws_data_path, path.size(), 0);
}

void HmsSvsMgr::SetSvsDrawSize(const HmsAviPf::Size& sz)
{
    GetGlobal()->iWindowWidth = sz.width;
    GetGlobal()->iWindowHeight = sz.height;
}

void HmsSvsMgr::SetDyamicSize(const HmsAviPf::Size & sz)
{
    pCCamera camera = FPPCamera_GetCamera();
    GetGlobal()->iWindowWidth = sz.width;
    GetGlobal()->iWindowHeight = sz.height;
    camera->projectmatrix = CMatrix_Perspective(camera->fovY, sz.width / sz.height, 100, 400000.0);
}

