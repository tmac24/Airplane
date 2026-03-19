#include "HmsLabel.h"
#include "base/HmsFileUtils.h"
#include "font/HmsFontAtlasCache.h"
#include "render/HmsGLProgramState.h"
#include "render/HmsGLProgram.h"
#include "base/HmsUTF8.h"
#include "font/HmsFontAtlas.h"
#include "font/HmsFont.h"
#include "render/HmsTexture2D.h"
#include "render/HmsGLStateCache.h"
#include "render/HmsRenderer.h"

NS_HMS_BEGIN

CHmsLabel::CHmsLabel()
	: m_bUseDistanceField(false)
	, m_bUseA8Shader(false)
	, m_bShadowEnabled(false)
	, m_bContextDirty(true)
	, m_bPixelAlign(true)
	, m_nUniformEffectColor(0)
	, m_pFontAtlas(nullptr)
	, m_vao(0)
	, m_nNumberOfLines(0)
	, m_fLinesOffsetY(0)
	, m_eAlignHorizontal(TextHAlignment::LEFT)
	, m_eAlignVertical(TextVAlignment::TOP)
	, m_colorEffect(Color4B::WHITE)
	, m_fAdditionalKerning(0.0f)
	, m_fMaxLineWidth(0.0f)
    , m_nMaxLineCount(0)
{
	m_vbo[0] = 0;
	m_vbo[1] = 0;

	m_blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;

	m_bUseA8Shader = true;

    m_u16StrSuffix.push_back('.');
    m_u16StrSuffix.push_back('.');
    m_u16StrSuffix.push_back('.');
}

CHmsLabel::~CHmsLabel()
{
	if (m_vbo[0])
	{
		glDeleteBuffers(2, m_vbo);
	}

#ifdef HMS_RENDER_USE_VAO
	if (m_vao)
	{
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}
#endif
}

CHmsLabel * CHmsLabel::Create(const TTFConfig & ttfConfig, const std::string & strText)
{
	CHmsLabel * pLabel = new CHmsLabel();
	if (pLabel && CHmsFileUtils::getInstance()->isFileExist(ttfConfig.fontFilePath))
	{
		pLabel->SetAnchorPoint(Vec2(0.5f, 0.5f));
		pLabel->SetTTFConfig(ttfConfig);
		pLabel->SetString(strText);
		pLabel->autorelease();
		return pLabel;
	}

	delete pLabel;
	return nullptr;
}
#include "HmsLog.h"
CHmsLabel * CHmsLabel::createWithTTF(const std::string & strText, const std::string & fontPath, float fFontSize)
{
	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = CHmsFileUtils::getInstance()->fullPathForFilename(fontPath);
	//ttfConfig.fontFilePath = "C:\\Users\\song.zhang079\\Desktop\\font\\msyh.ttc";
	ttfConfig.fontSize = fFontSize;
	//HMSLOG("TTF path = %s", ttfConfig.fontFilePath.c_str());
	return Create(ttfConfig, strText);
}

bool CHmsLabel::SetTTFConfig(const TTFConfig & ttfConfig)
{
	bool bRet = false;
	auto fontAtlas = FontAtlasCache::getFontAtlasTTF(&ttfConfig);
	if (nullptr == fontAtlas)
	{
		return bRet;
	}

	m_bContextDirty = true;

	SetFontAtlas(fontAtlas);
	m_ttfConfig = ttfConfig;
	if (m_ttfConfig.outlineSize > 0)
	{
		m_eLabelEffect = LabelEffect::OUTLINE;
	}
	else
	{
		m_eLabelEffect = LabelEffect::NORMAL;
	}
	UpdateShaderProgram();

	return bRet;
}

TTFConfig CHmsLabel::GetTTFConfig()
{
	return m_ttfConfig;
}

void CHmsLabel::SetFontAtlas(FontAtlas * fontAtlas)
{
	if (fontAtlas == m_pFontAtlas)
	{
		FontAtlasCache::releaseFontAtlas(fontAtlas);
		return;
	}

	if (m_pFontAtlas)
	{
		FontAtlasCache::releaseFontAtlas(m_pFontAtlas);
		m_pFontAtlas = nullptr;
	}

	m_pFontAtlas = fontAtlas;
}

