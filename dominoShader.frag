// Shader-ul de fragment / Fragment shader  
 
 #version 400

in vec3 ex_Color;
out vec3 out_Color;

void main(void)
{
	out_Color = ex_Color; 
}
 