#ifndef __CCRENDERCOMMAND_H_
#define __CCRENDERCOMMAND_H_

#include <stdint.h>
#include "HmsAviMacros.h"
#include "base/HmsTypes.h"

NS_HMS_BEGIN

/** Base class of the `RenderCommand` hierarchy.
*
 The `Renderer` knows how to render `RenderCommands` objects.
 */
class HMS_DLL RenderCommand
{
public:
    /**Enum the type of render command. */
    enum class Type
    {
       
        UNKNOWN_COMMAND,				/** Reserved type.*/   
        QUAD_COMMAND,					/** Quad command, used for draw quad.*/        
        CUSTOM_COMMAND,					/**Custom command, used for calling callback for rendering.*/
        BATCH_COMMAND,					/**Batch command, used for draw batches in texture atlas.*/  
        GROUP_COMMAND,					/**Group command, which can group command in a tree hierarchy.*/
        MESH_COMMAND,					/**Mesh command, used to draw 3D meshes.*/      
        PRIMITIVE_COMMAND,				/**Primitive command, used to draw primitives such as lines, points and triangles.*/ 
        TRIANGLES_COMMAND				/**Triangles command, used to draw triangles.*/
    };

	/**
	*Init function, will be called by all the render commands.
	*
	*param globalZOrder The global order of command, used for render command sorting.
	*param modelViewTransform Model view matrix when submitting the render command.
	*param flags Flag used to indicate whether the command should be draw at 3D mode or not.
	*returns 
	*by [3/27/2017 song.zhang079] 
	*/
    void init(float globalZOrder, const Mat4& modelViewTransform, uint32_t flags);
    
    /** Get global Z order. */
    inline float getGlobalOrder() const { return _globalOrder; }

    /** Returns the Command type. */
    inline Type getType() const { return _type; }
    
    /** Returns whether is transparent. */
    inline bool isTransparent() const { return _isTransparent; }
    
    /** Set transparent flag. */
    inline void setTransparent(bool isTransparent) { _isTransparent = isTransparent; }

	/**
	*Get skip batching status, if a rendering is skip batching, it will be forced to be rendering separately.
	*param   
	*returns 
	*by [3/27/2017 song.zhang079] 
	*/
    inline bool isSkipBatching() const { return _skipBatching; }

    /**Set skip batching.*/
    inline void setSkipBatching(bool value) { _skipBatching = value; }

    /**Whether the command should be rendered at 3D mode.*/
    inline bool is3D() const { return _is3D; }

    /**Set the command rendered in 3D mode or not.*/
    inline void set3D(bool value) { _is3D = value; }

    /**Get the depth by current model view matrix.*/
    inline float getDepth() const { return _depth; }
    
protected:
    RenderCommand();
    virtual ~RenderCommand();

    /**Type used in order to avoid dynamic cast, faster. */
    Type _type;

    /** Commands are sort by global Z order. */
    float _globalOrder;
    
    /** Transparent flag. */
    bool  _isTransparent;
    
    /**
     QuadCommand and TrianglesCommand could be auto batched if there material ID is the same, however, if
     a command is skip batching, it would be forced to draw in a separate function call, and break the batch.
     */
    bool _skipBatching;
    
    /** Is the command been rendered on 3D pass. */
    bool _is3D;
    
    /** Depth from the model view matrix.*/
    float _depth;
};

NS_HMS_END
#endif //__CCRENDERCOMMAND_H_
