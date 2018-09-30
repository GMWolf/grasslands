//
// Created by felix on 30/09/2018.
//
#include "Renderer.h"

static const char* vertex_shader_text =
        "#version 430 core\n"
        "\n"
        "layout(location = 0) in vec3 position;\n"
        "layout(location = 1) in vec3 normal;\n"
        "layout(location = 2) in vec2 texcoord;\n"
        //"layout(std430, binding = 2) meshBuffer MVP\n"
        //"{\n"
        //    "mat4 data[];\n"
        //"};\n"
        "uniform mat4 MVP;\n"
        "\n"
        "\n"
        "out Vertex {\n"
        "    vec3 normal;\n"
        "    vec2 texcoord;\n"
        "} OUT;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = MVP * vec4(position, 1.0);\n"
        "    OUT.normal = normal;\n"
        "    OUT.texcoord = texcoord;\n"
        "}";
static const char* fragment_shader_text =
        "#version 400 core\n"
        "\n"
        "uniform sampler2D tex;\n"
        "in Vertex {\n"
        "    vec3 normal;\n"
        "    vec2 texcoord;\n"
        "} IN;\n"
        "\n"
        "out vec4 outColor;\n"
        "void main()\n"
        "{\n"
        "    outColor = texture(tex, IN.texcoord);\n"
        "}";



Renderer::Renderer() {
    shader = new Shader(vertex_shader_text, fragment_shader_text);

}

ModelBatch &Renderer::getBatch(const MeshBuffer *buffer) {
        for(ModelBatch& b : batches) {
                if (b.meshBuffer == buffer) {
                        return b;
                }
        }

        return batches.emplace_back(buffer);
}

void Renderer::clearBatches() {
        for(ModelBatch& b : batches) {
                b.commandCount = 0;
        }
}

void Renderer::submit(Mesh &mesh) {
        ModelBatch& batch = getBatch(mesh.buffer);
        DrawElementsIndirectCommand& command = batch.commands[batch.commandCount++];

        command.count = mesh.elementCount;
        command.instanceCount = 1;
        command.firstIndex = mesh.first;
        command.baseVertex = mesh.baseVertex;
        command.baseInstance = 0;
}

void Renderer::renderBatches() {

        shader->use();
        shader->setUniform(shader->getUniformLocation("tex"), 0);

        for(const ModelBatch& b : batches) {
                b.meshBuffer->bindVa();
                glBindBuffer(GL_DRAW_INDIRECT_BUFFER, b.indirectBuffer);

                glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, 0, b.commandCount, 0);
        }

        clearBatches();
}

void Renderer::setProjection(const glm::mat4 &proj) {
        shader->setUniform(shader->getUniformLocation("MVP"), proj);
}

ModelBatch::ModelBatch(const MeshBuffer *buffer) : meshBuffer(buffer) {
        commandCount = 0;
        maxCommandCount = 1024;
        glCreateBuffers(1, &indirectBuffer);

        GLbitfield flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT ;
        glNamedBufferStorage(indirectBuffer, maxCommandCount * sizeof(DrawElementsIndirectCommand), nullptr, flags);

        flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        commands = static_cast<DrawElementsIndirectCommand *>(glMapNamedBufferRange(indirectBuffer, 0, maxCommandCount * sizeof(DrawElementsIndirectCommand), flags));

}
