#shader vertex
#version 330 core
		
in vec4 position; // layout(location = 0) // not needed, apparently
in vec4 color; // layout(location = 3)
//in vec2 uv;

uniform mat4 _VP;
uniform mat4 _M;

out vec4 out_color;
out vec2 out_uv;

void main(){
	mat4 mvp = _VP * _M;
	gl_Position = mvp * position;

	out_color = color;
	out_uv = color.xy;
}

#shader fragment
#version 330 core
			
in vec4 out_color;
in vec2 uv;

uniform sampler2D _Texture;

out vec4 color;

void main(){
	color = texture(_Texture, uv);
}