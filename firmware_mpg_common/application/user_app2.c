/**********************************************************************************************************************
File: user_app2.c                                                                

Description:
This is a user_app2.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp2Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp2RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_UserApp2"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp2Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u32 G_u32AntApiCurrentDataTimeStamp;                       
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;   
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;       


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp2_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp2_StateMachine;            /* The state machine function pointer */
//static u32 UserApp2_u32Timeout;                      /* Timeout counter used across states */
static fnCode_type UserApp2_StateMachine;            /* The state machine function pointer */
static u32 UserApp2_u32Timeout;                      /* Timeout counter used across states */

static u32 UserApp2_u32DataMsgCount = 0;   /* ANT_DATA packets received */
static u32 UserApp2_u32TickMsgCount = 0;   /* ANT_TICK packets received */

static u8 u8LastState = 0xff;
static u8 au8TickMessage[] = "EVENT x\n\r";  /* "x" at index [6] will be replaced by the current code */
static u8 au8DataContent[] = "xxxxxxxxxxxxxxxx";
static u8 au8LastAntData[ANT_APPLICATION_MESSAGE_BYTES] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static u8 au8TestMessage[] = {0, 0, 0, 0, 0xA5, 0, 0, 0};
bool bGotNewData;


//u8 u8CurrentEventCodeExample = G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX];



/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/
static void UserApp12SM_WaitChannelAssign();

static void UserApp2SM_WaitChannelClose(void);
static void UserApp2SM_ChannelOpen(void);
static void UserApp2SM_WaitChannelOpen(void);

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserApp2Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserApp2Initialize(void)
{
u8 au8WelcomeMessage[] = "ANT SLAVE DEMO";
  u8 au8Instructions[] = "B0 toggles radio";
  AntAssignChannelInfoType sAntSetupData;
  
  /* Clear screen and place start messages */
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8WelcomeMessage); 
  LCDMessage(LINE2_START_ADDR, au8Instructions); 

  /* Start with LED0 in RED state = channel is not configured */
  LedOn(RED);
  
 /* Configure ANT for this application */
  sAntSetupData.AntChannel          = ANT_CHANNEL_USERAPP;
  sAntSetupData.AntChannelType      = ANT_CHANNEL_TYPE_USERAPP;
  sAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  sAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  
  sAntSetupData.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
  sAntSetupData.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
  sAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  sAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  sAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  sAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;

  sAntSetupData.AntNetwork = ANT_NETWORK_DEFAULT;
  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    sAntSetupData.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  }
    
  /* If good initialization, set state to Idle */
  if( AntAssignChannel(&sAntSetupData) )
  {
    /* Channel is configured, so change LED to yellow */
    LedOff(RED);
    LedOn(YELLOW);
    UserApp2_StateMachine = UserApp2SM_Idle;
       // UserApp1_StateMachine = UserApp1SM_WaitChannelAssign;

  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    LedBlink(RED, LED_4HZ);
    UserApp2_StateMachine = UserApp2SM_Error;
  }

} /* end UserApp2Initialize() */

  
/*----------------------------------------------------------------------------------------------------------------------
Function UserApp2RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserApp2RunActiveState(void)
{
  UserApp2_StateMachine();

} /* end UserApp2RunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/

/* Wait for ANT channel assignment */
static void UserApp2SM_WaitChannelAssign()
{
  if( AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CONFIGURED)
  {
    /* Channel assignment is successful, so open channel and
    proceed to Idle state */
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp2_StateMachine = UserApp2SM_Idle;
  }
  
  /* Watch for time out */
  if(IsTimeUp(&UserApp2_u32Timeout, TIMEOUT_VALUE))
  {
    //DebugPrintf(UserApp1_au8MessageFail);
    UserApp2_StateMachine = UserApp2SM_Error;    
  }
     
} /* end UserApp2SM_AntChannelAssign */





static void UserApp2SM_WaitChannelClose(void)
{
  /* Monitor the channel status to check if channel is closed */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CLOSED)
  {
    LedOff(GREEN);
    LedOn(YELLOW);

    UserApp2_StateMachine = UserApp2SM_Idle;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp2_u32Timeout, TIMEOUT_VALUE) )
  {
    LedOff(GREEN);
    LedOff(YELLOW);
    LedBlink(RED, LED_4HZ);

    UserApp2_StateMachine = UserApp2SM_Error;
  }
    
} /* end UserAppSM_WaitChannelClose() */

