class Graph
{
 int w,h;
 int min = -200;
 int max = 200;
 float data[] = new float[100];
 float scale;
 float step = 0;
  Graph(int w_,int h_)
 {
   w = w_;
   h=h_;
   scale = (h/2)/200.0;
   step = w/100.0;
   for(int n=0;n<100;n++)
   {
     data[n] = random(-200,200);
   } 
 }
 void addData(float d)
 {
   for(int n=1;n<100;n++)
   {
     data[n-1] = data[n];
   }
   data[99] = d;
 }
 void display(PGraphics target,int x,int y)
 {
   target.pushStyle();
   target.fill(255);
   target.stroke(255,0,0);
   target.rect(x,y,w,h);
   target.line(x,y+h/2,x+w,y+h/2);
   for(int n=0;n<99;n++)
   {
     target.line(x + n*step,y+h/2+data[n]*scale,x+(n+1)*step,y+h/2+data[n+1]*scale);
   }
   target.popStyle();
 }
}