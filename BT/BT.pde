//required for BT enabling on startup

import android.content.Intent;
import android.os.Bundle;
import ketai.net.bluetooth.*;
import ketai.ui.*;
import ketai.net.*;

PFont fontMy;
boolean bReleased = true; //no permament sending when finger is tap
KetaiBluetooth bt;
boolean isConfiguring = true;
String msg = "";
KetaiList klist;
ArrayList devicesDiscovered = new ArrayList();
String s, v, a, m, c = "";
PImage background, battery_frame, battery_fill, cell_ring;
float battery = 0.8; // 0-1.0
float[] cells = {3.1,3.2,3.3,3.4,3.5,3.6,3.7,3.8};
final int batX = 210;
final int batY = 450;
final int cell_r = 230;
final float cell_max = 4.2;
final float cell_min = 3.2;
final float chargeMax = cell_max*8;
final float chargeMin = cell_min*8;
Slider gas,slider1,slider2;
PGraphics screen;
int screenW = 720;
int screenH = 1280;
Button abortButton;
//********************************************************************
// The following code is required to enable bluetooth at startup.
//********************************************************************

void onCreate(Bundle savedInstanceState) {
  super.onCreate(savedInstanceState);
  bt = new KetaiBluetooth(this);
}

void onActivityResult(int requestCode, int resultCode, Intent data) {
  bt.onActivityResult(requestCode, resultCode, data);
}

void setup() {
  size(displayWidth, displayHeight);
  
  orientation(PORTRAIT);
  background(0);
  screen = createGraphics(720, 1280);
  bt.start();
  //at app start select device…
  isConfiguring = true;
  //font size
  fontMy = createFont("SansSerif", 40);
  textFont(fontMy);

  abortButton= new Button("abort.png",0,1180);
  battery_frame = loadImage("battery_frame.png");
  battery_fill = loadImage("battery_fill.png");
  background = loadImage("background.png");
  cell_ring = loadImage("cell_ring.png");
  
  gas = new Slider("Thortle",450,800,150,350,50);
  slider1 = new Slider("Slider1",120,800,100,350,50);
  slider2 = new Slider("Slider2",230,800,100,350,50);
}

void draw() {
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
    if(mousePressed)
    {
      int scaleX = int(mouseX * ((1.0*screenW)/(1.0*width)));
      int scaleY = int(mouseY * ((1.0*screenH)/(1.0*height)));
      gas.update(scaleX,scaleY);
      String mes = "G"+gas.val+"\n";
      bt.broadcast(mes.getBytes());
      if(slider1.update(scaleX,scaleY))
      {
        mes = "B"+slider1.val+"\n";
        bt.broadcast(mes.getBytes());
      }
      if(slider2.update(scaleX,scaleY))
      {
        mes = "M"+slider2.val+"\n";
        bt.broadcast(mes.getBytes());
      }
      
    }
    else
    {
      gas.val = 0;
      bt.broadcast("G0\n".getBytes());
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
    screen.rect(batX+300 -(1.0-battery)*300, batY, (1.0-battery)*300, 150);
    screen.image(battery_frame, batX, batY);
    screen.fill(255);
    screen.textSize(80);
    screen.text(int(100*battery) + "%", batX+150, batY+100);
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
    abortButton.display(screen);
    gas.display(screen);
    slider1.display(screen);
    slider2.display(screen);
    screen.endDraw();
    image(screen, 0, 0, width, height);
  }
}
void parseMsg(String mes)
{
  switch(mes.charAt(0))
  {
    case 'C':
      String cell_str[] = mes.substring(1,mes.length()-1).split(";");
      if(cell_str.length == cells.length)
      {
        float sum = 0.0;
        for(int n=0;n<cells.length;n++)
        {
          cells[n] = int(cell_str[n])/1000.0;
          sum += cells[n];
        }
        battery = (sum-chargeMin)/(chargeMax-chargeMin);
      }
    break;
  }
}
String bt_info()
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
void onKetaiListSelection(KetaiList klist) {
  String selection = klist.getSelection();
  bt.connectToDeviceByName(selection);
  //dispose of list for now
  klist = null;
}

//Call back method to manage data received
void onBluetoothDataEvent(String who, byte[] data) {
  if (isConfiguring)
    return;
  msg += new String(data);
  if (msg.charAt(msg.length()-1) == '\n')
  {
    parseMsg(msg);
    msg = "";
  }
}

void mousePressed()
{ 
  int scaleX = int(mouseX * ((1.0*screenW)/(1.0*width)));
  int scaleY = int(mouseY * ((1.0*screenH)/(1.0*height)));
  if(abortButton.checkClick(scaleX,scaleY))
  {
    String mes ="X0\n";
    bt.broadcast(mes.getBytes());
  }
}