void CHmsLabel::UpdateShaderProgram()
{
	switch (m_eLabelEffect)
	{
	case HmsAviPf::LabelEffect::NORMAL:
		if (m_bUseDistanceField)
		{
			SetGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_LABEL_DISTANCEFIELD_NORMAL));
		}
		else if (m_bUseA8Shader)
		{
			SetGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_LABEL_NORMAL));
		}
		else if (m_bShadowEnabled)
		{
			SetGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR));
		}
		else
		{
			SetGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP));
		}
		break;
	case HmsAviPf::LabelEffect::OUTLINE:
		SetGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_LABEL_OUTLINE));
		m_nUniformEffectColor = glGetUniformLocation(GetGLProgram()->getProgram(), "u_effectColor");
		break;
	case HmsAviPf::LabelEffect::SHADOW:
		break;
	case HmsAviPf::LabelEffect::GLOW:
		if (m_bUseDistanceField)
		{
			SetGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_LABEL_DISTANCEFIELD_GLOW));
			m_nUniformEffectColor = glGetUniformLocation(GetGLProgram()->getProgram(), "u_effectColor");
		}
		break;
	case HmsAviPf::LabelEffect::ALL:
	default:
		break;
	}
	m_nUniformTextColor = glGetUniformLocation(GetGLProgram()->getProgram(), "u_textColor");
}

std::string CHmsLabel::GetString()
{
	std::string u8str;
	if (StringUtils::UTF16ToUTF8(m_strText, u8str))
	{
		return u8str;
	}
	return "";
}

void CHmsLabel::SetString(const char * strText)
{
	SetStringUTF8(strText);
}

void CHmsLabel::SetString(const std::string & strText)
{
	SetStringUTF8(strText);
}

void CHmsLabel::SetString(const std::u16string & strText)
{
	if (strText != m_strText)
	{
		m_strText = strText;
		m_bContextDirty = true;
	}
}

void CHmsLabel::SetStringUTF8(const std::string & strText)
{
	std::u16string tempString;
	StringUtils::UTF8ToUTF16(strText, tempString);
	SetString(tempString);
}

void CHmsLabel::SetPixelAlign(bool bPixelAligh)
{
	m_bPixelAlign = bPixelAligh;
}

bool CHmsLabel::GetPixelAlign()
{
	return m_bPixelAlign;
}

static int GetFirstWordLen(const std::u16string& utf16Text, int startIndex, int textLen)
{
	auto character = utf16Text[startIndex];
	if (StringUtils::IsCJKUnicode(character) || StringUtils::IsUnicodeSpace(character) || character == '\n')
	{
		return 1;
	}

	int len = 1;
	for (int index = startIndex + 1; index < textLen; ++index)
	{
		character = utf16Text[index];
		if (character == '\n' || StringUtils::IsUnicodeSpace(character) || StringUtils::IsCJKUnicode(character))
		{
			break;
		}
		len++;
	}

	return len;
}

