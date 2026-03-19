/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 HmsAviPf-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.

http://www.HmsAviPf-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "render/HmsTextureAtlas.h"

#include <stdlib.h>

#include "HmsAviMacros.h"
#include "render/HmsTextureCache.h"
#include "render/HmsGLProgram.h"
#include "render/HmsGLStateCache.h"
#include "render/HmsRenderer.h"
#include "render/HmsTexture2D.h"
#include "HmsGL.h"

//According to some tests GL_TRIANGLE_STRIP is slower, MUCH slower. Probably I'm doing something very wrong

// implementation TextureAtlas

NS_HMS_BEGIN

TextureAtlas::TextureAtlas()
    :_indices(nullptr)
    ,_dirty(false)
    ,_texture(nullptr)
    ,_quads(nullptr)
#if HMS_ENABLE_CACHE_TEXTURE_DATA
    ,_rendererRecreatedListener(nullptr)
#endif
{}

TextureAtlas::~TextureAtlas()
{
    CCLOGINFO("deallocing TextureAtlas: %p", this);

    HMS_SAFE_FREE(_quads);
    HMS_SAFE_FREE(_indices);

    glDeleteBuffers(2, _buffersVBO);

#ifdef HMS_RENDER_USE_VAO
	if (Renderer::UseGlVao())
    {
        glDeleteVertexArrays(1, &_VAOname);
        GL::bindVAO(0);
    }
#endif

    HMS_SAFE_RELEASE(_texture);
    
#if HMS_ENABLE_CACHE_TEXTURE_DATA
    Director::GetInstance()->getEventDispatcher()->removeEventListener(_rendererRecreatedListener);
#endif
}

ssize_t TextureAtlas::getTotalQuads() const
{
    return _totalQuads;
}

ssize_t TextureAtlas::getCapacity() const
{
    return _capacity;
}

Texture2D* TextureAtlas::getTexture() const
{
    return _texture;
}

void TextureAtlas::setTexture(Texture2D * var)
{
    HMS_SAFE_RETAIN(var);
    HMS_SAFE_RELEASE(_texture);
    _texture = var;
}

V3F_C4B_T2F_Quad* TextureAtlas::getQuads()
{
    //if someone accesses the quads directly, presume that changes will be made
    _dirty = true;
    return _quads;
}

void TextureAtlas::setQuads(V3F_C4B_T2F_Quad* quads)
{
    _quads = quads;
}

// TextureAtlas - alloc & init

TextureAtlas * TextureAtlas::create(const std::string& file, ssize_t capacity)
{
    TextureAtlas * textureAtlas = new  TextureAtlas();
    if(textureAtlas && textureAtlas->initWithFile(file, capacity))
    {
        textureAtlas->autorelease();
        return textureAtlas;
    }
    HMS_SAFE_DELETE(textureAtlas);
    return nullptr;
}

TextureAtlas * TextureAtlas::createWithTexture(Texture2D *texture, ssize_t capacity)
{
    TextureAtlas * textureAtlas = new  TextureAtlas();
    if (textureAtlas && textureAtlas->initWithTexture(texture, capacity))
    {
        textureAtlas->autorelease();
        return textureAtlas;
    }
    HMS_SAFE_DELETE(textureAtlas);
    return nullptr;
}

bool TextureAtlas::initWithFile(const std::string& file, ssize_t capacity)
{
#if 0
    // retained in property
    Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(file);

    if (texture)
    {   
        return initWithTexture(texture, capacity);
    }
    else
    {
        CCLOG("HmsAviPf: Could not open file: %s", file.c_str());
        return false;
    }
#else

	return false;
#endif
}

