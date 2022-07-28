// Shader-ul de varfuri  
 
 #version 400

layout (location = 0) in vec4 in_Position;
layout (location = 2) in vec3 in_Color;

out vec4 gl_Position; 
out vec3 ex_Color;
uniform mat4 mat;
uniform int cod;

void main(void)
{
    switch(cod)
    {
        case 1:
               gl_Position = in_Position; 
               break;
        case 2: 
               gl_Position = mat * in_Position;
               break;
    }
    ex_Color = in_Color;
} 
 