void CHmsLabel::UpdateTextString()
{
	if (m_bContextDirty && m_pFontAtlas)
	{
		//clear the vbo data
		m_vQuadString.clear();
		m_vQuadIndex.clear();

		m_mapAtlasDrawInfo.clear();
		auto itDrawInfo = m_mapAtlasDrawInfo.begin();

		m_pFontAtlas->prepareLetterDefinitions(m_strText);

        int len = 0;
        if (!MultilineTextWrapByCharMaxLineCount(m_strText, len))
        {
            m_bContextDirty = false;
            return;
        }
        
		UpdateAlignmentOffset();

		for (auto ctr = 0; ctr < len; ctr++)
		{
			auto & letterInfo = m_vLettersInfo[ctr];
			if (letterInfo.valid)
			{
				auto & letterDef = m_pFontAtlas->_letterDefinitions[letterInfo.utf16Char];
				Rect reusedRect;
				reusedRect.size.height = letterDef.height;
				reusedRect.size.width = letterDef.width;
				reusedRect.origin.x = letterDef.U;
				reusedRect.origin.y = letterDef.V;

				//auto px = letterDef
				float left = letterInfo.positionX + m_vLinesOffsetX[letterInfo.lineIndex];
				float right = left + letterDef.width;
				float top = letterInfo.positionY + m_fLinesOffsetY;
				float bottom = top - letterDef.height;

				V3F_C4B_T2F_Quad quad;
				quad.tl.vertices.set(left, top, 0.0f);
				quad.tr.vertices.set(right, top, 0.0f);
				quad.bl.vertices.set(left, bottom, 0.0f);
				quad.br.vertices.set(right, bottom, 0.0f);

				auto tex = m_pFontAtlas->getTexture(letterDef.textureID);
				letterInfo.atlasIndex = tex->getName();

 				float atlasWidth = (float)tex->getPixelsWide();
 				float atlasHeight = (float)tex->getPixelsHigh();

				left = reusedRect.origin.x / atlasWidth;
				right = (reusedRect.origin.x + reusedRect.size.width) / atlasWidth;
				top = reusedRect.origin.y / atlasHeight;
				bottom = (reusedRect.origin.y + reusedRect.size.height) / atlasHeight;

				quad.bl.texCoords.u = left;
				quad.bl.texCoords.v = bottom;
				quad.br.texCoords.u = right;
				quad.br.texCoords.v = bottom;
				quad.tl.texCoords.u = left;
				quad.tl.texCoords.v = top;
				quad.tr.texCoords.u = right;
				quad.tr.texCoords.v = top;

				quad.bl.colors = Color4B::WHITE;
				quad.br.colors = Color4B::WHITE;
				quad.tl.colors = Color4B::WHITE;
				quad.tr.colors = Color4B::WHITE;
				

				if (itDrawInfo != m_mapAtlasDrawInfo.end())
				{
					if (itDrawInfo->first != letterInfo.atlasIndex)
					{
						itDrawInfo = m_mapAtlasDrawInfo.find(letterInfo.atlasIndex);
					}
				}
				
				if (itDrawInfo == m_mapAtlasDrawInfo.end())
				{
					AtlasDrawInfo di;
					di.nAtlasTexture = letterInfo.atlasIndex;
					di.nQuadIndexStart = 0;
					m_mapAtlasDrawInfo[letterInfo.atlasIndex] = di;
					itDrawInfo = m_mapAtlasDrawInfo.find(letterInfo.atlasIndex);
				}

				itDrawInfo->second.vQuadIndex.push_back(m_vQuadString.size());
				m_vQuadString.push_back(quad);
			}
		}

		//update draw information
		int nPos = 0;
		m_vQuadIndex.reserve(6 * len);

		for (auto & ct:m_mapAtlasDrawInfo)
		{
			for (auto & ci : ct.second.vQuadIndex)
			{
				auto start = ci * 4;
				m_vQuadIndex.push_back(start);
				m_vQuadIndex.push_back(start+1);
				m_vQuadIndex.push_back(start+2);

				m_vQuadIndex.push_back(start+3);
				m_vQuadIndex.push_back(start+2);
				m_vQuadIndex.push_back(start+1);
			}
			ct.second.nQuadIndexStart = nPos;
			nPos = m_vQuadIndex.size();
		}

		//update vao and vbo
#ifdef HMS_RENDER_USE_VAO
		if (Renderer::UseGlVao())
		{
			if (0 == m_vao)
			{
				BindVaoAndVbo();
			}
			else
			{
				UpdateBindVaoAndVboData();
			}
		}
		else
#endif
		{
			if (m_vbo[0])
			{
				UpdateBindVboData();
			}
			else
			{
				BindVbo();
			}
		}

		m_bContextDirty = false;
	}
}

void CHmsLabel::Visit(Renderer * renderer, const Mat4 & parentTransform, uint32_t parentFlags)
{
	if (m_bVisible)
	{
		if (m_bContextDirty)
		{
			UpdateTextString();
		}

		CHmsNode::Visit(renderer, parentTransform, parentFlags);
	}
}

