//
// Created by felix on 30/09/2018.
//
#include <gtc/type_ptr.hpp>
#include "Renderer.h"
#include <fstream>
#include "Material.h"
#include "CUBELoader.h"


Renderer::Renderer(int width, int height) : width(width), height(height), shadowMap(2048), pingPong(width, height) {

    //Temp light stuff
    glCreateBuffers(1, &lightBuffer);
    glCreateBuffers(1, &lightIndexBuffer);
    //glCreateBuffers(1, &lightStartEndBuffer);

    struct {
        GLuint n = 150, pad0, pad1, pad2;
        Light light[150];
    } lightdata;
    for(int i = 0; i < lightdata.n; i++) {
        lightdata.light[i].radius = 5;
        lightdata.light[i].intensity = 10;
        lightdata.light[i].color = glm::vec3(1, 1, 1);
        lightdata.light[i].pos = (glm::vec3(rand() / (float)RAND_MAX - 0.5f, 0, rand() / (float)RAND_MAX - 0.5f) ) * 200.f;
        lightdata.light[i].pos += glm::vec3(0, 0.5, 0);
    }

    glNamedBufferStorage(lightBuffer, sizeof(lightdata), &lightdata, 0);
    GLuint tileCount = (((width + 15) / 16) * ((height + 15) / 16));
    std::cout << "tile count" << tileCount << std::endl;
    glNamedBufferStorage(lightIndexBuffer, tileCount * ((16 * sizeof(GLuint)) + sizeof(GLuint)), nullptr, 0);
    //glNamedBufferStorage(lightStartEndBuffer, tileCount  * 2 * sizeof(GLuint), nullptr, 0);

    lightCullShader = new Shader({
            {GL_COMPUTE_SHADER, "../shaders/lightCulling.glsl"_preprocess}
    });


    dispatchCompute = new Shader({
        {GL_COMPUTE_SHADER, "../shaders/dispatchGeom.glsl"_preprocess}
    });


    defaultDepthShader = new Shader({
        {GL_VERTEX_SHADER, "../shaders/depthVert.glsl"_preprocess},
        {GL_FRAGMENT_SHADER, "../shaders/depthFragment.glsl"_preprocess}
    });

    std::ifstream quadGFile("../shaders/quadGeom.glsl");
    std::string quadGText((std::istreambuf_iterator<char>(quadGFile)), (std::istreambuf_iterator<char>()));

    std::ifstream quadFragFile("../shaders/quadFrag.glsl");
    std::string quadFragText((std::istreambuf_iterator<char>(quadFragFile)), (std::istreambuf_iterator<char>()));

    std::string quadVertText = "#version 120\nvoid main() {\n gl_Position = vec4(0,0,0,0);}";

    quadShader = new Shader({
        {GL_VERTEX_SHADER, quadVertText},
        {GL_GEOMETRY_SHADER, quadGText},
        {GL_FRAGMENT_SHADER, quadFragText}
    });


    boxShader = new Shader({
        {GL_VERTEX_SHADER, quadVertText},
        {GL_GEOMETRY_SHADER, quadGText},
        {GL_FRAGMENT_SHADER, "../shaders/boxFilterFrag.glsl"_preprocess}
    });


    volumetricShader = new Shader({
        {GL_VERTEX_SHADER, quadVertText},
        {GL_GEOMETRY_SHADER, quadGText},
        {GL_FRAGMENT_SHADER, "../shaders/volumetricFog.glsl"_preprocess}
    });

    passShader = new Shader({
        {GL_VERTEX_SHADER, quadVertText},
        {GL_GEOMETRY_SHADER, quadGText},
        {GL_FRAGMENT_SHADER, "../shaders/passthrough.glsl"_preprocess}
    });




    //LUT = loadCubeLUT("../LUTs/Neon 770.CUBE");
    //LUT = loadCubeLUT("../LUTs/Django 25.CUBE");
    LUT = loadCubeLUT("../LUTs/Bourbon 64.CUBE");
    //LUT = loadCubeLUT("../LUTs/Faded 47.CUBE");
    gradeShader = new Shader ({
            {GL_VERTEX_SHADER, quadVertText},
            {GL_GEOMETRY_SHADER, quadGText},
            {GL_FRAGMENT_SHADER, "../shaders/grade.glsl"_preprocess}
    });


    lightDebugShader = new Shader({
        {GL_VERTEX_SHADER, quadVertText},
        {GL_GEOMETRY_SHADER, quadGText},
        {GL_FRAGMENT_SHADER, "../shaders/lightDebug.glsl"_preprocess}
    });

}