bool TextureAtlas::initWithTexture(Texture2D *texture, ssize_t capacity)
{
    HMSASSERT(capacity>=0, "Capacity must be >= 0");
    
//    CCASSERT(texture != nullptr, "texture should not be null");
    _capacity = capacity;
    _totalQuads = 0;

    // retained in property
    this->_texture = texture;
    HMS_SAFE_RETAIN(_texture);

    // Re-initialization is not allowed
    HMSASSERT(_quads == nullptr && _indices == nullptr, "_quads and _indices should be nullptr.");

    _quads = (V3F_C4B_T2F_Quad*)malloc( _capacity * sizeof(V3F_C4B_T2F_Quad) );
    _indices = (GLushort *)malloc( _capacity * 6 * sizeof(GLushort) );
    
    if( ! ( _quads && _indices) && _capacity > 0) 
    {
        //CCLOG("HmsAviPf: TextureAtlas: not enough memory");
        HMS_SAFE_FREE(_quads);
        HMS_SAFE_FREE(_indices);

        // release texture, should set it to null, because the destruction will
        // release it too. see HmsAviPf-x issue #484
        HMS_SAFE_RELEASE_NULL(_texture);
        return false;
    }

    memset( _quads, 0, _capacity * sizeof(V3F_C4B_T2F_Quad) );
    memset( _indices, 0, _capacity * 6 * sizeof(GLushort) );
    
#if HMS_ENABLE_CACHE_TEXTURE_DATA
    /** listen the event that renderer was recreated on Android/WP8 */
    _rendererRecreatedListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, HMS_CALLBACK_1(TextureAtlas::listenRendererRecreated, this));
    Director::GetInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_rendererRecreatedListener, -1);
#endif
    
    this->setupIndices();

    //if (Configuration::getInstance()->supportsShareableVAO())
	if (false)
    {
        setupVBOandVAO();
    }
    else
    {
        setupVBO();
    }

    _dirty = true;

    return true;
}

void TextureAtlas::listenRendererRecreated(EventCustom* event)
{  
    //if (Configuration::getInstance()->supportsShareableVAO())
	if (false)
    {
        setupVBOandVAO();
    }
    else
    {
        setupVBO();
    }
    
    // set _dirty to true to force it rebinding buffer
    _dirty = true;
}

std::string TextureAtlas::getDescription() const
{
    //return StringUtils::format("<TextureAtlas | totalQuads = %d>", static_cast<int>(_totalQuads));
	return "TextureAtlas";
}


void TextureAtlas::setupIndices()
{
    if (_capacity == 0)
        return;

    for( int i=0; i < _capacity; i++)
    {
        _indices[i*6+0] = i*4+0;
        _indices[i*6+1] = i*4+1;
        _indices[i*6+2] = i*4+2;

        // inverted index. issue #179
        _indices[i*6+3] = i*4+3;
        _indices[i*6+4] = i*4+2;
        _indices[i*6+5] = i*4+1;        
    }
}

//TextureAtlas - VAO / VBO specific

void TextureAtlas::setupVBOandVAO()
{
#ifdef HMS_RENDER_USE_VAO
    glGenVertexArrays(1, &_VAOname);
    GL::bindVAO(_VAOname);

#define kQuadSize sizeof(_quads[0].bl)

    glGenBuffers(2, &_buffersVBO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_quads[0]) * _capacity, _quads, GL_DYNAMIC_DRAW);

    // vertices
    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, vertices));

    // colors
    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_COLOR);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, colors));

    // tex coords
    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_TEX_COORD);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, texCoords));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices[0]) * _capacity * 6, _indices, GL_STATIC_DRAW);

    // Must unbind the VAO before changing the element buffer.
    GL::bindVAO(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //CHECK_GL_ERROR_DEBUG();
#endif
}

void TextureAtlas::setupVBO()
{
    glGenBuffers(2, &_buffersVBO[0]);

    mapBuffers();
}

void TextureAtlas::mapBuffers()
{
    // Avoid changing the element buffer for whatever VAO might be bound.
	GL::bindVAO(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_quads[0]) * _capacity, _quads, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices[0]) * _capacity * 6, _indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //CHECK_GL_ERROR_DEBUG();
}

// TextureAtlas - Update, Insert, Move & Remove

void TextureAtlas::updateQuad(V3F_C4B_T2F_Quad *quad, ssize_t index)
{
    HMSASSERT( index >= 0 && index < _capacity, "updateQuadWithTexture: Invalid index");

    _totalQuads = MAX( index+1, _totalQuads);

    _quads[index] = *quad;    


    _dirty = true;

}

