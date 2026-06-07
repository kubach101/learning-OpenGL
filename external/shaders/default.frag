#version 330 core
out vec4 FragColor;
in vec3 vPos;
uniform vec3 uLDir;
void main()
{
vec3 n = normalize(cross(dFdx(vPos), dFdy(vPos)));
float diff = max(dot(n, uLDir), 0.0);
vec3 c = vec3(1.0, 0.0, 1.0);
FragColor = vec4(c*diff, 1.0);
}