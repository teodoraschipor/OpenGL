
// Shader-ul de fragment / Fragment shader

#version 400

in vec4 ex_Color;
uniform int codCol;

out vec4 out_Color;

void main(void)
{
    if ( codCol==0 )
    out_Color = ex_Color;
    if ( codCol==1 ) // fuchsia-crayola:
    out_Color=vec4 ((1.0/255.0)*203.0, (1.0/255.0)*72.0, (1.0/255.0)*183.0, 0.0);
    if ( codCol==2 ) // space-cadet
    out_Color=vec4 ((1.0/255.0)*46.0, (1.0/255.0)*45.0, (1.0/255.0)*77.0, 0.0);
    if ( codCol==3 ) // amazon
    out_Color=vec4 ((1.0/255.0)*51.0, (1.0/255.0)*115.0, (1.0/255.0)*87.0, 0.0);
    if ( codCol==4 ) // russian-green:
    out_Color=vec4 ((1.0/255.0)*109.0, (1.0/255.0)*159.0, (1.0/255.0)*113.0, 0.0);
    if ( codCol==5 ) // eggshell:
    out_Color=vec4 ((1.0/255.0)*228.0, (1.0/255.0)*227.0, (1.0/255.0)*211.0, 0.0);
    if ( codCol==6 ) // champagne-pink:
    out_Color=vec4 ((1.0/255.0)*234.0, (1.0/255.0)*205.0, (1.0/255.0)*194.0, 0.0);
    if ( codCol==7 ) // coquelicot:
    out_Color=vec4 ((1.0/255.0)*252.0, (1.0/255.0)*68.0, (1.0/255.0)*15.0, 0.0);
    if ( codCol==8 ) // sea-green-crayola:
    out_Color=vec4 ((1.0/255.0)*30.0, (1.0/255.0)*225.0, (1.0/255.0)*188.0, 0.0);
    if ( codCol==9 ) // coquelicot:
    out_Color=vec4 ((1.0/255.0)*180.0, (1.0/255.0)*227.0, (1.0/255.0)*61.0, 0.0);
    if ( codCol==10 ) // coquelicot:
    out_Color=vec4 ((1.0/255.0)*225.0, (1.0/255.0)*252.0, (1.0/255.0)*49.0, 0.0);


}
