#version 460 core

in vec2 uv;
out vec4 FragColor;

uniform float time;
uniform double scale;
uniform double centerX;
uniform double centerY;
uniform int max_iterations;

layout(std430, binding = 0) buffer ReferenceOrbit {
    dvec2 W[];
};

void main() {

    // ========== Mandelbrot set ==========
    dvec2 dc = dvec2((uv.x - 0.5f) * scale * 1.75 + centerX, (uv.y - 0.5f) * scale - centerY);
    dvec2 dz = dvec2(0.0f, 0.0f);

    // ========== Julia set ==========
    // dvec2 dz = dvec2((uv.x - 0.5f) * scale * 1.75 + centerX, (uv.y - 0.5f) * scale - centerY);

    // Default Julia
    // dvec2 dc = dvec2(-0.7f, 0.27015f);

    // Julia_1
    // vec2 dc = vec2(-0.8f, 0.156f);

    // Julia_2
    // vec2 dc = vec2(-0.4f, 0.6f);

    // Julia_3
    // vec2 dc = vec2(-0.285f * sin(0.55f * time), 0.01f * sin(0.55f * time));

    // Julia_4
    // vec2 dc = vec2(-0.7269f, 0.1889f);

    // Julia_5
    // vec2 dc = vec2(-0.3192f, 0.6117f);

    int it = 0;
    dvec2 z;

    for (it = 0; it < max_iterations; it++) {

        dvec2 w = W[it];

        // z -> z^2 + c, z = x + iy
        // x + iy -> (x + iy)^2 + c = x^2 + 2ixy - y^2 + c
        // x + iy -> ( x^2 - y^2 ) + ( 2ixy ) + c
        //z = vec2(z.x * z.x - z.y * z.y, 2.f * z.x * z.y) + c;

        // delta_z = 2 * w_curr * delta_z + delta_z^2 + delta_c
        double dz2_re = dz.x * dz.x - dz.y * dz.y;
        double dz2_im = 2 * dz.x * dz.y;

        double cross_re = w.x * dz.x - w.y * dz.y;
        double cross_im = w.x * dz.y + w.y * dz.x;

        dz.x = dz2_re + 2.0f * cross_re + dc.x;
        dz.y = dz2_im + 2.0f * cross_im + dc.y;

        z = w + dz;
        double dot_z = dot(z,z);

        if (dot_z > 4.0f) break;

        // TODO: Rebase (чтобы при сильном приближении изображение не дёргалось)
    }
    double m = double(it) - log2(log2(float(dot(z,z))) / 2.0f);
    float t = float(m / double(max_iterations));
    FragColor = vec4(t, t * t, 1.0f - cos(t), 1.0f);

    // float t = it / max_iterations;

    // ===== Синусоидальная палитра =====
    // float r = sin(t * 6.28318f);
    // float g = sin((t + 0.33f) * 6.28318f);
    // float b = sin((t + 0.67f) * 6.28318f);
    // FragColor = vec4(r, 1.0f - g, b, 1.0f);
    // FragColor = vec4(1.0f - r, g, b, 1.0f);
    // FragColor = vec4(r, g, 1.0f - b, 1.0f);
    // FragColor = vec4(1.0f - r, g, 1.0f - b, 1.0f);
    
    // ===== Циклическая радуга =====
    // float r = clamp(abs(t * 6.0f - 3.0f) - 1.0f, 0.0f, 1.0f);
    // float g = clamp(2.0f - abs(t * 6.0f - 2.0f), 0.0f, 1.0f);
    // float b = clamp(2.0f - abs(t * 6.0f - 4.0f), 0.0f, 1.0f);
    // FragColor = vec4(r, g, b, 1.0f);
    // FragColor = vec4(r, 1.0f - g, 1.0f - b, 1.0f);
    // FragColor = vec4(1.0f - r, 1.0f - g, b, 1.0f);
    // FragColor = vec4(1.0f - r, g, 1.0f - b, 1.0f);

    // ===== Радуга с контрастом =====
    // vec3 color;
    // color.r = sin(3.14159 * t * 2.0f) * 0.8f + 0.5f;
    // color.g = sin(3.14159 * t * 2.0f + 2.094f) * 0.8f + 0.5f;
    // color.b = sin(3.14159 * t * 2.0f + 4.188f) * 0.8f + 0.5f;
    // FragColor = vec4(color, 1.0f);

    // ===== Сглаживание итераций =====
    // float smooth_t = float(it) - log2(log(it)) / log2(2.0f);
    // smooth_t = clamp(smooth_t / 350.0f, 0.0f, 1.0f);
    // FragColor = vec4(smooth_t, smooth_t * smooth_t, smooth_t, 1.0f);

    // ===== Бирюзовая схема =====
    // FragColor = vec4(t*t, t, t, 1.0f);

    // ===== Бирюзовый градиент =====
    // FragColor = vec4(0.0f, t, t, 1.0f);

    // ===== Инвертированный бирюзовый градиент =====
    // FragColor = vec4(0.0f, 1.0f - t, 1.0f - t, 1.0f);
    
    // ===== Розовая схема =====
    // FragColor = vec4(t, t*t, t, 1.0f);

    // ===== Розовый градиент =====
    // FragColor = vec4(t, 0.0f, t, 1.0f);

    // ===== Инвертированный розовый градиент =====
    // FragColor = vec4(1.0f - t, 0.0f, 1.0f - t, 1.0f);

    // ===== Золотая схема =====
    // FragColor = vec4(t, t, t*t, 1.0f);

    // ===== Жёлтый градиент =====
    // FragColor = vec4(t, t, 0.0f, 1.0f);

    // ===== Инвертированный жёлтый градиент =====
    // FragColor = vec4(1.0f - t, 1.0f - t, 0.0f, 1.0f);

    // ===== Зелёная схема =====
    // FragColor = vec4(t*t, t, t*t, 1.0f);

    // ===== Зелёный градиент =====
    // FragColor = vec4(0.0f, t, 0.0f, 1.0f);

    // ===== Инвертированный зелёный градиент =====
    // FragColor = vec4(0.0f, 1.0f - t, 0.0f, 1.0f);

    // ===== Красная схема =====
    // FragColor = vec4(t, t*t, t*t, 1.0f);

    // ===== Красный градиент =====
    // FragColor = vec4(t, 0.0f, 0.0f, 1.0f);

    // ===== Инвертированный красный градиент =====
    // FragColor = vec4(1.0f - t, 0.0f, 0.0f, 1.0f);

    // ===== Монохромный (Ч/Б) =====
    // FragColor = vec4(t*t, t*t, t*t, 1.0f);

    // ===== Инвертированный монохромный (Ч/Б) =====
    // FragColor = vec4(1.0f - t*t, 1.0f - t*t, 1.0f - t*t, 1.0f);
}