float HmsAviPf::CHmsLabel::GetLineHeight()
{
	float lineHeight = 0.0f;
	if (m_pFontAtlas)
	{
		lineHeight = m_pFontAtlas->getLineHeight();
	}
	return lineHeight;
}

void CHmsLabel::Draw(Renderer * renderer, const Mat4 & transform, uint32_t flags)
{
	if (m_strText.length() <= 0)
	{
		return;
	}

	bool transformUpdated = flags & FLAGS_TRANSFORM_DIRTY;

	m_customCommand.init(m_globalZOrder);
	m_customCommand.func = std::bind(&CHmsLabel::OnDraw, this, m_transformModeView, flags);
	renderer->addCommand(&m_customCommand);
}

#if 0
bool CHmsLabel::MultilineTextWrapByWord(int * horiKernings)
{
	int textLen = GetStringLength();
	int lineIndex = 0;
	float nextWordX = 0.f;
	float nextWordY = 0.f;
	float longestLine = 0.f;
	float letterRight = 0.f;

	float lineSpacing = 0.f;
	float highestY = 0.f;
	float lowestY = 0.f;
	float lineHeight = m_pFontAtlas->getLineHeight();
	float maxLineWidth = 0.f;
	FontLetterDefinition letterDef;
	Vec2 letterPosition;

	for (int index = 0; index < textLen;)
	{
		auto character = m_strText[index];
		if (character == '\n')
		{
			m_vLineWidth.push_back(letterRight);
			letterRight = 0.f;
			lineIndex++;
			nextWordX = 0.f;
			nextWordY -= lineHeight + lineSpacing;
			RecordPlaceholderInfo(index, character);
			index++;
			continue;
		}

		auto wordLen = GetFirstWordLen(m_strText, index, textLen);
		float wordHighestY = highestY;;
		float wordLowestY = lowestY;
		float wordRight = letterRight;
		float nextLetterX = nextWordX;
		bool newLine = false;
		for (int tmp = 0; tmp < wordLen; ++tmp)
		{
			int letterIndex = index + tmp;
			character = m_strText[letterIndex];
			if (character == '\r')
			{
				RecordPlaceholderInfo(letterIndex, character);
				continue;
			}
			if (m_pFontAtlas->getLetterDefinitionForChar(character, letterDef) == false)
			{
				RecordPlaceholderInfo(letterIndex, character);
				CCLOG("LabelTextFormatter error:can't find letter definition in font file for letter: %c", character);
				continue;
			}

			auto letterX = (nextLetterX + letterDef.offsetX) /*/ contentScaleFactor*/;
			if (maxLineWidth > 0.f && nextWordX > 0.f && letterX + letterDef.width > maxLineWidth)
			{
				m_vLineWidth.push_back(letterRight);
				letterRight = 0.f;
				lineIndex++;
				nextWordX = 0.f;
				nextWordY -= lineHeight + lineSpacing;
				newLine = true;
				break;
			}
			else
			{
				letterPosition.x = letterX;
			}
			letterPosition.y = (nextWordY - letterDef.offsetY) /*/ contentScaleFactor*/;
			RecordLetterInfo(letterPosition, character, letterIndex, lineIndex);

			
			if (horiKernings && letterIndex < textLen - 1)
				nextLetterX += horiKernings[letterIndex + 1];
			
			nextLetterX += letterDef.xAdvance + m_fAdditionalKerning;

			wordRight = letterPosition.x + letterDef.width;

			if (wordHighestY < letterPosition.y)
				wordHighestY = letterPosition.y;
			if (wordLowestY > letterPosition.y - letterDef.height)
				wordLowestY = letterPosition.y - letterDef.height;
		}

		if (newLine)
		{
			continue;
		}

		nextWordX = nextLetterX;
		letterRight = wordRight;
		if (highestY < wordHighestY)
			highestY = wordHighestY;
		if (lowestY > wordLowestY)
			lowestY = wordLowestY;
		if (longestLine < letterRight)
			longestLine = letterRight;

		index += wordLen;
	}

	m_vLineWidth.push_back(letterRight);

	m_nNumberOfLines = lineIndex + 1;
	auto textDesiredHeight = (m_nNumberOfLines * lineHeight) /*/ contentScaleFactor*/;
	if (m_nNumberOfLines > 1)
		textDesiredHeight += (m_nNumberOfLines - 1) * lineSpacing;


	Size contentSize(longestLine, textDesiredHeight);
	SetContentSize(contentSize);

// 	_tailoredTopY = contentSize.height;
// 	_tailoredBottomY = 0.f;
// 	if (highestY > 0.f)
// 		_tailoredTopY = contentSize.height + highestY;
// 	if (lowestY < -_textDesiredHeight)
// 		_tailoredBottomY = _textDesiredHeight + lowestY;

	return true;
}
#endif

