
//Declare a 2D texture as a uniform variable
uniform sampler2D texture;
uniform float pixScale;
uniform float sharpness;

uniform float brightnessScale;
uniform float maxScale;
float smootherstep(float x)
{
    // Scale, and saturate x to 0..1 range
    // Evaluate polynomial
    return x*x*x*(x*(x*6.0 - 15.0) + 10.0);
}


vec2 findClosestDot(vec2 coords, float xFactor, float yFactor) {
    
    
	// scale down the image
	coords.x *= xFactor;
	coords.y *= yFactor;
	
	float ymod = coords.y;
	
	coords.x = floor(coords.x+0.5);
	coords.y = floor(coords.y+0.5);
	int x = int(coords.x);
	ymod -= coords.y;
	float yOffset = 0.5 * sign(ymod);
	
	coords.y += yOffset*float(x%2);
    
    
	// scale up the image
	coords.x /= xFactor;
	coords.y /= yFactor;
	
	return coords;
}


float marekstep(float inp) {
    
	return 1.0 - (1.0/(1.0+exp(sharpness*(inp-0.5))));
}
float briScale(float brightness) {
    
    return  (1.0-brightnessScale) + (brightnessScale)*(1.0/brightness);
}
void main()
{
	vec2 coords = gl_TexCoord[0].xy;
	
	float xFactor = pixScale;
	float yFactor = xFactor;
	
	vec4 color = texture2D(texture,coords);
	
	vec2 quantCoords = findClosestDot(coords, xFactor, yFactor);
	
	vec4 quantColor = texture2D(texture,quantCoords);
	
	float b = (quantColor.r+quantColor.g+quantColor.b)/3.0;
	
	float bri = 1.0 - length(coords - quantCoords)*xFactor*briScale(b);
	
	bri = marekstep(bri);
	//bri = 1.0;
	/*bri *= (1.0 - b)*3.0;
     bri = 1.0 - bri;
     bri = 0.5 + bri*0.5;
     bri= smootherstep(smootherstep(bri));
     bri = 0.2;*/
	vec4 c = vec4(bri*quantColor.r, bri*quantColor.g, bri*quantColor.b, 1.0);
	//Multiply color by texture
	gl_FragColor = gl_Color * c;
}
