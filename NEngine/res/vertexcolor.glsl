#shader vertex
#version 330 core
		
in vec4 position; // layout(location = 0) // not needed, apparently
in vec4 color; // layout(location = 3)

//uniform vec4 _InputColor1;
//uniform vec4 _InputColor2;
uniform mat4 _VP;
uniform mat4 _M;
uniform float _Mult;
uniform float _Range;

out vec4 out_color;

void main(){
	mat4 mvp = _VP * _M;
	//mat4 mvp = _M;
	gl_Position = mvp * position;
	//gl_FrontColor = color;
	//out_color = color;
	
	//float grad = (2 + position.y)*0.2;
	//grad += length(position) * _Range;
	//grad = clamp(grad * _Mult, 0, 1);
	//vec3 vc = mix(_InputColor1.rgb, _InputColor2.rgb, grad);
	//out_color = color * vec4(vc, 1);

	out_color = color;
}

#shader fragment
#version 330 core
			
in vec4 out_color;

layout(location = 0) out vec4 color;

void main(){
	//color = vec4(1.0, 0, 0, 1);
	color = out_color;
}