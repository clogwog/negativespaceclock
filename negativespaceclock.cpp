#include "led-matrix.h"
#include "graphics.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <functional>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <ctime>
#include <signal.h>
#include <syslog.h>
#include <sys/time.h>
#include <cstdint>

using namespace std;

using rgb_matrix::GPIO;
using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;


volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
	 syslog( LOG_NOTICE, "interrupt handler ");
	  interrupt_received = true;
}

typedef uint8_t byte;

class pixel
{
public:
   pixel() : r(0), g(0), b(0), i(255), changed(false)
   {
   }

   void set_value(byte r, byte g, byte b, byte i)
   {
       if( this->r != r)
       {
	   this->r = r;
	   this->changed = true;
       }
       if( this->g != g)
       {
	   this->g = g;
	   this->changed = true;
       }
       if( this->b != b)
       {
	   this->b = b;
	   this->changed = true;
       }
       if ( this->i != i)
       {
	   this->i = i;
	   this->changed = true;
       }
   }


    byte r, g, b, i;
    bool changed;
};
#define SCREEN_SIZE 32
typedef pixel screen_type[SCREEN_SIZE][SCREEN_SIZE];

void init_screen(screen_type & screen)
{
    byte x =  rand() % 32;
    byte y =  rand() % 32;
    byte r =  rand() % 255;
    byte g =  rand() % 255;
    byte b =  rand() % 255;
    byte i =  rand() % 155;    
    screen[x][y].set_value(r,g,b,i);

}

int main(int argc, char *argv[]) 
{
  setlogmask( LOG_UPTO (LOG_NOTICE));
  GPIO io;
  if (!io.Init())
    return 1;

   int maxtime = 0;
   if ( argc > 1 )
   {
       string test(argv[1]);
       syslog( LOG_NOTICE, "running for %s seconds then quitting\n", test.c_str());
       maxtime = std::stoi( test );
   }


  //std::string font_type = "./pongnumberfont.bdf";
  std::string font_type = "./10x20.bdf";
  rgb_matrix::Font font;
  if (!font.LoadFont(font_type.c_str())) 
  {
	cout <<  "Couldn't load font " << font_type << std::endl;
        return 1;
  }

  Canvas *canvas = new RGBMatrix(&io, 32, 1);

   rgb_matrix::Color offColor(0,0,0);
  srand(time(0));
  screen_type screen ;
  init_screen(screen);

  time_t t = time(0);
  struct tm* now = localtime(&t);
  time_t startTime = time(0);

  byte mode = rand() %2;



  bool cont = true;
  while(cont)
  {

     t = time(0);
     now = localtime(&t);
     std::string strHour = std::to_string(now->tm_hour);
     if( strHour.length() == 1) strHour.insert(0, 1, '0');
     std::string strMinute = std::to_string(now->tm_min);
     if( strMinute.length() == 1) strMinute.insert(0,1,'0');

	  for( int x=0; x <=31; x++ )
	  {
		for( int y=0; y <=31;y++)
		{
		     if( screen[x][y].changed )
		     {
			 if ( mode == 0 ) // colour
		             canvas->SetPixel(x, y, screen[x][y].r  * (screen[x][y].i /255.0) , screen[x][y].g  * (screen[x][y].i /255.0), screen[x][y].b  * (screen[x][y].i /255.0));
			 else // monchrome
                             canvas->SetPixel(x, y, screen[x][y].r  * (screen[x][y].i /255.0) , screen[x][y].r  * (screen[x][y].i /255.0), screen[x][y].r  * (screen[x][y].i /255.0));
		         screen[x][y].changed = false;
		     }
		}
  	}
	rgb_matrix::DrawText(canvas, font, 0,14 , offColor, strHour.c_str());
        rgb_matrix::DrawText(canvas, font, 12, 30, offColor, strMinute.c_str());
	// wait a bit
	//usleep(10000);
	if ( interrupt_received )
		cont = false;

      if (maxtime > 0 )
      {
          if( difftime(t,startTime) > maxtime)
          {
              cont=false;
              printf("stopping now\n");
          }
      }

        init_screen(screen);
  }


  canvas->Clear();
  delete canvas;
  return 0;
}