bool CHmsLabel::MultilineTextWrapByCharMaxLineCount(const std::u16string &u16StrRaw, int &letterCount)
{
    m_pFontAtlas->prepareLetterDefinitions(m_u16StrSuffix);

    const std::u16string *pU16StrTmp = &u16StrRaw;
    std::u16string u16StrTmp;
    int strRawLen = u16StrRaw.length();

    int overMaxLineIndex = 0;
    for (int i = 0; i < 30; ++i)
    {
        letterCount = pU16StrTmp->length();

        if (MultilineTextWrapByChar(*pU16StrTmp, overMaxLineIndex))
        {
            return true;
        }
        else
        {
            pU16StrTmp = &u16StrTmp;

            //包含后缀之后行数超限，减少原始字符个数
            if (overMaxLineIndex >= 0 && overMaxLineIndex >= (strRawLen - 1))
            {
                if (strRawLen > 1)
                {
                    u16StrTmp = u16StrRaw.substr(0, strRawLen - 1);
                }
                else
                {
                    u16StrTmp.clear();
                }
            }
            else
            {
                //原始字符超限，直接截取到索引位置
                if (overMaxLineIndex >= 0 && overMaxLineIndex < strRawLen)
                {
                    u16StrTmp = u16StrRaw.substr(0, overMaxLineIndex + 1);
                }
                else
                {
                    u16StrTmp.clear();
                }
            }
            strRawLen = u16StrTmp.length();

            if (strRawLen > 0)
            {
                u16StrTmp += m_u16StrSuffix;
            }
        }
    }

    return false;
}

