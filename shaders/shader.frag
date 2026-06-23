#version 460 core

in vec2 uv;
out vec4 FragColor;

uniform float u_time;
uniform double u_scale;
uniform double u_centerX;
uniform double u_centerY;
uniform double u_aspect;
uniform int u_iterations;
uniform vec2 u_resolution;

layout(std430, binding = 0) buffer ReferenceOrbit {
    dvec2 W[];
};


void main() {
    
    // TODO: доделать SSAA (anti-aliasing)

    // vec2 uv = gl_FragCoord.xy / u_resolution;
    // const int samplesPerAxis = 2;
    // float sum = 0.0f;
    // int samples = samplesPerAxis * samplesPerAxis;
    // vec2 offset = 0.5f / vec2(samplesPerAxis);
    //
    // for (int i = 0; i < samplesPerAxis; i++) {
    //     for (int j = 0; j < samplesPerAxis; j++) {
    //         vec2 subPixel = vec2(i,j) / vec2(samplesPerAxis) + offset;
    //         // vec2 dUV = vec2(subPixel - 0.5f) / u_resolution;
    //         // vec2 eps = vec2(
    //         //     (dUV.x - 0.5f) * u_scale * u_aspect + u_centerX,
    //         //     (dUV.y - 0.5f) * u_scale - u_centerY
    //         // );
    //         vec2 uv_sample = uv + (subPixel - 0.5f) / u_resolution;
    //         sum += compute_fractal(uv_sample);
    //     }
    // }
    // float t = sum / float(samples);





    // ========== Mandelbrot set ==========
    // vec2 dc = vec2(
    //     (uv.x - 0.5f) * u_scale * u_aspect + u_centerX,
    //     (uv.y - 0.5f) * u_scale - u_centerY
    // );
    // dvec2 dz = dvec2(0.0f, 0.0f);

    // ========== Julia set ==========
    vec2 dz = vec2(
        (uv.x - 0.5f) * u_scale * u_aspect + u_centerX,
        (uv.y - 0.5f) * u_scale - u_centerY
    );

    // Default Julia
    // vec2 dc = vec2(-0.7f * sin(0.55f * u_time), 0.27015f * sin(0.55f * u_time));
    // vec2 dc = vec2(-0.7f, 0.27015f);

    // Julia_1
    // vec2 dc = vec2(-0.8f, 0.156f);

    // Julia_2
    // vec2 dc = vec2(-0.4f, 0.6f);

    // Julia_3
    // vec2 dc = vec2(-0.285f * sin(0.55f * u_time), 0.01f * sin(0.55f * u_time));

    // Julia_4
    vec2 dc = vec2(-0.7269f, 0.1889f);

    // Julia_5
    // vec2 dc = vec2(-0.3192f, 0.6117f);

    int it = 0;
    dvec2 z = dvec2(0.0f, 0.0f);

    for (it = 0; it < u_iterations; it++) {

        dvec2 w = W[it];

        vec2 dz2 = vec2(
            dz.x * dz.x - dz.y * dz.y,
            2 * dz.x * dz.y
        );

        vec2 cross = vec2(
            w.x * dz.x - w.y * dz.y,
            w.x * dz.y + w.y * dz.x
        );

        dz = vec2(
            dz2.x + 2 * cross.x + dc.x,
            dz2.y + 2 * cross.y + dc.y
        );

        z = w + dz;

        if (dot(z,z) > 4.0f) break;

        // TODO: Rebase 
    }

    // if (it == u_iterations) {
    //     FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    // } else {
    //     FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    // }

    // double m = double(it) - log2(log2(log2(log2(float(dot(z,z))))));
    // double m = double(it) + 1.0f - log2(log2(float(dot(z,z))));
    // double m = double(it) + 1.0f + log2(log2(float(dot(z,z)))) * 0.5f;

    double m = double(it) + 1.0f - log2(log2(log2(float(dot(z,z))))) * 0.5f;
    float t = float(m / u_iterations);
    FragColor = vec4(t, t * t, 1.0f - cos(t), 1.0f);

    // FragColor = vec4(t * t, t, 1.0f - cos(t), 1.0f);
    // FragColor = vec4(t, t, 1.0f - cos(t), 1.0f);
    // FragColor = vec4(t * t, t * t, t, 1.0f);

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
    // float smooth_t = float(t) - log2(log2(t)) / log2(2.0f);
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




// float compute_fractal(vec2 uv) {
//
//     // ========== Mandelbrot set ==========
//     vec2 dc = vec2(
//         (uv.x - 0.5f) * u_scale * u_aspect + u_centerX,
//         (uv.y - 0.5f) * u_scale - u_centerY
//     );
//     vec2 dz = vec2(0.0f, 0.0f);
//
//     // ========== Julia set ==========
//     // vec2 dz = vec2((uv.x - 0.5f) * u_scale * 1.75 + u_centerX, (uv.y - 0.5f) * u_scale - u_centerY);
//
//     // Default Julia
//     // vec2 dc = vec2(-0.7f, 0.27015f);
//
//     // Julia_1
//     // vec2 dc = vec2(-0.8f, 0.156f);
//
//     // Julia_2
//     // vec2 dc = vec2(-0.4f, 0.6f);
//
//     // Julia_3
//     // vec2 dc = vec2(-0.285f * sin(0.55f * u_time), 0.01f * sin(0.55f * u_time));
//
//     // Julia_4
//     // vec2 dc = vec2(-0.7269f, 0.1889f);
//
//     // Julia_5
//     // vec2 dc = vec2(-0.3192f, 0.6117f);
//
//     int it = 0;
//     vec2 z = vec2(0.0f, 0.0f);
//
//     for (it = 0; it < u_iterations; it++) {
//
//         vec2 w = W[it];
//
//         vec2 dz2 = vec2(
//             dz.x * dz.x - dz.y * dz.y,
//             2 * dz.x * dz.y
//         );
//
//         vec2 cross = vec2(
//             w.x * dz.x - w.y * dz.y,
//             w.x * dz.y + w.y * dz.x
//         );
//
//         dz = vec2(
//             dz2.x + 2.0f * cross.x + dc.x,
//             dz2.y + 2.0f * cross.y + dc.y
//         );
//
//         z = w + dz;
//
//         if (float(dot(z,z)) > 4.0f) break;
//
//         // TODO: Rebase 
//     }
//     // if (it == u_iterations) {
//     //     FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
//     // } else {
//     //     FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
//     // }
//
//     double m = double(it) - log2(log2(log2(log2(float(dot(z,z))))));
//     // double m = double(it) + 1.0f - log2(log2(float(dot(z,z))));
//     // double m = double(it) + 1.0f + log2(log2(float(dot(z,z)))) * 0.5f;
//     // double m = double(it) + 1.0f - log2(log2(log2(float(dot(z,z))))) * 0.5f;
//     return float(m / u_iterations);
// }
