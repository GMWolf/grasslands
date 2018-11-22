//
// Created by felix on 30/09/2018.
//
#include <gtc/type_ptr.hpp>
#include "Renderer.h"
#include <fstream>
#include "Material.h"
#include "CUBELoader.h"


Renderer::Renderer(int width, int height) : width(width), height(height), shadowMap(2048), pingPong(width, height, 1), OGBuffer(width, height, 8) {

    OGBuffer.addTexture(GL_RGBA8, GL_COLOR_ATTACHMENT0);
    OGBuffer.addTexture(GL_DEPTH_COMPONENT24, GL_DEPTH_ATTACHMENT);

    //Temp light stuff
    glCreateBuffers(1, &lightBuffer);
    glCreateBuffers(1, &lightIndexBuffer);

    glNamedBufferStorage(lightBuffer, sizeof(LightData), nullptr, GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT);
    lightData = static_cast<LightData *>(glMapNamedBufferRange(lightBuffer, 0, sizeof(LightData), GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT));
    GLuint tileCount = static_cast<GLuint>(((width + 15) / 16) * ((height + 15) / 16));
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


    skyboxShader = new Shader({
        {GL_VERTEX_SHADER, quadVertText},
        {GL_GEOMETRY_SHADER, quadGText},
        {GL_FRAGMENT_SHADER, "../shaders/SkyboxFrag.glsl"_preprocess}
    });

    partShader = new Shader({
        {GL_VERTEX_SHADER, "../shaders/ParticlesVert.glsl"_preprocess},
        {GL_GEOMETRY_SHADER, "../shaders/ParticlesGeo.glsl"_preprocess},
        {GL_FRAGMENT_SHADER, "../shaders/ParticlesFrag.glsl"_preprocess}
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

    batch.meshBuffer.bindVa();
    glBlendFunc(batch.matType.blendSourceFactor, batch.matType.blendDestFactor);
    if (batch.matType.cullBackfaces) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }

    //render indirect
    Shader* shader;
    if (pass.mask & (PASS_SHADOW | PASS_DEPTH_NON_TRANSMISIVE | PASS_DEPTH_TRANSMISIVE)) {
        shader = batch.matType.depthShaderOverride ? batch.matType.depthShaderOverride : defaultDepthShader;
    } else {
        shader = batch.matType.shader;
    }

    shader->use();

    static const std::vector<int> tsamplers {
            0, 1, 2, 3, 4, 5, 6, 7
    };

    glBindTextureUnit(9, shadowMap.btex);
    shader->setUniform("projection", pass.projection);
    shader->setUniform("MV", pass.projection * pass.view);
    shader->setUniform("tex", tsamplers);
    shader->setUniform("cubemaps", tsamplers);
    if (radiance) {
        shader->setUniform("radianceTex", (glm::ivec2)*radiance);
    } else {
        shader->setUniform("radianceTex", glm::ivec2(-1,-1));
    }

    shader->setUniform("eyePos", eyePos);
    shader->setUniform("lightColour", sunCol);
    shader->setUniform("lightDir", sunDir);

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

    if (batch.matType.alphaToCoverage) {
        glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    }

    glMultiDrawElementsIndirect(prim, GL_UNSIGNED_SHORT, 0, batch.batchSize, 0);

    if (batch.matType.alphaToCoverage) {
        glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    }
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
    } else {
        //std::cout << "culled " << batch.batchSize << " objects" << std::endl;
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


void Renderer::renderParicleSystem(ParticleSystem &particleSystem, PassInfo &pass) {

    glBlendFunc(particleSystem.blendSourceFactor, particleSystem.blendDestFactor);
    glEnable(GL_BLEND);
    particleSystem.bindVA();

    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);

    partShader->use();
    partShader->setUniform("VP", pass.projection * pass.view);
    partShader->setUniform("pixelSize", glm::vec2(1.f/width, 1.f/height));
    static const std::vector<int> tsamplers {0, 1, 2, 3, 4, 5, 6, 7};
    partShader->setUniform("tex", tsamplers);
    partShader->setUniform("t", (glm::ivec2)particleSystem.texture);

    glDrawArrays(GL_POINTS, 0, particleSystem.partCount);

    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
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


    depthPrepass();
    glTextureBarrier();

    cullLights();

    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    shadowPass();

    scenePass();

    volumetricPass();

    colorGradePass();

    pingpongToScreen();

    if (showLightDebug) {
        renderLightDebug();
    }
}

void Renderer::depthPrepass() {//Do first depth pass

    glEnable(GL_DEPTH_TEST);
    PassInfo depthPassInfo;
    depthPassInfo.view = view;
    depthPassInfo.projection = proj;
    depthPassInfo.mask = PASS_DEPTH_NON_TRANSMISIVE;

    // Draw opaque to pingpong A
    pingPong.swap();
    pingPong.getFBO().setDrawTarget();
    glClearDepth(1);
    glClear(GL_DEPTH_BUFFER_BIT);
    glColorMask(false, false, false, false);

    renderBatches(depthPassInfo); //NON TRANSMISSIVE

    // Draw transmissive to pingpong B
    pingPong.getBackFBO().setDrawTarget();
    glClearDepth(1);
    glClear(GL_DEPTH_BUFFER_BIT);
    PassInfo passInfo;
    passInfo.view = view;
    passInfo.projection = proj;
    passInfo.mask = PASS_DEPTH_TRANSMISIVE;

    renderBatches(passInfo);

    glColorMask(true, true, true, true);
}

void Renderer::cullLights() {//DO light culling


    lightCullShader->use();
    glBindTextureUnit(0, pingPong.getDepth());
    lightCullShader->setUniform("depthMapA", 0);
    glBindTextureUnit(1, pingPong.getBackDepth());
    lightCullShader->setUniform("depthMapB", 1);
    lightCullShader->setUniform("projection", proj);
    lightCullShader->setUniform("viewProj", proj * view);
    lightCullShader->setUniform("view", view);
    lightCullShader->setUniform("size", glm::vec2(width, height));
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, lightIndexBuffer);
    glDispatchCompute((width + 15) / 16, (height + 15) / 16, 1);

}

