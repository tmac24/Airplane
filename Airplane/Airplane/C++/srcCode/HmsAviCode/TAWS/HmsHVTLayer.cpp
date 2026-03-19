#include "HmsHVTLayer.h"
#include "render/HmsRenderer.h"
#include <iostream>
#include "HmsHVTDrawNode.h"
#include "display/HmsImageNode.h"

USING_NS_HMS;

HmsHVTLayer::HmsHVTLayer()
: m_rootdrawnode(nullptr)
{

}

HmsHVTLayer::~HmsHVTLayer()
{

}

bool HmsHVTLayer::Init()
{
	//m_rootdrawnode = CHmsDrawNode::Create();
	m_rootdrawnode = HmsHVTDrawNode::Create();
	//m_rootdrawnode->DrawSolidRect(Rect(0, 0, 200, 100),Color4F(0,0.7,0,1));
	this->SetContentSize(Size(800,800));

	this->AddChild(m_rootdrawnode);
	
	return true;
}

void HmsHVTLayer::CreateTestLines()
{
	std::vector<Vec2>* line1 = new std::vector<Vec2>;
	std::vector<Vec2>* line2 = new std::vector<Vec2>;
	std::vector<Vec2>* line3 = new std::vector<Vec2>;
	line1->push_back(Vec2(0, 0));
	line1->push_back(Vec2(30, 30));
	line1->push_back(Vec2(30, 100));
	line1->push_back(Vec2(0, 100));
	line1->push_back(Vec2(-200, 0));
		 
	line2->push_back(Vec2(10, -100));
	line2->push_back(Vec2(30, -70));
	line2->push_back(Vec2(60, -50));
	line2->push_back(Vec2(50, -15));
	line2->push_back(Vec2(20, 10));
		 
	line3->push_back(Vec2(-250, -250));
	line3->push_back(Vec2(-200, -211));
	line3->push_back(Vec2(-121, 25));
		 
	m_lines.push_back(line1);
	m_lines.push_back(line2);
	m_lines.push_back(line3);
}

void HmsHVTLayer::CreateTestLines2(const double& pixelsPerLon, const double& pixelsPerLat)
{
	std::vector<std::vector<Vec2d>> linesLonLat;

	std::vector<Vec2d> line1;
	std::vector<Vec2d> line2;
#if 0
	//this is test data
	line1.push_back(Vec2d(103.550279, 30.640945));
	line1.push_back(Vec2d(103.534325, 30.680666));
	line1.push_back(Vec2d(103.530117, 30.701969));
	line1.push_back(Vec2d(103.516449, 30.722449));
	
	line2.push_back(Vec2d(103.479385, 30.681041));
	line2.push_back(Vec2d(103.501737, 30.701086));
	line2.push_back(Vec2d(103.525480, 30.708713));
#endif
#if 1
	//this is true data
	line1.push_back(Vec2d(116.306352, 40.24905793));
	line1.push_back(Vec2d(116.3097487, 40.24515794));
	line1.push_back(Vec2d(116.312017, 40.24255535));
	line1.push_back(Vec2d(116.3144166, 40.23980192));
	line1.push_back(Vec2d(116.3169077, 40.23694008));
#else
	//this is fake data
	line1.push_back(Vec2d(116.306352, 40.24905793));
	line1.push_back(Vec2d(116.3087587, 40.24515794));
	line1.push_back(Vec2d(116.313007, 40.24255535));
	line1.push_back(Vec2d(116.3124266, 40.23980192));
	line1.push_back(Vec2d(116.3169077, 40.23694008));
#endif

	linesLonLat.push_back(line1);
	//linesLonLat.push_back(line2);

	ConvertLonLat2Pixels(linesLonLat, m_lines, pixelsPerLon, pixelsPerLat);
}

void HmsHVTLayer::Draw(Renderer * renderer, const Mat4 & transform, uint32_t flags)
{
	m_customCommand.init(m_localZOrder, transform, flags);
	m_customCommand.func = HMS_CALLBACK_0(HmsHVTLayer::onDraw, this, transform, flags);
	renderer->addCommand(&m_customCommand);
}

void HmsHVTLayer::onDraw(const Mat4 &transform, uint32_t flags)
{
	//std::cout << "poiajpi" << std::endl;
}

void HmsHVTLayer::UpdateData(const Vec2d& originalLonLat, const double& originalPixelsPerLon, const double& originalPixelsPerLat)
{
	clearLines();

	setLayerOriginalLonLat(originalLonLat);
	setLayerOriginalPixelsPerLon(originalPixelsPerLon);
	setLayerOriginalPixelsPerLat(originalPixelsPerLat);

	//test create lines,(one line is created by several points)
	//CreateTestLines();
	CreateTestLines2(originalPixelsPerLon, originalPixelsPerLat);
	m_rootdrawnode->SetTexturePath("res/AftIcon/line1.png");
	for (auto line : m_lines)
	{
		m_rootdrawnode->DrawLineStrip((Vec2*)&line->at(0),(int)line->size(), 3, 2, Color4B(255, 120, 0, 255), Color4B::BLACK);
		for (auto tower : *line)
		{
			auto pItem = CHmsImageNode::Create("res/NavImage/navTriangle.png");//todo :后面再改为一次性在本类中draw。而不使用子ImageNode的方式。
			if (pItem)
			{
				pItem->SetScale(0.5);
				pItem->SetAnchorPoint(Vec2(0.5, 0.5));
				pItem->SetPosition(tower);
				this->AddChild(pItem);
			}
		}
	}
}

void HmsHVTLayer::ConvertLonLat2Pixels(
	const std::vector<std::vector<Vec2d>>& lines, 
	std::vector<std::vector<Vec2>*>& lines_output,
	const double& pixelsPerLon, 
	const double& pixelsPerLat
	)
{
	for (auto line:lines)
	{
		std::vector<Vec2>* pline_output = new std::vector<Vec2>;
		for (auto point:line)
		{
			Vec2d deltaLonLat = point - m_LayerOriginalLonLat;
			pline_output->push_back(Vec2(deltaLonLat.x * pixelsPerLon, deltaLonLat.y * pixelsPerLat));
		}
		lines_output.push_back(pline_output);
	}
}

void HmsHVTLayer::UpdateDisplay(const float& fHeading, const Vec2d& planeLonLat, const double& pixelsPerLon, const double& pixelsPerLat)
{
	auto scallon = pixelsPerLon / m_LayerOriginalPixelsPerLon;
	auto scallat = pixelsPerLat / m_LayerOriginalPixelsPerLat;
	this->SetScaleY(scallat);
	this->SetScaleX(scallon);

	Vec2d deltaLonLat = planeLonLat - m_LayerOriginalLonLat;
	
	auto sz = this->GetContentSize();	

	//Vec2d deltaPixel = Vec2d(deltaLonLat.x * pixelsPerLon, deltaLonLat.y * pixelsPerLat);
	Vec2d deltaPixel = Vec2d(deltaLonLat.x * m_LayerOriginalPixelsPerLon, deltaLonLat.y * m_LayerOriginalPixelsPerLat);
	Vec2 deltaAp = Vec2(deltaPixel.x / sz.width, deltaPixel.y / sz.height);

	this->SetAnchorPoint(deltaAp);
	this->SetRotation(-fHeading);
	return;
}

void HmsHVTLayer::clearLines()
{
	for (auto pline:m_lines)
	{
		if (pline)
		{
			delete pline;
			pline = nullptr;
		}
	}
}
