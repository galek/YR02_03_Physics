#version 330
// Gizmo Vertex Shader

in vec3 Position;
in vec3 Information;
in vec4 Colour;
in int  Bitset;

out vec3 vPosition;
out vec3 vInformation;
out vec4 vColour;
out int  vID;
out int  vSmooth;
out int  vFacing;

const int IDMASK = 255;
const int SMOOTHMASK = 256;
const int FACINGMASK = 1024;

void main(){
	vPosition = Position;
	vInformation = Information;
	vColour = Colour;
	vID = Bitset & IDMASK;
	vSmooth = (Bitset & SMOOTHMASK) << 8;
	vFacing = (Bitset & FACINGMASK) << 9;
}