bool CHmsLabel::MultilineTextWrapByChar(const std::u16string &u16StrRaw, int &overMaxLineIndex)
{
    int nLetterCount = 0;
    auto horiKernings = m_pFontAtlas->getFont()->getHorizontalKerningForTextUTF16(u16StrRaw, nLetterCount);

    int textLen = u16StrRaw.length();
	int lineIndex = 0;
	float nextLetterX = 0.f;
	float nextLetterY = 0.f;
	float longestLine = 0.f;
	float letterRight = 0.f;

	float lineSpacing = 0.f;
	float highestY = 0.f;
	float lowestY = 0.f;
	FontLetterDefinition letterDef;
	Vec2 letterPosition;

	float lineHeight = m_pFontAtlas->getLineHeight();
	float maxLineWidth = m_fMaxLineWidth;

    m_vLineWidth.clear();

	for (int index = 0; index < textLen; index++)
	{
        auto character = u16StrRaw[index];
		if (character == '\r')
		{
			RecordPlaceholderInfo(index, character);
			continue;
		}
		if (character == '\n')
		{
			m_vLineWidth.push_back(letterRight);
			letterRight = 0.f;
			lineIndex++;
			nextLetterX = 0.f;
			nextLetterY -= lineHeight + lineSpacing;
			RecordPlaceholderInfo(index, character);

            if (m_nMaxLineCount > 0 && lineIndex >= m_nMaxLineCount)
            {
                overMaxLineIndex = index - 1;

                HMS_SAFE_DELETE_ARRAY(horiKernings);
                return false;
            }
			continue;
		}

		if (m_pFontAtlas->getLetterDefinitionForChar(character, letterDef) == false)
		{
			RecordPlaceholderInfo(index, character);
			CCLOG("LabelTextFormatter error:can't find letter definition in font file for letter: %c", character);
			continue;
		}

		auto letterX = (nextLetterX + letterDef.offsetX)/* / contentScaleFactor*/;
		if (maxLineWidth > 0.f && nextLetterX > 0.f && letterX + letterDef.width > maxLineWidth)
		{
			m_vLineWidth.push_back(letterRight);
			letterRight = 0.f;
			lineIndex++;
			nextLetterX = 0.f;
			nextLetterY -= lineHeight + lineSpacing;
			letterPosition.x = letterDef.offsetX /*/ contentScaleFactor*/;

            if (m_nMaxLineCount > 0 && lineIndex >= m_nMaxLineCount)
            {
                overMaxLineIndex = index - 1;

                HMS_SAFE_DELETE_ARRAY(horiKernings);
                return false;
            }
		}
		else
		{
			letterPosition.x = letterX;
		}
		letterPosition.y =  (nextLetterY - letterDef.offsetY) /*/ contentScaleFactor*/;
		RecordLetterInfo(letterPosition, character, index, lineIndex);

		if (horiKernings && index < textLen - 1)
			nextLetterX += horiKernings[index + 1];

		nextLetterX += letterDef.xAdvance + m_fAdditionalKerning;

		letterRight = letterPosition.x + letterDef.width;

		if (highestY < letterPosition.y)
			highestY = letterPosition.y;
		if (lowestY > letterPosition.y - letterDef.height)
			lowestY = letterPosition.y - letterDef.height;
		if (longestLine < letterRight)
			longestLine = letterRight;
	}

	m_vLineWidth.push_back(letterRight);

	m_nNumberOfLines = lineIndex + 1;
	auto textDesiredHeight = (m_nNumberOfLines * lineHeight) /*/ contentScaleFactor*/;
	if (m_nNumberOfLines > 1)
		textDesiredHeight += (m_nNumberOfLines - 1) * lineSpacing;

	Size contentSize(longestLine, textDesiredHeight);
	SetContentSize(contentSize);

    HMS_SAFE_DELETE_ARRAY(horiKernings);
	return true;
}

int CHmsLabel::GetStringLength()
{
	int nLen = static_cast<int>(m_strText.length());
	return nLen;
}

void CHmsLabel::RecordPlaceholderInfo(int letterIndex, char16_t utf16Char)
{
	if (static_cast<std::size_t>(letterIndex) < m_vLettersInfo.size())
	{
		auto & curInfo = m_vLettersInfo.at(letterIndex);
		curInfo.utf16Char = utf16Char;
		curInfo.valid = false;
	}
	else
	{
		LetterInfo tempInfo;
		tempInfo.utf16Char = utf16Char;
		tempInfo.valid = false;
		m_vLettersInfo.push_back(tempInfo);
	}

}

void CHmsLabel::RecordLetterInfo(const Vec2& point, char16_t utf16Char, int letterIndex, int lineIndex)
{
	LetterInfo * pCurInfo = nullptr;
	if (static_cast<std::size_t>(letterIndex) < m_vLettersInfo.size())
	{
		pCurInfo = &m_vLettersInfo.at(letterIndex);
	}
	else
	{
		LetterInfo tempInfo;
		m_vLettersInfo.push_back(tempInfo);
		pCurInfo = &*m_vLettersInfo.rbegin();
	}

	pCurInfo->lineIndex = lineIndex;
	pCurInfo->utf16Char = utf16Char;
	pCurInfo->valid = m_pFontAtlas->_letterDefinitions[utf16Char].validDefinition;
	pCurInfo->positionX = point.x;
	pCurInfo->positionY = point.y;
}

void CHmsLabel::BindVaoAndVbo()
{
#ifdef HMS_RENDER_USE_VAO
	//generate vbo and vao for trianglesCommand
	glGenVertexArrays(1, &m_vao);
	GL::bindVAO(m_vao);

	glGenBuffers(2, &m_vbo[0]);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C4B_T2F_Quad) * m_vQuadString.size(), m_vQuadString.data(), GL_DYNAMIC_DRAW);

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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * m_vQuadIndex.size(), m_vQuadIndex.data(), GL_DYNAMIC_DRAW);

	// Must unbind the VAO before changing the element buffer.
	GL::bindVAO(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}