void TextureAtlas::insertQuad(V3F_C4B_T2F_Quad *quad, ssize_t index)
{
    HMSASSERT( index>=0 && index<_capacity, "insertQuadWithTexture: Invalid index");

    _totalQuads++;
    HMSASSERT( _totalQuads <= _capacity, "invalid totalQuads");

    // issue #575. index can be > totalQuads
    auto remaining = (_totalQuads-1) - index;

    // last object doesn't need to be moved
    if( remaining > 0) 
    {
        // texture coordinates
        memmove( &_quads[index+1],&_quads[index], sizeof(_quads[0]) * remaining );        
    }

    _quads[index] = *quad;


    _dirty = true;

}

void TextureAtlas::insertQuads(V3F_C4B_T2F_Quad* quads, ssize_t index, ssize_t amount)
{
    HMSASSERT(index>=0 && amount>=0 && index+amount<=_capacity, "insertQuadWithTexture: Invalid index + amount");

    _totalQuads += amount;

    HMSASSERT( _totalQuads <= _capacity, "invalid totalQuads");

    // issue #575. index can be > totalQuads
    auto remaining = (_totalQuads-1) - index - amount;

    // last object doesn't need to be moved
    if( remaining > 0)
    {
        // tex coordinates
        memmove( &_quads[index+amount],&_quads[index], sizeof(_quads[0]) * remaining );
    }


    auto max = index + amount;
    int j = 0;
    for (ssize_t i = index; i < max ; i++)
    {
        _quads[index] = quads[j];
        index++;
        j++;
    }

    _dirty = true;
}

void TextureAtlas::insertQuadFromIndex(ssize_t oldIndex, ssize_t newIndex)
{
    HMSASSERT( newIndex >= 0 && newIndex < _totalQuads, "insertQuadFromIndex:atIndex: Invalid index");
    HMSASSERT( oldIndex >= 0 && oldIndex < _totalQuads, "insertQuadFromIndex:atIndex: Invalid index");

    if( oldIndex == newIndex )
    {
        return;
    }
    // because it is ambiguous in iphone, so we implement abs ourselves
    // unsigned int howMany = abs( oldIndex - newIndex);
    auto howMany = (oldIndex - newIndex) > 0 ? (oldIndex - newIndex) :  (newIndex - oldIndex);
    auto dst = oldIndex;
    auto src = oldIndex + 1;
    if( oldIndex > newIndex)
    {
        dst = newIndex+1;
        src = newIndex;
    }

    // texture coordinates
    V3F_C4B_T2F_Quad quadsBackup = _quads[oldIndex];
    memmove( &_quads[dst],&_quads[src], sizeof(_quads[0]) * howMany );
    _quads[newIndex] = quadsBackup;


    _dirty = true;
}

void TextureAtlas::removeQuadAtIndex(ssize_t index)
{
    HMSASSERT( index>=0 && index<_totalQuads, "removeQuadAtIndex: Invalid index");

    auto remaining = (_totalQuads-1) - index;

    // last object doesn't need to be moved
    if( remaining ) 
    {
        // texture coordinates
        memmove( &_quads[index],&_quads[index+1], sizeof(_quads[0]) * remaining );
    }

    _totalQuads--;


    _dirty = true;
}

void TextureAtlas::removeQuadsAtIndex(ssize_t index, ssize_t amount)
{
    HMSASSERT(index>=0 && amount>=0 && index+amount<=_totalQuads, "removeQuadAtIndex: index + amount out of bounds");

    auto remaining = (_totalQuads) - (index + amount);

    _totalQuads -= amount;

    if ( remaining )
    {
        memmove( &_quads[index], &_quads[index+amount], sizeof(_quads[0]) * remaining );
    }

    _dirty = true;
}

void TextureAtlas::removeAllQuads()
{
    _totalQuads = 0;
}

