#version 410 core

in vec2 fragmentPosition;
in vec4 fragmentColor;

out vec4 color;

uniform float time;

void main()
{
	color = vec4(cos(fragmentPosition.y/fragmentPosition.y*time),tan(fragmentPosition.x/fragmentPosition.y *time),sin(fragmentPosition.y/fragmentPosition.x*time), fragmentColor.a + time);﻿
}