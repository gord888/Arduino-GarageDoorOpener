//////////////////////////////////////////////////////////////////////////////
// Class definition
// modified version from (http://www.instructables.com/id/Arduino-Button-Tutorial/)to isolate delays in button handling

// button handling required vars
const int DEFAULT_LONGPRESS_DURATION = 5000; // approx 5000ms
const int DELAY                  =  20 ; // Delay per loop in ms
enum { EV_NONE=0, EV_SHORTPRESS, EV_LONGPRESS, EV_BUTTONDOWN };


class ButtonHandler {
  public:
    // Constructor
    ButtonHandler(int pin, int longpress_ur=DEFAULT_LONGPRESS_DURATION);

    // Initialization done after construction, to permit static instances
    void init();

    // Handler, to be called in the loop()
    int handle();

  protected:
    boolean was_pressed;     // previous state
    const int pin;           // pin to which button is connected
    const int longpress_dur; // the long press duration
    unsigned long press_start;;      // the millis() when it was pressed
  public:



};

ButtonHandler::ButtonHandler(int p, int lp)
: pin(p), longpress_dur(lp)
{
}

void ButtonHandler::init()
{
  pinMode(pin, INPUT_PULLUP);
  was_pressed = false;
}

int ButtonHandler::handle()
{
  int event;
  int now_pressed = !digitalRead(pin); // if the pin is set to a pullup, you need to "NOT" the input

  if (!now_pressed && was_pressed) {
    
    // handle release event
    if ((millis() - press_start) < longpress_dur)
    {
      event = EV_SHORTPRESS;
    }
    else
    {
      event = EV_LONGPRESS;
    }
  }
// NOT TESTED
//  else if(now_pressed && !was_prdesse)
//  {
//    event= EV_BUTTONDOWN;
//  }
  else
  {
    event = EV_NONE;
  }
  // update press running duration
  if (now_pressed)
  {
    // if it wasn't pressed before, then start the counter.  or check press_counter = 0
    if(!was_pressed)
    {
      press_start = millis();
    } 
  }

  // remember state, and we're done
  was_pressed = now_pressed;
  return event;
}

//////////////////////////////////////////////////////////////////////////////
