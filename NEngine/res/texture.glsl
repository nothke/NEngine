#shader vertex
#version 330 core
		
layout(location = 0) in vec4 position; // layout(location = 0) // not needed, apparently
layout(location = 1) in vec2 uv;
layout(location = 2) in vec4 color; // layout(location = 3)


uniform mat4 _VP;
uniform mat4 _M;
uniform vec4 _CamPos;

out vec4 v_color;
out vec2 v_uv;

void main(){
	mat4 mvp = _VP * _M;
	gl_Position = mvp * position;

	vec3 worldPos = position.xyz; // TODO: to world
	float fog = length(-_CamPos.xyz - worldPos) * 0.05;
	v_color = vec4(v_color.rgb, fog);
	v_uv = uv;
}

#shader fragment
#version 330 core
			
in vec4 v_color;
in vec2 v_uv;

out vec4 col;

uniform sampler2D _Texture;
uniform vec4 _InputColor1;
uniform vec4 _InputColor2;

void main(){
	//vec4 tex = texture(_Texture, v_uv);
	col = mix(_InputColor1, _InputColor2, v_color.rgba);
}