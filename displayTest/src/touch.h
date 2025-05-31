#ifndef TOUCH_H
#define TOUCH_H

#include <Wire.h>
#include <TAMC_GT911.h>
#include "point.h"

/* uncomment for GT911 */
#define TOUCH_GT911
#define TOUCH_GT911_SCL 20
#define TOUCH_GT911_SDA 19
#define TOUCH_GT911_INT -1
#define TOUCH_GT911_RST 38
#define TOUCH_GT911_ROTATION ROTATION_NORMAL
#define xMax 800
#define yMax 480

int touch_last_x = 0, touch_last_y = 0;
point touchArray[5];
int numTouches = 0;


TAMC_GT911 ts = TAMC_GT911(TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_INT, TOUCH_GT911_RST, xMax, yMax);

void touch_init()
{
  Wire.begin(TOUCH_GT911_SDA, TOUCH_GT911_SCL);
  ts.begin();
  ts.setRotation(TOUCH_GT911_ROTATION);
}

bool touch_touched()
{
  ts.read();
  if (ts.isTouched)
  {
    touch_last_x = xMax - ts.points[0].x;
    touch_last_y = yMax - ts.points[0].y;
    return true;
  }
  else
  {
    return false;
  }
}

bool getPoints()
{
  ts.read();
  if (ts.isTouched)
  {
    numTouches = ts.touches;
    for(int i = 0; i < numTouches; i++)
    {
      touchArray[i].x = xMax - ts.points[i].x ;
      touchArray[i].y = yMax - ts.points[i].y ;
    }
    return true;
  }
  else
  {
    return false;
  }
}


#endif