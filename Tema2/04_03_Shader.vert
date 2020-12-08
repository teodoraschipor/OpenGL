in vec4 in_Position;
in vec4 in_Color;

out vec4 gl_Position;
out vec4 ex_Color;
uniform mat4 myMatrix;
uniform int rectangle;



void main(void)
{

    if(rectangle == 0){
        gl_Position = myMatrix*in_Position;
        ex_Color = in_Color;
    }
    if(rectangle == 1){
     mat4 matr;  
	matr[0][0]=0.0003; matr[1][1]=0.0003; matr[2][2]=0; matr[3][3]=1; // diagonala principala
	matr[0][1]=0; matr[0][2]=0; matr[0][3]=0;
	matr[1][0]=0; matr[1][2]=0; matr[1][3]=0;
	matr[2][0]=0; matr[2][1]=0; matr[2][3]=0;
	matr[3][0]=0; matr[3][1]=0; matr[3][2]=0;
	gl_Position = matr*in_Position;
        ex_Color = in_Color;
    }
}