void Renderer::scenePass() {//SCENE PASS
    OGBuffer.setTarget();
    glClearDepth(1);
    glClearColor(0.7, 0.7, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);
    if (skybox) {
        skyboxShader->use();
        skyboxShader->setUniform("skybox", (glm::ivec2)*skybox);
        skyboxShader->setUniform("invMat", glm::inverse(proj * view));
        skyboxShader->setUniform("eyePos", eyePos);
        static const std::vector<int> tsamplers {
                0, 1, 2, 3, 4, 5, 6, 7
        };
        skyboxShader->setUniform("cubemaps", tsamplers);
        renderQuad();

    }

    PassInfo scenePass;
    scenePass.view = view;
    scenePass.projection = proj;
    scenePass.mask = PASS_DEFAULT;
    renderBatches(scenePass);
    for(ParticleSystem& part : particleSystems) {
        part.update(0.01);
        renderParicleSystem(part, scenePass);
    }


    //blit to pingpong
    OGBuffer.setReadTarget();
    pingPong.getFBO().setDrawTarget();
    glBlitFramebuffer(0,0, OGBuffer.width, OGBuffer.height, 0, 0, pingPong.getFBO().width, pingPong.getFBO().height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

void Renderer::renderLightDebug() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    lightDebugShader->use();
    lightDebugShader->setUniform("tileCountX", (GLuint)((width + 15) / 16));
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightIndexBuffer);
    renderQuad();
}

void Renderer::pingpongToScreen() {//Push to screen
    FrameBuffer::windowBuffer.setTarget();
    glClearColor(0, 0, 0, 1.0);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    passShader->use();
    glBindTextureUnit(0, pingPong.getTexture());
    passShader->setUniform(passShader->getUniformLocation("tex"), 0);
    passShader->setUniform(passShader->getUniformLocation("size"), glm::vec2(width, height));

    renderQuad();
}

void Renderer::volumetricPass() {//Volumetric light pass
    pingPong.swap();
    pingPong.getFBO().setTarget();
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
    volumetricShader->setUniform("size", glm::vec2(width, height));
    volumetricShader->setUniform("tileCountX", (GLuint)(width + 15) / 16);
    volumetricShader->setUniform("viewProj", proj * view);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, lightIndexBuffer);
    renderQuad();
}

void Renderer::colorGradePass() {//Color correct pass
    pingPong.swap();
    pingPong.getFBO().setTarget();
    gradeShader->use();
    gradeShader->setUniform("size", glm::vec2(width, height));
    glBindTextureUnit(0, pingPong.getBackTexture());
    gradeShader->setUniform("tex", 0);
    glBindTextureUnit(1, LUT);
    gradeShader->setUniform("LUT", 1);
    gradeShader->setUniform("lutSize", 32.0f);
    renderQuad();
}


void Renderer::shadowPass() {
    //Do shadow pass
    PassInfo passInfo;
    passInfo.mask = PASS_SHADOW;
    passInfo.projection = shadowMap.projection;
    passInfo.view = shadowMap.view;


    shadowMap.fbo.setTarget();
    glClearColor(1,1,1,1);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0, 2048, 2048);
    renderBatches(passInfo);

    //blur
    shadowMap.blurFbo.setTarget();
    boxShader->use();
    glBindTextureUnit(0, shadowMap.tex);
    glViewport(0,0, 2048, 2048);
    boxShader->setUniform("tex", 0);
    boxShader->setUniform("sampleSize", glm::vec2(1.f / 2048, 1.f / 2048));
    renderQuad();

    glGenerateTextureMipmap(shadowMap.btex);
}

void Renderer::setCamera(const Camera &cam) {
    this->cam = &cam;
    setProjection(cam.proj);
    setView(cam.view);
    setEyePos(cam.pos);
    shadowMap.computeProjections(cam, glm::normalize(-sunDir));
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

void Renderer::updateLights() {


    glm::vec4 frustumPlanes[6];
    frustumPlanes[0] = glm::vec4(1.0, 0.0, 0.0, 1.0);
    frustumPlanes[1] = glm::vec4(-1.0, 0.0, 0.0, -1.0 );
    frustumPlanes[2] = glm::vec4(0.0, 1.0, 0.0, 1.0 );
    frustumPlanes[3] = glm::vec4(0.0, -1.0, 0.0, -1.0);
    frustumPlanes[4] = glm::vec4(0.0, 0.0, -1.0, -cam->nearPlane);
    frustumPlanes[5] = glm::vec4(0.0, 0.0, 1.0, cam->farPlane);
/*
    for(int i = 0; i < 4; i++) {
        frustumPlanes[i] *= view * proj;
        frustumPlanes[i] /= glm::vec3(frustumPlanes[i]).length();
    }

    frustumPlanes[4] *= view;
    frustumPlanes[4] /= glm::vec3(frustumPlanes[4]).length();
    frustumPlanes[5] *= view;
    frustumPlanes[5] /= glm::vec3(frustumPlanes[5]).length();

    lightData->clear();

    for(Light& l : lights) {
        bool inside = true;
        for(int j = 0; j < 6; j++) {
            float dist = glm::dot(glm::vec4(l.pos, 1.0), frustumPlanes[j]) + l.radius;
            inside = inside && (dist > 0.0f);
        }

        if (inside) {
            lightData->addLight(l);
        }

    }
*/
}

