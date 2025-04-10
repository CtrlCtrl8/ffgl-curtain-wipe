#include <FFGL.h>
#include <FFGLShader.h>

class CurtainWipe : public CFreeFrameGLPlugin {
public:
    CurtainWipe() : CFreeFrameGLPlugin() {
        // Параметры плагина
        SetParamInfo(FFPARAM_POSITION, "Position", FF_TYPE_STANDARD, 0.5f);
        SetParamInfo(FFPARAM_WIDTH, "Width", FF_TYPE_STANDARD, 0.3f);
        SetParamInfo(FFPARAM_SMOOTHNESS, "Smoothness", FF_TYPE_STANDARD, 0.05f);
    }

    FFResult ProcessOpenGL(ProcessOpenGLStruct* pGL) override {
        // Получаем значения параметров
        float position = GetFloatParameter(FFPARAM_POSITION);
        float width = GetFloatParameter(FFPARAM_WIDTH);
        float smoothness = GetFloatParameter(FFPARAM_SMOOTHNESS);

        // Вершинный и фрагментный шейдеры
        const char* vertexShader = R"(
            #version 330 core
            layout(location = 0) in vec3 aPos;
            layout(location = 1) in vec2 aTexCoord;
            out vec2 TexCoord;
            void main() {
                gl_Position = vec4(aPos, 1.0);
                TexCoord = aTexCoord;
            }
        )";

        const char* fragmentShader = R"(
            #version 330 core
            uniform sampler2D inputTexture;
            uniform float position;
            uniform float width;
            uniform float smoothness;
            in vec2 TexCoord;
            out vec4 FragColor;

            void main() {
                vec4 color = texture(inputTexture, TexCoord);
                float leftEdge = position - width / 2.0;
                float rightEdge = position + width / 2.0;
                float fade = smoothstep(leftEdge - smoothness, leftEdge + smoothness, TexCoord.x) - 
                            smoothstep(rightEdge - smoothness, rightEdge + smoothness, TexCoord.x);
                FragColor = mix(vec4(0.0), color, fade);
            }
        )";

        // Компиляция шейдера
        FFGLShader shader;
        shader.Compile(vertexShader, fragmentShader);
        shader.Bind();

        // Передача параметров в шейдер
        shader.Set("position", position);
        shader.Set("width", width);
        shader.Set("smoothness", smoothness);

        // Рендеринг
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex2f(-1.0, -1.0);
        glTexCoord2f(1.0, 0.0); glVertex2f(1.0, -1.0);
        glTexCoord2f(1.0, 1.0); glVertex2f(1.0, 1.0);
        glTexCoord2f(0.0, 1.0); glVertex2f(-1.0, 1.0);
        glEnd();

        shader.Unbind();
        return FF_SUCCESS;
    }
};

// Экспорт плагина
EXPORT FFGLPlugin* CreatePluginInstance() {
    return new CurtainWipe();
}
