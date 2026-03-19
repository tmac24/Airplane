#pragma once

#include "base/HmsTypes.h"
#include "base/RefPtr.h"
#include "base/HmsNode.h"
#include "render/HmsCustomCommand.h"
#include "render/HmsMeshCommand.h"
#include "HmsMercatorEarthTile.h"

enum class SkyBoxType
{
	S_FRONT		= 0,
	S_BACK		= 5,
	S_LEFT		= 3,
	S_RIGHT		= 2,
	S_TOP		= 1,
	S_BOTTOM	= 4
};

class CHmsEarthSkyBox : public HmsAviPf::CHmsNode
{
public:
	CHmsEarthSkyBox();
	~CHmsEarthSkyBox();

	CREATE_FUNC(CHmsEarthSkyBox);

	virtual bool Init() override;

	virtual void Draw(HmsAviPf::Renderer *renderer, const HmsAviPf::Mat4& transform, uint32_t flags) override;

	void InitVertices(SkyBoxType t);

private:

	static GLuint			s_nVEAB;
	static GLushort		*	s_pIndices;
	static GLuint			s_nVerticalsCnt;
	static GLuint			s_nIndicesCnt;

	HmsAviPf::V3F_T2F	*											m_pData;
	GLuint															m_nVBO;
	bool															m_bDirty;
	HmsAviPf::CustomCommand											m_customCommand;
	HmsAviPf::MeshCommand											m_meshCommand;
	HmsAviPf::RefPtr<HmsAviPf::RenderState::StateBlock>				m_stateBlock;
	HmsAviPf::RefPtr<HmsAviPf::Texture2D>							m_texture;

};

class CHmsEarthSkyBoxMgr : public HmsAviPf::CHmsNode
{
public:
	CHmsEarthSkyBoxMgr();
	~CHmsEarthSkyBoxMgr();

	CREATE_FUNC(CHmsEarthSkyBoxMgr);

	virtual bool Init() override;

	virtual void Visit(HmsAviPf::Renderer * renderer, const HmsAviPf::Mat4 & parentTransform, uint32_t parentFlags) override;

private:
	HmsAviPf::Vector<CHmsEarthSkyBox*>			m_rootTile;
};