static void UserApp2SM_ChannelOpen(void)
{
   /* Check for BUTTON0 to close channel */
  if(WasButtonPressed(BUTTON0))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    
    /* Queue close channel, initialize the u8LastState variable and change LED to blinking green to indicate channel is closing */
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
    u8LastState = 0xff;
    LedOff(YELLOW);
    LedOff(BLUE);
    LedBlink(GREEN, LED_2HZ);
    
    /* Set timer and advance states */
    UserApp2_u32Timeout = G_u32SystemTime1ms;
    UserApp2_StateMachine = UserApp2SM_WaitChannelClose;
  } /* end if(WasButtonPressed(BUTTON0)) */
  
  /* A slave channel can close on its own, so explicitly check channel status */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) != ANT_OPEN)
  {
    LedBlink(GREEN, LED_2HZ);
    LedOff(BLUE);
    u8LastState = 0xff;
    
    UserApp2_u32Timeout = G_u32SystemTime1ms;
    UserApp2_StateMachine = UserApp2SM_WaitChannelClose;
  } /* if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) != ANT_OPEN) */
 
   /* Always check for ANT messages */
  if( AntReadAppMessageBuffer() )
  { 
    
     /* New data message: check what it is */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      
      UserApp2_u32DataMsgCount++;
      LedOff(GREEN);
      LedOn(BLUE);
      
      /*check if new data is the same as old data and update as we go*/
      bGotNewData=FALSE;
      for(u8 i=0;i<ANT_APPLICATION_MESSAGE_BYTES;i++)
      {
        if(G_au8AntApiCurrentMessageBytes[i]!=au8LastAntData[i])
        {
          bGotNewData=TRUE;
          au8LastAntData[i]=G_au8AntApiCurrentMessageBytes[i];
          
          au8DataContent[2*i]= HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[i]/16);
          au8DataContent[2*i+1]=HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[i]%16);
        }
      }
      if(bGotNewData)
      {
        /*We got new data: show on LCD*/
        LCDClearChars(LINE2_START_ADDR,20);
        LCDMessage(LINE2_START_ADDR, au8DataContent);
      }/*end if(bGotNewData)*/
        
      
    } /* end if(G_eAntApiCurrentMessageClass == ANT_DATA) */

    else if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
      UserApp2_u32TickMsgCount++;
        /* Look at the TICK contents to check the event code and respond only if it's different */
      if(u8LastState != G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX])
      {
        /* The state changed so update u8LastState and queue a debug message */
        u8LastState = G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX];
        au8TickMessage[6] = HexToASCIICharUpper(u8LastState);
        DebugPrintf(au8TickMessage);

        /* Parse u8LastState to update LED status */
        switch (u8LastState)
        {
          /* If we are paired but missing messages, blue blinks */
          case EVENT_RX_FAIL:
          {
            DebugPrintf("Event 2 failed\r\n");
            LedOff(GREEN);
            LedBlink(BLUE, LED_2HZ);
            break;
          }

          /* If we drop to search, LED is green */
          case EVENT_RX_FAIL_GO_TO_SEARCH:
          {
            LedOff(BLUE);
            LedOn(GREEN);
            break;
          }

          /* If the search times out, the channel should automatically close */
          case EVENT_RX_SEARCH_TIMEOUT:
          {
            DebugPrintf("Search timeout\r\n");
            break;
          }

          default:
          {
            DebugPrintf("Unexpected Event\r\n");
            break;
          }
        } /* end switch (G_au8AntApiCurrentMessageBytes) */
      } /* end if (u8LastState ...) */
     
    } /* end else if(G_eAntApiCurrentMessageClass == ANT_TICK) */
  } /* end AntReadAppMessageBuffer() */
  
}

static void UserApp2SM_WaitChannelOpen(void)
{
  /*Monitor the channel status to check if channel is opened*/
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP)==ANT_OPEN)
  {
    DebugPrintf("Opened\r\n");
    LedOn(GREEN);
    UserApp2_StateMachine=UserApp2SM_ChannelOpen;
  }
  /*Check for timeout*/
  if(IsTimeUp(&UserApp2_u32Timeout,TIMEOUT_VALUE))
  {
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
    LedOff(GREEN);
    LedOn(YELLOW);
    UserApp2_StateMachine=UserApp2SM_Idle;
  }
}/*end UserAppSM_WaitChannelOpen()*/

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp2SM_Idle(void)
{
 /* Look for BUTTON 0 to open channel */
  if(WasButtonPressed(BUTTON0))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    
    /* Queue open channel and change LED0 from yellow to blinking green to indicate channel is opening */
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);

    LedOff(YELLOW);
    LedBlink(GREEN, LED_2HZ);
    
    /* Set timer and advance states */
    UserApp2_u32Timeout = G_u32SystemTime1ms;
    UserApp2_StateMachine = UserApp2SM_WaitChannelOpen;
    DebugPrintf("Button 0 pressed\r\n");
  } /*en id(WasButtonPressed(BUTTON0))*/
#if 0
  /* Check for BUTTON0 to close channel */
  if(WasButtonPressed(BUTTON0))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    
    /* Queue close channel, initialize the u8LastState variable and change LED to blinking green to indicate channel is closing */
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
    u8LastState = 0xff;
    LedOff(YELLOW);
    LedOff(BLUE);
    LedBlink(GREEN, LED_2HZ);
    
    /* Set timer and advance states */
    UserApp2_u32Timeout = G_u32SystemTime1ms;
    UserApp2_StateMachine = UserApp2SM_WaitChannelClose;
  } /* end if(WasButtonPressed(BUTTON0)) */
  
#endif
    
} /* end UserApp2SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp2SM_Error(void)          
{
  
} /* end UserApp2SM_Error() */



/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserApp2SM_FailedInit(void)          
{
    
} /* end UserApp2SM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
