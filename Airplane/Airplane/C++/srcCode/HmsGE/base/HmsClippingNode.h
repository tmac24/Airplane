

#ifndef __MISCNODE_CCCLIPPING_NODE_H__
#define __MISCNODE_CCCLIPPING_NODE_H__

#include "base/HmsNode.h"
#include "render/HmsGroupCommand.h"
#include "render/HmsCustomCommand.h"

NS_HMS_BEGIN
/**
 *  @addtogroup _2d
 *  @{
 */
/** ClippingNode is a subclass of Node.
 * It draws its content (children) clipped using a stencil.
 * The stencil is an other Node that will not be drawn.
 * The clipping is done using the alpha part of the stencil (adjusted with an alphaThreshold).
 */
class HMS_DLL CHmsClippingNode : public CHmsNode
{
public:
    /** Creates and initializes a clipping node without a stencil.
     *
     * @return An autorelease ClippingNode.
     */
    static CHmsClippingNode* create();
    
    /** Creates and initializes a clipping node with an other node as its stencil.
     * The stencil node will be retained.
     * @param stencil The stencil node.
     */
	static CHmsClippingNode* create(CHmsNode *stencil);

    /** The Node to use as a stencil to do the clipping.
     * The stencil node will be retained.
     * This default to nil.
     *
     * @return The stencil node.
     */
	CHmsNode* getStencil() const;
    
    /** Set the Node to use as a stencil to do the clipping.
     *
     * @param stencil The Node to use as a stencil to do the clipping.
     */
	void setStencil(CHmsNode *stencil);

    /** If stencil has no children it will not be drawn.
     * If you have custom stencil-based node with stencil drawing mechanics other then children-based,
     * then this method should return true every time you wish stencil to be visited.
     * By default returns true if has any children attached.
     *
     * @return If you have custom stencil-based node with stencil drawing mechanics other then children-based,
     *         then this method should return true every time you wish stencil to be visited.
     *         By default returns true if has any children attached.
     * @js NA
     */
    virtual bool hasContent() const;

    /** The alpha threshold.
     * The content is drawn only where the stencil have pixel with alpha greater than the alphaThreshold.
     * Should be a float between 0 and 1.
     * This default to 1 (so alpha test is disabled).
     *
     * @return The alpha threshold value,Should be a float between 0 and 1.
     */
    GLfloat getAlphaThreshold() const;
    
    /** Set the alpha threshold. 
     * 
     * @param alphaThreshold The alpha threshold.
     */
    void setAlphaThreshold(GLfloat alphaThreshold);
    
    /** Inverted. If this is set to true,
     * the stencil is inverted, so the content is drawn where the stencil is NOT drawn.
     * This default to false.
     *
     * @return If the clippingNode is Inverted, it will be return true.
     */
    bool isInverted() const;
    
    /** Set the ClippingNode whether or not invert.
     *
     * @param inverted A bool Type,to set the ClippingNode whether or not invert.
     */
    void setInverted(bool inverted);

    // Overrides
    /**
     * @lua NA
     */
    virtual void OnEnter() override;
    /**
     * @lua NA
     */
   // virtual void OnEnterTransitionDidFinish() override;
    /**
     * @lua NA
     */
    //virtual void onExitTransitionDidStart() override;
    /**
     * @lua NA
     */
    virtual void OnExit() override;
    virtual void Visit(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags) override;
    
   // virtual void SetCameraMask(unsigned short mask, bool applyChildren = true) override;
    
public:
    CHmsClippingNode();
    
    /**
     * @js NA
     * @lua NA
     */
    virtual ~CHmsClippingNode();

    /** Initializes a clipping node without a stencil.
     */
    virtual bool Init() override;
    
    /** Initializes a clipping node with an other node as its stencil.
     The stencil node will be retained, and its parent will be set to this clipping node.
     */
	virtual bool Init(CHmsNode *stencil);

protected:
    /**draw fullscreen quad to clear stencil bits
    */
    void drawFullScreenQuadClearStencil();

	CHmsNode* _stencil;
    GLfloat _alphaThreshold;
    bool    _inverted;

    //renderData and callback
    void onBeforeVisit();
    void onAfterDrawStencil();
    void onAfterVisit();

    GLboolean _currentStencilEnabled;
    GLuint _currentStencilWriteMask;
    GLenum _currentStencilFunc;
    GLint _currentStencilRef;
    GLuint _currentStencilValueMask;
    GLenum _currentStencilFail;
    GLenum _currentStencilPassDepthFail;
    GLenum _currentStencilPassDepthPass;
    GLboolean _currentDepthWriteMask;

    GLboolean _currentAlphaTestEnabled;
    GLenum _currentAlphaTestFunc;
    GLclampf _currentAlphaTestRef;

    GLint _mask_layer_le;
    
    GroupCommand _groupCommand;
    CustomCommand _beforeVisitCmd;
    CustomCommand _afterDrawStencilCmd;
    CustomCommand _afterVisitCmd;

private:
    HMS_DISALLOW_COPY_AND_ASSIGN(CHmsClippingNode);
};
/** @} */
NS_HMS_END

#endif // __MISCNODE_CCCLIPPING_NODE_H__
