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


volatile bool G_bSuccessful=FALSE;
volatile bool G_bFailure=FALSE;

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
static bool bCompare=TRUE; 
static u8 u8Buzzer=0;
 u8* u8WelcomeMessage="Ready to Start";
 u8* u8SetPassword="Enter code";
 u8* u8StartGameTop="Press any button";
 u8* u8StartGameBot="to start!" ;
 /*hint messages*/
 static u8* u8HintCorrectButton[]={"No correct buttons","1 correct button","2 correct buttons","3 correct buttons","4 correct buttons"} ;
static u8* u8HintCorrectPlace[]= {"No correct spot","1 correct spot","2 correct spots","3 correct spots"};
 
static u8* u8GameEndMessages[]= {"Good job!","You Failed!"};

static u16 u16Success=0;
static u16 u16Fail=0;


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
  /*for(u8 i=0;i<4;i++)
  {
    UserApp2_u8Code[i]=i;
   
  }*/
  UserApp2_u32Counter=0;
  UserApp2_bGameInProgress=FALSE;
  UserApp2_u8NumberOfButtonsPressed=0; 
  
 LCDClearChars(LINE1_START_ADDR,20);
 LCDMessage(LINE1_START_ADDR,u8WelcomeMessage);
  
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    LCDMessage(LINE2_START_ADDR,u8SetPassword);
    UserApp2_StateMachine = UserApp2SM_SetPassword;
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
static void UserApp2SM_SetPassword(void){
  if(!UserApp2_bGameInProgress)
  {
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    UserApp2_u8Code[UserApp2_u8NumberOfButtonsPressed]=0;
    UserApp2_u8NumberOfButtonsPressed++;
  }
    if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    UserApp2_u8Code[UserApp2_u8NumberOfButtonsPressed]=1;
     UserApp2_u8NumberOfButtonsPressed++;
  }
    if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    UserApp2_u8Code[UserApp2_u8NumberOfButtonsPressed]=2;
     UserApp2_u8NumberOfButtonsPressed++;
  }
    if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    UserApp2_u8Code[UserApp2_u8NumberOfButtonsPressed]=3;
     UserApp2_u8NumberOfButtonsPressed++;
  }
 if(UserApp2_u8NumberOfButtonsPressed==4)
 {
   UserApp2_bGameInProgress=TRUE;
   LCDClearChars(LINE1_START_ADDR,20);
  LCDMessage(LINE1_START_ADDR,u8StartGameTop);
  LCDClearChars(LINE2_START_ADDR,20);
  LCDMessage(LINE2_START_ADDR,u8StartGameBot);
 }
}
 else
 { 
        
        if(WasButtonPressed(BUTTON0)||WasButtonPressed(BUTTON1)||WasButtonPressed(BUTTON2)||WasButtonPressed(BUTTON3))
        {
          ButtonAcknowledge(BUTTON0);
          ButtonAcknowledge(BUTTON1);
          ButtonAcknowledge(BUTTON2);
          ButtonAcknowledge(BUTTON3);
          UserApp2_u8NumberOfButtonsPressed=0;
          LCDClearChars(LINE1_START_ADDR,20);
          LCDClearChars(LINE2_START_ADDR,20);
      DebugPrintf("List of Attemps\r\n");
         UserApp2_StateMachine = UserApp2SM_AttemptInProgress;
        }
 }
   
}