// TextureAtlas - Resize
bool TextureAtlas::resizeCapacity(ssize_t newCapacity)
{
    HMSASSERT(newCapacity>=0, "capacity >= 0");
    if( newCapacity == _capacity )
    {
        return true;
    }
    auto oldCapactiy = _capacity;
    // update capacity and totolQuads
    _totalQuads = MIN(_totalQuads, newCapacity);
    _capacity = newCapacity;

    V3F_C4B_T2F_Quad* tmpQuads = nullptr;
    GLushort* tmpIndices = nullptr;
    
    // when calling initWithTexture(fileName, 0) on bada device, calloc(0, 1) will fail and return nullptr,
    // so here must judge whether _quads and _indices is nullptr.
    if (_quads == nullptr)
    {
        tmpQuads = (V3F_C4B_T2F_Quad*)malloc( _capacity * sizeof(_quads[0]) );
        if (tmpQuads != nullptr)
        {
            memset(tmpQuads, 0, _capacity * sizeof(_quads[0]) );
        }
    }
    else
    {
        tmpQuads = (V3F_C4B_T2F_Quad*)realloc( _quads, sizeof(_quads[0]) * _capacity );
        if (tmpQuads != nullptr && _capacity > oldCapactiy)
        {
            memset(tmpQuads+oldCapactiy, 0, (_capacity - oldCapactiy)*sizeof(_quads[0]) );
        }
        _quads = nullptr;
    }

    if (_indices == nullptr)
    {    
        tmpIndices = (GLushort*)malloc( _capacity * 6 * sizeof(_indices[0]) );
        if (tmpIndices != nullptr)
        {
            memset( tmpIndices, 0, _capacity * 6 * sizeof(_indices[0]) );
        }
    }
    else
    {
        tmpIndices = (GLushort*)realloc( _indices, sizeof(_indices[0]) * _capacity * 6 );
        if (tmpIndices != nullptr && _capacity > oldCapactiy)
        {
            memset( tmpIndices+oldCapactiy, 0, (_capacity-oldCapactiy) * 6 * sizeof(_indices[0]) );
        }
        _indices = nullptr;
    }

    if( ! ( tmpQuads && tmpIndices) ) {
        CCLOG("HmsAviPf: TextureAtlas: not enough memory");
        HMS_SAFE_FREE(tmpQuads);
        HMS_SAFE_FREE(tmpIndices);
        HMS_SAFE_FREE(_quads);
        HMS_SAFE_FREE(_indices);
        _capacity = _totalQuads = 0;
        return false;
    }

    _quads = tmpQuads;
    _indices = tmpIndices;


    setupIndices();
    mapBuffers();

    _dirty = true;

    return true;
}

void TextureAtlas::increaseTotalQuadsWith(ssize_t amount)
{
    HMSASSERT(amount>=0, "amount >= 0");
    _totalQuads += amount;
}

void TextureAtlas::moveQuadsFromIndex(ssize_t oldIndex, ssize_t amount, ssize_t newIndex)
{
    HMSASSERT(oldIndex>=0 && amount>=0 && newIndex>=0, "values must be >= 0");
    HMSASSERT(newIndex + amount <= _totalQuads, "insertQuadFromIndex:atIndex: Invalid index");
    HMSASSERT(oldIndex < _totalQuads, "insertQuadFromIndex:atIndex: Invalid index");

    if( oldIndex == newIndex )
    {
        return;
    }
    //create buffer
    size_t quadSize = sizeof(V3F_C4B_T2F_Quad);
    V3F_C4B_T2F_Quad* tempQuads = (V3F_C4B_T2F_Quad*)malloc( quadSize * amount);
    memcpy( tempQuads, &_quads[oldIndex], quadSize * amount );

    if (newIndex < oldIndex)
    {
        // move quads from newIndex to newIndex + amount to make room for buffer
        memmove( &_quads[newIndex], &_quads[newIndex+amount], (oldIndex-newIndex)*quadSize);
    }
    else
    {
        // move quads above back
        memmove( &_quads[oldIndex], &_quads[oldIndex+amount], (newIndex-oldIndex)*quadSize);
    }
    memcpy( &_quads[newIndex], tempQuads, amount*quadSize);

    free(tempQuads);

    _dirty = true;
}

