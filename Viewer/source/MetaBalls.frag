#version 120

uniform vec2 metaBalls[500];
uniform float metaBallsRadii[500];
uniform int ballCount;

void main() {
	vec2 relative = gl_TexCoord[0].xy * vec2(1280, 720);
	float sum = 0.0;

	for(int i = 0; i < ballCount; i++){
		float d = length(relative - metaBalls[i]);
		sum += (metaBallsRadii[i]*4) / (d*d*d);	
	}

	gl_FragColor = vec4(sum, sum, sum, 1);
}