void Renderer::setView(const glm::mat4 &v) {
    view = v;
}

void Renderer::setProjection(const glm::mat4 &p) {
    proj = p;
}

void Renderer::setEyePos(const glm::vec3 &pos) {
    eyePos = pos;
}

void Renderer::renderBatch(Batch &batch, PassInfo& pass) {

    if(!(pass.mask & batch.matType.mask)) {
        return;
    }

    dispatchCompute->use();

    dispatchCompute->setUniform(0, pass.projection * pass.view);

    //GPU Culling
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, batch.meshBuffer.meshDataBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, batch.computeCullCommandsBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, batch.indirectBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, batch.transformBuffer);

    glDispatchCompute(batch.batchSize, 1, 1);

    glMemoryBarrier(GL_COMMAND_BARRIER_BIT);

    //render indirect
    Shader* shader;
    if (pass.mask & (PASS_SHADOW | PASS_DEPTH_NON_TRANSMISIVE | PASS_DEPTH_TRANSMISIVE)) {
        shader = batch.matType.depthShaderOverride ? batch.matType.depthShaderOverride : defaultDepthShader;
    } else {
        shader = batch.matType.shader;
    }

    shader->use();

    static const std::vector<int> tsamplers {
            0, 1//, 2, 3, 4, 5, 6, 7
    };

    glBindTextureUnit(9, shadowMap.btex);
    shader->setUniform("projection", pass.projection);
    shader->setUniform("MV", pass.projection * pass.view);
    shader->setUniform("tex", tsamplers);

    shader->setUniform("eyePos", eyePos);
    shader->setUniform("lightColour", glm::vec3(4, 4, 3.25));
    shader->setUniform("lightDir", glm::vec3(1, 1, 0));

    shader->setUniform("shadowMap", 9);
    shader->setUniform("shadowVP",shadowMap.projection * shadowMap.view);
    shader->setUniform("time", time);

    shader->setUniform("tileCountX", (GLuint)((width + 15) / 16));

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, batch.indirectBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, batch.transformBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, batch.materialIndexBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, batch.matType.buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, lightBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, lightIndexBuffer);
    GLenum prim = batch.matType.primType;

    glMultiDrawElementsIndirect(prim, GL_UNSIGNED_SHORT, 0, batch.batchSize, 0);
}

void Renderer::renderBatch(StaticBatch &batch, PassInfo& pass) {

    glm::mat4 viewproj = pass.projection * pass.view;

    glm::vec4 corners[8];
    corners[0] = viewproj * glm::vec4(batch.min[0], batch.max[1], batch.min[2], 1.f);
    corners[1] = viewproj * glm::vec4(batch.min[0], batch.max[1], batch.max[2], 1.f);
    corners[2] = viewproj * glm::vec4(batch.max[0], batch.max[1], batch.max[2], 1.f);
    corners[3] = viewproj * glm::vec4(batch.max[0], batch.max[1], batch.min[2], 1.f);
    corners[4] = viewproj * glm::vec4(batch.max[0], batch.min[1], batch.min[2], 1.f);
    corners[5] = viewproj * glm::vec4(batch.max[0], batch.min[1], batch.max[2], 1.f);
    corners[6] = viewproj * glm::vec4(batch.min[0], batch.min[1], batch.max[2], 1.f);
    corners[7] = viewproj * glm::vec4(batch.min[0], batch.min[1], batch.min[2], 1.f);

    glm::bvec3 allGt(true);
    glm::bvec3 allLt(true);
    for(int i = 0; i < 8; i++) {
        glm::bvec3 gt = glm::greaterThan(glm::vec3(corners[i]), glm::vec3(corners[i].w));
        glm::bvec3 lt = glm::lessThan(glm::vec3(corners[i]), -glm::vec3(corners[i].w));

        allGt &= gt;
        allLt &= lt;
    }

    bool inside = !(any(allGt) || any(allLt));


    if(inside) {
        renderBatch(static_cast<Batch&>(batch), pass);
    }

}

