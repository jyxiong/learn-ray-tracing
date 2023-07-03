#pragma once

#include <utility>
#include "asteroid/core/macro.h"
namespace Asteroid
{

    enum class ShaderDataType
    {
        None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
    };

    static unsigned int ShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:
                return 4;
            case ShaderDataType::Float2:
                return 4 * 2;
            case ShaderDataType::Float3:
                return 4 * 3;
            case ShaderDataType::Float4:
                return 4 * 4;
            case ShaderDataType::Mat3:
                return 4 * 3 * 3;
            case ShaderDataType::Mat4:
                return 4 * 4 * 4;
            case ShaderDataType::Int:
                return 4;
            case ShaderDataType::Int2:
                return 4 * 2;
            case ShaderDataType::Int3:
                return 4 * 3;
            case ShaderDataType::Int4:
                return 4 * 4;
            case ShaderDataType::Bool:
                return 1;
            default: AST_CORE_ASSERT(false, "Unknown ShaderDataType!")
                return 0;
        }
    }

    struct BufferElement
    {
        std::string Name;
        ShaderDataType Type{};
        unsigned int Size{};
        size_t Offset{};
        bool Normalized{};

        BufferElement() = default;

        BufferElement(ShaderDataType type, std::string name, bool normalized = false)
            : Name(std::move(name)), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
        {
        }

        unsigned int GetComponentCount() const
        {
            switch (Type)
            {
                case ShaderDataType::Float:
                    return 1;
                case ShaderDataType::Float2:
                    return 2;
                case ShaderDataType::Float3:
                    return 3;
                case ShaderDataType::Float4:
                    return 4;
                case ShaderDataType::Mat3:
                    return 3 * 3;
                case ShaderDataType::Mat4:
                    return 4 * 4;
                case ShaderDataType::Int:
                    return 1;
                case ShaderDataType::Int2:
                    return 2;
                case ShaderDataType::Int3:
                    return 3;
                case ShaderDataType::Int4:
                    return 4;
                case ShaderDataType::Bool:
                    return 1;
                default: AST_CORE_ASSERT(false, "Unknown ShaderDataType!");
                    return 0;
            }
        }
    };

    class BufferLayout
    {
    public:
        BufferLayout() = default;

        BufferLayout(const std::initializer_list<BufferElement> &elements)
            : m_Elements(elements)
        {
            CalculateOffsetsAndStride();
        }

        inline unsigned int GetStride() const { return m_Stride; }

        inline const std::vector<BufferElement> &GetElements() const { return m_Elements; }

        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }

        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }

        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }

        std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

    private:
        void CalculateOffsetsAndStride()
        {
            unsigned int offset = 0;
            m_Stride = 0;
            for (auto &element: m_Elements)
            {
                element.Offset = offset;
                offset += element.Size;
                m_Stride += element.Size;
            }
        }

    private:
        std::vector<BufferElement> m_Elements;
        unsigned int m_Stride = 0;
    };

    class VertexBuffer
    {
    public:
        VertexBuffer(float *vertices, size_t size);

        ~VertexBuffer();

        void Bind() const;

        void Unbind() const;

        const BufferLayout &GetLayout() const { return m_Layout; }

        void SetLayout(const BufferLayout &layout) { m_Layout = layout; }

    private:
        uint32_t m_RendererID;
        BufferLayout m_Layout;
    };

    class IndexBuffer
    {
    public:
        IndexBuffer(unsigned int *indices, size_t count);

        ~IndexBuffer();

        void Bind() const;

        void Unbind() const;

        size_t GetCount() const { return m_Count; }

    private:
        unsigned int m_RendererID{};
        unsigned int m_Count;
    };

}