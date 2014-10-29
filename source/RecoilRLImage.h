#pragma once#include <GL/glew.h>#include <SOIL/SOIL.h>namespace Recoil {    namespace RenderLow {        class Image {        public:            Image(const char* texture_filename, bool repeat_h, bool repeat_v, float max_anisotropy = 4.0f);            ~Image(void);            void Bind(void);            static void Unbind(void);        private:            unsigned int texture_id;        };    }}