static void UserApp2SM_AttemptInProgress(void){

  /*timer for round*/
  UserApp2_u32Counter++;

  if(UserApp2_u32Counter==1)
  {
        
    LedBlink(GREEN,LED_1HZ);
    
  }
  if(UserApp2_u32Counter==40000)
  {
    LedOff(GREEN);
    LedBlink(YELLOW,LED_1HZ);
    
  }
  if(UserApp2_u32Counter==55000)
  {
    LedOff(YELLOW);
    LedBlink(RED,LED_1HZ);
    
  }
  else if(UserApp2_u32Counter==TIMEDONE)
  {
    LedOff(RED);
  }

  
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    UserApp2_u8Attempt[UserApp2_u8NumberOfButtonsPressed]=0;
    UserApp2_u8NumberOfButtonsPressed++;
    DebugPrintf("1");
  }
    if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    UserApp2_u8Attempt[UserApp2_u8NumberOfButtonsPressed]=1;
     UserApp2_u8NumberOfButtonsPressed++;
         DebugPrintf("2");

  }
    if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    UserApp2_u8Attempt[UserApp2_u8NumberOfButtonsPressed]=2;
     UserApp2_u8NumberOfButtonsPressed++;
         DebugPrintf("3");

  }
    if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    UserApp2_u8Attempt[UserApp2_u8NumberOfButtonsPressed]=3;
     UserApp2_u8NumberOfButtonsPressed++;
         DebugPrintf("4");

  }
  if(UserApp2_u8NumberOfButtonsPressed==4)
  {
    DebugPrintf("\r\n");
     LCDClearChars(LINE1_START_ADDR,20);
     LCDClearChars(LINE2_START_ADDR,20);

    bCompare=UserApp2_CompareCodes();
    if(bCompare==TRUE)
    {
      UserApp2_StateMachine=UserApp2SM_Success;
    }
   UserApp2_u8NumberOfButtonsPressed=0;
  }
  
    /*turning off buzzer*/
  if(bCompare==FALSE){
    u8Buzzer++;
    if(u8Buzzer==200)  
  {  
    bCompare=TRUE;
    u8Buzzer=0;
    PWMAudioOff(BUZZER1);
    PWMAudioOff(BUZZER2);
  }
  }

if(UserApp2_u32Counter==TIMEDONE)
{
           UserApp2_StateMachine = UserApp2SM_Failure;
}
  
}/*end of UserAppSM_AttemptInProgress*/

static void UserApp2SM_ResetGame(void){
  
   UserApp2_u32Counter=0;
  UserApp2_bGameInProgress=FALSE;
  UserApp2_u8NumberOfButtonsPressed=0; 
  
 LCDClearChars(LINE1_START_ADDR,20);
  LCDClearChars(LINE2_START_ADDR,20);

 LCDMessage(LINE1_START_ADDR,u8WelcomeMessage);
 LCDMessage(LINE2_START_ADDR,u8SetPassword);
   UserApp2_StateMachine = UserApp2SM_SetPassword;
  
}/*end of UserAppSM_AttemptInProgress*/

static void UserApp2SM_Success(void)
{
  LedOff(GREEN);
  LedOff(YELLOW);
  LedOff(RED);
  
  G_bSuccessful=TRUE;
  if(u16Success==0)
  {
  LCDClearChars(LINE1_START_ADDR,20);
  LCDClearChars(LINE2_START_ADDR,20);
  
   LCDMessage(LINE1_START_ADDR,u8GameEndMessages[0]);
  }
  u16Success++;
  
  if(u16Success==2000)
  {
    G_bSuccessful=FALSE;
    u16Success=0;
  UserApp2_StateMachine = UserApp2SM_ResetGame;
  }
}

static void UserApp2SM_Failure(void)
{
  LedOff(GREEN);
  LedOff(YELLOW);
  LedOff(RED);
  G_bFailure=TRUE;
  if(u16Fail==0)
  {
    
  LCDClearChars(LINE1_START_ADDR,20);
  LCDClearChars(LINE2_START_ADDR,20);
  
     LCDMessage(LINE1_START_ADDR,u8GameEndMessages[1]);
  }
  
  u16Fail++;
  
  
  if(u16Fail==2000)
  {
    G_bFailure=FALSE;
    u16Fail=0;  
    UserApp2_StateMachine = UserApp2SM_ResetGame;
  }
  
}
/* Wait for ??? */
static void UserApp2SM_Idle(void)
{
  
} /* end UserApp2SM_Idle() */
 