void TextureAtlas::moveQuadsFromIndex(ssize_t index, ssize_t newIndex)
{
    HMSASSERT(index>=0 && newIndex>=0, "values must be >= 0");
    HMSASSERT(newIndex + (_totalQuads - index) <= _capacity, "moveQuadsFromIndex move is out of bounds");

    memmove(_quads + newIndex,_quads + index, (_totalQuads - index) * sizeof(_quads[0]));
}

void TextureAtlas::fillWithEmptyQuadsFromIndex(ssize_t index, ssize_t amount)
{
    HMSASSERT(index>=0 && amount>=0, "values must be >= 0");
    V3F_C4B_T2F_Quad quad;
    memset(&quad, 0, sizeof(quad));

    auto to = index + amount;
    for (ssize_t i = index ; i < to ; i++)
    {
        _quads[i] = quad;
    }
}

// TextureAtlas - Drawing

void TextureAtlas::drawQuads()
{
    this->drawNumberOfQuads(_totalQuads, 0);
}

void TextureAtlas::drawNumberOfQuads(ssize_t numberOfQuads)
{
    HMSASSERT(numberOfQuads>=0, "numberOfQuads must be >= 0");
    this->drawNumberOfQuads(numberOfQuads, 0);
}

void TextureAtlas::drawNumberOfQuads(ssize_t numberOfQuads, ssize_t start)
{
    HMSASSERT(numberOfQuads>=0 && start>=0, "numberOfQuads and start must be >= 0");

    if(!numberOfQuads)
        return;
    
    GL::bindTexture2D(_texture->getName());

   // if (Configuration::getInstance()->supportsShareableVAO())
	if (Renderer::UseGlVao())
    {
        //
        // Using VBO and VAO
        //

        // FIXME:: update is done in draw... perhaps it should be done in a timer
        if (_dirty) 
        {
            glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);

#if USE_GL_MAP_BUFFER
			//use option   orphaning + glMapBuffer
			glBufferData(GL_ARRAY_BUFFER, sizeof(_quads[0]) * _capacity, nullptr, GL_DYNAMIC_DRAW);
			void *buf = glMapBufferOES(GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES);
			memcpy(buf, _quads, sizeof(_quads[0])* _totalQuads);
			glUnmapBuffer(GL_ARRAY_BUFFER);
#else
			//use option subdata
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(_quads[0]) * 0, sizeof(_quads[0])* _totalQuads, &_quads[0]);

			//use option data
			//glBufferData(GL_ARRAY_BUFFER, sizeof(_quads[0]) * _filledVertex, &_quads[0], GL_DYNAMIC_DRAW);
#endif
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            _dirty = false;
        }

        GL::bindVAO(_VAOname);

#if HMS_REBIND_INDICES_BUFFER
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
#endif

        glDrawElements(GL_TRIANGLES, (GLsizei) numberOfQuads*6, GL_UNSIGNED_SHORT, (GLvoid*) (start*6*sizeof(_indices[0])) );
        
        GL::bindVAO(0);
        
#if HMS_REBIND_INDICES_BUFFER
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif

    }
    else
    {
        //
        // Using VBO without VAO
        //

#define kQuadSize sizeof(_quads[0].bl)
        glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);

        // FIXME:: update is done in draw... perhaps it should be done in a timer
        if (_dirty) 
        {
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(_quads[0]) * _totalQuads , &_quads[0] );
            _dirty = false;
        }

        GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);

        // vertices
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof(V3F_C4B_T2F, vertices));

        // colors
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof(V3F_C4B_T2F, colors));

        // tex coords
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof(V3F_C4B_T2F, texCoords));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);

        glDrawElements(GL_TRIANGLES, (GLsizei)numberOfQuads*6, GL_UNSIGNED_SHORT, (GLvoid*) (start*6*sizeof(_indices[0])));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

   // HMS_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1,numberOfQuads*6);
    
   // CHECK_GL_ERROR_DEBUG();
}


NS_HMS_END

