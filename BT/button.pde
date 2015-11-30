class Button
{
  int w,h,x,y;
  PImage btn_img;
  Button(String filename,int x_,int y_)
  {
    btn_img = loadImage(filename);
    h = btn_img.height;
    w = btn_img.width;
    x = x_;
    y = y_;
  }
 void display(PGraphics target)
 {
   target.image(btn_img,x,y);
 }
 boolean checkClick(int cx, int cy)
 {
   return (cx >= x && cx <= x + w)&&(cy >= y && cy <= y + h);
 }
}