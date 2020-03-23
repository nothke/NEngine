#shader vertex
#version 330 core
		
layout(location = 0) in vec4 position; // layout(location = 0) // not needed, apparently
layout(location = 1) in vec2 uv;
layout(location = 2) in vec4 color; // layout(location = 3)

uniform mat4 _VP;
uniform mat4 _M;
uniform vec4 _CamPos;
uniform float _Mult;
uniform float _Range;
uniform vec4 _FogParams; // x - range, y - power, z - height offset, a - height mult
uniform float _Time;

out vec4 v_color;
out vec2 v_uv;

void main(){
	mat4 mvp = _VP * _M;
	
	vec3 worldPos = (_M * position).xyz; // TODO: to world
	float time = _Time * 10;
	float xwave = sin(time + sin(worldPos.z - worldPos.x * 0.3f + _Time * 2) * 3) * 0.1f;
	float zwave = sin(time + cos(worldPos.x + worldPos.z * 0.3f + _Time)) * 0.1f;

	vec3 off = vec3(xwave, 0, zwave) * 0.6;
	worldPos += off * color.r;

	vec3 localPos = (inverse(_M) * vec4(worldPos, 1)).xyz;

	gl_Position = mvp * vec4(localPos, 1);
	float fog = pow(length(-_CamPos.xyz - worldPos), _FogParams.y) / _FogParams.x;
	float heightFog = (_FogParams.z - worldPos.y) / _FogParams.w;
	fog = max(heightFog, fog);
	fog = clamp(fog, 0, 1);

	v_color = vec4(color.rgb, fog); // v_color gives funky results
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
	vec4 tex = texture(_Texture, v_uv);
	if (tex.a < 0.5) discard;
	col = mix(tex, _InputColor1, v_color.a);
}