void Renderer::renderBatch(DynamicBatch &batch, PassInfo& pass) {

    //wait on fence
    if(batch.fence[batch.bufferIndex]) {
        GLenum timeoutflag;
        int n = 0;
        do {
            timeoutflag = glClientWaitSync(batch.fence[batch.bufferIndex], GL_SYNC_FLUSH_COMMANDS_BIT, 10);
            n++;
        } while (timeoutflag == GL_TIMEOUT_EXPIRED);
        batch.fence[batch.bufferIndex] = nullptr;
    }

    //update the transforms
    for(int i = 0; i < batch.batchSize; i++) {
        batch.transforms[batch.bufferIndex][i] = batch.objects[i]->transform;
    }
    batch.transformBuffer = batch.transformBuffers[batch.bufferIndex];

    //submit
    renderBatch(static_cast<Batch&>(batch), pass);

    //fence
    batch.fence[batch.bufferIndex] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

    //change buffer id
    batch.bufferIndex = (batch.bufferIndex+1) % DynamicBatch::buffCount;
}

void Renderer::addObjects(std::vector<RenderObject *> &renderObjects) {

    std::map<baseMaterialType*, std::unique_ptr<Octree>> staticObjects;
    std::map<baseMaterialType*, std::unique_ptr<std::vector<RenderObject*>>> dynamicObjects;

    for(auto o : renderObjects) {
        baseMaterialType* matType = o->mat.type;

        if(o->isStatic) {
            if (staticObjects.find(matType) == staticObjects.end()) {
                staticObjects[matType] = std::make_unique<Octree>(100);
            }
            staticObjects[matType]->root.insert(o);
        } else {
            if (dynamicObjects.find(matType) == dynamicObjects.end()) {
                dynamicObjects[matType] = std::make_unique<std::vector<RenderObject*>>();
            }
            auto& vec = *dynamicObjects[matType];
            vec.emplace_back(o);

            if(vec.size() >= MAX_BATCH_SIZE) {
               dynamicBatches.emplace_back(vec);
               vec.clear();
            }
        }
    }


    for(auto& e : staticObjects) {
        addOctreeNodes(e.second->root);
    }

    for(auto& e : dynamicObjects) {
        dynamicBatches.emplace_back(*e.second);
        e.second->clear();
    }
}

void Renderer::addOctreeNodes(OctreeNode & node) {

    if (node.nodes) {
        for(int i = 0; i < 8; i++) {
            addOctreeNodes(node.nodes[i]);
        }
    } else {
        if(node.renderObjects.size() > 0) {
            staticBatches.emplace_back(node.renderObjects);
        }
    }

}

void Renderer::render(float time) {

    this->time = time;


    //Do first depth pass
    PassInfo depthPassInfo;
    depthPassInfo.view = view;
    depthPassInfo.projection = proj;
    depthPassInfo.mask = PASS_DEPTH_NON_TRANSMISIVE;

    pingPong.swap();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pingPong.getFBO());
    glClearDepth(1);
    glClear(GL_DEPTH_BUFFER_BIT);
    glColorMask(false, false, false, false);

    renderBatches(depthPassInfo); //NON TRANSMISIVE
    glTextureBarrier();


    //DO light culling
    lightCullShader->use();
    glBindTextureUnit(0, pingPong.getDepth());
    lightCullShader->setUniform("depthMap", 0);
    lightCullShader->setUniform("projection", proj);
    lightCullShader->setUniform("viewProj", proj * view);
    lightCullShader->setUniform("view", view);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, lightIndexBuffer);
    glDispatchCompute((width + 15) / 16, (height + 15) / 16, 1);



    depthPassInfo.mask = PASS_DEPTH_TRANSMISIVE;
    renderBatches(depthPassInfo); //TRANSMISIVE
    glColorMask(true, true, true, true);


    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    //SHADOWS
    shadowPass();

    //SCENE PASS
    pingPong.swap();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pingPong.getFBO());
    glClearDepth(1);
    glClearColor(0.7, 0.7, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0, width, height);
    PassInfo scenePass;
    scenePass.view = view;
    scenePass.projection = proj;
    scenePass.mask = PASS_DEFAULT;
    renderBatches(scenePass);


    //Volumetric light pass
    pingPong.swap();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pingPong.getFBO());
    volumetricShader->use();
    volumetricShader->setUniform("invMat", glm::inverse(proj * view));
    volumetricShader->setUniform("eyePos", eyePos);
    volumetricShader->setUniform("shadowVP", shadowMap.projection * shadowMap.view);
    glBindTextureUnit(0, shadowMap.btex);
    volumetricShader->setUniform("shadowMap", 0);
    glBindTextureUnit(1, pingPong.getBackTexture());
    volumetricShader->setUniform("inColour", 1);
    glBindTextureUnit(2, pingPong.getBackDepth());
    volumetricShader->setUniform("depthMap", 2);
    volumetricShader->setUniform("size", glm::vec2(width,height));
    renderQuad();

    //Color correct pass
    pingPong.swap();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pingPong.getFBO());
    gradeShader->use();
    gradeShader->setUniform("size", glm::vec2(this->width, this->height));
    glBindTextureUnit(0, pingPong.getBackTexture());
    gradeShader->setUniform("tex", 0);
    glBindTextureUnit(1, LUT);
    gradeShader->setUniform("LUT", 1);
    gradeShader->setUniform("lutSize", 32.0f);
    renderQuad();

    //Push to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0,0,0,1.0);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    passShader->use();
    glBindTextureUnit(0, pingPong.getTexture());
    passShader->setUniform(passShader->getUniformLocation("tex"), 0);
    passShader->setUniform(passShader->getUniformLocation("size"), glm::vec2(width, height));

    renderQuad();


    if (showLightDebug) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        lightDebugShader->use();
        lightDebugShader->setUniform("tileCountX", (GLuint)((width + 15) / 16));
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightIndexBuffer);
        renderQuad();
    }
}


