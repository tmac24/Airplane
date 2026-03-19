#ifndef _HMS_QUADCOMMAND_H_
#define _HMS_QUADCOMMAND_H_

#include "render/HmsRenderCommand.h"
#include "render/HmsGLProgramState.h"

/**
 * @addtogroup renderer
 * @{
 */

NS_HMS_BEGIN

/** 
 Command used to render one or more Quads, similar to TrianglesCommand.
 Every QuadCommand will have generate material ID by give textureID, glProgramState, Blend function
 if the material id is the same, these QuadCommands could be batched to save draw call.
 */
class HMS_DLL QuadCommand : public RenderCommand
{
public:
    /**Constructor.*/
    QuadCommand();
    /**Destructor.*/
    ~QuadCommand();
    
    /** Initializes the command.
     @param globalOrder GlobalZOrder of the command.
     @param textureID The openGL handle of the used texture.
     @param shader The specified glProgram and its uniform.
     @param blendType Blend function for the command.
     @param quads Rendered quads for the command.
     @param quadCount The number of quads when rendering.
     @param mv ModelView matrix for the command.
     @param flags to indicate that the command is using 3D rendering or not.
     */
    void init(float globalOrder, GLuint textureID, GLProgramState* shader, const BlendFunc& blendType, V3F_C4B_T2F_Quad* quads, ssize_t quadCount,
              const Mat4& mv, uint32_t flags);
    /**Apply the texture, shaders, programs, blend functions to GPU pipeline.*/
    void useMaterial() const;
    /**Get the material id of command.*/
    inline uint32_t getMaterialID() const { return _materialID; }
    /**Get the openGL texture handle.*/
    inline GLuint getTextureID() const { return _textureID; }
    /**Get the pointer of the rendered quads.*/
    inline V3F_C4B_T2F_Quad* getQuads() const { return _quads; }
    /**Get the number of quads for rendering.*/
    inline ssize_t getQuadCount() const { return _quadsCount; }
    /**Get the glprogramstate.*/
    inline GLProgramState* getGLProgramState() const { return _glProgramState; }
    /**Get the blend function.*/
    inline BlendFunc getBlendType() const { return _blendType; }
    /**Get the model view matrix.*/
    inline const Mat4& getModelView() const { return _mv; }
    
protected:
    /**Generate the material ID by textureID, glProgramState, and blend function.*/
    void generateMaterialID();
    
    /**Generated material id.*/
    uint32_t _materialID;
    /**OpenGL handle for texture.*/
    GLuint _textureID;
    /**GLprogramstate for the command. encapsulate shaders and uniforms.*/
    GLProgramState* _glProgramState;
    /**Blend function when rendering the triangles.*/
    BlendFunc _blendType;
    /**The pointer to the rendered quads.*/
    V3F_C4B_T2F_Quad* _quads;
    /**The number of quads for rendering.*/
    ssize_t _quadsCount;
    /**Model view matrix when rendering the triangles.*/
    Mat4 _mv;
};

NS_HMS_END

/**
 end of support group
 @}
 */
#endif //_HMS_QUADCOMMAND_H_
