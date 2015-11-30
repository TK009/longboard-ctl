package processing.test.bt;

import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import android.content.Intent; 
import android.os.Bundle; 
import ketai.net.bluetooth.*; 
import ketai.ui.*; 
import ketai.net.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class BT extends PApplet {

//required for BT enabling on startup







PFont fontMy;
boolean bReleased = true; //no permament sending when finger is tap
KetaiBluetooth bt;
boolean isConfiguring = true;
String msg = "";
KetaiList klist;
ArrayList devicesDiscovered = new ArrayList();
String s, v, a, m, c = "";
PImage background, battery_frame, battery_fill, cell_ring;
float battery = 0.8f; // 0-1.0
float[] cells = {3.1f,3.2f,3.3f,3.4f,3.5f,3.6f,3.7f,3.8f};
final int batX = 210;
final int batY = 450;
final int cell_r = 230;
final float cell_max = 4.2f;
final float cell_min = 3.2f;
final float chargeMax = cell_max*8;
final float chargeMin = cell_min*8;
PGraphics screen;
Graph g;

//********************************************************************
// The following code is required to enable bluetooth at startup.
//********************************************************************

public void onCreate(Bundle savedInstanceState) {
  super.onCreate(savedInstanceState);
  bt = new KetaiBluetooth(this);
}

public void onActivityResult(int requestCode, int resultCode, Intent data) {
  bt.onActivityResult(requestCode, resultCode, data);
}

public void setup() {
  
  //limit framerate for better battery life
  frameRate(10);
  orientation(PORTRAIT);
  background(0);
  screen = createGraphics(720, 1280);
  g = new Graph(400,200);
  bt.start();
  //at app start select device\u2026
  isConfiguring = true;
  //font size
  fontMy = createFont("SansSerif", 40);
  textFont(fontMy);


  battery_frame = loadImage("battery_frame.png");
  battery_fill = loadImage("battery_fill.png");
  background = loadImage("background.png");
  cell_ring = loadImage("cell_ring.png");
}

public void draw() {
  //at app start select device
  if (isConfiguring)
  {
    ArrayList names;
    background(78, 93, 75);
    if (bt.getDiscoveredDeviceNames().size() > 0)
      klist = new KetaiList(this, bt.getDiscoveredDeviceNames());
    else if (bt.getPairedDeviceNames().size() > 0)
      klist = new KetaiList(this, bt.getPairedDeviceNames());
    isConfiguring = false;
  } else
  {
    if ((mousePressed) && (bReleased == true))
    {
      //send with BT
      byte[] data = {'s', 'w', 'i', 't', 'c', 'h', '\r'};
      bt.broadcast(data);
      //first tap off to send next message
      bReleased = false;
    }
    if (mousePressed == false)
    {
      bReleased = true; //finger is up
    }
    screen.beginDraw();
    screen.image(background, 0, 0);
    screen.noStroke();
    screen.fill(0);
    screen.textSize(20);
    screen.textAlign(CENTER);
    //BT info
    screen.text(bt_info(), 360, 280);

    //BT end

    //draw battery
    screen.image(cell_ring,batX-75,batY-150);
    screen.image(battery_fill, batX, batY);
    screen.rect(batX+300 -(1.0f-battery)*300, batY, (1.0f-battery)*300, 150);
    screen.image(battery_frame, batX, batY);
    screen.fill(255);
    screen.textSize(80);
    screen.text(PApplet.parseInt(100*battery) + "%", batX+150, batY+100);
    //end battery
    
    //draw cell voltages
    screen.fill(0);
    screen.textSize(25);
    for(int n = 0;n<4;n++)
    {
      float ang = 3*PI/4 + n*PI/6;
      screen.pushMatrix();
      screen.translate(360 + cos(ang)*cell_r,batY + 75- sin(ang)*cell_r);
      screen.rotate(-1*(n+1)*PI/6 - PI/12);
      screen.text(String.format("%.2f",cells[n])+"V",0,0);
      screen.popMatrix();
    }
    for(int n = 0;n<4;n++)
    {
      float ang = PI/4 - n*PI/6;
      screen.pushMatrix();
      screen.translate(360 + cos(ang)*cell_r,batY + 75- sin(ang)*cell_r);
      screen.rotate((n+1)*PI/6 + PI/12);
      screen.text(String.format("%.2f",cells[n+4])+"V",0,0);
      screen.popMatrix();
    }
    
    g.display(screen,160,800);
    
    screen.endDraw();
    image(screen, 0, 0, width, height);
  }
}
public void parseMsg(String mes)
{
  switch(mes.charAt(0))
  {
    case 'C':
      String cell_str[] = mes.substring(1,mes.length()-1).split(";");
      if(cell_str.length == cells.length)
      {
        float sum = 0.0f;
        for(int n=0;n<cells.length;n++)
        {
          cells[n] = PApplet.parseInt(cell_str[n])/1000.0f;
          sum += cells[n];
        }
        battery = (sum-chargeMin)/(chargeMax-chargeMin);
      }
    break;
    case 'A':
      g.addData(PApplet.parseFloat(mes.substring(1,mes.length()-1)));
    break;
  }
}
public String bt_info()
{
  String info = "BT_INFO";
  if (bt.isStarted())
  {
    ArrayList<String> devices = bt.getConnectedDeviceNames();
    if(devices.size() > 0)
    {
      info = "Connected to ";
      for (String device: devices)
      {
        info+= device+" ";
      }
    }
    else
    {
      info = "No connection";
    }
  } else
  {
    info = "BLUETOOTH OFF";
  }

  return info;
}
public void onKetaiListSelection(KetaiList klist) {
  String selection = klist.getSelection();
  bt.connectToDeviceByName(selection);
  //dispose of list for now
  klist = null;
}

//Call back method to manage data received
public void onBluetoothDataEvent(String who, byte[] data) {
  if (isConfiguring)
    return;
  msg += new String(data);
  if (msg.charAt(msg.length()-1) == '\n')
  {
    parseMsg(msg);
    msg = "";
  }
}
class Graph
{
 int w,h;
 int min = -2000;
 int max = 2000;
 float data[] = new float[100];
 float scale;
 float step = 0;
  Graph(int w_,int h_)
 {
   w = w_;
   h=h_;
   scale = (h/2)/2000.0f;
   step = w/100.0f;
   for(int n=0;n<100;n++)
   {
     data[n] = random(-2000,2000);
   } 
 }
 public void addData(float d)
 {
   for(int n=1;n<100;n++)
   {
     data[n-1] = data[n];
   }
   data[99] = d;
 }
 public void display(PGraphics target,int x,int y)
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
  public void settings() {  size(displayWidth, displayHeight); }
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "BT" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