static bool UserApp2_CompareCodes()
{
  u8 u8zerosIdentical=0;
  u8 u8zerosAttempt=0;
  u8 u8zerosCode=0;
  
  u8 u8onesIdentical=0;
  u8 u8onesAttempt=0;
  u8 u8onesCode=0;
  
  u8 u8twosIdentical=0;
  u8 u8twosAttempt=0;
  u8 u8twosCode=0;
  
  u8 u8threesIdentical=0;
  u8 u8threesAttempt=0;
  u8 u8threesCode=0;
  
  u8 u8TotalIdentical=0;
  u8 u8TotalNumbers=0;
  bool bIdentical=TRUE;

  
  for(u8 i=0;i<4;i++){
    /*checking for identical places*/
    if(UserApp2_u8Code[i]==0&&UserApp2_u8Code[i]==UserApp2_u8Attempt[i])
    {
      u8zerosIdentical++;
    }
     if(UserApp2_u8Code[i]==1&&UserApp2_u8Code[i]==UserApp2_u8Attempt[i])
    {
      u8onesIdentical++;
    }
     if(UserApp2_u8Code[i]==2&&UserApp2_u8Code[i]==UserApp2_u8Attempt[i])
    {
      u8twosIdentical++;
    }
     if(UserApp2_u8Code[i]==3&&UserApp2_u8Code[i]==UserApp2_u8Attempt[i])
    {
      u8threesIdentical++;
    }
    /*Count number of each number in code*/
    if(UserApp2_u8Code[i]==0)
    {
      u8zerosCode++;
    }
     if(UserApp2_u8Code[i]==1)
    {
      u8onesCode++;
    }
     if(UserApp2_u8Code[i]==2)
    {
      u8twosCode++;
    }
     if(UserApp2_u8Code[i]==3)
    {
      u8threesCode++;
    }
    
       /*count number of each number in attempt*/
    if(UserApp2_u8Attempt[i]==0)
    {
      u8zerosAttempt++;
    }
     if(UserApp2_u8Attempt[i]==1)
    {
      u8onesAttempt++;
    }
     if(UserApp2_u8Attempt[i]==2)
    {
      u8twosAttempt++;
    }
     if(UserApp2_u8Attempt[i]==3)
    {
      u8threesAttempt++;
    }
    
    /* sound buzzer for wrong attempt*/
    if (UserApp2_u8Code[i]!=UserApp2_u8Attempt[i])
    {
       PWMAudioSetFrequency (BUZZER1,349 );
       PWMAudioOn(BUZZER1);
       PWMAudioSetFrequency (BUZZER2, 370);
       PWMAudioOn(BUZZER2);
       bIdentical=FALSE;
    }
  }
  /*calculating values*/
   if(u8zerosAttempt<u8zerosCode)
  {
    u8zerosAttempt-=u8zerosIdentical;
  }
  else
     {
       u8zerosAttempt=u8zerosCode;
    u8zerosAttempt-=u8zerosIdentical;
  } 
  if(u8onesAttempt<u8onesCode)
  {
    u8onesAttempt-=u8onesIdentical;
  }
  else
      {
        u8onesAttempt=u8onesCode;
    u8onesAttempt-=u8onesIdentical;
  }
   if(u8twosAttempt<u8onesCode)
  {
    u8twosAttempt-=u8twosIdentical;
  }
  else
      {
        u8twosAttempt=u8twosCode;
    u8twosAttempt-=u8twosIdentical;
  }
    if(u8threesAttempt<u8threesCode)
  {
    u8threesAttempt-=u8threesIdentical;
  } 
    else {
      u8threesAttempt=u8threesCode;
    u8threesAttempt-=u8threesIdentical;
  }
  
  u8TotalIdentical=u8zerosIdentical+u8onesIdentical+u8twosIdentical+u8threesIdentical;
  u8TotalNumbers=u8zerosAttempt+u8onesAttempt+u8twosAttempt+u8threesAttempt;
  

  if(!bIdentical)
  {
     LCDMessage(LINE1_START_ADDR,u8HintCorrectButton[u8TotalNumbers]);
     LCDMessage(LINE2_START_ADDR,u8HintCorrectPlace[u8TotalIdentical]);
  }
  
    
    return bIdentical;
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
