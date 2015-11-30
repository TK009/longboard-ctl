class Slider
{
  int x,y,h,w,val;
  String name;
  Slider(String name_,int x_,int y_,int w_,int h_,int val_)
  {
    x = x_;
    y = y_;
    w = w_;
    h = h_;
    val = val_;
    name = name_;
  }
  boolean update(int mousX,int mousY)
  {
    if((mousX >= x && mousX <= x + w)&&(mousY >= y && mousY <= y + h))
    {
      val = 100-int((1.0*(mousY - y)/h)*100);
      return true;
    }
    else
    {
      return false;
    }
  }
  void display(PGraphics target)
 {
   target.pushStyle();
   target.text(name,x+w/2,y-10);
   target.fill(50,200,50);
   target.stroke(100);
   target.line(x+w/2,y,x+w/2,y+h);
   target.noStroke();
   target.rect(x,y+((100.0-val)/100.0)*h ,w,20);
   target.popStyle();
 }
}