void Renderer::shadowPass() {
    //Do shadow pass
    PassInfo passInfo;
    passInfo.mask = PASS_SHADOW;
    passInfo.projection = shadowMap.projection;
    passInfo.view = shadowMap.view;


    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, shadowMap.fbo);
    glClearColor(1,1,1,1);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0, 2048, 2048);
    renderBatches(passInfo);

    //blur
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, shadowMap.bfbo);
    boxShader->use();
    glBindTextureUnit(0, shadowMap.tex);
    glViewport(0,0, 2048, 2048);
    boxShader->setUniform("tex", 0);
    boxShader->setUniform("sampleSize", glm::vec2(1.f / 2048, 1.f / 2048));
    renderQuad();

    glGenerateTextureMipmap(shadowMap.btex);


}



/*
void Renderer::renderPass(Pass *pass) {

    if (pass->setup) {
        pass->setup();
    }

    if (auto * p = dynamic_cast<ComputePass*>(pass)) {
        p->shader->use();
        glDispatchCompute(p->x, p->y, p->z);
        if (p->barrier) {
            glMemoryBarrier(p->barrier);
        }
        return;
    }

    if (pass->fbo) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pass->fbo);
    } else {
        pingPong.swap();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pingPong.getFBO());
    }

    glViewport(pass->viewportX, pass->viewportY, pass->viewportW, pass->viewportH);

    if (pass->clearBuffers) {
        glClearColor(pass->clearColour.x, pass->clearColour.y, pass->clearColour.z, pass->clearColour.w);
        glClearDepth(1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    if (auto * sp = dynamic_cast<ScenePass*>(pass)){
        renderPass(sp);
    } else if(auto * pp = dynamic_cast<PostPass*>(pass)) {
        renderPass(pp);
    }



}
 */
/*
void Renderer::renderPass(ScenePass *pass) {

    for(auto &batch : dynamicBatches) {
        renderBatch(batch, pass);
    }
    for(auto &batch : staticBatches) {
        renderBatch(batch, pass);
    }

}

void Renderer::renderPass(PostPass *postPass) {

    postPass->shader->use();

    postPass->shader->setUniform(postPass->shader->getUniformLocation("sampleSize"), glm::vec2(1.f / 2048));

    if (postPass->tex) {
        glBindTextureUnit(0, postPass->tex);
        postPass->shader->setUniform(postPass->shader->getUniformLocation("tex"), 0);
    }

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDrawArrays(GL_POINTS, 0, 1);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    if (postPass->generateMipMaps) {
        glGenerateTextureMipmap(postPass->dtex);
    }

}
*/
void Renderer::setCamera(const Camera &cam) {
    setProjection(cam.proj);
    setView(cam.view);
    setEyePos(cam.pos);
    shadowMap.computeProjections(cam, glm::normalize(glm::vec3(-1, -1, 0)));
}

void Renderer::renderBatches(PassInfo &pass) {
    for(auto& batch : dynamicBatches) {
        renderBatch(batch, pass);
    }
    for(auto& batch : staticBatches) {
        renderBatch(batch, pass);
    }
}

void Renderer::renderQuad() {
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDrawArrays(GL_POINTS, 0, 1);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}


