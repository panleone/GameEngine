#version 330 core

out vec4 FragColor;
uniform sampler2D postProcessing;


in VS_OUT {
    vec2 TexCoord;
} fs_in;

float Convert_sRGB_FromLinear (float theLinearValue) {
  return theLinearValue <= 0.0031308
       ? theLinearValue * 12.92
       : pow (theLinearValue, 1.0/2.4) * 1.055 - 0.055;
}
void main()
{
    float gamma = 2.2;
    vec4 texCol = texture(postProcessing, fs_in.TexCoord);
    FragColor.r = Convert_sRGB_FromLinear(texCol.x);
    FragColor.g = Convert_sRGB_FromLinear(texCol.y);
    FragColor.b = Convert_sRGB_FromLinear(texCol.z);
    FragColor.w = texCol.w;
}