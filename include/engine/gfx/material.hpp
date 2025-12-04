#pragma once

#include "engine/core/core.hpp"
#include "engine/gfx/shader.hpp"
#include "engine/gfx/texture.hpp"

namespace gfx {
    struct MaterialProperty {
        enum class Type {
            FLOAT,
            VEC3,
            VEC4,
            MAT4,
            TEXTURE
        };

        Type type;
        std::string name;

        union {
            float floatValue;
            glm::vec3 vec3Value;
            glm::vec4 vec4Value;
            glm::mat4 mat4Value;
            std::shared_ptr<Texture> textureValue;
        };

        MaterialProperty(const std::string& name, float value)
            : type(Type::FLOAT), name(name), floatValue(value) {}
        MaterialProperty(const std::string& name, const glm::vec3& value)
            : type(Type::VEC3), name(name), vec3Value(value) {}
        MaterialProperty(const std::string& name, const glm::vec4& value)
            : type(Type::VEC4), name(name), vec4Value(value) {}
        MaterialProperty(const std::string& name, const glm::mat4& value)
            : type(Type::MAT4), name(name), mat4Value(value) {}
        MaterialProperty(const std::string& name, std::shared_ptr<Texture> value)
            : type(Type::TEXTURE), name(name), textureValue(value) {}

        ~MaterialProperty() {
            if (type == Type::TEXTURE) {
                textureValue.~shared_ptr<Texture>();
            }
        }

        MaterialProperty& operator=(const MaterialProperty& other) {
            if (this != &other) {
                type = other.type;
                name = other.name;

                switch (type) {
                    case Type::FLOAT:
                        floatValue = other.floatValue;
                        break;
                    case Type::VEC3:
                        vec3Value = other.vec3Value;
                        break;
                    case Type::VEC4:
                        vec4Value = other.vec4Value;
                        break;
                    case Type::MAT4:
                        mat4Value = other.mat4Value;
                        break;
                    case Type::TEXTURE:
                        textureValue = other.textureValue;
                        break;
                }
            }

            return *this;
        }
    };

    class Material {
    public:
        Material() : _shader(nullptr) {}
        Material(std::shared_ptr<Shader> shader) : _shader(shader) {}

        void setShader(std::shared_ptr<Shader> shader) {
            _shader = shader;
            _properties.clear();
        }

        std::shared_ptr<Shader> getShader() const {
            return _shader;
        }

        template<typename T>
        void setProperty(const std::string& name, T value) {
            for (auto& prop : _properties) {
                if (prop.name == name) {
                    prop = MaterialProperty(name, value);
                    return;
                }
            }

            _properties.emplace_back(name, value);
        }

        const std::vector<MaterialProperty>& getProperties() const {
            return _properties;
        }

    private:
        std::shared_ptr<Shader> _shader;
        std::vector<MaterialProperty> _properties;
    };
};