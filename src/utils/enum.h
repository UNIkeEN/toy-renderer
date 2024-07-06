#pragma once

enum class RENDERER_TYPE {
    OpenGL,
    Vulkan,
    None
};

enum class CAMERA_TYPE {
    Orthographic,
    Perspective
};

enum class SHADER_TYPE {
    Wireframe,
    Solid,
    MaterialPreview,
    Rendered,
    Outline,
    Index,
    Custom,
};