void CHmsLabel::BindVbo()
{
	glGenBuffers(2, &m_vbo[0]);
	UpdateBindVboData();
}

void CHmsLabel::UpdateBindVaoAndVboData()
{
	GL::bindVAO(m_vao);
	//Set VBO data
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C4B_T2F_Quad) * m_vQuadString.size(), m_vQuadString.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * m_vQuadIndex.size(), m_vQuadIndex.data(), GL_DYNAMIC_DRAW);

	GL::bindVAO(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CHmsLabel::UpdateBindVboData()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C4B_T2F_Quad) * m_vQuadString.size(), m_vQuadString.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * m_vQuadIndex.size(), m_vQuadIndex.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CHmsLabel::OnDraw(const Mat4 &transform, uint32_t flags)
{
// 	static GLuint s_nTestTexture = 0;
// 	if (s_nTestTexture == 0)
// 	{
// 		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
// 		glGenTextures(1, &s_nTestTexture);
// 
// 		glBindTexture(GL_TEXTURE_2D, s_nTestTexture);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
// 		
// 		auto m_nWidth = 512;
// 		auto m_nHeight = 512;
// 		int nDataLen = m_nWidth*m_nHeight * 4;
// 		unsigned char * pData = new unsigned char[nDataLen];
// 		memset(pData, 255, nDataLen);
// 
// 		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_nWidth, m_nHeight,
// 			0, GL_RGBA, GL_UNSIGNED_BYTE, pData);
// 
// 		delete pData;
// 
// 		glBindTexture(GL_TEXTURE_2D, 0);
// 	}

	auto glProgram = GetGLProgram();
	glProgram->use();

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

#if 0
 	auto glState = GetGLProgramState();
 	glState->apply(transform);
#else
	glProgram->setUniformsForBuiltins(transform);
#endif
	if (m_nUniformTextColor)
	{
		glProgram->setUniformLocationWith4f(m_nUniformTextColor, m_colorDisplay.r / 255.0f, m_colorDisplay.g / 255.0f, m_colorDisplay.b / 255.0f, m_opacityDisplay / 255.0f);
	}
	if (m_nUniformEffectColor)
	{
		glProgram->setUniformLocationWith4fv(m_nUniformEffectColor, &m_colorEffect.r, 1);
	}



	GL::blendFunc(m_blendFunc.src, m_blendFunc.dst);
	glEnable(GL_BLEND);

	for (auto & c:m_mapAtlasDrawInfo)
	{
		GL::bindTexture2D(c.second.nAtlasTexture);
		//GL::bindTexture2D(0);
		glDrawElements(GL_TRIANGLES, (GLsizei)c.second.vQuadIndex.size()*6, GL_UNSIGNED_SHORT, (GLvoid*)(c.second.nQuadIndexStart*sizeof(GLushort)));
	}

	if (Renderer::UseGlVao())
	{
		GL::bindVAO(0);
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}

void CHmsLabel::UpdateAlignmentOffset()
{
	m_vLinesOffsetX.clear();
	switch (m_eAlignHorizontal)
	{
	case HmsAviPf::TextHAlignment::LEFT:
		m_vLinesOffsetX.assign(m_nNumberOfLines, 0);
		break;
	case HmsAviPf::TextHAlignment::CENTER:
		m_vLineWidth.reserve(m_nNumberOfLines);
		for (auto lw:m_vLineWidth)
		{
			m_vLinesOffsetX.push_back((m_sizeContent.width - lw) / 2.0f);
		}
		break;
	case HmsAviPf::TextHAlignment::RIGHT:
		m_vLinesOffsetX.reserve(m_nNumberOfLines);
		for (auto lw : m_vLineWidth)
		{
			m_vLinesOffsetX.push_back(m_sizeContent.width - lw);
		}
		break;
		break;
	default:
		break;
	}

	switch (m_eAlignVertical)
	{
	case HmsAviPf::TextVAlignment::TOP:
		m_fLinesOffsetY = m_sizeContent.height;
		break;
	case HmsAviPf::TextVAlignment::CENTER:
		m_fLinesOffsetY = m_sizeContent.height;
		break;
	case HmsAviPf::TextVAlignment::BOTTOM:
		m_fLinesOffsetY = m_sizeContent.height;
		break;
	default:
		break;
	}
}

void CHmsLabel::EnableOutline(const Color4F & outlineColor, int outlineSize /*= -1*/)
{
	m_colorEffect = outlineColor;
	if (outlineSize > 0 || m_eLabelEffect == LabelEffect::OUTLINE)
	{
		if (m_ttfConfig.outlineSize != outlineSize)
		{
			m_ttfConfig.outlineSize = outlineSize;
			SetTTFConfig(m_ttfConfig);
		}
	}
}

void CHmsLabel::EnableGlow(const Color4F & glowColor)
{
	if (m_ttfConfig.distanceFieldEnabled == false)
	{
		m_ttfConfig.outlineSize = 0;
		m_ttfConfig.distanceFieldEnabled = true;
		SetTTFConfig(m_ttfConfig);
		m_bContextDirty = true;
		m_eLabelEffect = LabelEffect::GLOW;
		m_colorEffect = glowColor;
		UpdateShaderProgram();
	}
}

void CHmsLabel::SetAdditionalKerning(float additionalKerning)
{
	m_fAdditionalKerning = additionalKerning;
}

void HmsAviPf::CHmsLabel::SetMaxLineCount(int lineCount)
{
    m_nMaxLineCount = lineCount;
}

void HmsAviPf::CHmsLabel::SetMaxLineWidth(float fMaxLineWidth)
{
	m_fMaxLineWidth = fMaxLineWidth;
}

void CHmsLabel::SetTextColor(const Color4B & color)
{
	SetColor(Color3B(color));
}

Size CHmsLabel::GetContentSize()
{
	if (m_bContextDirty)
	{
		UpdateTextString();
	}
	return m_sizeContent;
}

const Mat4& CHmsLabel::GetNodeToParentTransform() const
{
	if (!m_bPixelAlign)
	{
		return CHmsNode::GetNodeToParentTransform();
	}

	if (m_transformDirty)
	{
		// Translate values
		float x = m_pos.x;
		float y = m_pos.y;
		float z = m_posZ;

		if (m_ignoreAnchorPointForPosition)
		{
			x += m_anchorPointInPoints.x;
			y += m_anchorPointInPoints.y;
		}

		//bool needsSkewMatrix = (_skewX || _skewY);


		Vec2 anchorPoint(m_anchorPointInPoints.x * m_scaleX, m_anchorPointInPoints.y * m_scaleY);

		// calculate real position
		if (/*!needsSkewMatrix &&*/ !m_anchorPointInPoints.isZero())
		{
			x += -anchorPoint.x;
			y += -anchorPoint.y;
			x = std::floor(x);
			y = std::floor(y);
		}

		// Build Transform Matrix = translation * rotation * scale
		Mat4 translation;
		//move to anchor point first, then rotate
		Mat4::createTranslation(x + anchorPoint.x, y + anchorPoint.y, z, &translation);

		Mat4::createRotation(m_rotationQuat, &m_transform);

		m_transform = translation * m_transform;
		//move by (-anchorPoint.x, -anchorPoint.y, 0) after rotation
		m_transform.translate(-anchorPoint.x, -anchorPoint.y, 0);


		if (m_scaleX != 1.f)
		{
			m_transform.m[0] *= m_scaleX, m_transform.m[1] *= m_scaleX, m_transform.m[2] *= m_scaleX;
		}
		if (m_scaleY != 1.f)
		{
			m_transform.m[4] *= m_scaleY, m_transform.m[5] *= m_scaleY, m_transform.m[6] *= m_scaleY;
		}
		if (m_scaleZ != 1.f)
		{
			m_transform.m[8] *= m_scaleZ, m_transform.m[9] *= m_scaleZ, m_transform.m[10] *= m_scaleZ;
		}

		if (m_useAdditionalTransform)
		{
			m_transform = m_transform * m_additionalTransform;
		}

		m_transformDirty = false;
	}

	return m_transform;
}



NS_HMS_END
