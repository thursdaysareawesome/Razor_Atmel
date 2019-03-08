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


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp2_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp2_StateMachine;            /* The state machine function pointer */
//static u32 UserApp2_u32Timeout;                      /* Timeout counter used across states */

static u8 UserApp2_u8Code[CODELENGTH];
static u8 UserApp2_u8Attempt[CODELENGTH];
static u32 UserApp2_u32Counter;
static bool UserApp2_bGameInProgress;
static u8 UserApp2_u8NumberOfButtonsPressed;
 u8 u8WelcomeMessage="Ready to Start";

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

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
  
  
  /*initialize the variables*/
  for(u8 i=0;i<4;i++)
  {
    UserApp2_u8Code[i]=i;
   
  }
  UserApp2_u32Counter=0;
  UserApp2_bGameInProgress=TRUE;
  UserApp2_u8NumberOfButtonsPressed=0;
  
  LCDClearChars(LINE1_START_ADDR,20);
  LCDMessage(LINE1_START_ADDR,&au8WelcomeMessage);
  
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp2_StateMachine = UserApp2SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp2_StateMachine = UserApp2SM_FailedInit;
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


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp2SM_Idle(void)
{
  if(UserApp2_bGameInProgress){
  UserApp2_u32Counter++;
  }
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    DebugPrintf("Button 0 pressed");
    UserApp2_u8Attempt[UserApp2_u8NumberOfButtonsPressed]=0;
    UserApp2_u8NumberOfButtonsPressed++;
  }
    if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    DebugPrintf("Button 1 pressed");
    UserApp2_u8Attempt[UserApp2_u8NumberOfButtonsPressed]=1;
     UserApp2_u8NumberOfButtonsPressed++;
  }
    if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    DebugPrintf("Button 2 pressed");
    UserApp2_u8Attempt[UserApp2_u8NumberOfButtonsPressed]=2;
     UserApp2_u8NumberOfButtonsPressed++;
  }
    if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    DebugPrintf("Button 3 pressed");
    UserApp2_u8Attempt[UserApp2_u8NumberOfButtonsPressed]=3;
     UserApp2_u8NumberOfButtonsPressed++;
  }
  if(UserApp2_u8NumberOfButtonsPressed==4)
  {
  UserApp2_CompareCodes();
  UserApp2_u8NumberOfButtonsPressed=0;
  }
} /* end UserApp2SM_Idle() */
 
static bool UserApp2_CompareCodes()
{
  for(u8 i=0;i<CODELENGTH;i++)
  {
    if (UserApp2_u8Code[i]!=UserApp2_u8Attempt[i])
    {
      LedOn(RED);
        return FALSE;
    }
    
  }
  LedOn(BLUE);
    return TRUE;
}


#if 0
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp2SM_Error(void)          
{
  
} /* end UserApp2SM_Error() */
#endif


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserApp2SM_FailedInit(void)          
{
    
} /* end UserApp2SM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
