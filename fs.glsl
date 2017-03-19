#version 330 core
out vec4 color;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_pos;

void main() {
    
	// Ambient
    float ambientStrength = 0.8f;
    vec3 ambient = ambientStrength * light_color;

    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light_pos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light_color;

    vec3 result = (ambient + diffuse) * object_color;
    color = vec4(result, 1.0f);
}
