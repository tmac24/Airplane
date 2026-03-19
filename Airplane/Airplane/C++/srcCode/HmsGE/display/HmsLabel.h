#pragma once

#include "base/HmsNode.h"
#include "font/HmsTTFConfig.h"
#include "render/HmsQuadCommand.h"
#include "render/HmsCustomCommand.h"
#include <string>

NS_HMS_BEGIN

class FontAtlas;

class HMS_DLL CHmsLabel : public CHmsNode
{
protected:
	CHmsLabel();
	virtual ~CHmsLabel();

public:
	static CHmsLabel * Create(const TTFConfig & ttfConfig, const std::string & strText);

	static CHmsLabel * createWithTTF(const std::string & strText, const std::string & fontPath, float fFontSize);

	bool SetTTFConfig(const TTFConfig & ttfConfig);

	TTFConfig GetTTFConfig();

	std::string GetString();

	void SetString(const char * strText);

	void SetString(const std::string & strText);

	void SetString(const std::u16string & strText);

	void SetStringUTF8(const std::string & strText);

	void SetPixelAlign(bool bPixelAligh);

	bool GetPixelAlign();

	int GetStringLength();

	void EnableOutline(const Color4F & outlineColor, int outlineSize = -1);
	void EnableGlow(const Color4F & glowColor);

	void SetTextColor(const Color4B & color);

	void SetAdditionalKerning(float additionalKerning);

	void SetMaxLineWidth(float fMaxLineWidth);
    void SetMaxLineCount(int lineCount);

	float GetLineHeight();
	//use this lines need label was showed, or call get the content size first
	int GetLines(){ return m_nNumberOfLines; }

public:
	virtual void Draw(Renderer * renderer, const Mat4 & transform, uint32_t flags);

	virtual void Visit(Renderer * renderer, const Mat4 & parentTransform, uint32_t parentFlags);

	void OnDraw(const Mat4 &transform, uint32_t flags);

	virtual Size GetContentSize();

	void UpdateTextString();

	virtual const Mat4& GetNodeToParentTransform() const;
protected:
	void UpdateShaderProgram();
	
	void UpdateAlignmentOffset();
	void SetFontAtlas(FontAtlas * fontAtlas);

#if 0
	bool MultilineTextWrapByWord(int * horiKernings = nullptr);
#endif

    bool MultilineTextWrapByCharMaxLineCount(const std::u16string &u16StrRaw, int &letterCount);
    bool MultilineTextWrapByChar(const std::u16string &u16StrRaw, int &overMaxLineIndex);
    
	void RecordPlaceholderInfo(int letterIndex, char16_t utf16Char);
	void RecordLetterInfo(const Vec2& point, char16_t utf16Char, int letterIndex, int lineIndex);

	void BindVaoAndVbo();
	void UpdateBindVaoAndVboData();
	
	void BindVbo();
	void UpdateBindVboData();
protected:
	struct LetterInfo
	{
		char16_t utf16Char;
		bool valid;
		float positionX;
		float positionY;
		int atlasIndex;
		int lineIndex;
	};

	struct AtlasDrawInfo
	{
		GLuint						nAtlasTexture;			//the texture name
		std::vector<GLushort>		vQuadIndex;				//the quad index on the m_vQuadString
		GLuint						nQuadIndexStart;		//the start of the index of vbo
	};

	TTFConfig				m_ttfConfig;
	LabelEffect				m_eLabelEffect;
	std::u16string			m_strText;				//text for UTF-16

	FontAtlas				* m_pFontAtlas;

	bool					m_bUseDistanceField;
	bool					m_bUseA8Shader;
	bool					m_bShadowEnabled;

	bool					m_bContextDirty;		//Is the context dirty;
	bool					m_bPixelAlign;			//default true


	GLuint					m_nUniformEffectColor;
	GLuint					m_nUniformTextColor;

	GLuint					m_vao;
	GLuint					m_vbo[2];
	BlendFunc				m_blendFunc;
	CustomCommand			m_customCommand;

	std::vector<V3F_C4B_T2F_Quad>	m_vQuadString;
	std::vector<LetterInfo>			m_vLettersInfo;
	std::vector<GLushort>			m_vQuadIndex;						//

	std::unordered_map<GLuint, AtlasDrawInfo>	m_mapAtlasDrawInfo;		//Is used for draw char
	std::vector<float>							m_vLineWidth;			//record the every line width
	int											m_nNumberOfLines;
	float										m_fLinesOffsetY;		//the lines offset of y, used for vertical alignment
	std::vector<float>							m_vLinesOffsetX;		//the lines offset of x, used for horizontal aligment
	TextHAlignment								m_eAlignHorizontal;
	TextVAlignment								m_eAlignVertical;
	float										m_fAdditionalKerning;	//add kerning between two char
	float										m_fMaxLineWidth;		//the max width of the line, 0:is not use
    int                                         m_nMaxLineCount;
    std::u16string                              m_u16StrSuffix;

	Color4F										m_colorEffect;

private:
	HMS_DISALLOW_COPY_AND_ASSIGN(CHmsLabel);

};

NS_HMS_END