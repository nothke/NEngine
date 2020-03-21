#shader vertex
#version 330 core
		
layout(location = 0) in vec4 position; // layout(location = 0) // not needed, apparently
layout(location = 1) in vec2 uv;
layout(location = 2) in vec4 color; // layout(location = 3)


uniform mat4 _VP;
uniform mat4 _M;

out vec4 v_color;
out vec2 v_uv;

void main(){
	mat4 mvp = _VP * _M;
	gl_Position = mvp * position;

	v_color = color;
	v_uv = uv;
}

#shader fragment
#version 330 core
			
in vec4 v_color;
in vec2 v_uv;

out vec4 col;

uniform sampler2D _Texture;

void main(){
	col = texture(_Texture, v_uv) * v_color;
}