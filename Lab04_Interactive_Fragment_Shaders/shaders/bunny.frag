#version 330 core
in vec3 vColor;
in vec3 pos;
out vec4 FragColor;
uniform float uTime;
uniform float uPulseSpeed;
uniform float uGlowIntensity;
uniform int uRenderMode;
uniform vec3 uCheckerColorA;
uniform vec3 uCheckerColorB;
uniform float uCheckerFrequency;
uniform vec3 uSolidColor;

uniform vec3 uFireflyDark;
uniform vec3 uFireflyBright;

vec3 Checkerboard(vec2 u)
{
    vec2 f = u - floor(u);

    return ((f.x < 0.5) ^^ (f.y < 0.5))
        ? vec3(0.76, 0.54, 0.32)  // Warm oak
    : vec3(0.41, 0.25, 0.15); // Dark walnut
}

void main() {

      vec3 color=vColor;
      
     switch (uRenderMode)
    {
        case 0: // solid color 
        {
           color=uSolidColor;
            break;
        }

        case 1: // Checkerboard
        {
            vec2 cell = floor(pos.xy * uCheckerFrequency);
            float pattern = mod(cell.x + cell.y, 2.0);

            color = mix(uCheckerColorA,
                        uCheckerColorB,
                        pattern);
            break;
        }

        case 2: // Fireflies
        {
            float pulse = 0.5 + 0.5 *
                sin(uTime * uPulseSpeed * 6.2831853);

           

            float intensity = clamp(
        pulse * uGlowIntensity,
        0.0,
        1.0
    );


            color = mix(uFireflyDark, uFireflyBright,intensity);
            break;
        }

        default:
        {
            color = vec3(0.0, 1.0, 1.0);
            break;
        }
    }

    FragColor = vec4(color, 1.0);
}


