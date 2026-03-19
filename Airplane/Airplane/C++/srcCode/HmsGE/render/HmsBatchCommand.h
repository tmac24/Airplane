#ifndef _HMS_BATCHCOMMAND_H_
#define _HMS_BATCHCOMMAND_H_

#include "render/HmsRenderCommand.h"


NS_HMS_BEGIN

class TextureAtlas;
class GLProgram;
/**
Command used to draw batches in one TextureAtlas.
*/
class HMS_DLL BatchCommand : public RenderCommand
{
public:
    /**Constructor.*/
    BatchCommand();
    /**Destructor.*/
    ~BatchCommand();
    /**Init the batch command.
    @param globalZOrder GlobalZOrder of the render command.
    @param shader Shader used for draw the texture atlas.
    @param blendType Blend function for texture atlas.
    @param textureAtlas Texture atlas, will contain both quads and texture for rendering.
    @param modelViewTransform Model view transform used for rendering.
    @param flags Indicate the render command should be rendered in 3D mode or not.
    */
    void init(float globalZOrder, GLProgram* shader, BlendFunc blendType, TextureAtlas *textureAtlas, const Mat4& modelViewTransform, uint32_t flags);


    /**Execute the command, which will call openGL function to draw the texture atlas.*/
    void execute();

protected:
    //TODO: This member variable is not used. It should be removed.
    int32_t _materialID;
    /**Texture ID used for texture atlas rendering.*/
    GLuint _textureID;
    /**Shaders used for rendering.*/
    GLProgram* _shader;
    /**Blend function for rendering.*/
    BlendFunc _blendType;
    /**Texture atlas for rendering.*/
    TextureAtlas *_textureAtlas;

    /**ModelView transform.*/
    Mat4 _mv;
};
NS_HMS_END

/**
 end of support group
 @}
 */
#endif //_HMS_BATCHCOMMAND_H_
